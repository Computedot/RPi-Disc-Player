/*Test file to test cd playback*/

#include <stdio.h>
#include <vlc/vlc.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    libvlc_instance_t* inst;
    libvlc_media_t* m;
    libvlc_media_list_t* ml;
    libvlc_media_player_t* mp;
    libvlc_media_list_player_t* mlp;

    inst = libvlc_new(0, NULL);
    m = libvlc_media_new_location(inst, "cdda:///dev/sr0");
    ml = libvlc_media_list_new(inst);
    mp = libvlc_media_player_new(inst);
    mlp = libvlc_media_list_player_new(inst);
    libvlc_media_parse_with_options(m, libvlc_media_parse_local || libvlc_media_fetch_local, 2);

    libvlc_media_list_add_media(ml, m);
    libvlc_media_list_player_set_media_list(mlp, ml);
    libvlc_media_list_player_play(mlp);
    sleep(10);
}
