#include "encoder.h"
#include <cstdlib>
#include <ctime>
#include "RtMidi.h"
#include "wiringPi.h"
// Deus in adjutorium meum intende
// Soli Deo Gloria
constexpr unsigned int base_key = 36; // C2
constexpr unsigned int higher_key = 62; // D4
std::ofstream error_file {"/home/pi/error_file",std::ofstream::app};

// security

void write_time() {
    /* write current time on error_file
     */
    std::time_t result = std::time(nullptr);
    error_file << std::asctime(std::localtime(&result));
    error_file.flush();
}

void check_valid_pin(const uint_fast8_t pin) {
    /* Check that pin exists and can be used
     */
    if ((pin > 7 && pin < 21 )|| pin > 28) {
        error_file << "Invalid pin: " << pin << "\n";
        std::cerr << "Invalid pin: " << pin << "\n";
        throw_and_flush();
    }
}

void throw_and_flush() {
    /* Write date and time,
     * flush the error file
     * throw
     */
    write_time();
    error_file.flush();
    throw 1;
}
// setup

std::tuple<array12,array3> get_pins() {
    /* Get the pins and return arrays containing their numbers
     */
#if defined DEBUG && defined MOCK
    return {
        {7,0,2, 3,21,22, 23,24,25, 1,4,5 },
        {6, 26, 27}
    };
#endif 
    std::ifstream pins_file ("/home/pi/pins");
    // check file
    if (pins_file.fail()){
        error_file << "Invalid file\n";
        std::cerr << "Invalid file\n";
        throw_and_flush();
    }

    char pin_nb[3];
    array12 lines;
    for (int i=0;i<11;++i) {
        pins_file.getline(pin_nb,3,' ');
        lines[i] = std::atoi(pin_nb);
#ifdef DEBUG
        std::cout << i << " " << pin_nb << "\n";
#endif
    }
    // we need to add the last line
    pins_file.getline(pin_nb,3,'\n');
    lines[11] = std::atoi(pin_nb);

    array3 columns;
    for (int i=0;i<3;++i) {
        pins_file.getline(pin_nb,3,' ');
        columns[i] = std::atoi(pin_nb);
#ifdef DEBUG
        std::cout << i << " " << pin_nb << "\n";
#endif
    }
#ifdef DEBUG
    std::cout << "Pins lines: " ;
    for (const auto& elt : lines)
        std::cout << static_cast<int>(elt) << " ";
    std::cout << "\n";
    std::cout << "Pins columns: ";
    for (const auto& elt : columns)
        std::cout << static_cast<int>(elt) << " ";
    std::cout << "\n";
#endif
    // check validity of pins
    for (const auto& elt : lines)
        check_valid_pin(elt);
    for (const auto& elt : columns)
        check_valid_pin(elt);
    // check no duplicate pins
    for (const auto& lelt : lines) {
        for (const auto& celt : columns) {
            if (lelt == celt) {
                error_file << "Please check the pins: " << celt << " is used as line and column at the same time\n";
                std::cerr << "Please check the pins: " << celt << " is used as line and column at the same time\n";
            throw_and_flush();
            }
        }
    }
    check_duplicates(lines);
    check_duplicates(columns);

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
        error_file << "Channel can not be set correctly from file. Please check that the 'channel' file contains an integer between 0 and 15.\nDefault channel will be used: 0.";
        write_time();
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

void set_input_pins(const array3& columns) {
    /* set lines as inputs
     * (although this is the default value,
     * we do that by security)
     * and set the pull down resistor
     * to avoid bizarre results
     */
    for (const auto& c : columns) {
        pinMode(c,INPUT);
        pullUpDnControl(c,PUD_DOWN);
    }
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
            error_file << "No port available.\n";
            throw_and_flush();
        }
        midiout.openPort(1); // it seems that it's always the good one: port 0 is MIDI Through
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
#if 1
            std::cout << "Test key: " << key.key << " Status: " << std::boolalpha << key.status << "\n";
            std::cout << "Current column: " << static_cast<int>(columns[j]) << " Current status: " << std::boolalpha << status << "\n";
#endif
#endif
            if (status != key.status) {
                key.status = ! key.status;
                const auto signal = (status?NOTE_ON:NOTE_OFF);
                send_message(signal,key.key);
#ifdef DEBUG
                std::cout << "Sent message: " << (signal==NOTE_ON?"Note_On":"Note_Off") << ":"<<key.key<<"\n";
                std::cout << "Current column: " << static_cast<int>(columns[j]) << " Current line: " << static_cast<int>(lines[i]);
#endif
            }
            current_key += lines.size();
        }
        ++sub_base_key;
        digitalWrite(lines[i],LOW);
#ifdef DEBUG
        std::cout << "Line: " << static_cast<int>(lines[i]) << ": " << std::boolalpha << digitalRead(lines[i]) << std::endl;
#endif


    }
#ifdef DEBUG
    std::cout << std::endl;
#endif
} while (LOOP_NB);
#undef LOOP_NB
}






