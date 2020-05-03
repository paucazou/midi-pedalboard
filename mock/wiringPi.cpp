#include "wiringPi.h"

void pinMode(int pin, int mode) {
    std::cout << "pinMode. Pin: " << pin << " Mode: " << (mode == OUTPUT ? "Output" : "Not recognized") << "\n";

}

void digitalWrite(int pin, int value) {
    std::cout << "digitalWrite. Pin: " << pin << " Value: " << (value == LOW ? "Low":"High") << "\n";
}

int digitalRead(int pin) {
    std::cout << "digitalRead. Pin: " << pin << "\n";
    return 0;
}

int wiringPiSetup() {
    return 1;
}
