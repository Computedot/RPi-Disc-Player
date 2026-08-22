#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <vlc/vlc.h>

#include "controls.h"
#include "disc_detect.h"

void player_control(libvlc_media_list_player_t* media_list_player, function input, media_type current_media_type){
  switch(input){  
    case PLAY:  libvlc_media_list_player_play(media_list_player); break;
  }
  refresh();
}
