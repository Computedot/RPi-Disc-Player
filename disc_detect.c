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

const char* temp_disc_mountpoint = ("/mnt/temp/");
int old_drive_status = CDS_NO_DISC;

media_type get_media_type(int odd_desc, char* path_to_odd){
	/*
	 * Checking if the inserted disc is a (Red Book)
	 * audio CD
	 */
    if(ioctl(odd_desc, CDROM_DISC_STATUS)==CDS_AUDIO)
		return AUDIO_CD;
	/*
	 * Proceed to check if the disc is a Video DVD. Dirent could not access the drive itself,
	 * so I temporarily mounted the DVD drive to /mnt/temp, and try to find a VIDEO_TS folder.
	 * If the folder does exist, then the disc is a DVD. If not, then the media cannot be played.
	 */
	mount(path_to_odd, temp_disc_mountpoint, "udf", MS_RDONLY, NULL);
	usleep(1000);
	struct dirent* entry;
    DIR* dir = opendir(temp_disc_mountpoint);
    while((entry=readdir(dir))!=NULL){
		if(strcmp(entry->d_name, "VIDEO_TS")==0){
			umount(path_to_odd);
			return VIDEO_DVD;
		}
	}
	umount(path_to_odd);
	return NO_MEDIA;
}
