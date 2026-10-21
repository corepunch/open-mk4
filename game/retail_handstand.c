#include "retail_vm.h"
#include "kai/character.h"

static void kai_stage_event_step(Fighter *fighter);
static void kai_input_47cbf0(Fighter *fighter);
void retail_handstand_front(Fighter *fighter);
void retail_handstand_rear(Fighter *fighter);
void retail_handstand_spin(Fighter *fighter);
void retail_handstand_exit(Fighter *fighter);
static void handstand_stance(Fighter *fighter, bool walking);
static void spin_release(Fighter *fighter);
static void spin_wait(Fighter *fighter);
static void spin_query(Fighter *fighter);

static const MoveDef intro = {.clip = 1, .last = 19, .flags = SF_LOCK | SF_RECORD, .action = 0x200d};
static const MoveDef idle = {.clip = 4, .last = 29, .flags = SF_LOCK | SF_RECORD, .action = 0x200d};
static const MoveDef walk = {.clip = 5, .last = 24, .flags = SF_LOCK | SF_RECORD, .action = 0x200d};
static const MoveDef spin = {.clip = 2, .last = 8, .flags = SF_LOCK | SF_RECORD, .action = 0x408};
static const MoveDef front = {.clip = 0, .last = 12, .flags = SF_LOCK | SF_RECORD, .action = 0x200d};
static const MoveDef rear = {.clip = 6, .last = 12, .flags = SF_LOCK | SF_RECORD, .action = 0x114};
static const AttackRecord spin_record = {0x111a9, -0x3ae1, 0x16b43, 0x51a9, 126, 10, 0, 0, 0x170a};
static const AttackRecord front_record = {0x1851e, -0xbd2f, 0x10000, 0x9958, 127, 127, 5, 0, 0x1c28};
static const AttackRecord rear_record = {0x1e624, -0x2395, 0x10000, 0x9958, 167, 127, 5, 0, 0x170a};
static const ButtonTable handstand_buttons = {.retail_address = 0x4e83f8};

bool kai_can_handstand(const Fighter *fighter) {
  unsigned required = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_KICK);
  unsigned forbidden =
      BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_RUN);
  return g_battle.phase == PHASE_FIGHT && !fighter_airborne(fighter) && (fighter->held & required) == required && !(fighter->held & forbidden);
}

static void handstand_cleanup(Fighter *fighter) {
  mk4_script_stop_body(fighter);
}

/* 496d20: raw horizontal switches, .1 times the caller's multiplier.
 * 47c620 halves retreat after 48ea30; the LP spin keeps its full 1.1 rate. */
static void handstand_motion(Fighter *fighter, q16 multiplier, bool halve_retreat) {
  int raw = fighter->held & BUTTON_BIT(BUTTON_RIGHT) ? 1 : fighter->held & BUTTON_BIT(BUTTON_LEFT) ? -1 : 0;
  bool facing_left = fighter_opponent(fighter)->body.pos[AXIS_X] < fighter->body.pos[AXIS_X];
  q16 speed = q16_mul(0x1999, multiplier) * (facing_left ? -raw : raw);
  if (halve_retreat && speed < 0) {
    speed /= 2;
  }
  fighter_set_speed(fighter, speed);
}

static bool handstand_live(Fighter *fighter) {
  if (fighter->health <= 0) {
    fighter_finish_mmove(fighter, NULL);
    return false;
  }
  if (g_battle.phase != PHASE_FIGHT) {
    retail_handstand_exit(fighter);
    return false;
  }
  return true;
}

static void kai_stage_event_step(Fighter *fighter) {
  if (!handstand_live(fighter)) {
    return;
  }
  if ((fighter->held & BUTTON_BIT(BUTTON_UP)) && (fighter->held & (BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(BUTTON_RIGHT)))) {
    mk4_jump(fighter);
    return;
  }
  bool walking = (fighter->held & (BUTTON_BIT(BUTTON_LEFT) | BUTTON_BIT(BUTTON_RIGHT))) != 0;
  if (fighter_move(fighter)->clip != (walking ? 5 : 4)) {
    handstand_stance(fighter, walking);
    return;
  }
  handstand_motion(fighter, 0x18000, true);
}

