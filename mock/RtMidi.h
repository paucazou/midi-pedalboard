#ifndef RTMIDI_H
#define RTMIDI_H
#include <iostream>
#include <vector>

/* This is a mock header created to
 * compile and test the encoder on a regular
 * PC
 */
#define OUTPUT 1

struct RtMidiOut {
    int getPortCount() const;


    void openPort(int i) ;

    void sendMessage(const std::vector<unsigned char>* message);
};


#endif
