#ifndef SERVO_TEST_H
#define SERVO_TEST_H

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

namespace Tests
{
    void servoSweep(Adafruit_PWMServoDriver *pwm, int num);
    bool serialButton();
    void servoSetPos(Adafruit_PWMServoDriver *pwm, int num, int deg);
    void basicTest(Adafruit_PWMServoDriver *pwm, int num);
    void calibrateOffsets(Adafruit_PWMServoDriver *pwm);
};

#endif // SERVO_TEST_H