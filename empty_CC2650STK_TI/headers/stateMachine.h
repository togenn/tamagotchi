/*
 * stateMachine.h
 *
 *  Created on: 11 Oct 2022
 *      Author: Toni
 */

#ifndef HEADERS_STATEMACHINE_H_
#define HEADERS_STATEMACHINE_H_

#define COMM_INTERVAL 4

typedef enum state { WAITING=1, READ_ACCEL_DATA, COMMUNICATION, UPDATE_UI} state;
extern state programState;

#endif /* HEADERS_STATEMACHINE_H_ */
