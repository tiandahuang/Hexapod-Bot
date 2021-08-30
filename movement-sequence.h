#ifndef MOVEMENT_SEQUENCE_H
#define MOVEMENT_SEQUENCE_H

#include <Arduino.h>

struct jointMove {
    uint8_t num;
    uint8_t angle;
    uint16_t delay;
};

void fetchMove(volatile jointMove *buffer, int sequence_id, int sequence_num);

#endif // MOVEMENT_SEQUENCE_H