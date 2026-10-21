#include "native_services.h"

static const MoveDef dive = {.clip = 58, .last = 9, .flags = SF_LOCK | SF_RECORD, .action = 0x20f};
static const AttackRecord dive_contact = {0x12666, -0xa3d, 0x107ae, 0x851e, 10, 10, 0, 0, 0x147a};

static void dive_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter->body.on_land = NULL;
}

bool fujin_can_dive(const Fighter *fighter) {
  /* 45eeb0 requires root Y <= -1.8 and the opponent combo-hit count <=4 gate. */
  return fighter->body.pos[AXIS_Y] + fighter->body.root_y <= -0x1cccc && fighter_opponent(fighter)->hits <= 4;
}

void fujin_dive_land(Fighter *fighter) {
  /* 47bfd0 runs the shared landing entry46fd90, then controller return. */
  fighter_finish_mmove(fighter, mk4_land);
}

static void dive_recoil(Fighter *fighter) {
  fighter->action = ACTION_RETAIL_1006;
  retail_vm_enter_owned(fighter, 0x4e8298, fighter_ready, 5);
}

static void dive_query(Fighter *fighter) {
  /* 4704d0 checks the floor before each subsequent D[15] query. */
  if (!fighter_airborne(fighter)) {
    fujin_dive_land(fighter);
    return;
  }
  if (battle_contact(fighter, &dive_contact) != CONTACT_MISS) {
    dive_recoil(fighter);
  }
}

void fujin_dive_contact(Fighter *fighter) {
  /* 47bf90 selects4e8280:60 visits,D[15],contact/timeout47bfb0,
   * floor47bfd0. The clip holds its final pose while motion continues. */
  native_begin(fighter, 1, 60, dive_recoil);
  for (int frame = 1; frame <= 60; ++frame) {
    native_frame(fighter, frame, 9, dive_query);
  }
  native_play(fighter, false);
}

void fujin_dive(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &dive);
  fighter->body.physics = fighter->body.on_land = NULL;
  fighter->mmove_cleanup = dive_cleanup;
  retail_vm_enter_owned(fighter, 0x4e8258, fighter_ready, 5);
}
