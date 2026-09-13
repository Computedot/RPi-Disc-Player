#ifndef DISC_DETECT_H
#define DISC_DETECT_H

#include <vlc/vlc.h>

typedef enum {
    AUDIO_CD,
    VIDEO_DVD,
    UNKNOWN,
    NO_MEDIA,
    ERROR
} media_type;

typedef enum{
    DETECT_PENDING,
    DETECT_ERROR,
    DETECT_PLAYING
} detection_status;

media_type get_media_type(int odd_desc);

void get_detected_status(const libvlc_event_t* event, void* data);

#endif 
