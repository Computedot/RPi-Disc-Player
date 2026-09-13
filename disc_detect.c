#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <vlc/vlc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "disc_detect.h"

void get_detected_status(const libvlc_event_t* event, void* data){
	detection_status* detected_status = (detection_status*)data;
	if (event->type == libvlc_MediaPlayerEncounteredError)
        *detected_status = DETECT_ERROR;
    else if (event->type == libvlc_MediaPlayerPlaying)
        *detected_status = DETECT_PLAYING;
}

media_type get_media_type(int odd_desc){

	/*
	 * First, the program checks if the inserted disc is an Audio CD. If not, then it proceeds
	 * to check if the disc is a Video DVD. A temporary VLC instance is created,
	 * and the temporary player attempts to play the disc, as if
	 * it was a DVD. If playback is successful, then the disc is in fact a Video DVD.
	 * Else, the disc's contents cannot be played.
	 */
    if(ioctl(odd_desc, CDROM_DISC_STATUS)==CDS_AUDIO)
		return AUDIO_CD;

	FILE* logfile								= fopen("./logs/vlc_log.txt", "w");
	libvlc_instance_t * inst					= libvlc_new(0, NULL);
	libvlc_log_set_file(inst, logfile);
    libvlc_media_t *m 							= libvlc_media_new_location(inst, "dvd:///dev/sr0");
	libvlc_media_player_t *mp 					= libvlc_media_player_new_from_media(m);
	libvlc_event_manager_t *em					= libvlc_media_player_event_manager(mp);
	volatile detection_status detected_status	= DETECT_PENDING;
	libvlc_event_attach(em, libvlc_MediaPlayerEncounteredError, get_detected_status, (void*)&detected_status);
    libvlc_event_attach(em, libvlc_MediaPlayerPlaying, get_detected_status, (void*)&detected_status);
	libvlc_media_player_play(mp);
	for(int waited = 0; detected_status == DETECT_PENDING && waited < 30000; waited += 100)
        usleep(100000);
	libvlc_media_player_stop(mp);
	libvlc_media_player_release(mp);
	libvlc_media_release(m);
	libvlc_release(inst);
	fclose(logfile);
	switch(detected_status){
		case DETECT_PLAYING: return VIDEO_DVD;
		case DETECT_ERROR:	 return NO_MEDIA;
		default: 			 return ERROR;
	}
}
