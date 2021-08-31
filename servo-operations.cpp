#include "servo-operations.h"
#include "servo-joint-class.h"
#include "servo-constants.h"
#include "timer2.h"
#include "movement-sequence.h"

// Servo breakout boards
Adafruit_PWMServoDriver pwm_L = Adafruit_PWMServoDriver();
Adafruit_PWMServoDriver pwm_R = Adafruit_PWMServoDriver(0x41);

// Joint initialization

Joint Leg1_Inn(&pwm_R, 7, false, Joint::INNER,  5  );
Joint Leg1_Mid(&pwm_R, 8, false, Joint::MIDDLE, -2 );
Joint Leg1_Out(&pwm_R, 9, false, Joint::OUTER,  -7 );

Joint Leg2_Inn(&pwm_R, 10, false, Joint::INNER,  -10);
Joint Leg2_Mid(&pwm_R, 11, false, Joint::MIDDLE, 7  );
Joint Leg2_Out(&pwm_R, 12, false, Joint::OUTER,  13 );

Joint Leg3_Inn(&pwm_R, 13, false, Joint::INNER,  -10);
Joint Leg3_Mid(&pwm_R, 14, false, Joint::MIDDLE, 1  );
Joint Leg3_Out(&pwm_R, 15, false, Joint::OUTER,  -8 );

Joint Leg4_Inn(&pwm_L, 2, true, Joint::INNER,   -5 );
Joint Leg4_Mid(&pwm_L, 1, true, Joint::MIDDLE,  9  );
Joint Leg4_Out(&pwm_L, 0, true, Joint::OUTER,   -8 );

Joint Leg5_Inn(&pwm_L, 5, true, Joint::INNER,   -8 );
Joint Leg5_Mid(&pwm_L, 4, true, Joint::MIDDLE,  9  );
Joint Leg5_Out(&pwm_L, 3, true, Joint::OUTER,   12 );

Joint Leg6_Inn(&pwm_L, 8, true, Joint::INNER,   -25);
Joint Leg6_Mid(&pwm_L, 7, true, Joint::MIDDLE,  12 );
Joint Leg6_Out(&pwm_L, 6, true, Joint::OUTER,   -3 );

// Joint pointer access list
Joint *JointList[18] = {
    &Leg1_Inn, &Leg1_Mid, &Leg1_Out, 
    &Leg2_Inn, &Leg2_Mid, &Leg2_Out, 
    &Leg3_Inn, &Leg3_Mid, &Leg3_Out, 
    &Leg4_Inn, &Leg4_Mid, &Leg4_Out, 
    &Leg5_Inn, &Leg5_Mid, &Leg5_Out, 
    &Leg6_Inn, &Leg6_Mid, &Leg6_Out, 
};

// Input commands
#define NUMCOMMANDS 7
const char Cmd_0[] PROGMEM = "M";
const char Cmd_1[] PROGMEM = "J";
const char Cmd_2[] PROGMEM = "Sequence";
const char Cmd_3[] PROGMEM = "Analysis";
const char Cmd_4[] PROGMEM = "Put yourself away";
const char Cmd_5[] PROGMEM = "Freeze all motor functions";  // 27 chars
const char Cmd_6[] PROGMEM = "Continue";

// Input command pointer access list
const char* const CmdTable[] PROGMEM = {Cmd_0, Cmd_1, Cmd_2, Cmd_3, Cmd_4, Cmd_5, Cmd_6};

// Serial input buffer, stored as null-terminated c-string
static char InputBuffer[30];

// Buffer for input comparison
static char CompareBuffer[30];

// Buffer for loading movement sequence, volatile since it will be modified by ISR
static volatile jointMove SequenceBuffer;

// Current movement sequence number and ID;
static volatile uint8_t SequenceNum, SequenceID;

// Verbosity
bool VerboseMode = false;

void pwmInit() {
    pwm_L.begin();
    pwm_L.setOscillatorFrequency(27000000);
    pwm_L.setPWMFreq(SERVO_FREQ);
    pwm_R.begin();
    pwm_R.setOscillatorFrequency(27000000);
    pwm_R.setPWMFreq(SERVO_FREQ);

    timer2Init(249, sequenceLoadNext);
}

void updateServoPos() {
    for (uint8_t i = 0; i < 18; i++) {
        JointList[i]->setPos();
    }
}

/**------------------------------------------------
 * Commands and command helper functions start here
 */

// Input character array buffer, outputs unsigned integer representation (max 255)
uint8_t wordToInt(const char* &input) {
    uint8_t num = 0;
    while ((*input != ' ') && (*input != '\0')) {
        if (*input >= '0' && *input <= '9') {
            num = 10 * num + (*input - '0');
        }
        input++;
    }
    input++;
    return num;
}

