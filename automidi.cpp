#include "RtMidi.h"
#include <iostream>
#include <vector>

int main () {
    RtMidiOut midiout;
    if (midiout.getPortCount() == 0) {
        std::cerr << "No port available" << std::endl;
        return 1;
    }
    midiout.openPort(1);
    std::vector<unsigned char> message (3,0);
    message.at(0) = 0b1001'0000; // Note ON channel 0
    message.at(1) = 60; // C4 (Central C)
    message.at(2) = 100; // velocity
    midiout.sendMessage(&message);

    message.at(0) = 0b1000'0000; // Note OFF channel 0
    message.at(1) = 60; // C4 (Central C)
    message.at(2) = 0; // velocity
    midiout.sendMessage(&message);
    std::cout << "Messages sent" << std::endl;
    return 0;
}



