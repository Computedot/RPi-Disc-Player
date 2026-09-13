#ifndef PLAYER_H
#define PLAYER_H

#include <ncurses.h>

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
	LANG 	= '0',
	UP		= KEY_UP,
	DOWN	= KEY_DOWN,
	RIGHT	= KEY_RIGHT,
	LEFT	= KEY_LEFT,
	SELECT	= '\n',
	GET_E_TIME = 'A',
	GET_R_TIME = 'B'
} function;

void initialize_player(media_type current_media_type);

void cd_player_control(function input, WINDOW* child_win);

void dvd_player_control(function input, WINDOW* child_win);

void mutual_controls(function input, WINDOW* child_win);

void destroy_player(WINDOW* child_win);

void print_pressed_button(const char* pressed_button, WINDOW* child_win);

void print_time(WINDOW* child_win, int64_t time;);

#endif
