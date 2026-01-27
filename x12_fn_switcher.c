#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID  0x17ef
#define PRODUCT_ID 0x60fe

#define HID_REQUEST_SET_REPORT 0x09
#define HID_REPORT_TYPE_OUTPUT 0x02

#define INTERFACE 1
#define TIMEOUT 5000

static void warn_libusb(const char *msg, int err)
{
    fprintf(stderr, "Warning: %s: %s\n", msg, libusb_error_name(err));
}

static void fatal_libusb(const char *msg, int err)
{
    fprintf(stderr, "Error: %s: %s\n", msg, libusb_error_name(err));
    exit(EXIT_FAILURE);
}

int main(void)
{
    libusb_context *ctx = NULL;
    libusb_device_handle *handle = NULL;
    int res;

    // Magic payload
    unsigned char data[3] = { 0x09, 0xb4, 0x02 };

    // Initialize libusb
    res = libusb_init(&ctx);
    if (res < 0) fatal_libusb("libusb_init failed", res);

    // Open the device
    handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
    if (!handle) {
        fprintf(stderr, "Error: Keyboard not found\n");
        libusb_exit(ctx);
        return EXIT_FAILURE;
    }

    // Detach kernel driver; try auto detach first
    bool auto_detach = false;
    res = libusb_set_auto_detach_kernel_driver(handle, 1);
    if (res == 0) {
        auto_detach = true;
    } else {
        warn_libusb("auto-detach not available", res);
    }

    bool detached_manually = false;

    // Try to claim the interface. If BUSY and no auto-detach, detach then claim.
    res = libusb_claim_interface(handle, INTERFACE);
    if (res == LIBUSB_ERROR_BUSY && !auto_detach) {
        // Kernel driver likely bound; detach it.
        int active = libusb_kernel_driver_active(handle, INTERFACE);
        if (active == 1) {
            res = libusb_detach_kernel_driver(handle, INTERFACE);
            if (res == 0) {
                detached_manually = true;
            } else if (res == LIBUSB_ERROR_NOT_FOUND) {
                // Driver disappeared between checks; continue.
                warn_libusb("kernel driver already detached", res);
            } else {
                fatal_libusb("failed to detach kernel driver", res);
            }
        } else if (active < 0) {
            warn_libusb("kernel_driver_active failed", active);
            // Still attempt detach; worst case it returns NOT_FOUND.
            res = libusb_detach_kernel_driver(handle, INTERFACE);
            if (res == 0) detached_manually = true;
            else if (res != LIBUSB_ERROR_NOT_FOUND) warn_libusb("detach kernel driver failed", res);
        }

        // Retry claim after detach attempt
        res = libusb_claim_interface(handle, INTERFACE);
    }

    if (res < 0) fatal_libusb("failed to claim interface", res);

    // Perform the SET_REPORT control transfer
    res = libusb_control_transfer(handle,
                                  LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                  HID_REQUEST_SET_REPORT,
                                  (HID_REPORT_TYPE_OUTPUT << 8) | data[0],
                                  INTERFACE, data, sizeof(data), TIMEOUT);
    if (res < 0) {
        fatal_libusb("SET_REPORT control transfer failed", res);
    }

    // Release interface
    res = libusb_release_interface(handle, INTERFACE);
    if (res < 0) warn_libusb("release interface failed", res);

    // Attempt reattach if detached manually, but LIBUSB_ERROR_BUSY is OK.
    if (detached_manually) {
        res = libusb_attach_kernel_driver(handle, INTERFACE);
        if (res == LIBUSB_ERROR_BUSY) {
            // Likely already rebound automatically.
            warn_libusb("reattach returned BUSY (already attached)", res);
        } else if (res < 0) {
            warn_libusb("reattach kernel driver failed", res);
        }
    }

    // Close the device and exit libusb
    libusb_close(handle);
    libusb_exit(ctx);

    return EXIT_SUCCESS;
}
