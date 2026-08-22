#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <vlc/vlc.h>
#include <dirent.h>

#include "disc_detect.h"

int old_drive_status = CDS_NO_DISC;
int current_drive_status;

media_type get_media_type(int odd_desc, char* path_to_odd){	 
			//Check if the disc is a (red book) Audio CD
    	if(ioctl(odd_desc, CDROM_DISC_STATUS)==CDS_AUDIO){
	    	printf("YOU HAVE INSERTED AN AUDIO CD\n");
	      return AUDIO_CD;
      }
      struct dirent* entry;
      const char* path = "/home/stelios/Documents/";
      DIR* dir = opendir(path);
      while((entry=readdir(dir))!=NULL)
      	if(strcmp(entry->d_name, "VIDEO_TS")==0){
        printf("DVD");
				return VIDEO_DVD;
				}      
	    printf("UNKNOWN DISC MEDIA TYPE\n");
			return NO_MEDIA;
}
