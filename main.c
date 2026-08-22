#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <vlc/vlc.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>

#include "controls.h"
#include "disc_detect.h"

function input; 									//User input for controlling the player
media_type current_media_type = NO_MEDIA;
int current_tray_status;
int old_tray_status = CDS_NO_DISC;
int odd_desc; 										//The optical drive file descriptor
bool media_playing = false; 						//Signals if playable media has been loaded
char path_to_odd[] = ("/dev/sr0");

libvlc_instance_t* inst;
libvlc_media_t* media;
libvlc_media_list_t* media_list;
libvlc_media_list_player_t* media_list_player;
libvlc_media_player_t* player;

void initialize_player(){

}

int main(){
	inst = libvlc_new(0, NULL);
	media_list_player = libvlc_media_list_player_new(inst);
	media_list = libvlc_media_list_new(inst);
	libvlc_media_list_player_set_media_list(media_list_player, media_list);

	if(odd_desc==-1) return 100;

	//Initializing ncurses
	initscr();
	cbreak();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	printw("HELLO WORLD\n");
	refresh();

	while(true){
		/*
		 * The program should check for changes to the tray status every second, 
		 * since the eject button on the ODD is not handled by the program.
		 * If media is playing during a tray status change, then it is released
		 * and the player is stopped
		 */
		odd_desc = open("/dev/sr0", O_RDONLY | O_NONBLOCK);
		current_tray_status=ioctl(odd_desc, CDROM_DRIVE_STATUS);
		if(current_tray_status!=old_tray_status){
			old_tray_status = current_tray_status;
			current_media_type = NO_MEDIA;
			if(media_playing){
				libvlc_media_player_stop(player);
				libvlc_media_player_release(player);
				media_playing = false;
			}
			/* If a disc is detected, then the program attempts to recognise
			 * the media type (Audio CD or Video DVD) using the current_media_type
			 * function, found in disc_detect.c
			 */
			switch(current_tray_status){
				case CDS_NO_DISC:			printf("Please Insert a Disc\n"); break;
				case CDS_TRAY_OPEN:			printf("Tray Is Open\n"); break;
				case CDS_DRIVE_NOT_READY:	printf("Loading\n"); break;
				case CDS_DISC_OK:			current_media_type = get_media_type(odd_desc, path_to_odd); break;
			}
			close(odd_desc);
			sleep(2);
			refresh();
			/* If the media type is truly an Audio CD or Video DVD, then load the
			 * media to the media list player;
			 */
			switch(current_media_type){
				case AUDIO_CD:	media = libvlc_media_new_location(inst, "cdda:///dev/sr0");
								media_playing = true;
								break;
				case VIDEO_DVD: media = libvlc_media_new_location(inst, "dvdsimple:///dev/sr0");
								media_playing = true;
								break;
				case NO_MEDIA: 	media_playing = false;
								break;
				default: 		return 101;
			}
			if(media_playing){
				libvlc_media_parse_with_options(media, libvlc_media_parse_local || libvlc_media_fetch_local, 5);
				libvlc_media_list_add_media(media_list, media);
			}
		}
	/* If media is playing, then the program reads a user input to perform
	 * an action (like pause, skip, etc...). If no user input is detected,
	 * then the program proceeds normally.
	 */
	if(media_playing){	
		  input = getch();
			if(input!=ERR)
				player_control(media_list_player, input, current_media_type);
			refresh();
		}
		sleep(1);
	}
	endwin();
}
