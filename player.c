#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <vlc/vlc.h>
#include <unistd.h>

#include "player.h"
#include "disc_detect.h"
#include "window.h"

libvlc_state_t state;
libvlc_instance_t* inst;
libvlc_media_t* media;
libvlc_media_list_t* media_list;
libvlc_media_player_t* media_player;
libvlc_media_list_player_t* media_list_player;

function previous_function = STOP;
const char* vlc_args[] = {"--quiet", "--verbose=-1", "--no-keyboard-events", "--no-mouse-events"};
const int vlc_args_size = 4;

void initialize_player(media_type current_media_type){
    inst = libvlc_new(vlc_args_size, vlc_args);
    usleep(100);
    FILE* logfile = fopen("./log.txt", "w");
    libvlc_log_set_file(inst, logfile);
    freopen("./stderr_log.txt", "w", stderr);
    media_list_player = libvlc_media_list_player_new(inst);
    media_list = libvlc_media_list_new(inst);
    media_player = libvlc_media_player_new(inst);
    switch(current_media_type){
        case AUDIO_CD:
            media = libvlc_media_new_location(inst, "cdda:///dev/sr0");
            libvlc_media_parse_with_options(media, libvlc_media_parse_local || libvlc_media_fetch_local, 5);
            libvlc_media_list_add_media(media_list, media);
            libvlc_media_list_player_set_media_list(media_list_player, media_list);
            libvlc_media_list_player_set_media_player(media_list_player, media_player);
            return;
        case VIDEO_DVD:
            media = libvlc_media_new_location(inst, "dvdnav:///dev/sr0");
            libvlc_media_player_set_media(media_player, media);
            return;
    }
}

void cd_player_control(function input, WINDOW* child_win){
    char* pressed_button;
    switch(input){
        case PLAY:
            if(previous_function==PAUSE)
                libvlc_media_list_player_set_pause(media_list_player, 0);
            else if(previous_function==FFORW || previous_function==REW)
                libvlc_media_player_set_rate(media_player, 1);
            else
                libvlc_media_list_player_play(media_list_player);
            pressed_button = ("PLAY");
            print_pressed_button(pressed_button, child_win);
            previous_function = input;
            break;
        case PAUSE:
            libvlc_media_list_player_set_pause(media_list_player, 1);
            pressed_button = "PAUSE";
            print_pressed_button(pressed_button, child_win);
            previous_function = input;
            break;
        case NEXT:
            libvlc_media_list_player_next(media_list_player);
            pressed_button = "NEXT";
            print_pressed_button(pressed_button, child_win);
            break;
        case PREV:
            libvlc_media_list_player_previous(media_list_player);
            pressed_button = "PREVIOUS";
            print_pressed_button(pressed_button, child_win);
            break;
        default:
            mutual_controls(input, child_win);
    }
}

void dvd_player_control(function input, WINDOW* child_win){
    char* pressed_button;
    switch(input){
        case PLAY:
            if(previous_function==PAUSE)
                libvlc_media_player_set_pause(media_player, 0);
            else if(previous_function==FFORW || previous_function==REW)
                libvlc_media_player_set_rate(media_player, 1);
            else
                libvlc_media_player_play(media_player);
            pressed_button = ("PLAY");
            print_pressed_button(pressed_button, child_win);
            previous_function = input;
            break;
        case PAUSE:
            libvlc_media_player_set_pause(media_player, 1);
            pressed_button = "PAUSE";
            print_pressed_button(pressed_button, child_win);
            previous_function = input;
            break;
        case NEXT:
            libvlc_media_player_next_chapter(media_player);
            pressed_button = "NEXT";
            print_pressed_button(pressed_button, child_win);
            break;
        case PREV:
            libvlc_media_player_previous_chapter(media_player);
            pressed_button = "PREVIOUS";
            print_pressed_button(pressed_button, child_win);
            break;
        case UP:
            libvlc_media_player_navigate(media_player, libvlc_navigate_up);
            break;
        case DOWN:
            libvlc_media_player_navigate(media_player, libvlc_navigate_down);
            break;
        case RIGHT:
            libvlc_media_player_navigate(media_player, libvlc_navigate_right);
            break;
        case LEFT:
            libvlc_media_player_navigate(media_player, libvlc_navigate_left);
            break;
        case SELECT:
            libvlc_media_player_navigate(media_player, libvlc_navigate_activate);
            break;
        default:
            mutual_controls(input, child_win);
    }
}

void mutual_controls(function input, WINDOW* child_win){
    char* pressed_button;
    //Elapsed time (ms)
    int64_t e_time = libvlc_media_player_get_time(media_player);

    //Remaining time (ms)
    int64_t r_time = libvlc_media_player_get_length(media_player) - e_time;
    switch(input){
        case STOP:
            pressed_button = "STOP";
            previous_function = STOP;
            print_pressed_button(pressed_button, child_win);
            destroy_player(child_win);
            break;
        case FFORW:
            libvlc_media_player_set_rate(media_player, 2.0);
            previous_function = input;
            break;
        case REW:
            libvlc_media_player_set_time(media_player, e_time - 5000);
            previous_function = input;
            break;
        case GET_E_TIME:
            wprintw(child_win, "Elapsed Time: ");
            print_time(child_win, e_time);
            break;
        case GET_R_TIME:
            wprintw(child_win, "Remaining Time: ");
            print_time(child_win, r_time);
    }
}

void print_time(WINDOW* child_win, int64_t time){
    //Convert time to seconds (from milliseconds)
    time = time / 1000;
    int seconds = time % 60;
    int minutes = time / 60;
    if(seconds<10)
        wprintw(child_win, "%d : 0%d\n", minutes, seconds);
    else
        wprintw(child_win, "%d : %d\n", minutes, seconds);
    wrefresh(child_win);
    sleep(2);
    clean_win(child_win);
}

void destroy_player(WINDOW* child_win){
    destroy_win(child_win);
    libvlc_media_release(media);
    libvlc_media_list_release(media_list);
    libvlc_media_list_player_release(media_list_player);
    libvlc_release(inst);
    exit(0);
}

void print_pressed_button(const char* pressed_button, WINDOW* child_win){
    wprintw(child_win, "%s", pressed_button);
    wrefresh(child_win);
    sleep(1);
    clean_win(child_win);
}
