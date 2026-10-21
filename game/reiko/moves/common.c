#include "../character.h"

static const MoveDef s_lp_thrown = {153, 0, MOVE_END, 0, 0, 0, 0, 0, 0.f, 0.f, SF_LOCK | SF_REACTION, NULL, 0};

void reiko_script_lp_end(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  reiko_entry_retail_script_004de664(victim);
  reiko_entry_retail_script_004de668(fighter);
}

void reiko_script_lp_return(Fighter *fighter) {
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  fighter->flags &= ~(unsigned)FF_PAIRED;
  victim->flags &= ~(unsigned)FF_PAIRED;
  mk4_script_stop_body(fighter);
  fighter_finish_mmove(fighter, fighter_ready);
}

void reiko_script_opponent_reaction(Fighter *fighter, int reaction) {
  if (reaction == 175) {
    return;
  }

  if (reaction == 77) {
    mk4_mmove_lk_release(fighter, reaction);
    return;
  }
  require(false, "Unexpected Reiko opponent reaction");
}

void reiko_script_lp_reaction(Fighter *fighter, int reaction) {
  require(reaction == 102 || reaction == 107, "Unexpected reiko LP victim reaction");
  Fighter *victim = mk4_script_release_pair(fighter);
  victim->reaction = reaction;
  victim->flags |= FF_REACTING;
  fighter_start_move(victim, S_KNOCKDOWN, &s_lp_thrown);
  if (reaction == 102) {
    reiko_entry_retail_script_004e9bd0(victim);
  } else {
    reiko_entry_retail_script_004e9b40(victim);
  }
}
