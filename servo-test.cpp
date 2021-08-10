#include "servo-test.h"
#include "servo-constants.h"

#define VEL 2
// Sweep servo across 180 degrees
// input servo number and address of servo board
void Tests::servoSweep(Adafruit_PWMServoDriver *pwm, int num) {
    pwm->setPWM(num, 0, SERVOMIN);
    delay(200);
    for (int i = SERVOMIN; i < SERVOMAX; i++) {
        pwm->setPWM(num, 0, i);
        delay(VEL);
    }
    delay(100);
    for (int i = SERVOMAX; i > SERVOMIN; i--) {
        pwm->setPWM(num, 0, i);
        delay(VEL);
    }
    delay(100);
}

// set servo at port 0 of servo board to angle and sweep when anything is received on serial
void Tests::basicTest(Adafruit_PWMServoDriver *pwm, int num) {
    if (Tests::serialButton()) {
        Tests::servoSweep(pwm, 0);
    }
    Tests::servoSetPos(pwm, 0, 90);
}

bool Tests::serialButton() {
    if (Serial.available() > 0) {
        Serial.write("recieved:");
        Serial.write(Serial.readString().c_str());
        return true;
    }
    return false;
}

// angle(*angle in degrees*) returns value for servo PWM
int angle(int deg) {
    return map(deg, 0, 180, SERVOMIN, SERVOMAX);
}

// set servo num to position deg
void Tests::servoSetPos(Adafruit_PWMServoDriver *pwm, int num, int deg) {
    pwm->setPWM(num, 0, angle(deg));
}



// Offset Calibration
// calibrate 1 leg, plug into ports 0 (outer), 1 (middle), 2 (inner)

// input servo port number
int tweakOffsets(Adafruit_PWMServoDriver* pwm, int num) {
    int base_angle;
    if (num == 2) {
        base_angle = 50;
    }
    else {
        base_angle = 90;
    }
    int offset = 0;
    String input = "";
    Serial.write("\"done\" when done\n");
    while (true) {
        pwm->setPWM(num, 0, angle(base_angle)+offset);
        while (Serial.available() == 0);
        String input = Serial.readString();
        if (input.startsWith("done")) break;
        if (input[0] == '-') {
            offset = (-1) * input.substring(1).toInt();
        }
        else {
            offset = input.toInt();
        }
        Serial.println(offset);
    }
    return offset;
}

void Tests::calibrateOffsets(Adafruit_PWMServoDriver* pwm) {
    int offsets[3] = {0, 0, 0};
    Serial.write("Offset Calibration\n-----------------------------------------------\n\n");
    String input = "";
    while (true) {
        int servoID;
        Serial.write("Input 0 for Inner, 1 for Middle, 2 for Outer, \"quit\" to quit\n");
        while (Serial.available() == 0);    // wait for input
        input = Serial.readString();        // choose servo
        if (input.startsWith("quit")) break;
        servoID = input.toInt();
        if (servoID > 2) servoID = 0;       // invalid port results in 0
        Serial.write("Calibrating Servo ");
        Serial.println(servoID);
        offsets[servoID] = tweakOffsets(pwm, servoID);
    }
    Serial.write("Inner: "); Serial.print(offsets[0]); Serial.write(" ");
    Serial.write("Middle: "); Serial.print(offsets[1]); Serial.write(" ");
    Serial.write("Outer: "); Serial.print(offsets[2]); Serial.write(" ");
}
