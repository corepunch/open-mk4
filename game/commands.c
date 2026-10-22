#include "game.h"
#include "native.h"
#include "retail_data.h"
#include "retail_native.h"
#include "retail_vm.h"
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

Fighter *fighter_peer(Fighter *fighter) {
  return (Fighter *)fighter_opponent(fighter);
}

int fighter_victim_action(const Fighter *fighter) {
  return fighter_opponent(fighter)->action;
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
  fighter_execution_cancel(fighter);
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

static FighterFn match_command(const Fighter *fighter, Button button, bool released) {
  const Mk4Image *image = battle_data();
  unsigned player = fighter == &g_battle.fighters[1] ? 1u : 0u;
  RetailCommandCursor cursor = retail_command_cursor(image, (unsigned)fighter->roster, player, button, released);
  RetailCommand command;
  while (retail_command_next(image, &cursor, &command)) {
    if (command.pattern != 0xffffffffu) {
      CommandToken tokens[COMMAND_TOKENS];
      unsigned count = 0;
      for (uint32_t pattern = command.pattern; pattern; pattern >>= 8) {
        require(count < COMMAND_TOKENS, "Retail command pattern exceeds input history");
        switch (pattern & 255u) {
          case 1:
            tokens[count++] = TOKEN_UP;
            break;
          case 2:
            tokens[count++] = TOKEN_DOWN;
            break;
          case 0xaa:
            tokens[count++] = TOKEN_BACK;
            break;
          case 0xdd:
            tokens[count++] = TOKEN_FORWARD;
            break;
          default:
            fatal("Invalid retail command direction %02x", pattern & 255u);
        }
      }
      bool same = true;
      for (unsigned k = 0; k < count && same; ++k) {
        same = fighter->directions.token[count - 1 - k] == resolve_token(fighter, tokens[k]) + 1;
      }
      if (!same) {
        continue;
      }
      if (!(command.flags & CMD_BLOCK_OK) && (fighter->held & BUTTON_BIT(BUTTON_BLOCK))) {
        continue;
      }
      /* EventPacketDecoder: a late matching pattern ends the whole list. */
      if (command.window > 0 && g_battle.ticks - fighter->directions.tick[count - 1] > command.window) {
        return NULL;
      }
    }
    if (!(command.flags & CMD_AIR) && !(fighter->flags & FF_FOLLOW_UP) && fighter_airborne(fighter)) {
      continue;
    }
    if (fighter->flags & (FF_BUSY | FF_REACTING)) {
      return NULL;
    }
    if (!retail_native_gate(fighter, command.predicate)) {
      continue;
    }
    return retail_native_entry(command.target);
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
    FighterFn command = match_command(fighter, button, false);
    push_direction(&fighter->directions, button, g_battle.ticks);
    if (command != NULL) {
      fighter_interrupt(fighter, command);
    }
    return;
  }
  /* HP, LP, BLK and LK latch before matching, RUN after, HK never. */
  if (button != BUTTON_HIGH_KICK && button != BUTTON_RUN) {
    fighter->latch[button] = g_battle.ticks;
  }
  fighter->buffered |= BUTTON_BIT(button);
  FighterFn command = match_command(fighter, button, false);
  if (command != NULL) {
    fighter_interrupt(fighter, command);
  } else if ((g_battle.phase == PHASE_FIGHT || battle_can_finish(fighter)) && fighter->buttons != NULL) {
    retail_press_table(fighter, button);
  }
  if (button == BUTTON_RUN) {
    fighter->latch[button] = g_battle.ticks;
  }
}

/* LK release has no stance fallback and never rewrites the press latch.
 * sample_switches preserves the retail word-B press/release priority. */
void fighter_release(Fighter *fighter, Button button) {
  FighterFn command = match_command(fighter, button, true);
  if (command != NULL) {
    fighter_interrupt(fighter, command);
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