// Input servo ID followed by angle ("M## ###")
void cmdAngleAscii(const char* input, bool mirrored) {
    uint8_t num = wordToInt(input);
    uint8_t angle = wordToInt(input);

    if (VerboseMode) {
        Serial.print(F("Servo ID: ")); Serial.print(num); Serial.print(F(", "));    // optional, diagnostics only
        Serial.print(mirrored ? F("Mirrored") : F("Single")); Serial.print(F(", "));// optional, diagnostics only
        Serial.print(F("Angle: ")); Serial.println(angle);                          // optional, diagnostics only
    }
    if (num >= 0 && num < 18) {    // do nothing with invalid servo ID
        if (mirrored) {
            for (int i = 0; i < 18; i++) {
                if ((i % 3) == (num % 3)) JointList[i]->setAngle(angle);
            }
        }
        else {
            JointList[num]->setAngle(angle);
        }
    }
    else if (VerboseMode) {
        Serial.println(F("Error, invalid Servo ID"));                           // optional, diagnostics only
    }
}

// Run in ISR
// Loads the next servo position but does not actually move the servo
void sequenceLoadNext() {
    fetchMove(const_cast<jointMove*>(&SequenceBuffer), SequenceID, SequenceNum);
    loadDelay(SequenceBuffer.delay);
    JointList[SequenceBuffer.num]->setAngle(SequenceBuffer.angle);
    if (VerboseMode) {  // This runs in ISR so VerboseMode will clutter CPU time
        Serial.print(F("[num, angle, delay] : "));
        Serial.print(SequenceBuffer.num); Serial.print(", ");
        Serial.print(SequenceBuffer.angle); Serial.print(", ");
        Serial.println(SequenceBuffer.delay);
    }
    // TODO Add Stop Sequence
    SequenceNum++;
}

// Starts a movement sequence by arming interrupts and setting the sequence number
void cmdStartSequence() {
    SequenceNum = 0;    // TODO string input switch
    SequenceID = 0;
    timer2Arm();
}

// Unsafe quick reset to initial positions
void resetServoPos() {
    if (VerboseMode) {
        Serial.println(F("Resetting Position"));
    }
    for (uint8_t i = 0; i < 18; i++) {
        JointList[i]->resetPos();
    }
    updateServoPos();
}

// Freeze all motor functions (spin until "Continue" input)
void cmdFreezeAllMotorFunctions() {
    if (VerboseMode) {
        Serial.println(F("PAUSE, \"Continue\" to resume"));
    }
    timer2Disarm();
    while (serialInputAndParse() != 6);
    timer2Arm();
}

/**------------------------------------------------
 * End commands section
 */

// Returns true if input string starts with comparison string (c-string inputs)
bool strStartsWith(const char* input, const char* compare) {
    uint8_t i = 0;
    while (compare[i] != '\0') {
        if (input[i] != compare[i]) return false;
        i++;
    }
    return true;
}

// Input command ID, performs the command or calls command function
void commandSwitch(uint8_t command_num) {
    switch (command_num) {
        case 0 :    // "M## ###", single input servo control
            cmdAngleAscii(InputBuffer, false);
            break;
        case 1 :    // "J# ###", joint mirrored servo control
            cmdAngleAscii(InputBuffer, true);
            break;
        case 2 :    // "Sequence ...", begin scripted movement sequence
            cmdStartSequence();
            break;
        case 3 :    // "Analysis", verbose mode, echo all commands
            VerboseMode = true;
            Serial.println(F("Verbose Mode"));
            break;
        case 4 :    // "Put yourself away", reset position
            resetServoPos();
            break;
        case 5 :    // "Freeze all motor functions"
            cmdFreezeAllMotorFunctions();
            break;
        case 6 :    // "Continue", end verbose or freeze
            VerboseMode = false;
            break;
        default :   // unrecognized command
            break;
    }
}

// Serial input into char array, does nothing to buffers and returns 255 if Serial is empty
// Calls the command and returns the command number
uint8_t serialInputAndParse() {
    if (Serial.available() > 0) {
        uint8_t lf = Serial.readBytesUntil('\0', InputBuffer, 29);
        InputBuffer[lf] = '\0';

        uint8_t command_num = 0;
        for (command_num; command_num < NUMCOMMANDS; command_num++) {
            #ifndef __INTELLISENSE__
            strcpy_P(CompareBuffer, (char *)pgm_read_word(&(CmdTable[command_num])));
            #endif  // because Intellisense does not like pgm_read_word
            if (strStartsWith(InputBuffer, CompareBuffer)) {
                break;
            }
        }
        if (VerboseMode) {
            Serial.print(F("CMD NUM: ")); Serial.println(command_num);
        }
        // 'command_num' will be 0...NUMCOMMANDS, command_num=NUMCOMMANDS means unknown command
        return command_num;
    }
    else {
        return 255;
    }
}

void inputCommand() {
    uint8_t command_num = serialInputAndParse();
    commandSwitch(command_num);
}
