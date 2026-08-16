typedef enum{
    AUDIO_CD,
    VIDEO_DVD,
    MEDIA_DISC,
    UNKNOWN,
    NO_MEDIA
} media_type;

media_type get_media_type(int odd_desc, char* path_to_odd);
