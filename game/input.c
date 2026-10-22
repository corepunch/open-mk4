#include "input.h"

void input_update(InputBuffer *buffer, uint32_t held) {
  held &= (BUTTON_BIT(BUTTON_COUNT) - 1);
  buffer->suppressed &= held;
  buffer->pending |= held & ~buffer->held & ~buffer->suppressed;
  buffer->held = held;
}

InputFrame input_consume(InputBuffer *buffer) {
  uint32_t held = (buffer->held | buffer->pending) & ~buffer->suppressed;
  buffer->pending = 0;
  /* Opposite absolute directions cancel, independently on each axis. */
  const uint32_t horizontal = BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(BUTTON_RIGHT);
  const uint32_t vertical = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_DOWN);
  if ((held & horizontal) == horizontal) {
    held &= ~horizontal;
  }
  if ((held & vertical) == vertical) {
    held &= ~vertical;
  }
  return (InputFrame){held};
}

void input_suppress(InputBuffer *buffer) {
  buffer->suppressed = buffer->held;
  buffer->pending = 0;
}
