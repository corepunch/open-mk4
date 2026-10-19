#include "../character.h"

static void projectile_step(Projectile *object);
static const MoveDef s_projectile = {.clip = 17, .last = MOVE_END, .flags = SF_LOCK | SF_RECORD, .action = 0x305};
static const AttackRecord projectile_record = {22282, -7864, 41287, 15073, 66, 10, 2, 2, 9175};
static const MeshAnimation projectile_mesh = {.first = 41, .count = 1, .period = 1, .scale = Q16_ONE};

static void projectile_step(Projectile *object) {
  mk4_script_projectile_step(object, &projectile_record, false);
}

void raiden_script_lightning_spawn(Fighter *fighter, int task_type) {
  require(task_type == 65, "Incorrect generated projectile task");
  mk4_script_projectile_spawn(fighter, projectile_step, &projectile_mesh, 0x28f5, 0, 48);
}

void raiden_lightning(Fighter *fighter) {
  if (g_battle.phase != PHASE_FIGHT || fighter_airborne(fighter)) {
    return;
  }
  fighter_start_move(fighter, S_PUNCH, &s_projectile);
  raiden_entry_retail_script_004ed5f0(fighter);
}
