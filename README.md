# Thinkpad x12 FN Switcher for Linux
Because there is no option in the BIOS of the "Thinkpad" X12 to switch the FN-Key with the CTRL-Key, I had to capture the USB packet sent by the Lenovo Vantage software (which only runs on Windows) with Wireshark and send the packet manually on Linux.

This quickly put together repo contains the c-code to send the packet. Someone might find it useful (in my research I only found someone posting the captured packet, which at least sent me on the correct path).

- [Prerequisites](#prerequisites)
- [How To](#how-to)
- [Making the Fn Key Remap Persistent](#Making-the-Fn-Key-Remap-Persistent)
  - [Quick Install](#quick-install)
  - [Manual Install](#manual-install)
- [Disclaimer](#disclaimer)

## Prerequisites
Prerequisites

-libusb-1.0 development package (for USB communication)
-gcc

Install Required Packages

```
sudo apt install libusb-1.0-0-dev gcc  # Ubuntu/Debian
```

```
sudo dnf install libusb1-devel gcc     # Fedora
```
(If you use another distro I'm sure you know how to install gcc and compile c-code :P)


## How-To
Your Vendor_ID and Product_ID should be the same as mine but you can check with:

```
lsusb
```

Find the line corresponding to your keyboard. It should look something like this:

```
Bus 003 Device 027: ID 17ef:60fe Lenovo ThinkPad X12 Detachable Keyboard
```
Here, 17ef is the Vendor ID (VID) and 60fe is the Product ID (PID).


### Send the packet
Clone this repo or download x12_fn_switcher.c and compile it:
```
gcc x12_fn_switcher.c -o x12_fn_switcher -I/usr/include/libusb-1.0 -lusb-1.0
```

then, run it:

```
sudo ./x12_fn_switcher
```
Your Fn and Ctrl keys should now be swapped



### Making the Fn Key Remap Persistent
(this section is copy pasted from ChatGPT but verified and with small adjustements)

## Quick install

Note that the quick install uses python and all dependencies are provided, so this should work on immutable distros like Fedora Silverblue, Universal Blue variants, etc.

```
curl -L https://github.com/aarron-lee/thinkpad_x12_fn_switcher/raw/main/install.sh | sh
```

To uninstall, run:

```
curl -L https://github.com/aarron-lee/thinkpad_x12_fn_switcher/raw/main/uninstall.sh | sh
```

## Manual Install

By default, the Fn key remapping resets after unplugging the keyboard or rebooting. To make the remap permanent, we use udev rules to trigger the remapping program (x12_fn_switcher) whenever the keyboard is connected.
📂 Step 1: Copy the Program to the Correct Location

Before setting up automation, make sure the x12_fn_switcher binary is in the right place.

1️⃣ Copy the program to /usr/local/bin/ (system-wide location):

```
sudo cp x12_fn_switcher /usr/local/bin/x12_fn_switcher
```

2️⃣ Make it executable:

```
sudo chmod +x /usr/local/bin/x12_fn_switcher
```

🛠 Step 2: Create a udev Rule

We need a udev rule that will automatically run the remapping program whenever the keyboard is connected.

1️⃣ Open the udev rules file for editing:

```
sudo nano /etc/udev/rules.d/99-thinkpad-fn.rules
```

2️⃣ Add the following line (replace the ID if necessary):
```
ACTION=="add", ATTRS{idVendor}=="17ef", ATTRS{idProduct}=="60fe", RUN+="/usr/local/bin/x12_fn_switcher.sh"
```

🔧 Step 3: Create the Fn Remapping Script

Since udev executes scripts as root, we create a wrapper script that runs x12_fn_switcher.

1️⃣ Create the script:
```
sudo nano /usr/local/bin/x12_fn_switcher.sh
```
2️⃣ Add the following content:
```
#!/bin/bash
/usr/local/bin/x12_fn_switcher
```
3️⃣ Make the script executable:
```
sudo chmod +x /usr/local/bin/x12_fn_switcher.sh
```
🔄 Step 4: Reload udev Rules

After creating the rule, reload udev so it takes effect:
```
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## Disclaimer
ChatGPT helped me with the c stuff as I am not very familiar with hid and usb.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND...

## Attribution

Vendored dependency: https://github.com/apmorton/pyhidapi
