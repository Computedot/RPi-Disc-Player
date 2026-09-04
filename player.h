#ifndef PLAYER_H
#define PLAYER_H

#include "disc_detect.h"

typedef enum{
	PLAY	= '1',
	PAUSE	= '2',
	STOP 	= '3',
	FFORW 	= '4',
	REW 	= '5',
	NEXT 	= '6',
	PREV 	= '7',
	GO_TO	= '8',
	SUBS 	= '9',
	LANG 	= '0'
} function;

/*
 * The child's first task is to initialize the player.
 */
void initialize_player(media_type current_media_type);

/*
 * Translates the user's input to player controls.
 * \param input the user's input which corresponds to a certain player function
 */
void player_control(media_type current_media_type, function input, WINDOW* child_win);

void destroy_player(WINDOW* child_win);

#endif
