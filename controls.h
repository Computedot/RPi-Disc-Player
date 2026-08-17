#ifndef CONTROLS_H
#define CONTROLS_H

#include "disc_detect.h"

typedef enum{
	PLAY,
	PAUSE,
	STOP,
	FFORW,
	REW,
	NEXT,
	PREV,
	GOTO,
	SUBS,
	LANG
} function;

void player_control(libvlc_media_player_t* player, function input, media_type current_media_type);

#endif
