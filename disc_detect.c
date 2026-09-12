#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <stdlib.h>
#include <fcntl.h>

#include "disc_detect.h"
#include "player.h"

media_type get_media_type(int odd_desc){

	/*
	 * First, the program checks if the inserted disc is an Audio CD. If not, then it proceeds
	 * to check if the disc is a Video DVD. A temporary VLC instance is created,
	 * and the temporary player attempts to add the disc's contents to the player, as if
	 * it was a DVD. If the parsing is successful, then the disc is in fact a Video DVD.
	 * Else, the disc's contents cannot be played.
	 */
    if(ioctl(odd_desc, CDROM_DISC_STATUS)==CDS_AUDIO)
		return AUDIO_CD;
	if(initialize_player(VIDEO_DVD)==0)
		return VIDEO_DVD;
	else
		return NO_MEDIA;
}
