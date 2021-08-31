#ifndef MOVEMENT_SEQUENCE_H
#define MOVEMENT_SEQUENCE_H

#include <Arduino.h>

struct jointMove {
    uint8_t num;
    uint8_t angle;
    uint16_t delay;
};

void fetchMove(jointMove *buffer, uint8_t sequence_id, uint8_t sequence_num);

#endif // MOVEMENT_SEQUENCE_H