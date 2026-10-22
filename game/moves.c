#include "game.h"
#include "native.h"
#include "retail_vm.h"
#include "math_util.h"
#include <math.h>
#include <stdlib.h>

/* Script opcode 45 / 45b650: airborne targets use their reaction flag;
 * grounded targets use the attacker's follow-up window. */
bool fighter_fast_start(const Fighter *fighter) {
  const Fighter *victim = fighter_opponent(fighter);
  return fighter_airborne(victim) ? (victim->flags & FF_REACTING) != 0 : (fighter->flags & FF_FOLLOW_UP) != 0;
}

void fighter_ready(Fighter *fighter) {
  if (fighter_airborne(fighter)) {
    fighter_start_move(fighter, S_JUMP, roster_def(fighter->roster)->moves[S_JUMP]);
  } else {
    fighter_enter_state(fighter, S_IDLE);
  }
}

/* Retail action codes (+0x74) of the generic states. The stance reset leaves
 * 0 while standing; moves with recovered entries name their own code. */
// clang-format off: Keep table entries on one row, including long rows.
static const int default_action[S_COUNT] = {
  [S_CROUCH] = 0x2001,
  [S_JUMP] = 0x2007,
  [S_PUNCH] = 0x10b,
  [S_KICK] = 0x100,
  [S_SWEEP] = 0x10c,
  [S_UPPERCUT] = 0x109,
  [S_THROW] = 0x500,
  [S_BLOCK] = 0x2005,
  [S_HIT] = 0x4001,
  [S_KNOCKDOWN] = 0x4003,
};
// clang-format on

/* 490720/490e30/4702b0: restore the model orientation and root offset
 * when replacing a somersault with another air action. */
void fighter_upright(Fighter *fighter) {
  for (int i = 0; i < 3; ++i) {
    fighter->body.angle[i] = fighter->body.spin[i] = 0;
  }
  fighter->body.pos[AXIS_Y] += fighter->body.root_y;
  fighter->body.root_y = 0;
}

int fighter_selected_roster(const Fighter *fighter) {
  return fighter->morphed ? fighter->selected_roster : fighter->roster;
}

/* Retail controllers read the live character from task +0x34 (commands,
 * stance and move tables, CPU rows, rig and voices). 469ae0 rewrites it
 * during a round; 4233e0/423460 restore the selection at the next round.
 * Only the per-rig standing ground changes with it; combat, weapon and
 * stamp state stay with the actor. */
void fighter_change_character(Fighter *fighter, int roster) {
  const CharacterDef *def = roster_def(roster);
  bool grounded = !fighter_airborne(fighter);
  int selected = fighter_selected_roster(fighter);
  fighter->selected_roster = selected;
  fighter->morphed = roster != selected;
  fighter->roster = roster;
  fighter->body.ground = def->ground;
  if (grounded) {
    fighter->body.pos[AXIS_Y] = def->ground;
  }
}

/* 48e080 -> 48e1e0: a morphed actor restores its selected character at
 * a neutral reset once a round winner exists, or 600 ticks (0x258) after
 * the morph stamp. Retail queues indexed cue 0x2c, Shinnok's name entry
 * 4ead30[38 + 6]; the restored character's own name entry generalizes it. */
static void fighter_morph_expiry(Fighter *fighter) {
  if (!fighter->morphed || (g_battle.ko_winner == 0 && g_battle.ticks - fighter->morph_tick < 600)) {
    return;
  }
  fighter_change_character(fighter, fighter->selected_roster);
  battle_sound(SOUND_INDEXED, 38 + fighter->roster);
}

/* 0x46f650 / 0x48e080: return to neutral control. Grounded fighters are
 * snapped to the floor and stopped; combo counters and damage scaling reset. */
void fighter_neutral(Fighter *fighter) {
  fighter_morph_expiry(fighter);
  fighter->hits = 0;
  fighter->damage_total = 0;
  fighter->damage_scale = Q16_ONE;
  fighter->flags &= ~(unsigned)(FF_NEUTRAL_CLEAR | FF_REACTING);
  fighter->body.physics = NULL;
  fighter_upright(fighter);
  if (!fighter_airborne(fighter)) {
    fighter->body.pos[AXIS_Y] = fighter->body.ground;
    for (int i = 0; i < 3; ++i) {
      fighter->body.vel[i] = 0;
    }
    fighter->body.gravity = 0;
  }
}

