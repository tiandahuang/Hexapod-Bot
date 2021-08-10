
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN 100
#define SERVOMAX 460
#define SERVOMID 280
#define SERVO_FREQ 50

void setup() {
    Serial.begin(9600);
    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);
    delay(50);
}

#define VEL 2
void servoSweep(int num) {
    pwm.setPWM(num, 0, SERVOMIN);
    delay(200);
    for (int i = SERVOMIN; i < SERVOMAX; i++) {
        pwm.setPWM(num, 0, i);
        delay(VEL);
    }
    delay(100);
    for (int i = SERVOMAX; i > SERVOMIN; i--) {
        pwm.setPWM(num, 0, i);
        delay(VEL);
    }
    delay(100);
}

int serialButton() {
    if (Serial.available() > 0) {
        Serial.write("recieved:");
        Serial.write(Serial.readString().c_str());
        return 1;
    }
    return 0;
}

// angle(*angle in degrees*) returns value for servo PWM
int angle(int deg) {
    return map(deg, 0, 180, SERVOMIN, SERVOMAX);
}

// set servo num to position deg
void servo(int num, int deg) {
    pwm.setPWM(num, 0, angle(deg));
}

void loop() {
    if (serialButton()) {
        for (int i = 0; i < 5; i++) {
            servoSweep(i); servoSweep(i);
        }
    }
    servo(0, 90);
    servo(1, 90);
    servo(2, 180);
    servo(3, 180);
    servo(4, 180);
}