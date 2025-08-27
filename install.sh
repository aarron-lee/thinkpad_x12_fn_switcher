#/bin/bash

if [ "$EUID" -eq 0 ]; then
    echo "This script must be not be run as root, don't use sudo" >&2
    exit 1
fi

INSTALL_LOCATION=$HOME/.local/bin/fn_switcher
RUN_SCRIPT=$INSTALL_LOCATION/switch.sh

mkdir -p $INSTALL_LOCATION

cd $INSTALL_LOCATION

echo "Downloading files to $INSTALL_LOCATION..."

git clone --depth=1 https://github.com/aarron-lee/thinkpad_x12_fn_switcher.git

echo "Initializing python venv..."

python -m venv --system-site-packages venv

source ./venv/bin/activate

echo "pip install pyusb dependency..."

pip install pyusb

PYTHON_COMMAND=$INSTALL_LOCATION/venv/bin/python3

cat << EOF > "$RUN_SCRIPT"
#!/usr/bin/bash

$PYTHON_COMMAND $INSTALL_LOCATION/thinkpad_x12_fn_switcher/x12_fn_switcher.py
EOF

chmod +x $RUN_SCRIPT

# remove old rule if it previously existed
sudo rm -rf /etc/udev/rules.d/99-thinkpad-fn.rules

cat <<EOF >> "./99-thinkpad-fn.rules"
ACTION=="add", ATTRS{idVendor}=="17ef", ATTRS{idProduct}=="60fe", RUN+="$RUN_SCRIPT"
EOF

sudo mv ./99-thinkpad-fn.rules /etc/udev/rules.d/99-thinkpad-fn.rules

# handle for SE linux
sudo chcon -u system_u -r object_r --type=bin_t $RUN_SCRIPT

sudo udevadm control --reload-rules
sudo udevadm trigger

echo "Install complete. Manually running script once since the script only runs when the keyboard is connected"

./$RUN_SCRIPT