bool fighter_airborne(const Fighter *fighter) {
  return fighter->body.pos[AXIS_Y] < fighter->body.ground;
}

float fighter_x(const Fighter *fighter) {
  return q16_float(fighter->body.pos[AXIS_X]);
}

float fighter_z(const Fighter *fighter) {
  return q16_float(fighter->body.pos[AXIS_Z]);
}

float fighter_height(const Fighter *fighter) {
  return q16_float(fighter->body.ground - fighter->body.pos[AXIS_Y]);
}

/* Task +0x40/+0x44: this fighter's unit heading toward the opponent, as of
 * the previous world pump. */
q16 fighter_axis(const Fighter *fighter, int axis) {
  require(axis == AXIS_X || axis == AXIS_Z, "Heading has only X and Z");
  q16 value = g_battle.axis[axis == AXIS_X ? 0 : 1];
  return fighter == &g_battle.fighters[0] ? value : -value;
}

/* 0x490390: horizontal velocity = heading * speed; negative moves away. */
void fighter_set_speed(Fighter *fighter, q16 speed) {
  fighter->body.vel[AXIS_X] = q16_mul(fighter_axis(fighter, AXIS_X), speed);
  fighter->body.vel[AXIS_Z] = q16_mul(fighter_axis(fighter, AXIS_Z), speed);
}

/* 0x491050: horizontal velocity along the pair axis, positive toward the
 * screen's right. Retail orients the axis toward +X and reverses it when
 * camera flag 0x534af0 (4895ca) puts the camera behind the pair line; the
 * follow-camera heading answers the same side test. */
void fighter_set_screen_speed(Fighter *fighter, q16 speed) {
  q16 x = fighter_axis(fighter, AXIS_X), z = fighter_axis(fighter, AXIS_Z);
  if (x < 0) {
    x = -x;
    z = -z;
  }
  float right_x = -g_battle.view.z, right_z = g_battle.view.x;
  if (q16_float(x) * right_x + q16_float(z) * right_z < 0) {
    speed = -speed;
  }
  fighter->body.vel[AXIS_X] = q16_mul(x, speed);
  fighter->body.vel[AXIS_Z] = q16_mul(z, speed);
}

void fighter_start_move(Fighter *fighter, int state, const MoveDef *move) {
  require(move != NULL && state >= 0 && state < S_COUNT, "Invalid move entry");
  fighter_execution_cancel(fighter);
  native_motion_stop(fighter);
  if (fighter->state == S_WALK && !fighter_airborne(fighter) && (move->flags & SF_LOCK)) {
    fighter_set_speed(fighter, 0);
  }
  fighter->body.walking = false;
  bool was_locked = fighter->current_move != NULL && (fighter->current_move->flags & SF_LOCK);
  fighter->state = state;
  if (fighter->appearance == APPEARANCE_FROZEN || fighter->action == 0x1003) {
    fighter->appearance = APPEARANCE_NORMAL;
    fighter->frozen_clip = 0;
    fighter->flags &= ~(unsigned)FF_NO_GUARD;
  }
  fighter->frozen_clip = 0;
  memset(fighter->visual_offset, 0, sizeof fighter->visual_offset);
  fighter->flags &= ~(unsigned)(FF_PAIRED | FF_PAIR_RELEASE);
  fighter->current_move = move;
  fighter->execution.frames = NULL;
  fighter->execution.frame = 0;
  fighter->execution.nextframe = -1;
  fighter->execution.hold = false;
  fighter->execution.generation++;
  fighter->ticks = 0;
  fighter->connected = false;
  fighter->avoids_high = false;
  fighter->buffered = 0;
  fighter->contact = CONTACT_MISS;
  fighter->contact_tick = -1;
  fighter->pending = NULL;
  fighter->sample_override = -1;
  fighter->script_phase = 0;
  fighter->weapon_strike = NULL;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
  fighter->scene = (ScenePlayback){0};
  if (was_locked && !(move->flags & SF_LOCK)) {
    int roster = fighter->roster;
    fighter_neutral(fighter);
    if (fighter->roster != roster) {
      /* The neutral reset restored the selected character; enter this
       * state through that character's own definition. */
      fighter->current_move = NULL;
      fighter_enter_state(fighter, state);
      return;
    }
  }
  fighter->action = move->action != 0 ? move->action : default_action[state];
  if (move->flags & SF_LOCK) {
    fighter->buttons = NULL;
    fighter->flags |= FF_BUSY;
  } else if (state == S_CROUCH) {
    fighter_enter_stance(fighter, STANCE_CROUCHING);
  } else if (state == S_JUMP) {
    fighter_enter_stance(fighter, STANCE_JUMP);
  } else if (state == S_IDLE || state == S_WALK) {
    fighter_enter_stance(fighter, STANCE_STANDING);
  } else {
    /* Block and the win pose lock the buttons but leave commands enabled. */
    fighter->buttons = NULL;
    fighter->flags &= ~(unsigned)FF_BUSY;
  }
}

