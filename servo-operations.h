#ifndef SERVO_OPERATIONS_H
#define SERVO_OPERATIONS_H

#include <stdint.h>

void pwmInit();
void updateServoPos();
void inputCommand();
void commandSwitch(uint8_t command_num); // avoid commands that read input buf

uint8_t serialInputAndParse();
void cmdAngleAscii(const char* input, bool mirrored);
void resetServoPos();
void cmdFreezeAllMotorFunctions();

#endif // SERVO_OPERATIONS_H