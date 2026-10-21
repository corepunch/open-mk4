#include "native_services.h"

static const MoveDef victim = {.clip = 130, .last = 39, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = 0x4001};

/* 495980 has the throw/back gates but no air selector. The ordinary LP
 * callback handles air first; follow-up table4ecb40 enters this ground body. */

void jarek_weapon_reaction(Fighter *fighter) {
  require(fighter->reaction == 148 || fighter->reaction == 62, "Invalid knife victim entry");
  mk4_script_disarm_self(fighter);
  const Fighter *peer = fighter_opponent(fighter);
  fighter->body.pos[AXIS_X] = peer->body.pos[AXIS_X] + q16_mul(fighter_axis(peer, AXIS_X), 0x1b333);
  fighter->body.pos[AXIS_Z] = peer->body.pos[AXIS_Z] + q16_mul(fighter_axis(peer, AXIS_Z), 0x1b333);
  mk4_script_stop_body(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter_start_move(fighter, S_HIT, &victim);
  fighter->flags |= FF_REACTING;
  retail_vm_enter_owned(fighter, 0x4e8c08, mk4_mmove_thrown_finish, 12);
}
