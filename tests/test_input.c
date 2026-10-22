#include "input.h"
#include "sdl_input.h"
#include <stdio.h>
#include <stdlib.h>

static void check(bool condition, const char *message) {
  if (!condition) {
    fprintf(stderr, "FAIL input: %s (%s)\n", message, SDL_GetError());
    exit(1);
  }
}

static InputCommands key(HostInput *host, SDL_Scancode code, SDL_Keycode symbol, bool down) {
  SDL_Event event = {.type = down ? SDL_KEYDOWN : SDL_KEYUP};
  event.key.keysym.scancode = code;
  event.key.keysym.sym = symbol;
  return host_input_event(host, &event);
}

static InputCommands button(HostInput *host, int player, int code, bool down) {
  SDL_Event event = {.type = down ? SDL_CONTROLLERBUTTONDOWN : SDL_CONTROLLERBUTTONUP};
  event.cbutton.which = host->players[player].instance;
  event.cbutton.button = code;
  return host_input_event(host, &event);
}

static InputFrame consume(HostInput *host, int player) {
  InputFrame frames[2];
  host_input_consume(host, frames);
  return frames[player];
}

int main(void) {
  InputBuffer buffer = {0};
  uint32_t punch = BUTTON_BIT(BUTTON_HIGH_PUNCH);
  input_update(&buffer, punch);
  input_update(&buffer, 0);
  check(input_consume(&buffer).held == punch, "a short tap survives until a simulation tick");
  check(input_consume(&buffer).held == 0, "catch-up ticks do not repeat a released tap");
  input_update(&buffer, punch);
  input_suppress(&buffer);
  check(input_consume(&buffer).held == 0, "shell transitions suppress held buttons and queued taps");
  input_update(&buffer, 0);
  input_update(&buffer, punch);
  check(input_consume(&buffer).held == punch, "release rearms a suppressed button");
  input_update(&buffer, BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(BUTTON_RIGHT));
  input_consume(&buffer);
  check(input_consume(&buffer).held == 0, "opposite directions cancel");

  check(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) == 0, "SDL input initialization");
  int device = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, SDL_CONTROLLER_AXIS_MAX, SDL_CONTROLLER_BUTTON_MAX, 0);
  check(device >= 0, "virtual gamepad attaches");
  HostInput host;
  host_input_init(&host);
  check(host.players[0].controller != NULL, "gamepad receives first player slot");
  check(button(&host, 0, SDL_CONTROLLER_BUTTON_A, true).menu == MENU_CONFIRM, "gamepad emits semantic menu action");
  button(&host, 0, SDL_CONTROLLER_BUTTON_A, false);
  check(input_held(consume(&host, 0), BUTTON_LOW_KICK), "gamepad short tap uses the same frame contract");
  check(consume(&host, 0).held == 0, "gamepad tap is consumed once");

  key(&host, SDL_SCANCODE_U, SDLK_u, true);
  button(&host, 0, SDL_CONTROLLER_BUTTON_Y, true);
  check(input_held(consume(&host, 0), BUTTON_HIGH_PUNCH), "keyboard and pad combine");
  key(&host, SDL_SCANCODE_U, SDLK_u, false);
  check(input_held(consume(&host, 0), BUTTON_HIGH_PUNCH), "releasing one source preserves another source's hold");
  host_input_suppress(&host);
  check(consume(&host, 0).held == 0, "gamepad holds obey transition suppression");
  button(&host, 0, SDL_CONTROLLER_BUTTON_Y, false);
  button(&host, 0, SDL_CONTROLLER_BUTTON_Y, true);
  check(input_held(consume(&host, 0), BUTTON_HIGH_PUNCH), "gamepad release rearms suppression");

  SDL_Event axis = {.type = SDL_CONTROLLERAXISMOTION};
  axis.caxis.which = host.players[0].instance;
  axis.caxis.axis = SDL_CONTROLLER_AXIS_LEFTX;
  axis.caxis.value = 2000;
  host_input_event(&host, &axis);
  check(!input_held(consume(&host, 0), BUTTON_RIGHT), "stick drift stays inside dead zone");
  axis.caxis.value = 20000;
  check(host_input_event(&host, &axis).menu == MENU_RIGHT, "stick emits semantic direction");
  check(input_held(consume(&host, 0), BUTTON_RIGHT), "stick maps to absolute game direction");

  key(&host, SDL_SCANCODE_LEFT, SDLK_LEFT, true);
  check(input_held(consume(&host, 1), BUTTON_LEFT), "second keyboard player remains independent");
  SDL_Event lost = {.type = SDL_WINDOWEVENT};
  lost.window.event = SDL_WINDOWEVENT_FOCUS_LOST;
  host_input_event(&host, &lost);
  check(consume(&host, 0).held == 0 && consume(&host, 1).held == 0, "focus loss clears keyboard and suppresses pad");

  SDL_Event removed = {.type = SDL_CONTROLLERDEVICEREMOVED};
  removed.cdevice.which = host.players[0].instance;
  check(SDL_JoystickDetachVirtual(device) == 0, "virtual gamepad detaches");
  host_input_event(&host, &removed);
  check(host.players[0].controller == NULL && consume(&host, 0).held == 0, "disconnect clears held and queued pad input");
  check(key(&host, SDL_SCANCODE_RETURN, SDLK_RETURN, true).menu == MENU_CONFIRM, "keyboard emits semantic menu action");
  host_input_free(&host);
  SDL_Quit();
  puts("PASS: tick input buffering, source merging, virtual gamepad, menu actions and input lifetime");
  return 0;
}
