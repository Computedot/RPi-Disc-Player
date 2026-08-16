#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <vlc/vlc.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>

#include "controls.h"
#include "disc_detect.h"

int current_media_type = NO_MEDIA;
int current_tray_status;
int old_tray_status = CDS_NO_DISC;
bool media_playing = false;
char path_to_odd[] = ("/dev/sr0");

libvlc_instance_t* inst;
libvlc_media_t* media;
libvlc_media_player_t* player;

int main(){
	printf("HELLO\n");
	int odd_desc = open("/dev/sr0", O_RDONLY | O_NONBLOCK);
	if(odd_desc==-1) return 100;
	while(true){
		//The program should check for changes to the tray status every second, 
		//since the eject button on the ODD is not handled by the program.
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

			switch(current_tray_status){
				case CDS_NO_DISC:		printf("Please Insert a Disc\n"); break;
				case CDS_TRAY_OPEN:		printf("Tray Is Open\n"); break;
				case CDS_DRIVE_NOT_READY:	printf("Loading\n"); break;
				case CDS_DISC_OK:		current_media_type = get_media_type(odd_desc, path_to_odd); break;
			}
		}
		//Turn on the controls ONLY when a disc has been inserted and is either a CD or a DVD.
		//Some controls are shared.
		if(current_media_type!=NO_MEDIA && current_media_type!=UNKNOWN){
			media_playing = true;
			inst = libvlc_new(0, NULL);
			if(current_media_type == AUDIO_CD)
				media = libvlc_media_new_location(inst, "cdda:///dev/sr0");
			else if(current_media_type == VIDEO_DVD)
				media = libvlc_media_new_location(inst, "dvdnav:///dev/sr0");
			else
				return 101;
			player = libvlc_media_player_new_from_media(media);
			switch(get_current_function()){
				case PLAY:	libvlc_media_player_play(player);	break;
				case PAUSE:	libvlc_media_player_pause(player); 	break;
				case STOP:	libvlc_media_player_stop(player);	break;
				default: 		printf("UNKNOWN");
			}
		}
	sleep(1);
	}
}
