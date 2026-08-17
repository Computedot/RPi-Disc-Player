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

function input; 															//User input for controlling the player
media_type current_media_type = NO_MEDIA;
int current_tray_status;
int old_tray_status = CDS_NO_DISC;
int odd_desc; 														//The optical drive file descriptor
bool media_playing = false; 						//Signals whether a VLC instance has been created
char path_to_odd[] = ("/dev/sr0");

libvlc_instance_t* inst;
libvlc_media_t* media;
libvlc_media_player_t* player;

int main(){
	odd_desc = open("/dev/sr0", O_RDONLY | O_NONBLOCK);
	if(odd_desc==-1) return 100;

	//Initializing ncurses
	initscr();
	cbreak();
	noecho();
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
		current_tray_status=ioctl(odd_desc, CDROM_DRIVE_STATUS);
		if(current_tray_status!=old_tray_status){
			old_tray_status = current_tray_status;
			current_media_type = NO_MEDIA;
			if(media_playing){
				libvlc_media_player_stop(player);
				libvlc_media_player_release(player);
				libvlc_release(inst);
				media_playing = false;
			}
			/* If a disc is detected, then the program attempts to recognise
			 * the media type (Audio CD or Video DVD) using the current_media_type
			 * function, found in disc_detect.c
			 */
			switch(current_tray_status){
				case CDS_NO_DISC:		printf("Please Insert a Disc\n"); break;
				case CDS_TRAY_OPEN:		printf("Tray Is Open\n"); break;
				case CDS_DRIVE_NOT_READY:	printf("Loading\n"); break;
				case CDS_DISC_OK:		current_media_type = get_media_type(odd_desc, path_to_odd); break;
			}
			refresh();
		}
		/* If the inserted media is recognised, a VLC instance and player is created
		 * and user input is read. If no input has been given, the program continues normally.
		 * Else, the input is sent to the player_control() function, found in controls.c
		 */
		if(current_media_type==AUDIO_CD | current_media_type==VIDEO_DVD){
			media_playing = true;
      inst = libvlc_new(0, NULL);
      if(current_media_type == AUDIO_CD)
      	media = libvlc_media_new_location(inst, "cdda:///dev/sr0");
      else if(current_media_type == VIDEO_DVD)
        media = libvlc_media_new_location(inst, "dvdnav:///dev/sr0");
      else
      	return 101;
      player = libvlc_media_player_new_from_media(media);
		  input = getch();
			if(input!=ERR)
				player_control(player, input, current_media_type);
			refresh();
		}
		sleep(1);
	}
	endwin();
}
