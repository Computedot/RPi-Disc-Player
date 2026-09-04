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

function input; 									//User input for controlling the player
media_type current_media_type = NO_MEDIA;
int child_status;
int current_tray_status;
int old_tray_status = CDS_NO_DISC;
int odd_desc; 										//The optical drive file descriptor
char path_to_odd[] = ("/dev/sr0");
bool playable_media = false;


int main(){
	int i =0;
	WINDOW* parent_win;
	WINDOW* child_win;
	int parent_pid = getpid();
	int child_pid = -10;

	//Initializing ncurses



	while(true){
		/*
		 * The parent process only runs if playable_media is set to false, which happens when
		 * a) No media is inserted, or
		 * b) Non-playable media has been inserted, or
		 * c) The child process has detected a tray status change (which happens when the eject button is pressed), or
		 * d) The user has manually stopped the player.
		 *
		 * This cycle is repeated every seconduntil the system detects playable media, and switches to the child process,
		 * controlling the player. In each loop, a signal is sent to the child process to see if it exists, and kill if it
		 * does exist.
		 *
		 * The parent is in charge of checking the tray status and printing the corresponding message, as well as
		 * getting the current media type if a disc is detected.
		 */
		if(getpid()==parent_pid){
			parent_win = create_win();
			odd_desc = open("/dev/sr0", O_RDONLY | O_NONBLOCK);
			if(odd_desc==-1) return 100;
			current_tray_status=ioctl(odd_desc, CDROM_DRIVE_STATUS);
			if(current_tray_status!=old_tray_status){
				old_tray_status = current_tray_status;
				current_media_type = NO_MEDIA;
			   /*
				* If a disc is detected, then the program attempts to recognise
				* the media type (Audio CD or Video DVD) using the current_media_type
				* function, found in disc_detect.c
				*/
				switch(current_tray_status){
					case CDS_NO_DISC:			wprintw(parent_win, "Please Insert a Disc\n"); break;
					case CDS_TRAY_OPEN:			wprintw(parent_win, "Tray Is Open\n"); break;
					case CDS_DRIVE_NOT_READY:	wprintw(parent_win, "Loading\n"); break;
					case CDS_DISC_OK:			current_media_type = get_media_type(odd_desc, path_to_odd); break;
				}
				switch(current_media_type){
					case AUDIO_CD:	wprintw(parent_win, "AUDIO CD");
									playable_media = true;
									break;
					case VIDEO_DVD:	wprintw(parent_win, "VIDEO DVD");
									playable_media = true;
									break;
					default:		wprintw(parent_win, "UNKNOWN MEDIA");
									playable_media = false;
				}
				wrefresh(parent_win);
				sleep(1);
			}
			/*
			 * If playable media has been detected, then the child process is created, which controls the media player.
			 * The child's first task is to initialize the player.
			 * The child process is destroyed if:
			 * a) The drive/disc status changes, or
			 * b) The user manually stops the player.
			 */
			if(playable_media){
				destroy_win(parent_win);
				child_pid = fork();
				if(child_pid==0){
					initialize_player(current_media_type);
					child_win = create_win();
				}
				else{
					close(odd_desc);
					do{
						sleep(1);
						waitpid(child_pid, &child_status, 0);
					}
					while(!WIFEXITED(child_status));
				}
			}
		}

	   /*
		* If the current media type is playable, then the child process checks for disc tray status changes.
		* If no changes have been detected, it reads a user input to perform
		* an action (like pause, skip, etc...). If no user input is detected,
		* then the program proceeds normally. The playable_media media flag is used to determine whether or not to
		* return to the parent, and kill the child process.
		*/
		if(playable_media && child_pid==0){
			odd_desc = open("/dev/sr0", O_RDONLY | O_NONBLOCK);
			current_tray_status=ioctl(odd_desc, CDROM_DRIVE_STATUS);
			close(odd_desc);
			if(current_tray_status!=old_tray_status)
				destroy_player(child_win);
			else{
				usleep(300);
				input = wgetch(child_win);
				if(input!=ERR)
					player_control(current_media_type, input, child_win);
				wrefresh(child_win);
			}
		}
		sleep(1);
	}
	endwin();
}
