import thinkpad_hid as hid

VENDOR_ID = 0x17ef
PRODUCT_ID = 0x60fe
INTERFACE_NUMBER = 1
USAGE_PAGE = 1
USAGE = 2

# {'path': b'/dev/hidraw2',
# 'vendor_id': 6127,
# 'product_id': 24830,
# 'serial_number': '',
# 'release_number': 13113,
# 'manufacturer_string': 'Darfon',
# 'product_string': 'Thinkpad X12 Detachable Gen 1 Folio case -1',
# 'usage_page': 1,
# 'usage': 2,
# 'interface_number': 1,
# 'bus_type': <BusType.USB: 1>}

def is_x12_keyboard(dev):
    return (dev['product_id'] == PRODUCT_ID
        and dev['usage'] == USAGE
        and dev['usage_page'] == USAGE_PAGE
        and dev['interface_number'] == INTERFACE_NUMBER
    )

def main():
    # The data to be sent, including the Report ID (0x09).
    # The first byte is the Report ID.
    data = [0x09, 0xb4, 0x02]

    try:
        hid_device = None

        print("Searching for HID device with VID: 0x{:04x}, PID: 0x{:04x}".format(VENDOR_ID, PRODUCT_ID))
        for dev in hid.enumerate(VENDOR_ID):
            if is_x12_keyboard(dev):
                hid_device = hid.Device(path=dev['path'])
                break
        if hid_device:
            print("Device Found. Writing data: {}".format([hex(d) for d in data]))

            hid_device.nonblocking = 1

            print(f"Sending feature report with data: {[hex(d) for d in data]}")
            bytes_written = hid_device.write(bytes(data))

            hid_device.close()
            print(f"{bytes_written} bytes written to device successfully. Exiting.")
        else:
            print("Device not found. Exiting...")

    except IOError as e:
        print("Error: {}".format(e))
        print("Could not open the device. Make sure the correct VENDOR_ID and PRODUCT_ID are set,")
        print("and that you have the necessary permissions to access the device (e.g., as root).")

if __name__ == "__main__":
    main()
