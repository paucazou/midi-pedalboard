#include "encoder.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include "RtMidi.h"
#include "wiringPi.h"
constexpr unsigned int base_key = 36; // C2
constexpr unsigned int higher_key = 62; // D4
std::ofstream error_file {"/home/pi/error_file",std::ofstream::app};

// security

void write_time() {
    /* write current time on error_file
     */
    std::time_t result = std::time(nullptr);
    error_file << std::asctime(std::localtime(&result));
}

void check_valid_pin(const uint_fast8_t pin) {
    /* Check that pin exists and can be used
     */
    if (pin > 16 && pin < 21) {
        write_time();
        error_file << "Invalid pin: " << pin << "\n";
        throw 1;
    }
}
// setup

std::tuple<array12,array3> get_pins() {
    /* Get the pins and return arrays containing their numbers
     */
#if defined DEBUG && defined MOCK
    return {
        {8,9,7, 0,2,3, 12,13,14, 21,22,23 },
        {15,16,1}
    };
#endif 
    std::ifstream pins_file ("/home/pi/pins");
    // check file
    if (pins_file.fail()){
        write_time();
        error_file << "Invalid file\n";
        throw 1;
    }

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
    // check validity of pins
    for (const auto& elt : lines)
        check_valid_pin(elt);
    for (const auto& elt : columns)
        check_valid_pin(elt);
    // check no duplicate pins
    for (const auto& lelt : lines)
        for (const auto& celt : columns)
            if {lelt == celt} {
                write_time();
                error_file << "Please check the pins: " << celt << " is used as line and column at the same time\n";
            }
    check_duplicates(lines);
    check_duplicates(columns);

#ifdef DEBUG
    std::cout << "Pins lines: " ;
    for (const auto& elt : lines)
        std::cout << elt << " ";
    std::cout << "\n";
    std::cout << "Pins columns: ";
    for (const auto& elt : columns)
        std::cout << elt << " ";
    std::cout << "\n";
#endif
    return {lines, columns};
}

uint_fast8_t get_channel() {
    /* Return the channel selected by user
     * in case of error, channel selected is zero
     */
#if defined DEBUG && defined MOCK
    return 1;
#endif
    std::ifstream channel_file("/home/pi/channel");
    uint_fast8_t channel;
    channel_file >> channel;
    // no need to check if channel is under 15 since channel is unsigned
    if (channel_file.fail() || channel > 15) {
        return 0;
    }
#ifdef DEBUG
    std::cout << "Channel selected : " << channel << "\n";
#endif
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
    // TODO if necessary, make this function constexpr
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






