#include <ncurses.h>

#include "window.h"

WINDOW* create_win(){
    initscr();
    WINDOW* window = newwin(WIN_HEIGHT, WIN_WIDTH, 0, 0);
	cbreak();
	nodelay(window, TRUE);
	keypad(window, TRUE);
    return window;
}

void destroy_win(WINDOW* window){
    int i;
    int j;
    for(i=0; i<WIN_HEIGHT; i++){
        for(j=0; j<WIN_WIDTH; j++)
            mvwaddch(window, i, j, ' ');
        j = 0;
        mvwaddch(window, i, j, ' ');
    }
    wrefresh(window);
    delwin(window);
}