static void handstand_step_sound(Fighter *fighter) {
  battle_sound(SOUND_C, 55);
  kai_stage_event_step(fighter);
}

static void handstand_stance(Fighter *fighter, bool walking) {
  fighter_start_move(fighter, S_KICK, walking ? &walk : &idle);
  fighter->body.pos[AXIS_Y] = fighter->body.ground;
  fighter->body.gravity = 0;
  fighter->mmove_cleanup = handstand_cleanup;
  fighter->buttons = &handstand_buttons;
  fighter->flags &= ~(unsigned)FF_BUSY;
  /* Native control and continuous presentation share the fixed-frame runner.
   * Seven 25-pose walking cycles keep the seven-visit step sound independent
   * of clip wrapping. No task counter or retained C stack is required. */
  int count = walking ? 175 : 30;
  native_begin(fighter, 1, count, NULL);
  for (int frame = 1; frame <= count; ++frame) {
    native_frame(fighter, frame, (frame - 1) % (walking ? 25 : 30), walking && frame % 7 == 0 ? handstand_step_sound : kai_stage_event_step);
  }
  handstand_motion(fighter, 0x18000, true);
  native_play(fighter, false);
}

void kai_script_stage_event(Fighter *fighter) {
  handstand_stance(fighter, false);
}

static void kai_input_47cbf0(Fighter *fighter) {
  weapon_followup_victim(fighter);
  retail_vm_enter_owned(fighter, 0x4e88c8, fighter_ready, 11);
}

void kai_input_47c520(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  fighter_start_move(fighter, S_KICK, &intro);
  fighter->facing = fighter->body.pos[AXIS_X] <= fighter_opponent(fighter)->body.pos[AXIS_X] ? 1 : -1;
  fighter->mmove_cleanup = handstand_cleanup;
  if (fighter->armed) {
    kai_input_47cbf0(fighter);
  } else {
    retail_vm_enter_owned(fighter, 0x4e8320, fighter_ready, 11);
  }
}

void retail_handstand_exit(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &intro);
  fighter->mmove_cleanup = handstand_cleanup;
  retail_vm_enter_owned(fighter, 0x4e83e8, fighter_ready, 11);
}

void retail_handstand_front(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &front);
  fighter->mmove_cleanup = handstand_cleanup;
  retail_vm_enter_owned(fighter, 0x4e8358, fighter_ready, 11);
}

void retail_handstand_rear(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &rear);
  fighter->mmove_cleanup = handstand_cleanup;
  retail_vm_enter_owned(fighter, 0x4e83a0, fighter_ready, 11);
}

static void handstand_contact(Fighter *fighter) {
  if (!fighter->connected) {
    battle_contact(fighter, fighter->native_record);
  }
}

void kai_script_contact_b10_continuation(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 0 && fighter_sample(fighter) == 9, "Handstand HK needs its selected startup");
  fighter->native_record = &front_record;
  native_begin(fighter, 1, 3, NULL);
  native_script_end(fighter, 0x4e8390, fighter_ready, 11);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, frame == 1 ? 10 : 11, handstand_contact);
  }
  native_play(fighter, false);
}

void kai_handstand_rear_contact(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 6 && fighter_sample(fighter) == 9, "Handstand LK needs its selected startup");
  fighter->native_record = &rear_record;
  native_begin(fighter, 1, 3, NULL);
  native_script_end(fighter, 0x4e83d8, fighter_ready, 11);
  for (int frame = 1; frame <= 3; ++frame) {
    native_frame(fighter, frame, 9 + frame, handstand_contact);
  }
  native_play(fighter, false);
}

static void spin_motion(Fighter *fighter) {
  handstand_motion(fighter, 0x11999, false);
}

static void spin_release(Fighter *fighter) {
  mk4_script_stop_body(fighter);
  mk4_mmove_bind(fighter, 2, 9, 8);
  native_begin(fighter, 0, 8, kai_script_stage_event);
  for (int frame = 0; frame <= 8; ++frame) {
    native_frame(fighter, frame, 8 - frame, NULL);
  }
  native_play(fighter, false);
}

