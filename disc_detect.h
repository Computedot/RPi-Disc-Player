#ifndef DISC_DETECT_H
#define DISC_DETECT_H

typedef enum {
    AUDIO_CD,
    VIDEO_DVD,
    UNKNOWN,
    NO_MEDIA
} media_type;

media_type get_media_type(int odd_desc);

#endif 
