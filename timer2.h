/** 
 * Arduino built-in micros, millis(), delay(), all use Timer0 overflow trigger
 * Timer2 is set to interrupt on comparison trigger for tighter timings
 * Probably breaks tone()
 * Run this in setup() to override Arduino default settings
 */ 

#ifndef TIMER2_H
#define TIMER2_H

#include "Arduino.h"

void timer2Init(uint8_t load, void (*func)());
void timer2Arm();
void timer2Disarm();
void loadDelay(uint32_t load);

#endif // TIMER2_H