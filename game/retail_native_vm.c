/* Transitional native ABI bridge. Script fetch, bounds and entry are shared;
 * the remaining native implementations are consolidated separately. */
#include "scorpion/character.h"
#include "retail_vm.h"
#include "retail_native.h"

bool retail_native_vm(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
  if (kind == RETAIL_CALL && address == 0x46a950) {
    mk4_script_avoid_clear(fighter);
  } else if (kind == RETAIL_CALL && address == 0x46a970) {
    mk4_script_avoid_set(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46a8d0) {
    /* Checked native tail: 429380/429600 reverses to zero, then 46f650
     * returns to the controller. Keep it on the VM's animation clock. */
    retail_vm_reverse_return(fighter);
  } else if (kind == RETAIL_SPAWN && address == 0x49a6a0 && argument == 67) {
    scorpion_script_spawn_spear(fighter, (int)argument);
  } else if (kind == RETAIL_TAIL && address == 0x486000) {
    scorpion_script_spear_pull(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x487420) {
    scorpion_spear_stun_script(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x49a680) {
    scorpion_script_spear_return(fighter);
  } else if (kind == RETAIL_HANDLER && address == 0x45b1f0) {
    scorpion_script_reaction(fighter, (int)argument);
  } else if (kind == RETAIL_HANDLER && address == 0x45b230) {
    scorpion_script_opponent_reaction(fighter, (int)argument);
  } else if (kind == RETAIL_TAIL && address == 0x46a760) {
    scorpion_script_kick_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46f080) {
    scorpion_script_sweep_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46cde0) {
    scorpion_uppercut_script(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46b740) {
    scorpion_script_crouch_lk_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46bb60) {
    scorpion_script_knee_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46bf10) {
    scorpion_script_close_hp_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46c2c0) {
    scorpion_script_combo_hp_tail(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46f220) {
    scorpion_script_roundhouse_start(fighter);
  } else if (kind == RETAIL_TAIL && address == 0x46f270) {
    scorpion_script_roundhouse_tail(fighter);
  } else if (address == 0x416ae0 && (kind == RETAIL_CALL)) {
    scorpion_script_fire_decor(fighter);
  } else if (address == 0x416d40 && (kind == RETAIL_CALL)) {
    scorpion_script_fire_flame(fighter);
  } else if (address == 0x41ea50 && (kind == RETAIL_TAIL)) {
    scorpion_script_beast_begin(fighter);
  } else if (address == 0x41ebd0 && (kind == RETAIL_TAIL)) {
    scorpion_script_beast_attack(fighter);
  } else if (address == 0x41ec70 && (kind == RETAIL_TAIL)) {
    scorpion_script_beast_victim_pieces(fighter);
  } else if (address == 0x41ed80 && (kind == RETAIL_SPAWN)) {
    scorpion_script_beast_gore(fighter, (int)argument);
  } else if (address == 0x41f1b0 && (kind == RETAIL_TAIL)) {
    scorpion_script_beast_park(fighter);
  } else if (address == 0x42bdf0 && (kind == RETAIL_TAIL)) {
    scorpion_script_lk_rebase(fighter);
  } else if (address == 0x42be00 && (kind == RETAIL_TAIL)) {
    scorpion_script_lk_damage(fighter);
  } else if (address == 0x42be10 && (kind == RETAIL_TAIL)) {
    scorpion_script_lk_return(fighter);
  } else if (address == 0x42ce60 && (kind == RETAIL_TAIL)) {
    scorpion_script_lp_return(fighter);
  } else if (address == 0x43d460 && (kind == RETAIL_TAIL)) {
    scorpion_script_fan_tail(fighter);
  } else if (address == 0x43d530 && (kind == RETAIL_TAIL)) {
    scorpion_script_fan_cycle(fighter);
  } else if (address == 0x43d6f0 && (kind == RETAIL_TAIL)) {
    scorpion_script_fan_victory(fighter);
  } else if (address == 0x43db50 && (kind == RETAIL_TAIL)) {
    scorpion_script_fan_victim_end(fighter);
  } else if (address == 0x443930 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_begin(fighter);
  } else if (address == 0x443a30 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_victim_start(fighter);
  } else if (address == 0x443a70 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_repeat1(fighter);
  } else if (address == 0x443b40 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_repeat2(fighter);
  } else if (address == 0x443c10 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_victory(fighter);
  } else if (address == 0x444000 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_victim_cue(fighter);
  } else if (address == 0x444030 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_victim_repeat(fighter);
  } else if (address == 0x444060 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_victim_blend(fighter);
  } else if (address == 0x444100 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_victim_impact(fighter);
  } else if (address == 0x444150 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_victim_settle(fighter);
  } else if (address == 0x444190 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_victim_end(fighter);
  } else if (address == 0x450d40 && (kind == RETAIL_SPAWN)) {
    scorpion_script_transform_effect(fighter, (int)argument);
  } else if (address == 0x467fc0 && (kind == RETAIL_TAIL)) {
    scorpion_script_spike_tail(fighter);
  } else if (address == 0x468640 && (kind == RETAIL_TAIL)) {
    scorpion_fire_script(fighter);
  } else if (address == 0x46aae0 && (kind == RETAIL_TAIL)) {
    scorpion_script_air_release(fighter);
  } else if (address == 0x46ab20 && (kind == RETAIL_TAIL)) {
    scorpion_script_air_land(fighter);
  } else if (address == 0x46c7f0 && (kind == RETAIL_TAIL)) {
    scorpion_script_air_kick_tail(fighter);
  } else if (address == 0x46d260 && (kind == RETAIL_TAIL)) {
    scorpion_script_draw_equip(fighter);
  } else if (address == 0x46dd00 && (kind == RETAIL_TAIL)) {
    retail_vm_enter(fighter, 0x4e6330, fighter_ready);
  } else if (address == 0x46dd10 && (kind == RETAIL_TAIL)) {
    retail_vm_enter(fighter, 0x4e6360, fighter_ready);
  } else if (address == 0x46dd20 && (kind == RETAIL_TAIL)) {
    retail_vm_enter(fighter, 0x4e63b0, fighter_ready);
  } else if (address == 0x46ddd0 && (kind == RETAIL_CALL)) {
    mk4_script_jab_contact(fighter);
  } else if (address == 0x46de60 && (kind == RETAIL_CALL || kind == RETAIL_PREDICATE)) {
    fighter->retail_vm.result = mk4_script_jab_gate(fighter);
  } else if (address == 0x46df90 && (kind == RETAIL_TAIL)) {
    scorpion_script_air_lk_tail(fighter);
  } else if (address == 0x46e3d0 && (kind == RETAIL_TAIL)) {
    scorpion_script_air_diagonal_tail(fighter);
  } else if (address == 0x46e730 && (kind == RETAIL_TAIL)) {
    scorpion_script_air_hp_tail(fighter);
  } else if (address == 0x46eab0 && (kind == RETAIL_TAIL)) {
    weapon_air_frames_native(fighter, fighter->retail_vm.selector);
  } else if (address == 0x46ec10 && (kind == RETAIL_TAIL)) {
    weapon_air_frames_native(fighter, fighter->retail_vm.selector);
  } else if (address == 0x46eea0 && (kind == RETAIL_TAIL)) {
    scorpion_script_air_lp_tail(fighter);
  } else if (address == 0x470420 && (kind == RETAIL_TAIL)) {
    scorpion_script_weapon_ready(fighter);
  } else if (address == 0x47fcf0 && (kind == RETAIL_TAIL)) {
    scorpion_teleport_script(fighter);
  } else if (address == 0x47ffc0 && (kind == RETAIL_TAIL)) {
    scorpion_script_teleport_land(fighter);
  } else if (address == 0x480040 && (kind == RETAIL_PHYSICS)) {
    fighter->body.physics = scorpion_script_teleport_motion;
  } else if (address == 0x4807b0 && (kind == RETAIL_TAIL)) {
    scorpion_spin_script(fighter);
  } else if (address == 0x483df0 && (kind == RETAIL_TAIL)) {
    scorpion_script_burn_tail(fighter);
  } else if (address == 0x484b60 && (kind == RETAIL_TAIL)) {
    mk4_mmove_air_getup(fighter);
  } else if (address == 0x484c00 && (kind == RETAIL_TAIL)) {
    scorpion_script_teleport_victim_tail(fighter);
  } else if (address == 0x48a1f0 && (kind == RETAIL_CALL)) {
    scorpion_script_beast_voice(fighter);
  } else if (address == 0x4913a0 && (kind == RETAIL_PHYSICS)) {
    fighter->body.physics = fighter_friction;
  } else {
    return false;
  }
  return true;
}
