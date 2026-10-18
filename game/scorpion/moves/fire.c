#include "../character.h"

static void fire_contact(Fighter *fighter);
static const AttackRecord fire_record = {0x24e56, -0xc978, 0x23cac, 0xb7ce, 173, 173, 0, ATTACK_UNBLOCKABLE, 0x1eb8};
static const MoveDef s_fire = {.clip = 448, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = ACTION_RETAIL_0115};

static void fire_contact(Fighter *fighter) {
  battle_contact(fighter, &fire_record);
}

void scorpion_fire_script(Fighter *fighter) {
  bool fast = fighter->script_phase != 0;
  int first = fighter->ticks + 1, last = fast ? 39 : 54;
  native_begin(fighter, first, last, scorpion_special_ready);
  for (int frame = first; frame <= last; ++frame) {
    int pose = fast ? mini(34, frame < 19 ? frame - 2 : frame < 32 ? frame - 3 : frame - 4) : mini(34, frame < 48 ? 11 + (frame - 13) / 2 : frame - 19);
    native_frame(fighter, frame, pose, frame == (fast ? 19 : 27) ? fire_contact : NULL);
  }
  native_play(fighter, false);
}

void scorpion_fire(Fighter *fighter) {
  scorpion_stop_body(fighter);
  fighter_start_move(fighter, S_PUNCH, &s_fire);
  fighter->sample_override = 1;
  fighter->script_phase = (fighter_opponent(fighter)->flags & FF_REACTING) != 0;
  fighter_start_fiber(fighter, scorpion_entry_retail_script_004e5b58, NULL);
}

void scorpion_script_fire_decor(Fighter *fighter) {
  battle_particles(fighter, 8, 4, true);
}

void scorpion_script_fire_flame(Fighter *fighter) {
  battle_particles(fighter, 9, 4, true);
}
