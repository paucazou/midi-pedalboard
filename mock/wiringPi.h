#ifndef WIRING_PI_H
#define WIRING_PI_H
#include <iostream>
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
#define PUD_DOWN 0

/* This is a mock file
 */

void pinMode(int pin, int mode);

void pullUpDnControl (int pin, int pud);

void digitalWrite(int pin, int value) ;

int digitalRead(int pin);

int wiringPiSetup();



#endif

