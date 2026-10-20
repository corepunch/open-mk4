#include "../character.h"
#include "retail_vm.h"

static bool native(Fighter *fighter, uint32_t address, RetailNativeKind kind, unsigned argument) {
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
    script_fire_decor(fighter);
  } else if (address == 0x416d40 && (kind == RETAIL_CALL)) {
    script_fire_flame(fighter);
  } else if (address == 0x41ea50 && (kind == RETAIL_TAIL)) {
    script_beast_begin(fighter);
  } else if (address == 0x41ebd0 && (kind == RETAIL_TAIL)) {
    script_beast_attack(fighter);
  } else if (address == 0x41ec70 && (kind == RETAIL_TAIL)) {
    script_beast_victim_pieces(fighter);
  } else if (address == 0x41ed80 && (kind == RETAIL_SPAWN)) {
    script_beast_gore(fighter, (int)argument);
  } else if (address == 0x41f1b0 && (kind == RETAIL_TAIL)) {
    script_beast_park(fighter);
  } else if (address == 0x42bdf0 && (kind == RETAIL_TAIL)) {
    script_lk_rebase(fighter);
  } else if (address == 0x42be00 && (kind == RETAIL_TAIL)) {
    script_lk_damage(fighter);
  } else if (address == 0x42be10 && (kind == RETAIL_TAIL)) {
    script_lk_return(fighter);
  } else if (address == 0x42ce60 && (kind == RETAIL_TAIL)) {
    script_lp_return(fighter);
  } else if (address == 0x43d460 && (kind == RETAIL_TAIL)) {
    script_fan_tail(fighter);
  } else if (address == 0x43d530 && (kind == RETAIL_TAIL)) {
    script_fan_cycle(fighter);
  } else if (address == 0x43d6f0 && (kind == RETAIL_TAIL)) {
    script_fan_victory(fighter);
  } else if (address == 0x43db50 && (kind == RETAIL_TAIL)) {
    script_fan_victim_end(fighter);
  } else if (address == 0x443930 && (kind == RETAIL_TAIL)) {
    script_burn_begin(fighter);
  } else if (address == 0x443a30 && (kind == RETAIL_TAIL)) {
    script_burn_victim_start(fighter);
  } else if (address == 0x443a70 && (kind == RETAIL_TAIL)) {
    script_burn_repeat1(fighter);
  } else if (address == 0x443b40 && (kind == RETAIL_TAIL)) {
    script_burn_repeat2(fighter);
  } else if (address == 0x443c10 && (kind == RETAIL_TAIL)) {
    script_burn_victory(fighter);
  } else if (address == 0x444000 && (kind == RETAIL_TAIL)) {
    script_burn_victim_cue(fighter);
  } else if (address == 0x444030 && (kind == RETAIL_TAIL)) {
    script_burn_victim_repeat(fighter);
  } else if (address == 0x444060 && (kind == RETAIL_TAIL)) {
    script_burn_victim_blend(fighter);
  } else if (address == 0x444100 && (kind == RETAIL_TAIL)) {
    script_burn_victim_impact(fighter);
  } else if (address == 0x444150 && (kind == RETAIL_TAIL)) {
    script_burn_victim_settle(fighter);
  } else if (address == 0x444190 && (kind == RETAIL_TAIL)) {
    script_burn_victim_end(fighter);
  } else if (address == 0x450d40 && (kind == RETAIL_SPAWN)) {
    script_transform_effect(fighter, (int)argument);
  } else if (address == 0x467fc0 && (kind == RETAIL_TAIL)) {
    script_spike_tail(fighter);
  } else if (address == 0x468640 && (kind == RETAIL_TAIL)) {
    fire_script(fighter);
  } else if (address == 0x46aae0 && (kind == RETAIL_TAIL)) {
    script_air_release(fighter);
  } else if (address == 0x46ab20 && (kind == RETAIL_TAIL)) {
    script_air_land(fighter);
  } else if (address == 0x46c7f0 && (kind == RETAIL_TAIL)) {
    script_air_kick_tail(fighter);
  } else if (address == 0x46d260 && (kind == RETAIL_TAIL)) {
    script_draw_equip(fighter);
  } else if (address == 0x46dd00 && (kind == RETAIL_TAIL)) {
    require(scorpion_try_retail_vm(fighter, 0x4e6330), "Missing retail continuation");
  } else if (address == 0x46dd10 && (kind == RETAIL_TAIL)) {
    require(scorpion_try_retail_vm(fighter, 0x4e6360), "Missing retail continuation");
  } else if (address == 0x46dd20 && (kind == RETAIL_TAIL)) {
    require(scorpion_try_retail_vm(fighter, 0x4e63b0), "Missing retail continuation");
  } else if (address == 0x46ddd0 && (kind == RETAIL_CALL)) {
    mk4_script_jab_contact(fighter);
  } else if (address == 0x46de60 && (kind == RETAIL_CALL || kind == RETAIL_PREDICATE)) {
    fighter->retail_vm.result = mk4_script_jab_gate(fighter);
  } else if (address == 0x46df90 && (kind == RETAIL_TAIL)) {
    script_air_lk_tail(fighter);
  } else if (address == 0x46e3d0 && (kind == RETAIL_TAIL)) {
    script_air_diagonal_tail(fighter);
  } else if (address == 0x46e730 && (kind == RETAIL_TAIL)) {
    script_air_hp_tail(fighter);
  } else if (address == 0x46eab0 && (kind == RETAIL_TAIL)) {
    weapon_air_frames_native(fighter, fighter->retail_vm.selector);
  } else if (address == 0x46ec10 && (kind == RETAIL_TAIL)) {
    weapon_air_frames_native(fighter, fighter->retail_vm.selector);
  } else if (address == 0x46eea0 && (kind == RETAIL_TAIL)) {
    script_air_lp_tail(fighter);
  } else if (address == 0x470420 && (kind == RETAIL_TAIL)) {
    script_weapon_ready(fighter);
  } else if (address == 0x47fcf0 && (kind == RETAIL_TAIL)) {
    teleport_script(fighter);
  } else if (address == 0x47ffc0 && (kind == RETAIL_TAIL)) {
    script_teleport_land(fighter);
  } else if (address == 0x480040 && (kind == RETAIL_PHYSICS)) {
    fighter->body.physics = script_teleport_motion;
  } else if (address == 0x4807b0 && (kind == RETAIL_TAIL)) {
    spin_script(fighter);
  } else if (address == 0x483df0 && (kind == RETAIL_TAIL)) {
    script_burn_tail(fighter);
  } else if (address == 0x484b60 && (kind == RETAIL_TAIL)) {
    mk4_mmove_air_getup(fighter);
  } else if (address == 0x484c00 && (kind == RETAIL_TAIL)) {
    script_teleport_victim_tail(fighter);
  } else if (address == 0x48a1f0 && (kind == RETAIL_CALL)) {
    script_beast_voice(fighter);
  } else if (address == 0x4913a0 && (kind == RETAIL_PHYSICS)) {
    fighter->body.physics = fighter_friction;
  } else {
    return false;
  }
  return true;
}

