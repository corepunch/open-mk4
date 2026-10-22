#include "sdl_input.h"
#include <string.h>

// clang-format off: Keep table entries on one row, including long rows.
static const SDL_Scancode keys[2][BUTTON_COUNT] = {
    {SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_U, SDL_SCANCODE_L, SDL_SCANCODE_I, SDL_SCANCODE_J, SDL_SCANCODE_K, SDL_SCANCODE_H},
    {SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_3, SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_2, SDL_SCANCODE_RSHIFT},
};
// clang-format on

static uint32_t controller_button(int button) {
  switch (button) {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
      return BUTTON_BIT(BUTTON_UP);
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
      return BUTTON_BIT(BUTTON_DOWN);
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
      return BUTTON_BIT(BUTTON_LEFT);
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
      return BUTTON_BIT(BUTTON_RIGHT);
    case SDL_CONTROLLER_BUTTON_Y:
      return BUTTON_BIT(BUTTON_HIGH_PUNCH);
    case SDL_CONTROLLER_BUTTON_X:
      return BUTTON_BIT(BUTTON_LOW_PUNCH);
    case SDL_CONTROLLER_BUTTON_B:
      return BUTTON_BIT(BUTTON_HIGH_KICK);
    case SDL_CONTROLLER_BUTTON_A:
      return BUTTON_BIT(BUTTON_LOW_KICK);
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
      return BUTTON_BIT(BUTTON_BLOCK);
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
      return BUTTON_BIT(BUTTON_RUN);
    default:
      return 0;
  }
}

static void update(PlayerInput *player) {
  input_update(&player->buffer, player->keyboard | player->buttons | player->stick);
}

static void update_stick(PlayerInput *player) {
  /* Digital fighting controls: each axis has a symmetric quarter-range dead zone. */
  const int deadzone = 8192;
  player->stick = 0;
  if (player->x < -deadzone) {
    player->stick |= BUTTON_BIT(BUTTON_LEFT);
  }
  if (player->x > deadzone) {
    player->stick |= BUTTON_BIT(BUTTON_RIGHT);
  }
  if (player->y < -deadzone) {
    player->stick |= BUTTON_BIT(BUTTON_UP);
  }
  if (player->y > deadzone) {
    player->stick |= BUTTON_BIT(BUTTON_DOWN);
  }
  update(player);
}

static void open_controller(HostInput *input, int device) {
  if (!SDL_IsGameController(device)) {
    return;
  }
  SDL_JoystickID instance = SDL_JoystickGetDeviceInstanceID(device);
  for (int i = 0; i < 2; ++i) {
    if (input->players[i].controller && input->players[i].instance == instance) {
      return;
    }
  }
  for (int i = 0; i < 2; ++i) {
    PlayerInput *player = &input->players[i];
    if (player->controller) {
      continue;
    }
    player->controller = SDL_GameControllerOpen(device);
    if (!player->controller) {
      return;
    }
    player->instance = instance;
    for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button) {
      if (SDL_GameControllerGetButton(player->controller, button)) {
        player->buttons |= controller_button(button);
      }
    }
    player->x = SDL_GameControllerGetAxis(player->controller, SDL_CONTROLLER_AXIS_LEFTX);
    player->y = SDL_GameControllerGetAxis(player->controller, SDL_CONTROLLER_AXIS_LEFTY);
    update_stick(player);
    input_suppress(&player->buffer);
    return;
  }
}

void host_input_init(HostInput *input) {
  memset(input, 0, sizeof *input);
  if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
    return;
  }
  for (int device = 0; device < SDL_NumJoysticks(); ++device) {
    open_controller(input, device);
  }
}

