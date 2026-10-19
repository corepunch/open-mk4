#include "../character.h"

void jarek_ready(Fighter *fighter) {
  fighter_ready(fighter);
}

void jarek_idle(Fighter *fighter) {
  weapon_stance(fighter, S_IDLE);
}

void jarek_walk(Fighter *fighter) {
  weapon_stance(fighter, S_WALK);
}

void jarek_block(Fighter *fighter) {
  weapon_stance(fighter, S_BLOCK);
}

void jarek_script_lk_return(Fighter *fighter) {
  mk4_mmove_lk_owner_return(fighter, fighter_ready);
}

void jarek_script_lp_select(Fighter *fighter) {
  q16 along = 0;
  int64_t distance2 = 0;
  for (int axis = AXIS_X; axis <= AXIS_Z; axis += 2) {
    q16 delta = fighter->saved_motion[axis];
    along += q16_mul(delta, fighter_axis(fighter, axis));
    distance2 += ((int64_t)delta * delta) >> 16;
  }
  if (along >= 0 && distance2 < 0x280000) {
    jarek_entry_retail_script_004de6b8(fighter);
  } else {
    jarek_entry_retail_script_004de640(fighter);
  }
}

void jarek_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  jarek_entry_retail_script_004de664(victim);
  jarek_entry_retail_script_004de668(fighter);
}

void jarek_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, jarek_ready);
}

void jarek_script_current_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    jarek_fatality_contact_hold(fighter);
    return;
  }
  require(reaction == 102 || reaction == 107, "Unexpected Jarek LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  battle_react(victim, reaction);
}

void jarek_script_opponent_reaction(Fighter *fighter, int reaction) {
  mk4_mmove_lk_release(fighter, reaction);
}
