#include "native_services.h"

#include <math.h>

/* B,F+LK 478ee0: a native-only circling step behind the peer, with no
 * contact query. Entry stamps slot 17, stores action 0x2014, plays C[0x63]
 * and binds clip 306 (53cf38) at pose 0. 4245a0 takes the angle from this
 * fighter to the peer and adds pi (4d0320); 487e20 then places the body at
 * peer + radius * (cos, sin) on twenty visits. The angle turns -0x283a per
 * visit while the radius shrinks linearly from the distance towards 1.2. */
static const MoveDef s_orbit = {.clip = 306, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x2014};
static const MoveDef s_orbit_stance = {.clip = 284, .last = 39, .flags = SF_LOCK, .action = 0x2014};

/* 45e710: at least 1.3 units apart, slot 17 at least 80 ticks old, the peer
 * not near the wall unless it is nearer the centre than this fighter
 * (48f100), and the peer not in flip-kick action 0x21e (45f540). */
bool reiko_can_orbit(const Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  int64_t radius2 = ((int64_t)g_battle.arena_radius * g_battle.arena_radius) >> 16;
  int64_t own2 =
      (((int64_t)fighter->body.pos[AXIS_X] * fighter->body.pos[AXIS_X]) >> 16) + (((int64_t)fighter->body.pos[AXIS_Z] * fighter->body.pos[AXIS_Z]) >> 16);
  int64_t peer2 = (((int64_t)peer->body.pos[AXIS_X] * peer->body.pos[AXIS_X]) >> 16) + (((int64_t)peer->body.pos[AXIS_Z] * peer->body.pos[AXIS_Z]) >> 16);
  bool clear = peer2 < own2 || peer2 <= radius2 - 10 * Q16_ONE;
  return g_battle.phase == PHASE_FIGHT && fighter_distance(fighter) >= q16_float(0x14ccc) && mk4_special_elapsed(fighter, 0x11, 80) && clear &&
         peer->action != 0x21e;
}

static void orbit_place(Fighter *fighter) {
  /* saved_motion holds the current angle, radius and per-visit decrement. */
  const Fighter *peer = fighter_opponent(fighter);
  double radians = q16_float(fighter->saved_motion[0]);
  q16 radius = fighter->saved_motion[1];
  fighter->body.pos[AXIS_X] = peer->body.pos[AXIS_X] + (q16)llround(cos(radians) * radius);
  fighter->body.pos[AXIS_Z] = peer->body.pos[AXIS_Z] + (q16)llround(sin(radians) * radius);
  fighter->saved_motion[0] -= 0x283a;
  fighter->saved_motion[1] = radius - fighter->saved_motion[2];
}

/* 478f49: face the peer (490bd0), show the stance clip (4611c0 ->
 * 490f60/53d408), wait twenty visits, then 470420 stops and returns. */
static void orbit_finish(Fighter *fighter) {
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter_start_move(fighter, S_KICK, &s_orbit_stance);
  fighter->sample_override = 0;
  native_hold(fighter, 20, mk4_mmove_thrown_finish);
}

void reiko_orbit(Fighter *fighter) {
  const Fighter *peer = fighter_opponent(fighter);
  mk4_special_stamp(fighter, 0x11);
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &s_orbit);
  fighter->sample_override = 0;
  battle_sound(SOUND_C, 0x63);
  double dx = (double)peer->body.pos[AXIS_X] - fighter->body.pos[AXIS_X];
  double dz = (double)peer->body.pos[AXIS_Z] - fighter->body.pos[AXIS_Z];
  q16 distance = g_battle.distance;
  fighter->saved_motion[0] = (q16)llround(atan2(dz, dx) * Q16_ONE) + 0x3243f;
  fighter->saved_motion[1] = distance;
  fighter->saved_motion[2] = q16_mul(distance - 0x13333, 0xccc);
  native_begin(fighter, 0, 19, orbit_finish);
  for (int frame = 0; frame < 20; ++frame) {
    native_frame(fighter, frame, 0, orbit_place);
  }
  native_play(fighter, true);
}
