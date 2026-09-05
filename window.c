#include <ncurses.h>

#include "window.h"

WINDOW* create_win(){
    WINDOW* window = newwin(WIN_HEIGHT, WIN_WIDTH, 0, 0);
    nodelay(window, TRUE);
	keypad(window, TRUE);
    wmove(window, 0, 0);
    return window;
}

void clean_win(WINDOW* window){
    int i;
    int j;
    for(i=0; i<WIN_HEIGHT; i++){
        for(j=0; j<WIN_WIDTH; j++)
            mvwaddch(window, i, j, ' ');
        j = 0;
        mvwaddch(window, i, j, ' ');
    }
    wmove(window, 0, 0);
    wrefresh(window);
}

void destroy_win(WINDOW* window){
    clean_win(window);
    delwin(window);
}
