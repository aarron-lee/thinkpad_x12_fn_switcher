import usb.core
import usb.util
import sys
from time import sleep

# converted from c to python via Google Gemini, with some minor manual edits to make it fully functional

# Constants
VENDOR_ID = 0x17ef  # Your VID
PRODUCT_ID = 0x60fe  # Your PID
HID_REQUEST_SET_REPORT = 0x09
HID_REPORT_TYPE_OUTPUT = 0x02
INTERFACE = 1
TIMEOUT = 5000

# Find the device
device = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)

if device is None:
    sys.exit("Error: Keyboard not found")

try:
    # Check if a kernel driver is active and detach it
    if device.is_kernel_driver_active(INTERFACE):
        sys.stdout.write("Detaching kernel driver...\n")
        device.detach_kernel_driver(INTERFACE)

    sleep(1)

    # Set the active configuration. The first one is usually what you want.
    # sys.stdout.write("Setting configuration...\n")
    # device.set_configuration()

    # Data from Wireshark (Toggle Fn key)
    data = bytes([0x09, 0xb4, 0x02])

    # Send USB Control Transfer
    sys.stdout.write("Sending SET_REPORT Control Transfer with wIndex=1...\n")

    # libusb_control_transfer in Python equivalent
    # bmRequestType: LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE
    # 0x20 | 0x01 = 0x21
    # bRequest: HID_REQUEST_SET_REPORT (0x09)
    # wValue: (HID_REPORT_TYPE_OUTPUT << 8) | data[0] -> (0x02 << 8) | 0x09 = 0x0209
    # wIndex: INTERFACE (1)
    # data: data
    # timeout: TIMEOUT
    sys.stdout.write("ctrl transfer...\n")

    bytes_sent = device.ctrl_transfer(
        bmRequestType=0x21,
        bRequest=HID_REQUEST_SET_REPORT,
        wValue=(HID_REPORT_TYPE_OUTPUT << 8) | data[0],
        wIndex=INTERFACE,
        data_or_wLength=data,
        timeout=TIMEOUT
    )

    sys.stdout.write(f"Control Transfer successfully sent ({bytes_sent} bytes)\n")

except usb.core.USBError as e:
    sys.stderr.write(f"Error while sending: {e}\n")

finally:
    # Reattach kernel driver if it was detached
    if device.is_kernel_driver_active(INTERFACE) is False:
        device.reset()
        # try:
        #     device.attach_kernel_driver(INTERFACE)
        #     sys.stdout.write("Re-attached kernel driver\n")
        # except usb.core.USBError as e:
        #     sys.stderr.write(f"Could not re-attach kernel driver: {e}\n")

    # The device object will be closed automatically when it goes out of scope.
    # However, you can also explicitly call: usb.util.dispose_resources(device)
