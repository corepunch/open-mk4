#include "retail_native.h"
#include "retail_data.h"
#include "retail_vm.h"
#include "script.h"

/* Native throw-victim entries are selected by the original reaction table,
 * independently of either fighter's model or the originating script. */
static void victim_begin(Fighter *fighter) {
  static const MoveDef move = {.clip = -1, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD | SF_REACTION, .action = ACTION_RETAIL_4003};
  int clip = fighter_move(fighter)->clip;
  int sample = fighter_sample(fighter);
  fighter_start_move(fighter, S_KNOCKDOWN, &move);
  if (clip >= 0) {
    retail_vm_bind_animation(fighter, 0x4fad7c + (uint32_t)clip * 12, sample);
  }
  mk4_script_touchable(fighter);
}

static void victim_485860(Fighter *fighter) {
  victim_begin(fighter);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->facing = -fighter->facing;
  retail_vm_enter(fighter, 0x4e9a20, mk4_mmove_thrown_finish);
}

static void victim_4857b0(Fighter *fighter) {
  victim_begin(fighter);
  retail_vm_bind_animation(fighter, 0x4fb55c, 0);
  fighter->body.physics = fighter_friction;
  const Mk4Image *image = retail_vm_image();
  unsigned count = mk4_image_u32(image, 0x4ea8a0);
  require(count > 0 && count <= 32, "Invalid native impact sound list");
  battle_sound(SOUND_TOKEN, (int)mk4_image_u32(image, 0x4ea8a4 + 4 * battle_random(count)));
  retail_vm_enter(fighter, 0x4e99e0, mk4_mmove_thrown_finish);
}

static void victim_485810(Fighter *fighter) {
  victim_begin(fighter);
  retail_vm_bind_animation(fighter, 0x4fb5c8, 0);
  mk4_script_save_motion(fighter);
  retail_vm_enter(fighter, 0x4e99f0, mk4_mmove_thrown_finish);
}

static void victim_4854e0(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  retail_vm_enter(fighter, 0x4e99c0, mk4_mmove_thrown_finish);
}

static void victim_485480(Fighter *fighter) {
  mk4_script_disarm_self(fighter);
  victim_begin(fighter);
  fighter->body.pos[AXIS_Y] -= 0x4ccc;
  retail_vm_enter(fighter, 0x4ea398, mk4_mmove_thrown_finish);
}

FighterFn retail_throw_reaction(const Mk4Image *image, unsigned reaction) {
  switch (retail_reaction_target(image, reaction)) {
    case 0x485860:
      return victim_485860;
    case 0x4857b0:
      return victim_4857b0;
    case 0x485810:
      return victim_485810;
    case 0x4854e0:
      return victim_4854e0;
    case 0x485480:
      return victim_485480;
    default:
      return NULL;
  }
}

void retail_throw_victim_continue(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4ea3c0, mk4_mmove_thrown_finish);
}

void retail_throw_victim_impact(Fighter *fighter) {
  retail_vm_enter(fighter, 0x4ea3d0, mk4_mmove_thrown_finish);
}
