#/bin/bash

if [ "$EUID" -eq 0 ]; then
    echo "This script must be not be run as root, don't use sudo" >&2
    exit 1
fi

INSTALL_LOCATION=$HOME/.local/bin/fn_switcher

mkdir -p $INSTALL_LOCATION

chmod +x ./x12_fn_switcher

cp ./x12_fn_switcher $INSTALL_LOCATION

# remove old rule if it previously existed
sudo rm -rf /etc/udev/rules.d/99-thinkpad-fn.rules

cat <<EOF >> "./99-thinkpad-fn.rules"
ACTION=="add", ATTRS{idVendor}=="17ef", ATTRS{idProduct}=="60fe", RUN+="$INSTALL_LOCATION/x12_fn_switcher"
EOF

sudo mv ./99-thinkpad-fn.rules /etc/udev/rules.d/99-thinkpad-fn.rules

# handle for SE linux
sudo chcon -u system_u -r object_r --type=bin_t $INSTALL_LOCATION/x12_fn_switcher

sudo udevadm control --reload-rules
sudo udevadm trigger

echo "Install complete. Manually running script once since the script only runs when the keyboard is connected"
