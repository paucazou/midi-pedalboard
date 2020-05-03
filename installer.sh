#!/bin/bash

# check dir
if [[ $PWD != $HOME ]]; then
    cd $HOME
fi

echo If this installer does not work, please reinstall Raspbian first before contacting us.

echo Setting midi...
# https://ixdlab.itu.dk/wp-content/uploads/sites/17/2017/10/Setting-Up-Raspberry-Pi-for-MIDI.pdf
echo "dtoverlay=dwc2" | sudo tee -a /boot/config.txt
echo "dwc2" | sudo tee -a /etc/modules
echo "libcomposite" | sudo tee -a /etc/modules
echo "g_midi" | sudo tee -a /etc/modules

sudo echo 'cd /sys/kernel/config/usb_gadget/
mkdir -p midi_over_usb
cd midi_over_usb
echo 0x1d6b > idVendor # Linux Foundation
echo 0x0104 > idProduct # Multifunction Composite Gadget 
echo 0x0100 > bcdDevice # v1.0.0
echo 0x0200 > bcdUSB # USB2
mkdir -p strings/0x409
echo "fedcba9876543210" > strings/0x409/serialnumber
echo "Paucazou" > strings/0x409/manufacturer  
echo "Pedalboard_Midi_Encoder" > strings/0x409/product
ls /sys/class/udc > UDC
'> /usr/bin/midi_over_usb

sudo chmod +x /usr/bin/midi_over_usb

echo "/usr/bin/midi_over_usb
exit 0" > /etc/rc.local
# TODO ajouter script encodeur au démarrage automatique

# choose channel
digit_re='^[0-9]+$'
while ! [[ $CHANNEL =~ $re ]] || [[ $CHANNEL -lt 15]] ; do
    echo Which channel do you want to use? Please be sure to enter an integer between 0 and 15.
    read CHANNEL
done
echo $CHANNEL > channel
echo You wan always change the channel by setting it in the channel file in your home.

# pins
echo Setting pins. You can change pins default by editing pins file in your home. Pins number are the ones used by WiringPi library. More here: https://cdn.sparkfun.com/assets/learn_tutorials/6/7/6/PiZerov2.pdf
echo "8 9 7 0 2 3 12 13
15 16 1 4" > pins

echo Downloading libraries...
sudo apt-get update
sudo apt-get install librtmidi
sudo apt-get install wiringpi

sudo 
