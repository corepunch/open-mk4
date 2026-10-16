#include "game.h"
#include "math_util.h"
#include <math.h>
#include <stdlib.h>

static void frame_event(Battle *battle, Fighter *fighter) {
  const MoveSequence *sequence = fighter_move(fighter)->sequence;
  if (sequence != NULL) {
    for (int i = 0; i < sequence->event_count; ++i) {
      const MoveEvent *event = &sequence->events[i];
      if (event->at == fighter->ticks) {
        event->think(battle, fighter);
        if (fighter_move(fighter)->sequence != sequence) {
          return;
        }
      }
    }
  }
}

void fighter_start_move(Battle *battle, Fighter *fighter, int state, const MoveDef *move) {
  require(move != NULL && state >= 0 && state < S_COUNT, "Invalid move entry");
  if (move->sequence != NULL) {
    const MoveSequence *sequence = move->sequence;
    require(sequence->segments != NULL && sequence->segment_count > 0 && sequence->count > 0 &&
                sequence->end != NULL && sequence->segments[0].at == 0 &&
                sequence->event_count >= 0 &&
                (sequence->event_count == 0 || sequence->events != NULL),
        "Invalid move sequence");
    for (int i = 0; i < sequence->segment_count; ++i) {
      const MoveSegment *segment = &sequence->segments[i];
      require(segment->period > 0 && segment->at >= 0 && segment->at < sequence->count &&
                  (i == 0 || sequence->segments[i - 1].at < segment->at),
          "Invalid playback segment");
    }
    for (int i = 0; i < sequence->event_count; ++i) {
      const MoveEvent *event = &sequence->events[i];
      require(event->at >= 0 && event->at < sequence->count && event->think != NULL &&
                  (i == 0 || sequence->events[i - 1].at <= event->at),
          "Invalid move event");
    }
  }
  fighter->state = state;
  fighter->current_move = move;
  fighter->ticks = 0;
  fighter->connected = false;
  frame_event(battle, fighter);
}

void fighter_enter_state(Battle *battle, Fighter *fighter, int state) {
  const CharacterDef *def = roster_def(fighter->roster);
  require(state >= 0 && state < def->move_count, "Invalid character state");
  if (fighter->state == state && fighter->current_move != NULL) {
    return;
  }
  if (def->enter != NULL && def->enter[state] != NULL) {
    def->enter[state](battle, fighter);
  } else {
    fighter_start_move(battle, fighter, state, def->moves[state]);
  }
}

void fighter_step_sequence(Battle *battle, Fighter *fighter) {
  const MoveSequence *sequence = fighter_move(fighter)->sequence;
  require(sequence != NULL, "Move has no sequence");
  if (fighter->ticks >= sequence->count) {
    sequence->end(battle, fighter);
    return;
  }
  frame_event(battle, fighter);
}

double fighter_frame(const Fighter *fighter, int frame_count) {
  const MoveDef *move = fighter_move(fighter);
  require(frame_count > 0, "Empty move clip");
  if (move->sequence != NULL) {
    const MoveSequence *sequence = move->sequence;
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
    double progress = t < move->startup ? 0.4 * t / move->startup
                      : t < move->startup + move->active
                          ? 0.4 + 0.2 * (t - move->startup) / move->active
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