void host_input_free(HostInput *input) {
  for (int i = 0; i < 2; ++i) {
    SDL_GameControllerClose(input->players[i].controller);
  }
  memset(input, 0, sizeof *input);
  SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void host_input_suppress(HostInput *input) {
  for (int i = 0; i < 2; ++i) {
    input_suppress(&input->players[i].buffer);
  }
}

void host_input_consume(HostInput *input, InputFrame frames[2]) {
  for (int i = 0; i < 2; ++i) {
    frames[i] = input_consume(&input->players[i].buffer);
  }
}

static InputCommands keyboard_commands(SDL_Keycode key) {
  InputCommands commands = {0};
  switch (key) {
    case SDLK_UP:
    case SDLK_w:
      commands.menu = MENU_UP;
      break;
    case SDLK_DOWN:
    case SDLK_s:
      commands.menu = MENU_DOWN;
      break;
    case SDLK_LEFT:
    case SDLK_a:
      commands.menu = MENU_LEFT;
      break;
    case SDLK_RIGHT:
    case SDLK_d:
      commands.menu = MENU_RIGHT;
      break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
      commands.menu = MENU_CONFIRM;
      break;
    case SDLK_SPACE:
      commands.menu = MENU_CONFIRM;
      break;
    case SDLK_ESCAPE:
    case SDLK_BACKSPACE:
      commands.menu = MENU_BACK;
      break;
  }
  switch (key) {
    case SDLK_ESCAPE:
      commands.shell = SHELL_QUIT;
      break;
    case SDLK_SPACE:
      commands.shell = SHELL_PAUSE;
      break;
    case SDLK_r:
      commands.shell = SHELL_RESET;
      break;
    case SDLK_RETURN:
      commands.shell = SHELL_REMATCH;
      break;
    case SDLK_F1:
      commands.shell = SHELL_NEXT_P1;
      break;
    case SDLK_F2:
      commands.shell = SHELL_NEXT_P2;
      break;
    case SDLK_F3:
      commands.shell = SHELL_TOGGLE_CPU;
      break;
    case SDLK_F4:
      commands.shell = SHELL_NEXT_ARENA;
      break;
  }
  return commands;
}

static MenuCommand direction_command(uint32_t pressed) {
  if (pressed & BUTTON_BIT(BUTTON_UP)) {
    return MENU_UP;
  }
  if (pressed & BUTTON_BIT(BUTTON_DOWN)) {
    return MENU_DOWN;
  }
  if (pressed & BUTTON_BIT(BUTTON_LEFT)) {
    return MENU_LEFT;
  }
  if (pressed & BUTTON_BIT(BUTTON_RIGHT)) {
    return MENU_RIGHT;
  }
  return MENU_NONE;
}

InputCommands host_input_event(HostInput *input, const SDL_Event *event) {
  InputCommands commands = {0};
  if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
    for (int i = 0; i < 2; ++i) {
      input->players[i].keyboard = 0;
      update(&input->players[i]);
    }
    host_input_suppress(input);
  } else if (event->type == SDL_CONTROLLERDEVICEADDED) {
    open_controller(input, event->cdevice.which);
  } else if (event->type == SDL_CONTROLLERDEVICEREMOVED) {
    for (int i = 0; i < 2; ++i) {
      PlayerInput *player = &input->players[i];
      if (!player->controller || player->instance != event->cdevice.which) {
        continue;
      }
      SDL_GameControllerClose(player->controller);
      player->controller = NULL;
      player->buttons = player->stick = 0;
      player->x = player->y = 0;
      update(player);
      /* Drop queued pad presses on disconnect, preserving keyboard state. */
      player->buffer.pending = 0;
    }
    /* A third attached pad can take a newly vacant slot. */
    for (int device = 0; device < SDL_NumJoysticks(); ++device) {
      open_controller(input, device);
    }
  } else if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
    for (int i = 0; i < 2; ++i) {
      PlayerInput *player = &input->players[i];
      for (int button = 0; button < BUTTON_COUNT; ++button) {
        if (event->key.keysym.scancode != keys[i][button]) {
          continue;
        }
        if (event->type == SDL_KEYDOWN) {
          player->keyboard |= BUTTON_BIT(button);
        } else {
          player->keyboard &= ~BUTTON_BIT(button);
        }
      }
      update(player);
    }
    if (event->type == SDL_KEYDOWN && !event->key.repeat) {
      commands = keyboard_commands(event->key.keysym.sym);
    }
  } else if (event->type == SDL_CONTROLLERBUTTONDOWN || event->type == SDL_CONTROLLERBUTTONUP || event->type == SDL_CONTROLLERAXISMOTION) {
    SDL_JoystickID instance = event->type == SDL_CONTROLLERAXISMOTION ? event->caxis.which : event->cbutton.which;
    for (int i = 0; i < 2; ++i) {
      PlayerInput *player = &input->players[i];
      if (!player->controller || player->instance != instance) {
        continue;
      }
      uint32_t previous = player->buttons | player->stick;
      if (event->type == SDL_CONTROLLERAXISMOTION) {
        if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
          player->x = event->caxis.value;
        }
        if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
          player->y = event->caxis.value;
        }
        update_stick(player);
      } else {
        uint32_t bit = controller_button(event->cbutton.button);
        if (event->type == SDL_CONTROLLERBUTTONDOWN) {
          player->buttons |= bit;
        } else {
          player->buttons &= ~bit;
        }
        update(player);
        if (event->type == SDL_CONTROLLERBUTTONDOWN) {
          if (event->cbutton.button == SDL_CONTROLLER_BUTTON_A) {
            commands.menu = MENU_CONFIRM;
          }
          if (event->cbutton.button == SDL_CONTROLLER_BUTTON_B) {
            commands.menu = MENU_BACK;
          }
          if (event->cbutton.button == SDL_CONTROLLER_BUTTON_START) {
            commands.menu = MENU_CONFIRM;
            commands.shell = SHELL_PAUSE;
          }
          if (event->cbutton.button == SDL_CONTROLLER_BUTTON_BACK) {
            commands.shell = SHELL_REMATCH;
          }
        }
      }
      MenuCommand direction = direction_command((player->buttons | player->stick) & ~previous);
      if (direction != MENU_NONE) {
        commands.menu = direction;
      }
    }
  }
  return commands;
}
