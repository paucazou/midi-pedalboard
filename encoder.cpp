#include "encoder.h"
#include <cstdlib>
#include <fstream>
#include "RtMidi.h"
#include "wiringPi.h"
constexpr unsigned int base_key = 36; // C2
constexpr unsigned int higher_key = 62; // D4

// setup
std::tuple<array12,array3> get_pins() {
    /* Get the pins and return arrays containing their numbers
     */
    //TODO check the validity of the pins
#ifdef DEBUG
    return {
        {8,9,7, 0,2,3, 12,13,14, 21,22,23 },
        {15,16,1}
    };
#endif 
    std::ifstream pins_file ("/home/pi/pins");
    char pin_nb[2];
    array12 lines;
    for (int i=0;i<=12;++i) {
        pins_file.getline(pin_nb,2,' ');
        lines[i] = std::atoi(pin_nb);
    }

    array3 columns;
    for (int i=0;i<=3;++i) {
        pins_file.getline(pin_nb,2,' ');
        columns[i] = std::atoi(pin_nb);
    }
    return {lines, columns};
}

uint_fast8_t get_channel() {
    /* Return the channel selected by user
     */
#ifdef DEBUG
    return 1;
#endif
    std::ifstream channel_file("/home/pi/channel");
    uint_fast8_t channel;
    channel_file >> channel;
    return channel;
}

void set_output_pins(const array12& lines) {
    // set lines as outputs
    for (const auto& line : lines)
        pinMode(line,OUTPUT);
}

void light_on () {
    /* light on the LED
     */
    constexpr int LED_PIN = 29;
    pinMode(LED_PIN,OUTPUT);
    digitalWrite(LED_PIN,HIGH);
}

status_array get_notes_status_array() {
    /* Return the notes status array
     */
    constexpr int lines{12}, columns{3};
    constexpr size_t notes_nb { 27 };
    status_array array;
    array.reserve(notes_nb);

    unsigned int current_key = base_key;
    unsigned int sub_base_key = base_key;

    for (int i=0;i<lines;++i) {
        current_key = sub_base_key;
        for (int j=0;j<columns && current_key <= higher_key;++j) {
            array.push_back({current_key,false});
            current_key += lines;
        }
        ++sub_base_key;
    }
#ifdef DEBUG
    std::cout << "ID | Key | Value\n";
    for (size_t i=0; i < array.size(); ++i) {
        const auto&elt = array.at(i);
        std::cout << i << " | " << elt.key << " | " << std::boolalpha << elt.status << "\n";
    }
#endif 
    return array;
}

// main loop

void send_message(uint_fast8_t value, uint_fast8_t key) {
    /* Send a midi message
     * value must be the note on or off followed by the channel
     */
    static RtMidiOut midiout;
    // the message is required to be a vector of uchar. It is set to 3 elements with value zero
    static std::vector<unsigned char> message (3,0); 
    static bool check_done { false };

    if (! check_done) {

        // checks
        if (midiout.getPortCount() == 0) {
            // TODO logs raise error
        }
        midiout.openPort(1); // it seems that it's always the good one: port 0 is MIDI Trhough
        check_done = true;

    }

    message[0] = value;
    message[1] = key;
    midiout.sendMessage(&message);
}

void loop( const array12& lines, const array3& columns, const uint_fast8_t channel, status_array& key_array) {
    constexpr int NOTE_ON_BASE = 0b1001'0000;
    constexpr int NOTE_OFF_BASE = 0b1000'0000;
    const unsigned char NOTE_ON = NOTE_ON_BASE + channel;
    const unsigned char NOTE_OFF = NOTE_OFF_BASE + channel;
#ifndef LOOP_NB
#define LOOP_NB true
#endif
do {
    unsigned int current_key = base_key;
    unsigned int sub_base_key = base_key;
    for (size_t i=0, idx=0; i<lines.size(); ++i) {
        current_key = sub_base_key;
        digitalWrite(lines[i],HIGH);
        for (size_t j=0; j < columns.size() && current_key <= higher_key; ++j,++idx) {
            const bool status = digitalRead(columns[j]);
            auto& key {key_array[idx]};
#ifdef DEBUG
            std::cout << "Test key: " << key.key << " Status: " << std::boolalpha << key.status << "\n";
#endif
            if (status != key.status) {
                key.status = ! key.status;
                const auto signal = (status?NOTE_ON:NOTE_OFF);
                send_message(signal,key.key);
            }
            current_key += lines.size();
        }
        ++sub_base_key;
        digitalWrite(lines[i],LOW);


    }
} while (LOOP_NB);
#undef LOOP_NB
}






