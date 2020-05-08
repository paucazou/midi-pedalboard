# MIDI-Pedalboard
MIDI-Pedalboard is a simple project which transforms a Raspberry Pi into a midi encoder for a 27 steps organ pedalboard.
## Installation
### Harware
You need a 27 steps pedalboard with a 12\*3 matrix. Check the 'pins' file to see which GPio you need to use, or set them if you're not satisfied. Note that the numbers are that used by the wiringPi library. There are 12 pins for the first line, 3 for the second one. In the first line, the first pin is the C, the second is the C#, the third the D, etc. In the second line, the first pin is the first octave, the second the second octave, etc. If the hardware do not match these specifications, see Re-use below. You can alse add a LED between pin 29 and a GND pin: the LED will be on when the encoder is ready.
### Software
1. Install a new Raspbian on a Raspberry Pi.
2. Download this package in /home/pi/
3. Check your internet connection, run installer.sh and answer questions.
4. Reboot the Pi: the encoder is ready to use.
## Use
1. Connect the Pi to your computer with the USB.
2. When the LED is on, you can start playing.
## Re-use
If you want to use this software for a keyboard or a pedalboard with more than 27 keys or steps, it's not a great deal to change some parameters.
Actually, the first step is to set the 'pins' file according to your matrix. For example, if you want a 8\*8 matrix, you can have this:
```
7 0 2 3 21 22 23 24
25 1 4 5 6 26 27 28
```
The second step is to set lines 8 and 9 in 'encoder.cpp'. Currently, these lines are:
```
constexpr unsigned int base_key = 36; // C2
constexpr unsigned int higher_key = 62; // D4
```
Just change the numbers. For example, for a 5 octave keyboard, just change line 9 as follows:
```
constexpr unsigned int higher_key = 96; // C7
```
Do not hesitate to contact us if necessary.

