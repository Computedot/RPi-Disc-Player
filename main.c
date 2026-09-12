#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>
#include <vlc/vlc.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "player.h"
#include "disc_detect.h"
#include "window.h"

const char path_to_odd[] = ("/dev/sr0");

int main(){
	//Initializing ncurses
	initscr();
	cbreak();
	curs_set(0);
	noecho();

	//Declaring variables
	WINDOW* parent_win;
	WINDOW* child_win;
	function input;
	media_type current_media_type;
	bool playable_media;
	int old_tray_status;
	int current_tray_status;
	int child_pid;
	int child_status;
	int odd_desc;

	child_pid = -10;

	while(true){
		if(child_pid!=0){
			playable_media 		= false;
			current_media_type	= NO_MEDIA;
			odd_desc			= open("/dev/sr0", O_RDONLY | O_NONBLOCK);
			parent_win			= create_win();
			current_tray_status	= ioctl(odd_desc, CDROM_DRIVE_STATUS);
			old_tray_status		= current_tray_status;

			/*
			* If a disc is detected, then the program attempts to recognise
			* the media type (Audio CD or Video DVD) using the current_media_type
			* function, found in disc_detect.c
			*/
			switch(current_tray_status){
				case CDS_NO_DISC:			wprintw(parent_win, "Please Insert a Disc\n"); break;
				case CDS_TRAY_OPEN:			wprintw(parent_win, "Tray Is Open\n"); break;
				case CDS_DRIVE_NOT_READY:	wprintw(parent_win, "Loading\n"); break;
				case CDS_DISC_OK:
					current_media_type = get_media_type(odd_desc);
					switch(current_media_type){
						case AUDIO_CD:	wprintw(parent_win, "AUDIO CD\n");
										playable_media = true;
										break;
						case VIDEO_DVD:	wprintw(parent_win, "VIDEO DVD\n");
										playable_media = true;
										break;
						default:		wprintw(parent_win, "UNKNOWN MEDIA/NO MEDIA\n");
										playable_media = false;
					}
			}
			wrefresh(parent_win);
			sleep(2);
			destroy_win(parent_win);

			/*
			* If playable media has been detected, then the child process is created, which controls the media player,
			* while the parent is waiting for the child to finish.
			* Else, no child is created and the program checks for changes to the drive every second.
			* When finally a change is detected, the program exits and is restarted, after having unmounted
			* and remounted the disc drive.
			*/
			if(playable_media){
				close(odd_desc);
				child_pid = fork();
				usleep(1000);
				if(child_pid!=0){
					do{
						waitpid(child_pid, &child_status, 0);
						sleep(1);
					}
					while(!WIFEXITED(child_status));
				}
				else{
					child_win = create_win();
					initialize_player(current_media_type);
					usleep(1000);
				}
			}
			else{
				do{
					current_tray_status=ioctl(odd_desc, CDROM_DRIVE_STATUS);
					sleep(1);
				}
				while(current_tray_status==old_tray_status);
				close(odd_desc);
			}
		}

		/*
		* The child's first task is to initialize the player.
		* The child process is destroyed if:
		* a) The drive/disc status changes, or
		* b) The user manually stops the player.
		* Before reading user input, the child checks for disc tray status changes
		* If no changes have been detected, it reads a user input to perform
		* an action (like pause, skip, etc...).
		*/
		if(child_pid==0){
			odd_desc = open("/dev/sr0", O_RDONLY | O_NONBLOCK);
			current_tray_status = ioctl(odd_desc, CDROM_DRIVE_STATUS);
			close(odd_desc);
			if(current_tray_status!=CDS_DISC_OK)
				destroy_player(child_win);
			else{
				usleep(1000);
				input = wgetch(child_win);
				if(input!=ERR){
					switch(current_media_type){
						case AUDIO_CD:	cd_player_control(input, child_win);	break;
						case VIDEO_DVD:	dvd_player_control(input, child_win);	break;
					}
				}
			}
			usleep(10000);
		}
	}
	endwin();
}
