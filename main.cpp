#include "encoder.h"
#include "wiringPi.h"
#define WIRINGPI_CODES

int main() {
    // setup
    const auto [lines, columns] { get_pins() };
    const uint_fast8_t channel { get_channel() };
    if (wiringPiSetup()<0) {
        // print some logs
        return 1;
    }
    set_output_pins(lines);
    set_input_pins(columns);
    light_on();
    auto key_array { get_notes_status_array() };

    // main loop
    loop(lines, columns, channel, key_array);


    return 0;
}
