#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <vlc/vlc.h>

#include "controls.h"
#include "disc_detect.h"

void player_control(libvlc_media_player_t* player, function input, media_type current_media_type){
  switch(input){  
    case PLAY:  libvlc_media_player_play(player); printw("PLAY"); break;
    case PAUSE: libvlc_media_player_pause(player); break;
    case STOP:  libvlc_media_player_stop(player); break;
    case NEXT:  libvlc_media_player_next_chapter(player); break;
    case PREV:  libvlc_media_player_previous_chapter(player); break;
    case FFORW: libvlc_media_player_set_rate(player, 2.0f); break;
    case REW:   libvlc_media_player_set_rate(player, -1.0f); break;
  }
  refresh();
}
