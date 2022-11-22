/*
 * musicPlayer.h
 *
 *  Created on: 21 Nov 2022
 *      Author: Same
 */

#ifndef HEADERS_MUSICPLAYER_H_
#define HEADERS_MUSICPLAYER_H_

void initBackgroundMusic(void);
void stopMusic();
void startMusic();

//private
void musicTimerFxn(UArg arg0);



#endif /* HEADERS_MUSICPLAYER_H_ */
