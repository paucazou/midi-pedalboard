#ifndef ENCODER_H
#define ENCODER_H

#include <array>
#include <cstdint>
#include <iostream>
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
