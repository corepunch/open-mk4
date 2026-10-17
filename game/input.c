#include "game.h"
#include <SDL.h>
#include <math.h>
#include <string.h>

static void bind(InputDevice *device, Button button, int scancode) {
  device->code[button] = scancode;
}

void input_bind_player(InputDevice *device, int player) {
  int i;
  memset(device, 0, sizeof *device);
  for (i = 0; i < BUTTON_COUNT; ++i)
    device->code[i] = -1;
  if (player == 0) {
    bind(device, BUTTON_LEFT, SDL_SCANCODE_A);
    bind(device, BUTTON_RIGHT, SDL_SCANCODE_D);
    bind(device, BUTTON_UP, SDL_SCANCODE_W);
    bind(device, BUTTON_DOWN, SDL_SCANCODE_S);
    bind(device, BUTTON_HIGH_PUNCH, SDL_SCANCODE_U);
    bind(device, BUTTON_LOW_PUNCH, SDL_SCANCODE_J);
    bind(device, BUTTON_HIGH_KICK, SDL_SCANCODE_I);
    bind(device, BUTTON_LOW_KICK, SDL_SCANCODE_K);
    bind(device, BUTTON_BLOCK, SDL_SCANCODE_L);
    bind(device, BUTTON_RUN, SDL_SCANCODE_H);
    return;
  }
  bind(device, BUTTON_LEFT, SDL_SCANCODE_LEFT);
  bind(device, BUTTON_RIGHT, SDL_SCANCODE_RIGHT);
  bind(device, BUTTON_UP, SDL_SCANCODE_UP);
  bind(device, BUTTON_DOWN, SDL_SCANCODE_DOWN);
  bind(device, BUTTON_HIGH_PUNCH, SDL_SCANCODE_KP_4);
  bind(device, BUTTON_LOW_PUNCH, SDL_SCANCODE_KP_1);
  bind(device, BUTTON_HIGH_KICK, SDL_SCANCODE_KP_5);
  bind(device, BUTTON_LOW_KICK, SDL_SCANCODE_KP_2);
  bind(device, BUTTON_BLOCK, SDL_SCANCODE_KP_3);
  bind(device, BUTTON_RUN, SDL_SCANCODE_RSHIFT);
}

static unsigned read_held(const InputDevice *device, const unsigned char *keys) {
  unsigned held = 0;
  int i;
  for (i = 0; i < BUTTON_COUNT; ++i) {
    int code = device->code[i];
    if (code >= 0 && keys[code])
      held |= BUTTON_BIT(i);
  }
  return held;
}

void input_sample(InputDevice *device, const unsigned char *keys, Input *out) {
  unsigned held = read_held(device, keys);
  device->suppressed &= held;
  out->held = held & ~device->suppressed;
}

/* Keys held through a shell action (reset, fighter change) stay ignored until
 * released, so they cannot become presses in the new match. */
void input_sync(InputDevice *device, const unsigned char *keys) {
  device->suppressed = read_held(device, keys);
}

/* keycode is an SDL_Keycode. Shell keys never share the fighter bitmask. */
ShellCommand input_shell_command(int keycode) {
  switch (keycode) {
    case SDLK_ESCAPE:
      return SHELL_QUIT;
    case SDLK_SPACE:
      return SHELL_PAUSE;
    case SDLK_r:
      return SHELL_RESET;
    case SDLK_RETURN:
      return SHELL_REMATCH;
    case SDLK_F1:
      return SHELL_NEXT_P1;
    case SDLK_F2:
      return SHELL_NEXT_P2;
    case SDLK_F3:
      return SHELL_TOGGLE_CPU;
    case SDLK_F4:
      return SHELL_NEXT_ARENA;
    default:
      return SHELL_NONE;
  }
}

/* CPU controllers run inside simulation. The host only selects ownership. */
void input_cpu(int player, Input *out) {
  memset(out, 0, sizeof *out);
  g_battle.fighters[player].human = false;
}
