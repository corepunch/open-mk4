#include "game.h"
#include "math_util.h"
#include <math.h>

/* Retail press handling, docs/scorpion/input-and-commands.md sections 3-6.
 * A press first walks the character's special-command records for that
 * button (command_match 0x45de00). A match interrupts the fighter. Otherwise
 * the current stance table handles the press. Either way, the selected entry
 * starts on the next tick, after the interrupted task is rescheduled. */

const Fighter *fighter_opponent(const Battle *battle, const Fighter *fighter) {
  return fighter == &battle->fighters[0] ? &battle->fighters[1] : &battle->fighters[0];
}

float fighter_distance(const Battle *battle, const Fighter *fighter) {
  return fabsf(fighter_x(fighter_opponent(battle, fighter)) - fighter_x(fighter));
}

/* 0x48ebd0: the raw switch pointing away from the opponent is held. */
bool fighter_holds_back(const Battle *battle, const Fighter *fighter) {
  bool facing_left = fighter_opponent(battle, fighter)->body.pos[AXIS_X] < fighter->body.pos[AXIS_X];
  return (fighter->held & BUTTON_BIT(facing_left ? BUTTON_RIGHT : BUTTON_LEFT)) != 0;
}

/* 0x45dc00 begin-action: lock the button table and mark the fighter busy
 * until the entry or its stance releases them. */
void fighter_interrupt(Fighter *fighter, FighterFn entry) {
  require(entry != NULL, "Interrupt without an entry");
  fighter->pending = entry;
  fighter->buttons = NULL;
  fighter->flags |= FF_BUSY;
}

static Button resolve_token(const Battle *battle, const Fighter *fighter, CommandToken token) {
  bool facing_left = fighter_opponent(battle, fighter)->body.pos[AXIS_X] < fighter->body.pos[AXIS_X];
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

static const CommandDef *match_command(const Battle *battle, const Fighter *fighter, Button button) {
  const Controls *controls = roster_def(fighter->roster)->controls;
  for (int i = 0; i < controls->command_count; ++i) {
    const CommandDef *command = &controls->commands[i];
    if (command->button != button) {
      continue;
    }
    if (command->count > 0) {
      bool same = true;
      require(command->count <= COMMAND_TOKENS && command->count <= INPUT_HISTORY, "Command pattern too long");
      for (int k = 0; k < command->count && same; ++k) {
        same = fighter->directions.token[command->count - 1 - k] == resolve_token(battle, fighter, command->tokens[k]) + 1;
      }
      if (!same) {
        continue;
      }
      if (!(command->flags & CMD_BLOCK_OK) && (fighter->held & BUTTON_BIT(BUTTON_BLOCK))) {
        continue;
      }
      /* A late pattern ends the whole list, hiding later records. */
      if (command->window > 0 && battle->ticks - fighter->directions.tick[command->count - 1] > (uint64_t)command->window) {
        return NULL;
      }
    }
    if (!(command->flags & CMD_AIR) && !(fighter->flags & FF_FOLLOW_UP) && fighter_airborne(fighter)) {
      continue;
    }
    if (fighter->flags & (FF_BUSY | FF_REACTING)) {
      return NULL;
    }
    if (command->predicate != NULL && !command->predicate(battle, fighter)) {
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

void fighter_press(Battle *battle, Fighter *fighter, Button button) {
  if (button <= BUTTON_RIGHT) {
    push_direction(&fighter->directions, button, battle->ticks);
    return;
  }
  /* HP, LP, BLK and LK latch before matching, RUN after, HK never. */
  if (button != BUTTON_HIGH_KICK && button != BUTTON_RUN) {
    fighter->latch[button] = battle->ticks;
  }
  fighter->buffered |= BUTTON_BIT(button);
  const CommandDef *command = match_command(battle, fighter, button);
  if (command != NULL) {
    fighter_interrupt(fighter, command->enter);
  } else if (fighter->buttons != NULL && fighter->buttons->press[button] != NULL) {
    fighter->buttons->press[button](battle, fighter);
  }
  if (button == BUTTON_RUN) {
    fighter->latch[button] = battle->ticks;
  }
}

void fighter_enter_stance(Fighter *fighter, Stance stance) {
  require(stance >= 0 && stance < STANCE_COUNT, "Invalid stance");
  fighter->buttons = roster_def(fighter->roster)->controls->stances[stance];
  fighter->flags &= ~(unsigned)FF_BUSY;
}
