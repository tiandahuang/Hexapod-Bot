#include "servo-operations.h"
#include "servo-test.h"

void setup() {
    Serial.begin(115200);
    pwmInit();
    delay(50);

    commandSwitch(3); // Verbose on

    // Tests::calibrateOffsets(&pwm_R);
}

void loop() {
    inputCommand();
    updateServoPos();
}