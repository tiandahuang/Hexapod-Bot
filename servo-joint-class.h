#ifndef SERVO_JOINT_CLASS_H
#define SERVO_JOINT_CLASS_H

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

class Joint
{
public: 
    enum JointType : uint8_t {INNER, MIDDLE, OUTER};
    
    Joint(Adafruit_PWMServoDriver *board, uint8_t pin, bool flipped, JointType jtype, int8_t offset);
    
    void setAngle(uint8_t angle);
    void setPos();
    void resetPos();
private:
    Adafruit_PWMServoDriver *board;
    uint8_t pin;
    bool flipped;
    JointType jtype;
    uint8_t pos, initpos, lowlimit, highlimit;
    int8_t offset;
};

#endif // SERVO_JOINT_CLASS_H