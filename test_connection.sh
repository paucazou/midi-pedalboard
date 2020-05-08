#!/bin/bash

# check dir
if [[ $PWD != $HOME ]]; then
    cd $HOME
fi


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
/home/pi/automidi
exit 0" > /etc/rc.local


# download
echo Downloading libraries...
sudo apt-get update
sudo apt-get install librtmidi

# compiling
echo Compiling program...

g++ -Wall -Wextra -std=c++17 -I /usr/include/rtmidi -lasound -lpthread -lrtmidi -D__LINUX_ALSA__ automidi.cpp -o automidi
echo The device will reboot. Press a key to continue
sudo reboot
