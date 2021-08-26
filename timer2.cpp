#include "timer2.h"

void (*functionCall)();
/** 
 * Set up Timer2 compare interrupt, but does not arm the interrupt
 * Input uint8_t for the compare value
 * load value [16,000,000/(prescaler * freq)] - 1 +- error
 * TODO: Input void function pointer for execution on interrupt
 */
void timer2Init(uint8_t load, void (*func)()) {
    noInterrupts();
    #ifndef __INTELLISENSE__
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    TIMSK2 = 0;
    bitSet(TCCR2A, WGM21);  // CTC Mode
    bitSet(TCCR2B, CS22);   // TCCR2B CS22/21/20 sets prescaler [100] is 64
    OCR2A = load;   // compare register
    #endif
    functionCall = func;
    interrupts();
}

// Arm interrupts
void timer2Arm() {
    #ifndef __INTELLISENSE__
    bitSet(TIMSK2, OCIE2A);
    #endif
}

// Disarm interrupts
void timer2Disarm() {
    #ifndef __INTELLISENSE__
    bitClear(TIMSK2, OCIE2A);
    #endif
}

static volatile uint32_t ms = 0xFFFFFFFF;   // initialize big
void loadDelay(uint32_t load) {
    ms = load;
}

ISR(TIMER2_COMPA_vect) {
    // Serial.println("pog");
    ms--;
    if (ms == 0) functionCall();
}