static void spin_hold(Fighter *fighter) {
  if (handstand_live(fighter) && !(fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH))) {
    spin_release(fighter);
  }
}

static void spin_wait(Fighter *fighter) {
  fighter->action = 0x408;
  fighter->body.physics = spin_motion;
  int pose = fighter_sample(fighter);
  native_begin(fighter, 1, 8, NULL);
  for (int frame = 1; frame <= 8; ++frame) {
    native_frame(fighter, frame, pose, frame == 8 ? spin_query : spin_hold);
  }
  native_play(fighter, false);
}

static void spin_query(Fighter *fighter) {
  if (!handstand_live(fighter)) {
    return;
  }
  Fighter *victim = (Fighter *)fighter_opponent(fighter);
  ContactResult result = CONTACT_MISS;
  if ((victim->action & 0xf00) != 0x200) {
    result = battle_contact(fighter, &spin_record);
  }
  battle_sound(SOUND_C, 53);
  if (result == CONTACT_BLOCK || (result != CONTACT_MISS && victim->hits >= 3)) {
    fighter->action = 0x100f;
    fighter_set_speed(fighter, 0x6666);
    fighter->body.physics = fighter_friction;
    native_hold(fighter, 12, spin_wait);
  } else if (fighter->held & BUTTON_BIT(BUTTON_LOW_PUNCH)) {
    spin_wait(fighter);
  } else {
    spin_release(fighter);
  }
}

void kai_handstand_spin_begin(Fighter *fighter) {
  require(fighter_move(fighter)->clip == 3 && fighter_sample(fighter) == 0, "Handstand LP needs its selected spin clip");
  spin_wait(fighter);
}

void retail_handstand_spin(Fighter *fighter) {
  fighter_start_move(fighter, S_KICK, &spin);
  fighter->mmove_cleanup = handstand_cleanup;
  retail_vm_enter_owned(fighter, 0x4e8338, fighter_ready, 11);
}

void kai_handstand_victim_tail(Fighter *fighter) {
  int first = fighter_sample(fighter);
  native_begin(fighter, first, 39, mk4_mmove_thrown_finish);
  for (int frame = first; frame <= 39; ++frame) {
    native_frame(fighter, frame, frame, NULL);
  }
  native_play(fighter, false);
}

void kai_handstand_reaction(Fighter *fighter) {
  static const MoveDef stagger = {.clip = 134, .last = 17, .flags = SF_LOCK | SF_REACTION, .action = 0x4001};
  static const MoveDef knockdown = {.clip = 131, .last = 79, .flags = SF_LOCK | SF_REACTION, .action = 0x4003};
  bool crouch = fighter->action == 0x2001 || fighter->action == 0x2010 || fighter->action == 0x2002 || fighter->action == 0x106 || fighter->action == 0x107;
  require(fighter->reaction == 126 || fighter->reaction == 127 || fighter->reaction == 167, "Unexpected handstand victim reaction");
  mk4_script_stop_body(fighter);
  if (fighter->reaction != 167) {
    battle_wall_recoil(fighter);
  }
  mk4_script_disarm_self(fighter);
  fighter_start_move(fighter, fighter->reaction == 127 ? S_KNOCKDOWN : S_HIT, fighter->reaction == 127 ? &knockdown : &stagger);
  fighter->flags |= FF_REACTING;
  if (fighter->reaction == 127) {
    retail_vm_enter_owned(fighter, 0x4ea170, mk4_mmove_thrown_finish, 11);
  } else if (fighter->reaction == 167) {
    fighter->body.physics = fighter_friction;
    retail_vm_enter_owned(fighter, 0x4ea1c0, mk4_mmove_thrown_finish, 11);
  } else {
    mk4_script_shake(fighter, 0);
    if (crouch) {
      fighter->body.physics = fighter_friction;
      retail_vm_enter_owned(fighter, 0x4e9360, mk4_mmove_thrown_finish, 11);
    } else {
      retail_vm_enter_owned(fighter, 0x4e9308, mk4_mmove_thrown_finish, 11);
    }
  }
}
