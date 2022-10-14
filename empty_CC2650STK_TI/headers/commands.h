
#ifndef HEADERS_COMMANDS_H_
#define HEADERS_COMMANDS_H_

 typedef enum command {
    EMPTY = 0,
    EAT,
    PET,
    EXERCISE
} command;

static inline const char* getCommandAsStr(command cmd) {
    static const char* commandStrings[] = {"", "EAT", "PET", "EXERCISE"};

    return commandStrings[cmd];
}


extern command commandToSend;

#endif /* HEADERS_COMMANDS_H_ */
