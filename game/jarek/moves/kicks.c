#include "native_services.h"

void jarek_script_low_kick_avoid(Fighter *fighter) {
  fighter->avoids_high = true;
}

void jarek_script_low_kick_clear(Fighter *fighter) {
  fighter->avoids_high = false;
}
