#include "game.h"
#include "math_util.h"
#include <math.h>
#include <stdlib.h>

static void frame_event(Fighter *fighter) {
  const MoveSequence *sequence = fighter->sequence;
  uint64_t continuation = fighter->continuation;
  if (sequence != NULL && fighter->sequence_event_tick != fighter->ticks) {
    fighter->sequence_event_tick = fighter->ticks;
    for (int i = 0; i < sequence->event_count; ++i) {
      const MoveEvent *event = &sequence->events[i];
      if (event->at == fighter->ticks) {
        event->think(fighter);
        if (fighter->continuation != continuation || fighter->pending != NULL) {
          return;
        }
      }
    }
  }
}

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

/* 0x46f650 / 0x48e080: return to neutral control. Grounded fighters are
 * snapped to the floor and stopped; combo counters and damage scaling reset. */
void fighter_neutral(Fighter *fighter) {
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

void fighter_start_move(Fighter *fighter, int state, const MoveDef *move) {
  require(move != NULL && state >= 0 && state < S_COUNT, "Invalid move entry");
  fighter_cancel_fiber(fighter);
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
  fighter->flags &= ~(unsigned)FF_PAIRED;
  fighter->current_move = move;
  fighter->sequence = NULL;
  fighter->continuation++;
  fighter->ticks = 0;
  fighter->connected = false;
  fighter->avoids_high = false;
  fighter->buffered = 0;
  fighter->contact = CONTACT_MISS;
  fighter->contact_tick = -1;
  fighter->pending = NULL;
  fighter->script = NULL;
  fighter->sample_override = -1;
  fighter->script_phase = 0;
  fighter->script_timer = 0;
  fighter->weapon_strike = NULL;
  fighter->weapon_travel = false;
  fighter->blend_duration = 0;
  fighter->blend_weight = -1;
  fighter->scene = (ScenePlayback){0};
  if (was_locked && !(move->flags & SF_LOCK)) {
    fighter_neutral(fighter);
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
  if (move->sequence != NULL) {
    fighter_start_sequence(fighter, move->sequence);
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

/* A phase owns its tick clock and continuation, but retains the move's
 * combat identity, action, contact result and input restrictions. */
void fighter_start_sequence(Fighter *fighter, const MoveSequence *sequence) {
  fighter_cancel_fiber(fighter);
  require(sequence != NULL && sequence->segments != NULL && sequence->segment_count > 0 && sequence->count > 0 && sequence->end != NULL &&
              sequence->segments[0].at == 0 && sequence->event_count >= 0 && (sequence->event_count == 0 || sequence->events != NULL),
      "Invalid move phase");
  for (int i = 0; i < sequence->segment_count; ++i) {
    const MoveSegment *segment = &sequence->segments[i];
    require(segment->period > 0 && segment->at >= 0 && segment->at < sequence->count && (i == 0 || sequence->segments[i - 1].at < segment->at),
        "Invalid phase segment");
  }
  for (int i = 0; i < sequence->event_count; ++i) {
    const MoveEvent *event = &sequence->events[i];
    require(
        event->at >= 0 && event->at < sequence->count && event->think != NULL && (i == 0 || sequence->events[i - 1].at <= event->at), "Invalid phase event");
  }
  fighter->sequence = sequence;
  fighter->continuation++;
  fighter->ticks = 0;
  fighter->sequence_event_tick = -1;
  fighter->sequence_finished = false;
  fighter->script = NULL;
  fighter->sample_override = -1;
  frame_event(fighter);
}

void fighter_step_sequence(Fighter *fighter) {
  const MoveSequence *sequence = fighter->sequence;
  require(sequence != NULL, "Move has no sequence");
  if (fighter->sequence_finished || fighter->pending != NULL) {
    return;
  }
  if (fighter->ticks >= sequence->count) {
    if (sequence->until == NULL || sequence->until(fighter)) {
      fighter->sequence_finished = true;
      sequence->end(fighter);
    }
    return;
  }
  frame_event(fighter);
}

/* The displayed sample of a recovered sequence, or the move tick otherwise. */
int fighter_sample(const Fighter *fighter) {
  return fighter->sample_override >= 0 ? fighter->sample_override : fighter->sequence != NULL ? (int)fighter_frame(fighter, 1 << 16) : fighter->ticks;
}

double fighter_frame(const Fighter *fighter, int frame_count) {
  const MoveDef *move = fighter_move(fighter);
  require(frame_count > 0, "Empty move clip");
  if (fighter->sample_override >= 0) {
    return mini(fighter->sample_override, frame_count - 1);
  }
  if (fighter->sequence != NULL) {
    const MoveSequence *sequence = fighter->sequence;
    int index = mini(maxi(fighter->ticks, 0), sequence->count - 1);
    const MoveSegment *segment = &sequence->segments[0];
    for (int i = 1; i < sequence->segment_count && sequence->segments[i].at <= index; ++i) {
      segment = &sequence->segments[i];
    }
    int sample = segment->first + segment->step * ((index - segment->at) / segment->period);
    require(sample >= 0 && sample < frame_count, "Move sample outside clip");
    return (double)sample;
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
