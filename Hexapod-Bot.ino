#include "servo-operations.h"
#include "timer2.h"

void heartbeat() {
    digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 0x1);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    pwmInit();
    delayMicroseconds(50000);

    commandSwitch(3); // Verbose on
}

void loop() {
    heartbeat();
    inputCommand();
    updateServoPos();
}