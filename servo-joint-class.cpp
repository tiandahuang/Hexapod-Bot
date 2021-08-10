#include "servo-joint-class.h"
#include "servo-constants.h"

Joint::Joint(Adafruit_PWMServoDriver *board, uint8_t pin, bool flipped, JointType jtype, int8_t offset) :
    board(board), pin(pin), flipped(flipped), jtype(jtype), offset(offset) {
        switch (jtype) {
            case INNER :
                initpos = 90; lowlimit = 30; highlimit = 150;
                break;
            case MIDDLE :
                initpos = 10; lowlimit = 0; highlimit = 135;
                break;
            case OUTER :
                initpos = 20; lowlimit = 10; highlimit = 130;
                break;
        }
        pos = initpos;
    }

// angle(*angle in degrees*) returns value for servo PWM
int angleToPulseWidth(int deg, bool flipped) {
    if (flipped) {
        return map(deg, 0, 180, SERVOMAX, SERVOMIN);
    }
    else {
        return map(deg, 0, 180, SERVOMIN, SERVOMAX);
    }
}


// Set servo angle, input degrees
// Does not move servo
void Joint::setAngle(uint8_t angle) {
    if (angle < lowlimit) pos = lowlimit;
    else if (angle > highlimit) pos = highlimit;
    else pos = angle;    
}

// Move servo to stored position
void Joint::setPos() {
    board->setPWM(pin, 0, angleToPulseWidth(pos, flipped) + offset);
}

// Reset servo to starting position
void Joint::resetPos() {
    pos = initpos;
    Joint::setPos();
}
