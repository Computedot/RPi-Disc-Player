#include <stdio.h>
#include <stdlib.h>

#include "controls.h"

function get_current_function(){
  switch(getchar()){
    case 'p': return PLAY;
    case 'o': return PAUSE;
    case 's': return STOP;
  }
  return PLAY;
}
