#ifndef CONTROLS_H
#define CONTROLS_H

#include "disc_detect.h"

typedef enum{
	PLAY 	= '1',
	PAUSE = '2',
	STOP 	= '3',
	FFORW = '4',
	REW 	= '5',
	NEXT 	= '6',
	PREV 	= '7',
	GO_TO	= '8',
	SUBS 	= '9',
	LANG 	= '0'
} function;

void player_control(libvlc_media_list_player_t* media_list_player, function input, media_type current_media_type);

#endif
