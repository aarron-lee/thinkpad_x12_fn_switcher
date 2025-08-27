#/bin/bash

sudo rm -rf $HOME/.local/bin/fn_switcher
sudo rm -rf /etc/udev/rules.d/99-thinkpad-fn.rules

sudo udevadm control --reload-rules
sudo udevadm trigger
