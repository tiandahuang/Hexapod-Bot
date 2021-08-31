#include "movement-sequence.h"

// terminate sequences with {255, 255, 0}
const jointMove Seq0[] PROGMEM = 
{ // servo number, desired angle, delay before move (in ms)
    {0, 90, 100},
    {255, 255, 0}
};

// sequence pointer list
const jointMove* const SequenceTable[] PROGMEM = {Seq0};

// Fetch a single move and place into buffer
// Input the sequence id and the number of the move
// TODO: move this over to servo-operations.cpp and add servo set angle functionality, maybe phase out movement-sequence.cpp entirely
void fetchMove(jointMove *buffer, uint8_t sequence_id, uint8_t sequence_num) {
    memcpy_P(buffer, &(SequenceTable[sequence_id][sequence_num]), sizeof(jointMove));
}