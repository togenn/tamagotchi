
#ifndef HEADERS_COMMANDS_H_
#define HEADERS_COMMANDS_H_

 typedef enum command {
    EMPTY = 0,
    EAT,
    PET,
    EXERCISE
} command;

const char* strings[] = {"", "EAT", "PET", "EXERCISE"};

extern command commandToSend;


#endif /* HEADERS_COMMANDS_H_ */
