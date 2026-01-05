#/bin/bash

# source: https://bbs.archlinux.org/viewtopic.php?id=276116

# Unknown, no known solution for x12 detachable
# KEYBOARD_KEY_4b=previoussong                     # Notification key
# KEYBOARD_KEY_4c=playpause                        # Answer Voip call key
# KEYBOARD_KEY_4d=nextsong                         # Hang Voip call key

THINKPAD_REBINDS=/etc/udev/hwdb.d/90-thinkpad-rebinds.hwdb

if [ "$EUID" -eq 0 ]; then

# remaps * (star) button to playpause
cat << EOF > "$THINKPAD_REBINDS"
evdev:name:Darfon Thinkpad X12 Detachable Gen 1 Folio case -1:dmi:bvn*:bvr*:bd*:svnLENOVO*:pn*:*
 KEYBOARD_KEY_c0004=playpause
EOF

sudo systemd-hwdb update
sudo udevadm trigger

echo "complete!"
 else
     echo "This script must be be run as root, use sudo" >&2
     exit 1
 fi