void fighter_note_contact(Fighter *fighter, ContactResult result) {
  require(result >= CONTACT_BLOCK && result <= CONTACT_AIR_HIT, "Invalid contact result");
  fighter->connected = true;
  fighter->contact = result;
  fighter->contact_tick = fighter->ticks;
}

void fighter_enter_state(Fighter *fighter, int state) {
  const CharacterDef *def = roster_def(fighter->roster);
  require(state >= 0 && state < def->move_count, "Invalid character state");
  if (fighter->state == state && fighter->current_move != NULL) {
    return;
  }
  if (def->enter != NULL && def->enter[state] != NULL) {
    def->enter[state](fighter);
  } else {
    fighter_start_move(fighter, state, def->moves[state]);
  }
}

static int mmove_sample(const Fighter *fighter) {
  const mmove_t *move = fighter->execution.frames;
  return move->samples != NULL ? move->samples[fighter->execution.frame - move->firstframe] : fighter->execution.frame;
}

/* Explicit script/native sample, or the engine state tick otherwise. */
int fighter_sample(const Fighter *fighter) {
  return fighter->sample_override >= 0 ? fighter->sample_override : fighter->execution.frames != NULL ? mmove_sample(fighter) : fighter->ticks;
}

double fighter_frame(const Fighter *fighter, int frame_count) {
  const MoveDef *move = fighter_move(fighter);
  require(frame_count > 0, "Empty move clip");
  if (fighter->sample_override >= 0) {
    return mini(fighter->sample_override, frame_count - 1);
  }
  if (fighter->execution.frames != NULL) {
    int sample = mmove_sample(fighter);
    require(sample >= 0 && sample < frame_count, "Mmove sample outside clip");
    return sample;
  }
  /* Legacy approximation, pending recovery of the other move scripts. */
  int end = frame_count - 1;
  int first = mini(move->first, end);
  int last = move->last == MOVE_END ? end : mini(move->last, end);
  double span = (double)abs(last - first);
  double at = (double)fighter->ticks * 0.5;
  if (move->flags & SF_ATTACK) {
    double t = (double)maxi(0, fighter->ticks);
    double progress = t < move->startup                  ? 0.4 * t / move->startup
                      : t < move->startup + move->active ? 0.4 + 0.2 * (t - move->startup) / move->active
                                                         : 0.6 + 0.4 * (t - move->startup - move->active) / move->recovery;
    at = clampd(progress, 0.0, 1.0) * span;
  } else if (fighter->state == S_KNOCKDOWN) {
    at = fmin((double)maxi(0, fighter->ticks) * 0.65, span);
  } else if (move->flags & SF_HOLD) {
    at = fmin(at, span);
  } else {
    at = fmod(fmax(0., at), span + 1.0);
  }
  return last < first ? (double)first - at : (double)first + at;
}
