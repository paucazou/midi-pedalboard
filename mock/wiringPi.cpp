#include "wiringPi.h"
#include <string>

void pinMode(int pin, int mode) {
    std::cout << "pinMode. Pin: " << pin << " Mode: " << (mode == OUTPUT ? "Output" : "Not recognized") << "\n";

}

void pullUpDnControl (int pin, int pud) {
    std::string val;
    if (pud == PUD_DOWN)
        val = "PUD_DOWN";
    else
        val = "NOT RECOGNIZED";
    std::cout << "pullUpDnControl. Pin: " << pin << " Value: " << val << "\n";
}

void digitalWrite(int pin, int value) {
    std::cout << "digitalWrite. Pin: " << pin << " Value: " << (value == LOW ? "Low":"High") << "\n";
}

int digitalRead(int pin) {
    std::cout << "digitalRead. Pin: " << pin << "\n";
    if (pin == 6)
        return 1;
    return 0;
}

int wiringPiSetup() {
    return 1;
}
