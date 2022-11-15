
#ifndef HEADERS_COMMANDS_H_
#define HEADERS_COMMANDS_H_

#define COMM_AMOUNT 3

#include "stateMachine.h"

 typedef enum command {
    EMPTY_COMMAND = 0,
    EAT,
    PET,
    EXERCISE,
} command;

typedef enum msg1 {
    EMPTY_MSG1=0,
    SUNNY,
    DARK
} msg1;

typedef enum msg2 {
    EMPTY_MSG2=0,
    HOT,
    COLD
} msg2;

static inline const char* getCommandAsStr(command cmd) {
    static const char* commandStrings[] = {"", "EAT", "PET", "EXERCISE"};

    return commandStrings[cmd];
}

static inline const char* getMsg1AsStr(command cmd) {
    static const char* msg1Strings[] = {"", "SUNNY", "DARK"};

    return msg1Strings[cmd];
}

static inline const char* getMsg2AsStr(command cmd) {
    static const char* msg2Strings[] = {"", "HOT", "COLD"};

    return msg2Strings[cmd];
}


extern command commandsToSend[COMM_AMOUNT];
extern msg1 msg1ToSend;
extern msg2 msg2ToSend;

#endif /* HEADERS_COMMANDS_H_ */