bool scorpion_try_retail_vm(Fighter *fighter, uint32_t entry) {
  const Mk4Image *image = retail_vm_image();
  FighterFn finish = scorpion_ready;
  uint32_t begin = entry, end;
  switch (entry) {
    case 0x4e6d08:
      end = 0x4e6d9c;
      break;
    case 0x4e60b8:
      end = 0x4e610c;
      break;
    case 0x4ed448:
      end = 0x4ed4a4;
      finish = scorpion_script_spear_done;
      break;
    case 0x4ed4a8:
      end = 0x4ed4c0;
      break;
    case 0x4e9d00:
      end = 0x4e9d1c;
      break;
    case 0x4e9f20:
      end = 0x4e9f30;
      break;
    case 0x4e5ca0:
      end = 0x4e6cb0;
      break;
    case 0x4e5c70:
      end = 0x4e5c80;
      break;
    case 0x4e6798:
      end = 0x4e6e8c;
      break;
    case 0x4e62a0:
      end = 0x4e62bc;
      break;
    case 0x4e6158:
      end = 0x4e617c;
      break;
    case 0x4e6190:
      end = 0x4e61c8;
      break;
    case 0x4e61e0:
      end = 0x4e61f4;
      break;
    case 0x4e69b0:
      end = 0x4e69c4;
      break;
    case 0x4e6844:
      end = 0x4e685c;
      break;
    case 0x4e6860:
      end = 0x4e6874;
      break;
    case 0x4e6878:
      end = 0x4e688c;
      break;
    case 0x4d2e58:
      end = 0x4d2e84;
      break;
    case 0x4d2e88:
      end = 0x4d2eac;
      break;
    case 0x4d2eb0:
      end = 0x4d2ef8;
      break;
    case 0x4d2ef8:
      end = 0x4d2f10;
      break;
    case 0x4d2f10:
      end = 0x4d2f40;
      break;
    case 0x4de2f8:
      end = 0x4de318;
      break;
    case 0x4de318:
      end = 0x4de324;
      break;
    case 0x4de328:
      end = 0x4de358;
      break;
    case 0x4de798:
      end = 0x4de7e0;
      begin = 0x4de6e4;
      break;
    case 0x4e0118:
      end = 0x4e0128;
      break;
    case 0x4e0138:
      end = 0x4e0148;
      break;
    case 0x4e0148:
      end = 0x4e0158;
      break;
    case 0x4e0158:
      end = 0x4e0174;
      break;
    case 0x4e0188:
      end = 0x4e01a0;
      break;
    case 0x4e01a0:
      end = 0x4e01b0;
      break;
    case 0x4e0278:
      end = 0x4e0288;
      break;
    case 0x4e0288:
      end = 0x4e0294;
      break;
    case 0x4e0298:
      end = 0x4e02a4;
      break;
    case 0x4e02a8:
      end = 0x4e02b8;
      break;
    case 0x4e02b8:
      end = 0x4e02cc;
      break;
    case 0x4e02d0:
      end = 0x4e02f4;
      break;
    case 0x4e0300:
      end = 0x4e0310;
      break;
    case 0x4e0310:
      end = 0x4e031c;
      break;
    case 0x4e0320:
      end = 0x4e0334;
      break;
    case 0x4e0338:
      end = 0x4e0348;
      break;
    case 0x4e0348:
      end = 0x4e0354;
      break;
    case 0x4e0358:
      end = 0x4e0364;
      break;
    case 0x4e5b58:
      end = 0x4e5b88;
      break;
    case 0x4e5e28:
      end = 0x4e5e4c;
      break;
    case 0x4e5e50:
      end = 0x4e5e78;
      break;
    case 0x4e61f8:
      end = 0x4e6258;
      break;
    case 0x4e62c0:
      end = 0x4e62d4;
      break;
    case 0x4e62d8:
      end = 0x4e62ec;
      break;
    case 0x4e62f0:
      end = 0x4e63a8;
      break;
    case 0x4e6330:
      end = 0x4e63d4;
      break;
    case 0x4e6360:
      end = 0x4e63d4;
      begin = 0x4e634c;
      break;
    case 0x4e63b0:
      end = 0x4e63c8;
      break;
    case 0x4e63d8:
      end = 0x4e6438;
      break;
    case 0x4e6500:
      end = 0x4e6558;
      break;
    case 0x4e6658:
      end = 0x4e666c;
      break;
    case 0x4e66d0:
      end = 0x4e66e0;
      break;
    case 0x4e66f0:
      end = 0x4e6700;
      break;
    case 0x4e6710:
      end = 0x4e6724;
      break;
    case 0x4e6950:
      end = 0x4e696c;
      break;
    case 0x4e87b0:
      end = 0x4e87dc;
      break;
    case 0x4e8818:
      end = 0x4e8830;
      break;
    case 0x4e8850:
      end = 0x4e8868;
      break;
    case 0x4e97f8:
      end = 0x4e9814;
      break;
    case 0x4e9910:
      end = 0x4e9938;
      break;
    case 0x4e9938:
      end = 0x4e996c;
      break;
    case 0x4e9970:
      end = 0x4e997c;
      finish = mk4_script_thrown_ready;
      break;
    case 0x4ec8b0:
      end = 0x4ec988;
      break;
    case 0x4ec918:
      end = 0x4ec988;
      break;
    default:
      return false;
  }
  retail_vm_start_at(fighter, image, entry, begin, end, finish, native);
  return true;
}
