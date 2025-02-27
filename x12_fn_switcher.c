#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID  0x17ef  // Your VID
#define PRODUCT_ID 0x60fe  // Your PID
#define HID_REQUEST_SET_REPORT 0x09
#define HID_REPORT_TYPE_OUTPUT 0x02
#define INTERFACE 1 
#define TIMEOUT 5000

int main() {
    libusb_device_handle *handle;
    int res;

    // Data from Wireshark (Toggle Fn key)
    unsigned char data[3] = { 0x09, 0xb4, 0x02 };

    // Initialize libusb
    if (libusb_init(NULL) < 0) {
        fprintf(stderr, "Error: Failed to initialize libusb\n");
        return EXIT_FAILURE;
    }

    // Open the device
    handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (!handle) {
        fprintf(stderr, "Error: Keyboard not found\n");
        libusb_exit(NULL);
        return EXIT_FAILURE;
    }

    // Detach kernel driver if active
    if (libusb_kernel_driver_active(handle, INTERFACE)) {
        printf("Detaching kernel driver...\n");
        libusb_detach_kernel_driver(handle, INTERFACE);
    }

    // Send USB Control Transfer with **wIndex = 1**
    printf("Sending SET_REPORT Control Transfer with wIndex=1...\n");
    res = libusb_control_transfer(handle,
                                  LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                  HID_REQUEST_SET_REPORT,
                                  (HID_REPORT_TYPE_OUTPUT << 8) | data[0],
                                  INTERFACE, data, sizeof(data), TIMEOUT);
    
    if (res < 0) {
        fprintf(stderr, "Error while sending: %s\n", libusb_error_name(res));
    } else {
        printf("Control Transfer successfully sent (%d bytes)\n", res);
    }

    // Reattach kernel driver if needed
    libusb_attach_kernel_driver(handle, INTERFACE);

    // Close the device and exit libusb
    libusb_close(handle);
    libusb_exit(NULL);

    return EXIT_SUCCESS;
}
