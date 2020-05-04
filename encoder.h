#ifndef ENCODER_H
#define ENCODER_H

#include <array>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>


// types
struct NoteStatus {
    const unsigned int key;
    bool status = false;
};
using status_array = std::vector<NoteStatus>;
using array12 = std::array<uint_fast8_t,12>;
using array3 = std::array<uint_fast8_t,3>;

// security

void write_time();

void check_valid_pin(const uint_fast8_t pin);

void throw_and_flush();

template <typename T>
void check_duplicates (const T& arr) {
    std::ofstream error_file {"/home/pi/error_file",std::ofstream::app};
    for (size_t i=0;i<arr.size();++i) {
        for (size_t j=0; j<arr.size();++j) {
            if (j == i)
                continue;
            if (arr[j] == arr[j]){
                write_time();
                error_file << "Please check the pins: " << static_cast<int>(arr[j]) << " is used more than one time.\n";
                std::cerr << "Pins error\n";
                error_file.flush();
                    throw 1;
            }
        }
    }

}

// setup

std::tuple<array12,array3> get_pins();
uint_fast8_t get_channel();
void set_output_pins(const array12&);
void light_on();
status_array get_notes_status_array();

// main loop
void send_message(uint_fast8_t, uint_fast8_t);
void loop( const array12& lines, const array3& columns, const uint_fast8_t channel, status_array& key_array);




#endif
