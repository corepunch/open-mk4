#include "retail_native.h"
#include "retail_vm.h"

/* Original native entries supply literal script operands. Selectors and authored
 * animation/control flow and native descriptor rows come from the image. */
/* Native470980's initial phase binds the descriptor in its original row. */
void retail_air_bind_row(Fighter *fighter, uint32_t row) {
  require(row >= RETAIL_SCRIPT_BEGIN && row <= RETAIL_SCRIPT_END - 12 && row % 4 == 0, "Invalid native air parameter row");
  const Mk4Image *image = fighter->retail_vm.active ? fighter->retail_vm.image : retail_vm_image();
  retail_vm_bind_animation(fighter, mk4_image_u32(image, row) * 4, 0);
}

void retail_air_attack(Fighter *fighter, unsigned kind) {
  static const int actions[] = {0x603, 0x604, 0x600, 0x602, 0x601};
  static const uint32_t scripts[] = {0x4e6658, 0x4e6710, 0x4e61f8, 0x4e6500, 0x4e63d8};
  require(kind < sizeof scripts / sizeof *scripts, "Invalid native air attack");
  bool combo = fighter_airborne(fighter) && fighter->body.gravity == 0;
  retail_native_begin(fighter, S_JUMP, actions[kind], true);
  fighter_upright(fighter);
  fighter->script_phase = combo ? 1 : 0;
  fighter->native_variant = (int)kind;
  fighter->body.on_land = NULL;
  fighter->body.physics = NULL;
  retail_face_peer(fighter);
  uint32_t script = scripts[kind];
  if (fighter->armed && kind < 2) {
    script = kind == 0 ? 0x4e66d0 : 0x4e66f0;
  }
  retail_vm_enter(fighter, script, fighter_ready);
}

void retail_air_high_punch(Fighter *fighter) {
  retail_air_attack(fighter, 0);
}

void retail_air_low_punch(Fighter *fighter) {
  retail_air_attack(fighter, 1);
}

void retail_air_kick(Fighter *fighter) {
  retail_air_attack(fighter, 2);
}

void retail_air_diagonal_high_kick(Fighter *fighter) {
  retail_air_attack(fighter, 3);
}

void retail_air_diagonal_low_kick(Fighter *fighter) {
  retail_air_attack(fighter, 4);
}
