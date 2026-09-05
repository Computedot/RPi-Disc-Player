#ifndef WINDOW_H
#define WINDOW_W

#include <ncurses.h>

#define WIN_HEIGHT  20
#define WIN_WIDTH   20

WINDOW* create_win();
void destroy_win(WINDOW* window);
void clean_win(WINDOW* window);

#endif
