
#ifndef HEADERS_COMMANDS_H_
#define HEADERS_COMMANDS_H_

#define COMM_AMOUNT 3

#include <inttypes.h>
#include <stdbool.h>

#include "stateMachine.h"

 typedef enum command {
    EMPTY_COMMAND = 0,
    EAT,
    PET,
    EXERCISE,
} command;

typedef enum {
    EMPTY_MSG = 0,
    SUNNY,
    DARK,
    HOT,
    COLD
} customMsg;

typedef struct {
    uint8_t eatAmount;
    uint8_t petAmount;
    uint8_t exerciseAmount;
    customMsg msg1ToSend;
    customMsg msg2ToSend;
    bool customMsgSent;
} commandStruct;

static inline const char* getCommandAsStr(command cmd) {
    static const char* commandStrings[] = {"", "EAT", "PET", "EXERCISE"};

    return commandStrings[cmd];
}

static inline const char* getCustomMsgAsStr(customMsg msg) {
    static const char* msgStrings[] = {"", "SUNNY", "DARK", "HOT", "COLD"};

    return msgStrings[msg];
}


extern commandStruct commandsToSend;


#endif /* HEADERS_COMMANDS_H_ */
