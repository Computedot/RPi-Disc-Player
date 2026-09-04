#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <vlc/vlc.h>
#include <unistd.h>
#include <signal.h>

#include "player.h"
#include "disc_detect.h"
#include "window.h"

libvlc_state_t state;
libvlc_instance_t* inst;
libvlc_media_t* media;
libvlc_media_list_t* media_list;
libvlc_media_player_t* media_player;
libvlc_media_list_player_t* media_list_player;

bool media_playing = false;

void initialize_player(media_type current_media_type){
    inst = libvlc_new(0, NULL);
    switch(current_media_type){
        case AUDIO_CD:	media = libvlc_media_new_location(inst, "cdda:///dev/sr0"); break;
        case VIDEO_DVD: media = libvlc_media_new_location(inst, "dvdsimple:///dev/sr0"); break;
    }
    media_list_player = libvlc_media_list_player_new(inst);
    media_list = libvlc_media_list_new(inst);
    media_player = libvlc_media_player_new(inst);
    libvlc_media_parse_with_options(media, libvlc_media_parse_local || libvlc_media_fetch_local, 5);
    libvlc_media_list_add_media(media_list, media);
    libvlc_media_list_player_set_media_list(media_list_player, media_list);
}

void player_control(media_type current_media_type, function input, WINDOW* child_win){
  switch(input){  
      case PLAY:
          if(!media_playing){
              libvlc_media_list_player_play(media_list_player);
              media_playing = true;
          }
          break;
      case STOP:
          if(media_playing)
            destroy_player(child_win);
          break;
      case PAUSE: libvlc_media_list_player_pause(media_list_player); break;
      case NEXT:  libvlc_media_list_player_next(media_list_player); break;
      case PREV:  libvlc_media_list_player_previous(media_list_player); break;
    }
  refresh();
}

void destroy_player(WINDOW* child_win){
    destroy_win(child_win);
    endwin();
    libvlc_media_release(media);
    libvlc_media_list_release(media_list);
    libvlc_media_list_player_release(media_list_player);
    libvlc_release(inst);
    exit(0);
}
