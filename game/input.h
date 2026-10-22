#ifndef MK4_INPUT_H
#define MK4_INPUT_H
#include <stdbool.h>
#include <stdint.h>

/* Retail switches in the order the switch scan queues their press events:
 * word 0x4d00a4 holds directions, HP, BLK and HK; word 0x4d00a8 holds LP,
 * LK and RUN. Directions are absolute, not facing relative. See
 * docs/scorpion/input-and-commands.md. */
typedef enum Button {
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_HIGH_PUNCH,
  BUTTON_BLOCK,
  BUTTON_HIGH_KICK,
  BUTTON_LOW_PUNCH,
  BUTTON_LOW_KICK,
  BUTTON_RUN,
  BUTTON_COUNT
} Button;

#define BUTTON_BIT(button) (1u << (button))

/* held is the switches down during this tick. The simulation derives presses
 * with the retail two-sample rule: a switch must be up in both previous
 * samples, so a one-tick release between taps does not repeat a press. */
typedef struct InputFrame {
  uint32_t held;
} InputFrame;

static inline bool input_held(InputFrame in, Button button) {
  return (in.held & BUTTON_BIT(button)) != 0;
}

/* Host-side accumulator. A press and release between ticks is delivered once.
 * Catch-up ticks then receive current held state. Multiple transitions before
 * a tick intentionally collapse to one sample; this is not an event replay. */
typedef struct InputBuffer {
  uint32_t held, pending, suppressed;
} InputBuffer;

void input_update(InputBuffer *buffer, uint32_t held);
InputFrame input_consume(InputBuffer *buffer);
void input_suppress(InputBuffer *buffer);
#endif
