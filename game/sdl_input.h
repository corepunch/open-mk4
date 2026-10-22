#ifndef MK4_SDL_INPUT_H
#define MK4_SDL_INPUT_H
#include "input.h"
#include <SDL.h>

#include "host_commands.h"

typedef struct PlayerInput {
  InputBuffer buffer;
  uint32_t keyboard, buttons, stick;
  SDL_GameController *controller;
  SDL_JoystickID instance;
  int16_t x, y;
} PlayerInput;

typedef struct HostInput {
  PlayerInput players[2];
} HostInput;

void host_input_init(HostInput *input);
void host_input_free(HostInput *input);
InputCommands host_input_event(HostInput *input, const SDL_Event *event);
void host_input_suppress(HostInput *input);
void host_input_consume(HostInput *input, InputFrame frames[2]);
#endif
