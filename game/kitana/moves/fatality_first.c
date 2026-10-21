#include "../character.h"

void kitana_script_first_release(Fighter *fighter) {
  battle_shatter(fighter_peer(fighter));
  fighter->body.physics = kitana_scene_end_watch;
  kitana_entry_retail_script_004e01d0(fighter);
}
