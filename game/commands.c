#include "game.h"
#include "native.h"
#include "math_util.h"
#include <math.h>

/* Retail press handling, docs/scorpion/input-and-commands.md sections 3-6.
 * A press first walks the character's special-command records for that
 * button (command_match 0x45de00). A match interrupts the fighter. Otherwise
 * the current stance table handles the press. Either way, the selected entry
 * starts on the next tick, after the interrupted task is rescheduled. */

const Fighter *fighter_opponent(const Fighter *fighter) {
  return fighter == &g_battle.fighters[0] ? &g_battle.fighters[1] : &g_battle.fighters[0];
}

float fighter_distance(const Fighter *fighter) {
  const Fighter *other = fighter_opponent(fighter);
  return hypotf(fighter_x(other) - fighter_x(fighter), fighter_z(other) - fighter_z(fighter));
}

/* 0x48ebd0: the raw switch pointing away from the opponent is held. */
bool fighter_holds_back(const Fighter *fighter) {
  bool facing_left = fighter_opponent(fighter)->body.pos[AXIS_X] < fighter->body.pos[AXIS_X];
  return (fighter->held & BUTTON_BIT(facing_left ? BUTTON_RIGHT : BUTTON_LEFT)) != 0;
}

/* 0x45dc00 begin-action: lock the button table and mark the fighter busy
 * until the entry or its stance releases them. */
void fighter_interrupt(Fighter *fighter, FighterFn entry) {
  require(entry != NULL, "Interrupt without an entry");
  fighter_cancel_fiber(fighter);
  fighter_cancel_mmove(fighter);
  native_motion_stop(fighter);
  fighter->avoids_high = false;
  fighter->pending = entry;
  fighter->buttons = NULL;
  fighter->flags = (fighter->flags | FF_BUSY) & ~(unsigned)FF_PAIRED;
}

static Button resolve_token(const Fighter *fighter, CommandToken token) {
  bool facing_left = fighter_opponent(fighter)->body.pos[AXIS_X] < fighter->body.pos[AXIS_X];
  switch (token) {
    case TOKEN_UP:
      return BUTTON_UP;
    case TOKEN_DOWN:
      return BUTTON_DOWN;
    case TOKEN_BACK:
      return facing_left ? BUTTON_RIGHT : BUTTON_LEFT;
    case TOKEN_FORWARD:
      return facing_left ? BUTTON_LEFT : BUTTON_RIGHT;
  }
  require(false, "Invalid command token");
  return BUTTON_UP;
}

static const CommandDef *match_command(const Fighter *fighter, Button button, bool released) {
  const Controls *controls = roster_def(fighter->roster)->controls;
  for (int i = 0; i < controls->command_count; ++i) {
    const CommandDef *command = &controls->commands[i];
    if (command->button != button || !!(command->flags & CMD_RELEASE) != released) {
      continue;
    }
    if (command->count > 0) {
      bool same = true;
      require(command->count <= COMMAND_TOKENS && command->count <= INPUT_HISTORY, "Command pattern too long");
      for (int k = 0; k < command->count && same; ++k) {
        same = fighter->directions.token[command->count - 1 - k] == resolve_token(fighter, command->tokens[k]) + 1;
      }
      if (!same) {
        continue;
      }
      if (!(command->flags & CMD_BLOCK_OK) && (fighter->held & BUTTON_BIT(BUTTON_BLOCK))) {
        continue;
      }
      /* A late pattern ends the whole list, hiding later records. */
      if (command->window > 0 && g_battle.ticks - fighter->directions.tick[command->count - 1] > (uint64_t)command->window) {
        return NULL;
      }
    }
    if (!(command->flags & CMD_AIR) && !(fighter->flags & FF_FOLLOW_UP) && fighter_airborne(fighter)) {
      continue;
    }
    if (fighter->flags & (FF_BUSY | FF_REACTING)) {
      return NULL;
    }
    if (command->predicate != NULL && !command->predicate(fighter)) {
      continue;
    }
    return command;
  }
  return NULL;
}

static void push_direction(InputHistory *history, Button direction, uint64_t tick) {
  for (int i = INPUT_HISTORY - 1; i > 0; --i) {
    history->token[i] = history->token[i - 1];
    history->tick[i] = history->tick[i - 1];
  }
  history->token[0] = (uint8_t)(direction + 1);
  history->tick[0] = tick;
}

void fighter_press(Fighter *fighter, Button button) {
  if (button <= BUTTON_RIGHT) {
    const CommandDef *command = match_command(fighter, button, false);
    push_direction(&fighter->directions, button, g_battle.ticks);
    if (command != NULL) {
      fighter_interrupt(fighter, command->enter);
    }
    return;
  }
  /* HP, LP, BLK and LK latch before matching, RUN after, HK never. */
  if (button != BUTTON_HIGH_KICK && button != BUTTON_RUN) {
    fighter->latch[button] = g_battle.ticks;
  }
  fighter->buffered |= BUTTON_BIT(button);
  const CommandDef *command = match_command(fighter, button, false);
  if (command != NULL) {
    fighter_interrupt(fighter, command->enter);
  } else if ((g_battle.phase == PHASE_FIGHT || battle_can_finish(fighter)) && fighter->buttons != NULL && fighter->buttons->press[button] != NULL) {
    fighter->buttons->press[button](fighter);
  }
  if (button == BUTTON_RUN) {
    fighter->latch[button] = g_battle.ticks;
  }
}

/* LK release has no stance fallback and never rewrites the press latch.
 * sample_switches preserves the retail word-B press/release priority. */
void fighter_release(Fighter *fighter, Button button) {
  const CommandDef *command = match_command(fighter, button, true);
  if (command != NULL) {
    fighter_interrupt(fighter, command->enter);
  }
}

void fighter_enter_stance(Fighter *fighter, Stance stance) {
  require(stance >= 0 && stance < STANCE_COUNT, "Invalid stance");
  const Controls *controls = roster_def(fighter->roster)->controls;
  fighter->buttons = controls->stances[stance];
  if (stance == STANCE_DIAGONAL_AIR && fighter->buttons == NULL) {
    fighter->buttons = controls->stances[STANCE_JUMP];
  }
  fighter->flags &= ~(unsigned)FF_BUSY;
}
