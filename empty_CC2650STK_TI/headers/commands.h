
#ifndef HEADERS_COMMANDS_H_
#define HEADERS_COMMANDS_H_

 typedef enum command {
    EMPTY = 0,
    EAT,
    PET,
    EXERCISE,
    ACTIVATE
} command;

extern command commandToSend;


#endif /* HEADERS_COMMANDS_H_ */
