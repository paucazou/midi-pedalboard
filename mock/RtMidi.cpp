#include "RtMidi.h"

int RtMidiOut::getPortCount() const {
    constexpr int port_nb = 1;
    std::cout << "Port count called: " << port_nb << "\n";

    return port_nb;
}


void RtMidiOut::openPort(int i) {
    std::cout << "Port opened: " << i << "\n";
}

void RtMidiOut::sendMessage(const std::vector<unsigned char>* message) {
    std::cout << "Message sent: ";
    for (const auto& elt : *message)
        std::cout << static_cast<int>(elt) << "_";
    std::cout << "\n";
}

