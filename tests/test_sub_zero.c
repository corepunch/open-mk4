#include "game.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void check(bool condition, const char *message) {
  if (!condition) {
    fprintf(stderr, "FAIL Sub-Zero: %s\n", message);
    exit(1);
  }
}

static void fight(int player, float distance) {
  battle_init(player == 0 ? 4 : 0, player == 1 ? 4 : 0);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[0].body.pos[AXIS_X] = (q16)(-distance * Q16_ONE / 2);
  g_battle.fighters[1].body.pos[AXIS_X] = (q16)(distance * Q16_ONE / 2);
}

static void advance(int ticks) {
  for (int i = 0; i < ticks; ++i) {
    battle_step((Input[2]){0});
  }
}

static void press(int player, unsigned held) {
  Input input[2] = {0};
  input[player].held = held;
  battle_step(input);
  advance(1);
}

static void sweep_frames(void) {
  for (int player = 0; player < 2; ++player) {
    for (int interrupt = 0; interrupt < 2; ++interrupt) {
      fight(player, 1.f);
      Fighter *fighter = &g_battle.fighters[player];
      Fighter *victim = &g_battle.fighters[1 - player];
      press(player, BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT));
      advance(17);
      check(victim->health == Q16_ONE && !fighter_fiber_active(fighter), "sweep startup damaged early or allocated a fiber");
      if (interrupt) {
        fighter_interrupt(fighter, fighter_ready);
      }
      advance(80);
      check(victim->health == Q16_ONE - (interrupt ? 0 : 0x23d7), "sweep contact or interruption damage changed");
      check(fighter->state == S_IDLE && fighter->current_mmove == NULL, "sweep failed to recover");
    }
  }
}

static void normals(void) {
  for (int player = 0; player < 2; ++player) {
    unsigned back = BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT);
    unsigned buttons[] = {BUTTON_BIT(BUTTON_HIGH_PUNCH), BUTTON_BIT(BUTTON_LOW_PUNCH), BUTTON_BIT(BUTTON_HIGH_KICK), BUTTON_BIT(BUTTON_LOW_KICK),
        back | BUTTON_BIT(BUTTON_HIGH_KICK), back | BUTTON_BIT(BUTTON_LOW_KICK), BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH),
        BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_KICK), BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_KICK)};
    const int clips[] = {296, 294, 71, 70, 313, 63, 260, 315, 52};
    for (int i = 0; i < 9; ++i) {
      fight(player, 4.f);
      press(player, buttons[i]);
      check(fighter_move(&g_battle.fighters[player])->clip == clips[i], "stance/character selector chose wrong normal");
      check(g_battle.fighters[player].current_mmove != NULL && !fighter_fiber_active(&g_battle.fighters[player]), "normal must enter frame phases");
      advance(120);
      check(g_battle.fighters[player].state == S_IDLE && g_battle.fighters[1 - player].health == Q16_ONE, "miss must recover without damage");
      check(!fighter_fiber_active(&g_battle.fighters[player]), "normal recovery must retire its stack");
    }
    for (int outcome = 0; outcome < 4; ++outcome) {
      fight(player, outcome == 3 ? 4.f : 1.f);
      Input input[2] = {0};
      if (outcome == 1) {
        input[1 - player].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      if (outcome == 2) {
        g_battle.fighters[1 - player].body.pos[AXIS_Y] -= Q16_ONE;
        fighter_enter_state(&g_battle.fighters[1 - player], S_JUMP);
      }
      input[player].held = BUTTON_BIT(BUTTON_HIGH_KICK);
      battle_step(input);
      input[player].held = 0;
      for (int t = 0; t < 11; ++t) {
        battle_step(input);
      }
      check(g_battle.fighters[1 - player].health == Q16_ONE, "kick damaged before first contact");
      battle_step(input);
      q16 damage = outcome == 3 ? 0 : outcome == 1 ? q16_mul(0x2147, 0x3333) : 0x2147;
      check(g_battle.fighters[1 - player].health == Q16_ONE - damage, "kick record/chip/air/miss damage");
      advance(160);
      check(g_battle.fighters[1 - player].health == Q16_ONE - damage, "kick must connect once");
      check(g_battle.fighters[player].state == S_IDLE, "kick recovery must release controller");
    }
    fight(player, .65f);
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    advance(7);
    check(g_battle.fighters[player].flags & FF_FOLLOW_UP, "close HP must open its cancel");
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter_move(&g_battle.fighters[player])->clip == 532, "combo HP must select Sub-Zero clip 532");
    advance(120);
    check(g_battle.fighters[player].state == S_IDLE, "combo HP must recover");
    fight(player, .65f);
    press(player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_KICK));
    advance(7);
    check(
        g_battle.fighters[1 - player].reaction == 26 && g_battle.fighters[1 - player].health == Q16_ONE - 0x1999, "crouch HK must use its own launcher record");
    advance(180);
    check(g_battle.fighters[1 - player].state == S_IDLE, "crouch HK victim must finish its reaction");
    fight(player, 4.f);
    press(player, BUTTON_BIT(BUTTON_LOW_KICK));
    advance(6);
    check(g_battle.fighters[player].avoids_high, "low-kick avoidance begins at sample 6");
    battle_react(&g_battle.fighters[player], 37);
    advance(1);
    check(!g_battle.fighters[player].avoids_high, "interruption must clear low-kick avoidance");
    for (int button = BUTTON_HIGH_PUNCH; button <= BUTTON_LOW_KICK; ++button) {
      if (button == BUTTON_BLOCK) {
        continue;
      }
      fight(player, 4.f);
      g_battle.fighters[player].body.pos[AXIS_Y] -= 3 * Q16_ONE;
      fighter_enter_state(&g_battle.fighters[player], S_JUMP);
      press(player, BUTTON_BIT(button));
      check(fighter_move(&g_battle.fighters[player])->flags & SF_RECORD, "air attack must use script records");
      advance(160);
      check(g_battle.fighters[player].state == S_IDLE && !fighter_airborne(&g_battle.fighters[player]), "air attack must land and restore controls");
    }
  }
}

static void ice_command(int player, int delay, unsigned extra) {
  press(player, BUTTON_BIT(BUTTON_DOWN));
  advance(delay);
  press(player, BUTTON_BIT(player ? BUTTON_LEFT : BUTTON_RIGHT) | BUTTON_BIT(BUTTON_LOW_PUNCH) | extra);
}

static int objects(int kind) {
  int count = 0;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    count += g_battle.projectiles[i].kind == kind;
  }
  return count;
}

static int sound_events(int bank, int selector) {
  int count = 0;
  for (int i = 0; i < g_battle.sound_count; ++i) {
    count += g_battle.sounds[i].bank == bank && g_battle.sounds[i].selector == selector;
  }
  return count;
}

static void ice(void) {
  fight(0, 4.f);
  Fighter *shaken = &g_battle.fighters[1];
  battle_react(shaken, 67);
  advance(1);
  q16 origin = shaken->body.pos[AXIS_X];
  check(shaken->visual_offset[AXIS_X] == -0xccc && shaken->visual_offset[AXIS_Y] == 0xccc, "freeze shake must open along the victim heading and down");
  advance(3);
  check(shaken->visual_offset[AXIS_X] == 0 && shaken->body.pos[AXIS_X] == origin, "freeze shake must undo its offset without combat drift");
  advance(121);
  check(shaken->visual_offset[AXIS_Y] == 0xccc, "freeze shake must restart after the 88-tick hold");
  fighter_enter_state(shaken, S_IDLE);
  check(shaken->visual_offset[AXIS_Y] == 0, "interrupting freeze must clear its presentation offset");
  for (int reacting = 0; reacting < 2; ++reacting) {
    fight(0, 4.f);
    g_battle.fighters[0].flags |= FF_FOLLOW_UP;
    Fighter *victim = &g_battle.fighters[1];
    victim->body.pos[AXIS_Y] -= Q16_ONE;
    fighter_enter_state(victim, S_JUMP);
    victim->flags |= reacting ? FF_REACTING : 0;
    ice_command(0, 2, 0);
    advance(8);
    check(objects(PROJECTILE_ICE) == reacting, "ice command must use the airborne victim reaction flag, not the owner follow-up flag");
  }
  for (int player = 0; player < 2; ++player) {
    fight(player, 2.f);
    Fighter *victim = &g_battle.fighters[1 - player];
    ice_command(player, 2, 0);
    check(fighter_move(&g_battle.fighters[player])->clip == 16, "D,F,LP must enter ice");
    check(sound_events(SOUND_INDEXED, 4) == 1 && sound_events(SOUND_C, 0) == 0, "ice entry must queue its indexed cue once before the release sound");
    advance(14);
    check(objects(PROJECTILE_ICE) == 0, "ice must wait for startup");
    advance(1);
    check(sound_events(SOUND_C, 0) == 1 && sound_events(SOUND_INDEXED, 4) == 0, "normal ice release must retain C[0] without repeating its indexed entry cue");
    advance(13);
    check(victim->appearance == APPEARANCE_FROZEN && victim->action == 0x1003 && victim->health == Q16_ONE, "ice must freeze without direct damage");
    check(victim->buttons == NULL && (victim->flags & FF_NO_GUARD), "frozen victim must be locked and unable to guard");
    int frozen_age = victim->ticks;
    q16 x = victim->body.pos[AXIS_X];
    int pose = victim->sample_override;
    advance(141 - frozen_age);
    check(victim->appearance == APPEARANCE_FROZEN && victim->body.pos[AXIS_X] == x && victim->sample_override == pose,
        "freeze must hold pose and body through tick 141");
    advance(1);
    check(victim->appearance == APPEARANCE_NORMAL && victim->state == S_IDLE && victim->buttons != NULL && !(victim->flags & FF_NO_GUARD),
        "ground thaw must restore controls at tick 142");

    fight(player, 14.f);
    ice_command(player, 2, 0);
    advance(15);
    check(objects(PROJECTILE_ICE) == 1, "ice object must spawn");
    Projectile *shot = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (g_battle.projectiles[i].kind == PROJECTILE_ICE) {
        shot = &g_battle.projectiles[i];
      }
    }
    check(shot != NULL && shot->mesh_animation != NULL && shot->mesh_animation->scale == Q16_ONE,
        "ice command must bind its original mesh sequence at full size");
    Projectile initial = *shot;
    initial.ticks = 0;
    check(projectile_mesh_frame(&initial) == 74, "ice spawn rig must remain separate from its animated track");
    check(projectile_mesh_frame(shot) == 75, "ice must begin its animated track with mesh 75");
    advance(1);
    int trails = 0;
    for (int i = 0; i < MAX_PARTICLES; ++i) {
      const Particle *p = &g_battle.particles[i];
      if (p->ticks > 0 && p->visual != NULL && p->visual->billboard) {
        check(p->visual->animation.first == 339 && p->scale > 0 && p->scale <= 0x4000, "ice trail must use its original quarter-size shrinking sprite");
        ++trails;
      }
    }
    check(trails == 20, "each ice animation tick must emit ten independent trail nodes");
    battle_react(&g_battle.fighters[player], 37);
    advance(1);
    check(objects(PROJECTILE_ICE) == 1, "independent ice object must survive owner interruption");
    for (int frame = 77; frame <= 82; ++frame) {
      check(projectile_mesh_frame(shot) == frame, "ice must visit every track mesh once per tick independently of the owner");
      advance(1);
    }
    check(projectile_mesh_frame(shot) == 75, "ice must loop from mesh 82 to 75 without replaying its spawn rig");
    advance(180);
    check(objects(PROJECTILE_ICE) == 0, "ice must expire after contact/bounds");

    for (int gate = 0; gate < 4; ++gate) {
      fight(player, 4.f);
      if (gate == 1) {
        g_battle.fighters[1 - player].hits = 2;
      }
      if (gate == 2) {
        g_battle.fighters[player].body.pos[AXIS_Y] -= Q16_ONE;
        fighter_enter_state(&g_battle.fighters[player], S_JUMP);
      }
      ice_command(player, gate == 0 ? 27 : 2, gate == 3 ? BUTTON_BIT(BUTTON_BLOCK) : 0);
      check(fighter_move(&g_battle.fighters[player])->clip != 16, "ice must reject late/juggle/air/block gates");
      check(sound_events(SOUND_INDEXED, 4) == 0, "rejected ice command must not play the entry cue");
    }
    fight(player, 14.f);
    g_battle.fighters[player].flags |= FF_FOLLOW_UP;
    ice_command(player, 2, 0);
    check(g_battle.fighters[player].script_phase == 1 && sound_events(SOUND_INDEXED, 4) == 1, "fast ice must use the same one-shot entry cue");
    advance(7);
    check(objects(PROJECTILE_ICE) == 0 && sound_events(SOUND_INDEXED, 4) == 0, "fast ice must wait seven ticks without repeating its cue");
    advance(1);
    check(objects(PROJECTILE_ICE) == 1 && sound_events(SOUND_C, 0) == 1 && sound_events(SOUND_INDEXED, 4) == 0,
        "fast ice must play C[0] at its eight-tick release");
    fight(player, 4.f);
    ice_command(player, 24, 0);
    check(fighter_move(&g_battle.fighters[player])->clip == 16, "ice accepts inclusive 26-tick command window");
    fight(player, 14.f);
    ice_command(player, 2, 0);
    advance(58);
    check(objects(PROJECTILE_ICE) == 1 && g_battle.fighters[player].state == S_IDLE, "ice flight must outlive attacker recovery");
    ice_command(player, 2, 0);
    check(fighter_move(&g_battle.fighters[player])->clip == 16 && objects(PROJECTILE_ICE) == 1, "ice gate must allow another shot when no clone exists");
    battle_next_round();
    check(objects(PROJECTILE_ICE) == 0 && g_battle.fighters[1 - player].appearance == APPEARANCE_NORMAL, "round reset must clear ice and freeze");

    fight(player, 2.f);
    ice_command(player, 2, 0);
    Input guard[2] = {0};
    guard[1 - player].held = BUTTON_BIT(BUTTON_BLOCK);
    for (int t = 0; t < 45; ++t) {
      battle_step(guard);
    }
    check(g_battle.fighters[1 - player].health == Q16_ONE && g_battle.fighters[1 - player].appearance == APPEARANCE_NORMAL && objects(PROJECTILE_ICE) == 0,
        "block must consume ice without damage or freeze");

    fight(player, 4.f);
    victim = &g_battle.fighters[1 - player];
    battle_react(victim, 67);
    advance(1);
    check(victim->appearance == APPEARANCE_FROZEN, "explicit freeze reaction");
    battle_react(victim, 67);
    advance(2);
    check(victim->appearance == APPEARANCE_NORMAL && g_battle.fighters[player].appearance == APPEARANCE_FROZEN,
        "double freeze must backfire and release original victim");
    battle_react(&g_battle.fighters[player], 37);
    advance(1);
    check(g_battle.fighters[player].appearance == APPEARANCE_NORMAL && !(g_battle.fighters[player].flags & FF_NO_GUARD),
        "new hit must clear frozen presentation and guard restriction");

    fight(player, 4.f);
    victim = &g_battle.fighters[1 - player];
    victim->body.pos[AXIS_Y] -= Q16_ONE;
    fighter_enter_state(victim, S_JUMP);
    battle_react(victim, 67);
    advance(143);
    check(victim->body.gravity == 0x147 && fighter_airborne(victim), "air thaw must restore its own gravity");
    advance(150);
    check(victim->state == S_IDLE && !fighter_airborne(victim) && victim->appearance == APPEARANCE_NORMAL, "air thaw must land and recover");
  }
}

static void independent_contacts(void) {
  fight(0, 1.f);
  Fighter *owner = &g_battle.fighters[0];
  Fighter *victim = &g_battle.fighters[1];
  const AttackRecord record = {Q16_ONE, -Q16_ONE, Q16_ONE, Q16_ONE, 67, 67, 0, 0, 0};
  check(battle_contact_from(owner, &record, owner->body.pos, owner->facing) == CONTACT_HIT, "object contact must resolve");
  check(!owner->connected, "independent object must not consume the owner's current strike");
  check(battle_contact(owner, &record) == CONTACT_HIT && owner->connected, "fighter contact must latch its own result");
  owner->connected = false;
  Projectile ray = {.owner = 0, .facing = 1, .heading = {Q16_ONE, 0}};
  ray.pos[AXIS_X] = victim->body.pos[AXIS_X] - Q16_ONE / 2;
  ray.pos[AXIS_Y] = owner->body.pos[AXIS_Y];
  victim->body.pos[AXIS_Z] = Q16_ONE / 5;
  check(battle_projectile_contact(&ray, &record) == CONTACT_MISS, "projectile bearing gate must reject a nearby sidestepped victim");
  victim->body.pos[AXIS_Z] = 0;
  check(battle_projectile_contact(&ray, &record) == CONTACT_HIT && !owner->connected, "aligned projectile must contact without latching the owner");
  for (int air = 0; air < 2; ++air) {
    victim->body.pos[AXIS_Y] = victim->body.ground - air * Q16_ONE;
    for (int followup = 0; followup < 2; ++followup) {
      owner->flags = followup ? FF_FOLLOW_UP : 0;
      for (int reacting = 0; reacting < 2; ++reacting) {
        victim->flags = reacting ? FF_REACTING : 0;
        check(fighter_fast_start(owner) == (air ? reacting : followup), "opcode 45 must select the appropriate fighter's flags");
      }
    }
  }
}

static void clone_command(int player) {
  press(player, BUTTON_BIT(BUTTON_DOWN));
  press(player, BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT) | BUTTON_BIT(BUTTON_LOW_PUNCH));
}

static void idle_object(Projectile *p) {
  (void)p;
}

static void clone_tests(void) {
  fight(0, 4.f);
  clone_command(0);
  advance(8);
  check(g_battle.fighters[0].sample_override == 8 && g_battle.fighters[0].body.spin[AXIS_X] == -0x6147,
      "clone retreat must finish forward with native angular motion");
  advance(15);
  check(g_battle.fighters[0].script_phase == 1 && g_battle.fighters[0].body.spin[AXIS_X] == 0 && g_battle.fighters[0].body.angle[AXIS_X] == 0,
      "clone retreat must restore orientation and retract after the apex");
  advance(100);
  check(g_battle.fighters[0].state == S_IDLE && g_battle.fighters[0].body.root_y == 0, "clone landing must restore its root and controls");
  for (int player = 0; player < 2; ++player) {
    fight(player, 2.f);
    clone_command(player);
    check(objects(PROJECTILE_CLONE) == 1 && g_battle.fighters[player].action == 0x405, "clone must capture pose immediately");
    Projectile *clone = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (g_battle.projectiles[i].kind == PROJECTILE_CLONE) {
        clone = &g_battle.projectiles[i];
      }
    }
    q16 clone_x = clone->pos[AXIS_X];
    check(clone->clip >= -1 && clone->sample >= 0, "clone must own its captured pose");
    check(clone->light.def != NULL && clone->light.phase == 0x18000 && clone->light.strength == Q16_ONE - 0xccc && clone->light.pos[AXIS_Y] == -25 * Q16_ONE,
        "clone must open its independent orbit light at the recovered phase, height and fade");
    battle_react(&g_battle.fighters[player], 37);
    advance(2);
    check(objects(PROJECTILE_CLONE) == 1 && clone->pos[AXIS_X] == clone_x, "clone must remain stationary after owner interruption");
    check(clone->light.phase == 0x48000 && clone->light.strength == Q16_ONE - 3 * 0xccc, "clone light must keep its own clock after owner interruption");
    g_battle.fighters[1 - player].body.pos[AXIS_X] = clone_x + clone->facing * 0x6000;
    advance(2);
    check(objects(PROJECTILE_CLONE) == 0 && g_battle.fighters[1 - player].appearance == APPEARANCE_FROZEN, "clone must freeze once on contact");
    check(g_battle.fighters[1 - player].health == Q16_ONE, "clone freeze must not deal direct damage");
    battle_reset_match();
    check(g_battle.fighters[player].clone_ready == 0 && objects(PROJECTILE_CLONE) == 0, "rematch clears clone state");

    fight(player, .7f);
    clone_command(player);
    check(objects(PROJECTILE_CLONE) == 0 && g_battle.fighters[player].action == 0x406, "near clone must retreat without spawning");
    advance(100);
    check(g_battle.fighters[player].state == S_IDLE, "near branch must land and release controls");

    fight(player, 4.f);
    clone_command(player);
    advance(50);
    check(g_battle.fighters[player].state == S_IDLE && objects(PROJECTILE_CLONE) == 1, "retreat must recover while clone remains");
    ice_command(player, 2, 0);
    check(fighter_move(&g_battle.fighters[player])->clip != 16, "live clone must gate ice blast");
    advance(80);
    check(objects(PROJECTILE_CLONE) == 0, "clone must expire");
    uint64_t ready = g_battle.fighters[player].clone_ready;
    g_battle.ticks = ready - 4;
    clone_command(player);
    check(g_battle.fighters[player].action != 0x405, "clone cooldown rejects immediately below boundary");
    advance(100);
    clone_command(player);
    check(objects(PROJECTILE_CLONE) == 1, "clone cooldown must permit another activation");

    fight(player, 4.f);
    g_battle.fighters[player].clone_ready = 3;
    clone_command(player);
    check(objects(PROJECTILE_CLONE) == 1, "clone accepts cooldown at boundary");
    battle_next_round();
    g_battle.phase = PHASE_FIGHT;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      check(battle_spawn_projectile(&g_battle.fighters[player], PROJECTILE_SWORD, 200, idle_object) != NULL, "fill object pool");
    }
    clone_command(player);
    advance(100);
    check(g_battle.fighters[player].state == S_IDLE && objects(PROJECTILE_CLONE) == 0, "full pool must not strand clone owner");

    fight(player, 4.f);
    g_battle.fighters[player].body.pos[AXIS_Y] -= Q16_ONE;
    fighter_enter_state(&g_battle.fighters[player], S_JUMP);
    clone_command(player);
    check(objects(PROJECTILE_CLONE) == 1, "clone command permits air activation");
    advance(180);
    check(g_battle.fighters[player].state == S_IDLE, "air clone must land and recover");
  }

  battle_init(4, 4);
  g_battle.phase = PHASE_FIGHT;
  clone_command(0);
  clone_command(1);
  check(objects(PROJECTILE_CLONE) == 2, "mirror clones must have independent owners");
  battle_reset_match();
  check(objects(PROJECTILE_CLONE) == 0, "mirror reset clears both objects");
}

static unsigned slide_chord(void) {
  return BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
}

static void slide_tests(void) {
  for (int player = 0; player < 2; ++player) {
    for (int outcome = 0; outcome < 4; ++outcome) {
      fight(player, outcome == 2 ? 8.f : 2.f);
      Input input[2] = {0};
      input[player].held = slide_chord();
      if (outcome == 1) {
        input[1 - player].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      if (outcome == 3) {
        g_battle.fighters[1 - player].body.pos[AXIS_Y] -= 0x3333;
        fighter_enter_state(&g_battle.fighters[1 - player], S_JUMP);
      }
      battle_step(input);
      input[player].held = 0;
      battle_step(input);
      check(fighter_move(&g_battle.fighters[player])->clip == 95, "slide chord must enter clip 95");
      check(g_battle.fighters[player].current_mmove != NULL && !fighter_fiber_active(&g_battle.fighters[player]), "slide entry must use frame phases");
      for (int t = 0; t < 8; ++t) {
        battle_step(input);
      }
      check(g_battle.fighters[1 - player].health == Q16_ONE, "slide damaged before contact window");
      for (int t = 0; t < 18; ++t) {
        battle_step(input);
      }
      q16 damage = outcome == 2 ? 0 : outcome == 1 ? q16_mul(0x147a, 0x3333) : 0x147a;
      check(g_battle.fighters[1 - player].health == Q16_ONE - damage, "slide hit/block/miss/air record damage");
      check(g_battle.fighters[1 - player].reaction == (outcome == 2 ? 0 : outcome == 1 ? 165 : 122), "slide must select explicit victim reaction");
      check((g_battle.fighters[player].slide_ready != 0) == (outcome == 0 || outcome == 3), "only unblocked slide contact starts cooldown");
      check(!fighter_fiber_active(&g_battle.fighters[player]), "slide branch allocated a coroutine");
      advance(180);
      check(g_battle.fighters[player].state == S_IDLE && g_battle.fighters[1 - player].state == S_IDLE, "slide must recover both fighters");
      check(!(g_battle.fighters[player].flags & FF_NO_GUARD), "blocked slide must clear its recovery guard restriction");
      check(g_battle.fighters[1 - player].health == Q16_ONE - damage, "slide must deal damage once");
    }
    fight(player, 2.f);
    Input crouch_input[2] = {0};
    crouch_input[player].held = slide_chord();
    crouch_input[1 - player].held = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_DOWN);
    battle_step(crouch_input);
    crouch_input[player].held = 0;
    for (int t = 0; t < 12; ++t) {
      battle_step(crouch_input);
    }
    check(g_battle.fighters[1 - player].reaction == 165 && fighter_move(&g_battle.fighters[1 - player])->clip == 255,
        "crouching slide block must select clip 255");
    check(g_battle.fighters[player].flags & FF_NO_GUARD, "blocked slide must enter no-guard recovery");
    fighter_interrupt(&g_battle.fighters[player], fighter_ready);
    check(!(g_battle.fighters[player].flags & FF_NO_GUARD) && g_battle.fighters[player].mmove_cleanup == NULL,
        "interrupting slide recovery must release its guard restriction exactly once");
    const Button triggers[] = {BUTTON_LOW_PUNCH, BUTTON_BLOCK, BUTTON_LOW_KICK};
    for (int k = 0; k < 3; ++k) {
      fight(player, 6.f);
      Input input[2] = {0};
      input[player].held = slide_chord() & ~BUTTON_BIT(triggers[k]);
      for (int t = 0; t < 80; ++t) {
        battle_step(input);
      }
      input[player].held = slide_chord();
      battle_step(input);
      advance(1);
      check(fighter_move(&g_battle.fighters[player])->clip == 95, "each LP/BLK/LK press can complete slide chord");
    }
    const Button forbidden[] = {BUTTON_UP, BUTTON_HIGH_PUNCH, BUTTON_HIGH_KICK, BUTTON_RUN};
    for (int k = 0; k < 4; ++k) {
      fight(player, 6.f);
      press(player, slide_chord() | BUTTON_BIT(forbidden[k]));
      check(fighter_move(&g_battle.fighters[player])->clip != 95, "slide must reject forbidden extra buttons");
    }
    for (int gate = 0; gate < 3; ++gate) {
      fight(player, 6.f);
      if (gate == 0) {
        g_battle.fighters[player].slide_ready = 2;
      } else if (gate == 1) {
        g_battle.fighters[player].body.pos[AXIS_Y] -= Q16_ONE;
        fighter_enter_state(&g_battle.fighters[player], S_JUMP);
      } else {
        battle_react(&g_battle.fighters[player], 37);
      }
      press(player, slide_chord());
      check(fighter_move(&g_battle.fighters[player])->clip != 95, "slide must reject cooldown/air/reaction gates");
    }
    fight(player, 6.f);
    g_battle.fighters[player].slide_ready = 1;
    press(player, slide_chord() | BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT));
    check(fighter_move(&g_battle.fighters[player])->clip == 95, "slide cooldown is inclusive and does not require direction history");
    advance(4);
    battle_react(&g_battle.fighters[player], 37);
    advance(100);
    check(g_battle.fighters[1 - player].health == Q16_ONE && g_battle.fighters[player].state == S_IDLE, "interrupted startup must not resume slide or contact");
  }
}

static void weapon_command(int player) {
  press(player, BUTTON_BIT(BUTTON_DOWN));
  advance(2);
  press(player, BUTTON_BIT(player ? BUTTON_LEFT : BUTTON_RIGHT) | BUTTON_BIT(BUTTON_HIGH_KICK));
}

static void draw_scepter(int player) {
  weapon_command(player);
  check(fighter_move(&g_battle.fighters[player])->clip == 244, "D,F,HK must draw scepter");
  advance(21);
  check(!g_battle.fighters[player].armed, "scepter must wait until draw event");
  advance(28); /* Generated full-clip playback includes its final suspension. */
  check(g_battle.fighters[player].armed && g_battle.fighters[player].weapon->type == 0x6b && fighter_move(&g_battle.fighters[player])->clip == 286,
      "draw must equip descriptor 6b and armed stance");
}

static void weapons(void) {
  for (int player = 0; player < 2; ++player) {
    unsigned back = BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT);
    for (int attack = 0; attack < 4; ++attack) {
      for (int outcome = 0; outcome < 3; ++outcome) {
        fight(player, 4.f);
        draw_scepter(player);
        if (outcome != 2) {
          g_battle.fighters[player].body.pos[AXIS_X] = player ? Q16_ONE / 2 : -Q16_ONE / 2;
          g_battle.fighters[1 - player].body.pos[AXIS_X] = player ? -Q16_ONE / 2 : Q16_ONE / 2;
        }
        unsigned button = BUTTON_BIT(attack % 2 == 0 ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH) | (attack >= 2 ? back : 0);
        Input input[2] = {0};
        input[player].held = button;
        input[1 - player].held = outcome == 1 ? BUTTON_BIT(BUTTON_BLOCK) | (attack == 1 ? BUTTON_BIT(BUTTON_DOWN) : 0) : 0;
        battle_step(input);
        input[player].held = 0;
        battle_step(input);
        const int clips[] = {231, 250, 237, 228};
        const q16 damages[] = {11796, 9830, 9830, 0};
        check(fighter_move(&g_battle.fighters[player])->clip == clips[attack], "scepter must select distinct directional strikes");
        for (int i = 0; i < 50; ++i) {
          battle_step(input);
        }
        q16 expected = outcome == 2 ? 0 : outcome == 1 ? q16_mul(damages[attack], 0x3333) : damages[attack];
        check(g_battle.fighters[1 - player].health == Q16_ONE - expected, "scepter hit/block/miss must apply one record");
        if (attack == 3 && outcome == 0) {
          check(g_battle.fighters[1 - player].appearance == APPEARANCE_FROZEN, "back LP must freeze without damage");
        }
        advance(180);
        check(g_battle.fighters[player].state == S_IDLE && g_battle.fighters[player].armed && g_battle.fighters[1 - player].state == S_IDLE,
            "weapon strike and victim must recover");
      }
    }
    fight(player, 4.f);
    draw_scepter(player);
    press(player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH));
    check(fighter_move(&g_battle.fighters[player])->clip == 260, "scepter must keep normal crouching LP");
    advance(80);
    battle_react(&g_battle.fighters[player], 51);
    check(!g_battle.fighters[player].armed && objects(PROJECTILE_SWORD) == 1, "scepter launch must drop actual weapon");
    Projectile *drop = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (g_battle.projectiles[i].kind == PROJECTILE_SWORD) {
        drop = &g_battle.projectiles[i];
      }
    }
    check(drop != NULL && drop->spin[AXIS_Z] == 0x20c4 && drop->vel[AXIS_Y] == -0x2666, "dropped weapon must open the recovered first settling arc");
    int impacts = 0;
    for (int tick = 0; tick < 180; ++tick) {
      int previous = drop->bounce;
      advance(1);
      if (drop->bounce != previous) {
        bool sounded = false;
        for (int i = 0; i < g_battle.sound_count; ++i) {
          sounded |= g_battle.sounds[i].bank == SOUND_C && g_battle.sounds[i].selector == 0x15;
        }
        check(sounded, "each settling impact must request the retail weapon sound");
        impacts++;
      }
    }
    check(impacts == 3 && drop->bounce == 3 && drop->angle[AXIS_Z] == 0x191eb && drop->spin[AXIS_Z] == 0,
        "shared weapon settling must complete three arcs and retain the final resting angle");
    check(drop != NULL && drop->phase == 2 && drop->weapon->type == 0x6b, "resting weapon must retain scepter descriptor");
    Fighter *other = &g_battle.fighters[1 - player];
    other->body.pos[AXIS_X] = drop->pos[AXIS_X];
    other->body.pos[AXIS_Z] = drop->pos[AXIS_Z];
    fighter_enter_state(other, S_CROUCH);
    press(1 - player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RUN));
    check(other->armed && other->weapon->type == 0x6b && drop->kind == 0, "Scorpion must pick up Sub-Zero's actual weapon");
    advance(20);
    other->body.pos[AXIS_X] = player ? -3 * Q16_ONE : 3 * Q16_ONE;
    g_battle.fighters[player].body.pos[AXIS_X] = player ? 3 * Q16_ONE : -3 * Q16_ONE;
    press(1 - player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter_move(other)->clip == 231, "foreign scepter must use scepter attacks");

    fight(player, 4.f);
    draw_scepter(player);
    weapon_command(player);
    check(fighter_move(&g_battle.fighters[player])->clip == 208, "armed D,F,HK must throw");
    advance(23);
    check(!g_battle.fighters[player].armed, "throw must detach scepter");
    g_battle.contacts_off = true;
    battle_react(&g_battle.fighters[player], 37);
    advance(180);
    check(objects(PROJECTILE_SWORD) == 0, "thrown weapon must leave arena after bounded flight");
    weapon_command(player);
    check(fighter_move(&g_battle.fighters[player])->clip != 244, "spent weapon must not draw again");
    battle_init(4, 0);
    check(!g_battle.fighters[0].armed && !g_battle.fighters[0].weapon_used && g_battle.fighters[0].weapon == NULL, "round reset clears weapon descriptor");

    fight(player, 4.f);
    draw_scepter(player);
    press(player, BUTTON_BIT(BUTTON_UP));
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter_move(&g_battle.fighters[player])->clip == 252, "armed air HP must use shared weapon strike");
    advance(180);
    check(g_battle.fighters[player].state == S_IDLE && g_battle.fighters[player].armed, "armed air strike must land and retain weapon");

    fight(player, 4.f);
    g_battle.fighters[player].flags |= FF_FOLLOW_UP;
    weapon_command(player);
    check(g_battle.fighters[player].armed && fighter_move(&g_battle.fighters[player])->clip == 237, "follow-up draw must equip and enter back HP");
    advance(90);
    check(g_battle.fighters[player].state == S_IDLE, "follow-up draw must recover");

    fight(player, 4.f);
    int sword_owner = 1 - player;
    Button forward = sword_owner ? BUTTON_LEFT : BUTTON_RIGHT;
    press(sword_owner, BUTTON_BIT(forward));
    advance(2);
    press(sword_owner, BUTTON_BIT(forward) | BUTTON_BIT(BUTTON_HIGH_KICK));
    advance(50);
    check(g_battle.fighters[sword_owner].armed, "Scorpion draw for foreign pickup");
    g_battle.fighters[sword_owner].disarm(&g_battle.fighters[sword_owner]);
    advance(100);
    drop = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (g_battle.projectiles[i].kind == PROJECTILE_SWORD) {
        drop = &g_battle.projectiles[i];
      }
    }
    check(drop != NULL && drop->weapon->type == 0x60, "dropped sword descriptor");
    g_battle.fighters[player].body.pos[AXIS_X] = drop->pos[AXIS_X];
    g_battle.fighters[player].body.pos[AXIS_Z] = drop->pos[AXIS_Z];
    fighter_enter_state(&g_battle.fighters[player], S_CROUCH);
    press(player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RUN));
    check(g_battle.fighters[player].armed && g_battle.fighters[player].weapon->type == 0x60, "Sub-Zero must pick up sword descriptor");
    advance(20);
    g_battle.fighters[player].body.pos[AXIS_X] = player ? 3 * Q16_ONE : -3 * Q16_ONE;
    g_battle.fighters[sword_owner].body.pos[AXIS_X] = player ? -3 * Q16_ONE : 3 * Q16_ONE;
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter_move(&g_battle.fighters[player])->clip == 235, "Sub-Zero's foreign sword must retain sword attacks");
    advance(80);
    press(player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH));
    advance(4);
    check(fighter_move(&g_battle.fighters[player])->clip == 224, "foreign sword must enable the sword spin");
    advance(20);
    check(g_battle.fighters[player].state == S_IDLE, "foreign sword spin must release");

    for (int reaction = 150; reaction <= 151; ++reaction) {
      fight(player, 3.f);
      Fighter *victim = &g_battle.fighters[1 - player];
      victim->hits = 4;
      battle_react(victim, reaction);
      advance(1);
      check(fighter_move(victim)->clip == (reaction == 151 ? 131 : 192), "scepter reaction and freeze hit-cap branch");
      advance(180);
      check(victim->state == S_IDLE && victim->appearance == APPEARANCE_NORMAL, "scepter reactions must recover");
    }
  }
}

static void weapon_recovery(void) {
  fight(0, 4.f);
  draw_scepter(0);
  Fighter *fighter = &g_battle.fighters[0];
  g_battle.fighters[1].body.pos[AXIS_X] = fighter->body.pos[AXIS_X] + Q16_ONE;
  fighter->weapon->lp(fighter);
  for (int i = 0; i < 50 && fighter->blend_duration == 0; ++i) {
    advance(1);
  }
  check(fighter->blend_duration == 10 && fighter->blend_clip == 250 && fighter->blend_sample == 31, "scepter LP contact must finish then blend to stance");
  advance(9);
  check(fighter->blend_ticks == 9 && fighter->buttons == NULL, "stance blend retains recovery control lock");
  advance(1);
  check(fighter->state == S_IDLE && fighter->blend_duration == 0, "stance blend returns control after ten steps");

  fight(0, 4.f);
  fighter = &g_battle.fighters[0];
  Fighter *victim = &g_battle.fighters[1];
  fighter->flags |= FF_FOLLOW_UP;
  victim->hits = 4;
  victim->flags |= FF_NO_GUARD;
  weapon_command(0);
  check(
      victim->reaction == 120 && victim->damage_scale == 0x4ccc && (victim->flags & FF_NO_GUARD), "follow-up draw must prepare victim and scale a long combo");
  advance(10);
  check(!fighter->weapon_travel && fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0, "follow-up draw must omit ordinary back-HP travel");
}

static void air_throw_setup(void) {
  for (int i = 0; i < 2; ++i) {
    Fighter *fighter = &g_battle.fighters[i];
    fighter->body.pos[AXIS_Y] = -3 * Q16_ONE;
    fighter_enter_state(fighter, S_JUMP);
    fighter->body.vel[AXIS_Y] = 0;
    fighter->body.gravity = 0;
  }
}

static void air_throws(void) {
  for (int player = 0; player < 2; ++player) {
    fight(player, 1.f);
    air_throw_setup();
    Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
    owner->body.root_y = -0x1000;
    owner->body.angle[AXIS_Z] = 0x18000;
    owner->body.spin[AXIS_Z] = 0x3000;
    press(player, BUTTON_BIT(BUTTON_BLOCK));
    check(fighter_move(owner)->clip == 82 && fighter_move(victim)->clip == 202 && owner->action == 0x502, "air BLOCK must enter the shared paired throw");
    check(owner->body.gravity == 0 && victim->body.gravity == 0 && owner->body.root_y == 0 && owner->body.spin[AXIS_Z] == 0,
        "air throw must cancel jump physics and rotation before pairing");
    check(victim->body.pos[AXIS_X] == owner->body.pos[AXIS_X] && victim->body.pos[AXIS_Y] == owner->body.pos[AXIS_Y] && victim->facing == owner->facing,
        "air throw must align the victim with the upright owner");
    check(sound_events(SOUND_INDEXED, 13) == 1, "air throw must play indexed selector 13 once on entry");
    advance(27);
    check(victim->health == Q16_ONE && owner->sample_override == 27 && victim->sample_override == 27, "air throw must hold both poses in sync until sample 28");
    advance(1);
    check(victim->health == Q16_ONE - 0x2147 && victim->reaction == 89 && victim->script_phase == 1,
        "air throw must damage once and start victim release at tick 28");
    check(!(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED) && victim->body.gravity == 0x28f && victim->body.vel[AXIS_Y] == 0x3333 + 0x28f,
        "both players must release the victim into the same tick of falling physics");
    int impacts = 0;
    for (int tick = 29; tick <= 54; ++tick) {
      advance(1);
      check(sound_events(SOUND_INDEXED, 13) == 0, "air throw must not repeat its entry cue");
      impacts += sound_events(SOUND_A, 0x13);
      if (sound_events(SOUND_A, 0x13)) {
        check(sound_events(SOUND_IMPACT_VOICE, roster_def(victim->roster)->asset) == 1 && g_battle.camera.shake_pulses == 3 &&
                  g_battle.camera.shake_interval == 4 && g_battle.camera.shake_amount == 0x2666,
            "floor impact must use the victim's roster voice and original camera shake");
      }
      if (tick == 29) {
        check(owner->body.vel[AXIS_Y] == -0x1999 + 0x28f && abs(owner->body.vel[AXIS_X]) == 0x7ae,
            "air throw release must use the script's forward speed, vertical launch and gravity");
      }
      if (tick == 29 || tick == 38) {
        check(owner->sample_override == 29, "air throw must hold the counted advance at sample 29 during its ten-tick wait");
      }
      if (tick == 39) {
        check(owner->sample_override == 29, "air throw must retain the current sample until the next forward visit");
      }
    }
    check(impacts == 1, "victim landing must emit its floor impact exactly once");
    advance(150);
    check(owner->state == S_IDLE && victim->state == S_IDLE && owner->buttons != NULL && victim->buttons != NULL,
        "both air throw participants must finish landing/getup and regain their own controls");
    check(victim->health == Q16_ONE - 0x2147, "air throw must not deal repeat damage during recovery");

    for (int gate = 0; gate < 8; ++gate) {
      fight(player, 1.f);
      air_throw_setup();
      owner = &g_battle.fighters[player];
      victim = &g_battle.fighters[1 - player];
      if (gate == 0 || gate == 1) {
        g_battle.fighters[gate == 0 ? player : 1 - player].body.pos[AXIS_Y] = -0x23332;
      } else if (gate == 2) {
        victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + 2 * Q16_ONE;
      } else if (gate == 3) {
        victim->body.pos[AXIS_Z] = 2 * Q16_ONE;
      } else if (gate == 4) {
        victim->body.pos[AXIS_Y] -= 2 * Q16_ONE;
      } else {
        victim->flags |= gate == 5 ? FF_REACTING : gate == 6 ? FF_UNTOUCHABLE : FF_PAIRED;
      }
      fighter_press(owner, BUTTON_BLOCK);
      check(owner->pending == NULL && sound_events(SOUND_INDEXED, 13) == 0, "air throw must reject low, distant and unavailable victims without a cue");
    }
    fight(player, 1.f);
    air_throw_setup();
    owner = &g_battle.fighters[player];
    victim = &g_battle.fighters[1 - player];
    owner->body.pos[AXIS_Y] = victim->body.pos[AXIS_Y] = -0x23333;
    victim->body.pos[AXIS_Z] = Q16_ONE;
    fighter_press(owner, BUTTON_BLOCK);
    check(owner->pending != NULL, "air throw must accept the inclusive retail height and 3-D squared-distance boundaries");
    victim->body.pos[AXIS_Y] = victim->body.ground;
    advance(1);
    check(fighter_move(owner)->clip != 82 && sound_events(SOUND_INDEXED, 13) == 0, "queued air throw must recheck its victim before taking control");

    for (int participant = 0; participant < 2; ++participant) {
      for (int tick = 0; tick <= 27; tick += 27) {
        fight(player, 1.f);
        air_throw_setup();
        press(player, BUTTON_BIT(BUTTON_BLOCK));
        advance(tick);
        battle_react(&g_battle.fighters[participant ? 1 - player : player], 11);
        advance(180);
        check(g_battle.fighters[1 - player].health == Q16_ONE, "interruption before or on the release tick must cancel throw damage");
        check(g_battle.fighters[0].state == S_IDLE && g_battle.fighters[1].state == S_IDLE &&
                  !((g_battle.fighters[0].flags | g_battle.fighters[1].flags) & FF_PAIRED),
            "interrupting either participant must release the pair and recover both controllers");
      }
    }
    battle_init(4, 4);
    g_battle.phase = PHASE_FIGHT;
    draw_scepter(1 - player);
    g_battle.fighters[0].body.pos[AXIS_X] = 0;
    g_battle.fighters[1].body.pos[AXIS_X] = Q16_ONE;
    air_throw_setup();
    press(player, BUTTON_BIT(BUTTON_BLOCK));
    check(fighter_move(&g_battle.fighters[player])->clip == 82 && !g_battle.fighters[1 - player].armed, "air throw must disarm the captured victim");

    fight(player, 1.f);
    air_throw_setup();
    g_battle.fighters[player].wins = 1;
    g_battle.fighters[1 - player].health = 0x1000;
    press(player, BUTTON_BIT(BUTTON_BLOCK));
    advance(28);
    check(g_battle.phase == PHASE_FINISH && g_battle.finish_waiting && g_battle.fighters[1 - player].health == 0,
        "lethal air throw must finish the victim's falling reaction before opening the finish window");
    for (int tick = 0; tick < 120 && g_battle.finish_waiting; ++tick) {
      advance(1);
    }
    check(!g_battle.finish_waiting && g_battle.phase == PHASE_FINISH && g_battle.fighters[1 - player].action == 0x1009 &&
              !fighter_airborne(&g_battle.fighters[player]) && !fighter_airborne(&g_battle.fighters[1 - player]),
        "lethal air throw must land both fighters and hand off to the shared finish controller");
  }
}

static int ground_throw_disarms;

static void ground_throw_disarm(Fighter *fighter) {
  ground_throw_disarms++;
  fighter->armed = false;
}

static void start_ground_throw(int player, bool lk) {
  fight(player, .6f);
  ground_throw_disarms = 0;
  Fighter *victim = &g_battle.fighters[1 - player];
  victim->armed = true;
  victim->disarm = ground_throw_disarm;
  press(player, BUTTON_BIT(lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH));
  check(g_battle.fighters[player].ticks == 0 && fighter_move(&g_battle.fighters[player])->clip == (lk ? 41 : 76) &&
            fighter_move(victim)->clip == (lk ? 165 : 153),
      "Sub-Zero throws must select retail owner/victim clips for both player slots");
  check((lk ? fighter_fiber_active(&g_battle.fighters[player]) && fighter_fiber_active(victim)
            : g_battle.fighters[player].current_mmove != NULL && victim->current_mmove != NULL && !fighter_fiber_active(&g_battle.fighters[player]) &&
                    !fighter_fiber_active(victim)) &&
            (g_battle.fighters[player].flags & FF_PAIRED) && (victim->flags & FF_PAIRED),
      "retail throw entry must own both playback lifetimes and lock the pair");
}

static void ground_throws(void) {
  for (int player = 0; player < 2; ++player) {
    for (int lk = 0; lk < 2; ++lk) {
      start_ground_throw(player, lk);
      Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
      const int entry_facing = owner->facing;
      const int damage_at = lk ? 52 : 25, disarm_at = lk ? 39 : 25, release_at = lk ? 57 : 25, owner_end = lk ? 117 : 49;
      const q16 damage = lk ? 0x2e14 : 0x2666;
      for (int tick = 1; tick <= 130; ++tick) {
        advance(1);
        if (!lk) {
          check(!fighter_fiber_active(owner) && !fighter_fiber_active(victim), "LP throw allocated a fiber during capture, release or getup");
        }
        check(victim->health == Q16_ONE - (tick >= damage_at ? damage : 0), "throw must damage once at LP 25 / LK 52 without chip or Scorpion timing");
        check(ground_throw_disarms == (tick >= disarm_at ? 1 : 0), "LP must disarm on release; LK must disarm earlier at sample 39");
        if (tick < release_at) {
          check((owner->flags & FF_PAIRED) && (victim->flags & FF_PAIRED), "throw must retain the pair until its retail release boundary");
        } else {
          check(!(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED), "release must detach both actors in the same world tick");
        }
        if (tick < owner_end) {
          check(owner->state == S_THROW && (lk ? fighter_fiber_active(owner) : owner->current_mmove != NULL && !fighter_fiber_active(owner)),
              "owner must remain locked through its full retail recovery");
        } else {
          check(owner->state == S_IDLE && !fighter_fiber_active(owner), "owner must return at LP 49 / LK 117");
        }
        if (!lk) {
          if (tick < 25) {
            check(fighter_sample(owner) == tick && fighter_sample(victim) == tick, "LP paired poses must stay synchronized through sample 24");
          }
          if (tick == 25 || tick == 41) {
            check(victim->reaction == 102 && fighter_move(victim)->clip == 142 && fighter_sample(victim) == tick - 25,
                "LP release must select reaction 102 / tumble 142 without a one-tick player-order difference");
          }
          if (tick == 42 || tick == 58) {
            check(fighter_move(victim)->clip == 141 && fighter_sample(victim) == tick - 42, "LP must play tumble 141 after 17 samples of 142");
          }
          if (tick == 59 || tick == 87 || tick == 107) {
            check(
                fighter_move(victim)->clip == 143 && fighter_sample(victim) == (tick < 87 ? tick - 59 : 28), "LP must finish clip 143 then hold twenty ticks");
          }
          if (tick == 108 || tick == 122) {
            check(fighter_move(victim)->clip == 328 && fighter_sample(victim) == tick - 108 && victim->action == ACTION_RETAIL_200E,
                "LP victim must reverse into getup 328 after the twenty-tick hold");
            check(victim->facing == -entry_facing, "LP reverse getup must turn from the aligned throw facing");
          }
          if (tick == 123) {
            check(victim->state == S_IDLE && !fighter_fiber_active(victim), "LP victim must finish its own getup independently of the owner");
          }
        } else {
          if (tick <= 10) {
            check(fighter_sample(owner) == tick && fighter_sample(victim) == 0, "LK opening advances only the owner for ten samples");
          }
          if (tick < 52) {
            check(owner->body.vel[AXIS_X] == 0 && victim->body.vel[AXIS_X] == 0, "LK must save its entry momentum without moving during paired startup");
          }
          if (tick == 52) {
            check(abs(victim->body.vel[AXIS_X]) == 0x1999 && owner->body.vel[AXIS_X] == q16_mul(-victim->body.vel[AXIS_X], 0xe666),
                "LK damage must restore opposite entry momentum and start owner friction in the same world tick");
          }
          if (tick == 39 || tick == 44 || tick == 52) {
            check(fighter_sample(owner) == tick && fighter_sample(victim) == tick - 10, "LK paired count operands must preserve the ten-sample cursor offset");
          }
          if (tick >= 57 && tick < 117) {
            check(fighter_move(owner)->clip == CLIP_ASSET_IDLE && fighter_sample(owner) == 0, "LK owner must hold an idle pose for sixty ticks after release");
          }
          if (tick == 57 || tick == 61 || tick == 66 || tick == 71 || tick == 76 || tick == 81) {
            int expected = tick == 57 ? 45 : tick == 61 || tick == 71 || tick == 81 ? 41 : 46;
            check(fighter_move(victim)->clip == 165 && fighter_sample(victim) == expected, "LK victim must execute all three reverse/forward 41..46 cycles");
          }
          if (tick == 86 || tick == 126) {
            check(fighter_move(victim)->clip == 200 && fighter_sample(victim) == tick - 86, "LK victim must finish with retail clip 200");
          }
          if (tick == 127) {
            check(victim->state == S_IDLE && !fighter_fiber_active(victim), "LK victim must release control after clip 200");
          }
        }
      }
      check(
          victim->state == S_IDLE && victim->body.gravity == 0 && victim->body.on_land == NULL && owner->body.vel[AXIS_X] == 0 && victim->body.vel[AXIS_X] == 0,
          "throw recovery must clear physics/callbacks and restore the victim's own controller");

      for (int interrupted = 0; interrupted < 2; ++interrupted) {
        start_ground_throw(player, lk);
        advance(damage_at - 1);
        fighter_interrupt(&g_battle.fighters[interrupted ? 1 - player : player], fighter_ready);
        advance(150);
        if (victim->health != Q16_ONE || owner->state != S_IDLE || victim->state != S_IDLE || fighter_fiber_active(owner) || fighter_fiber_active(victim) ||
            (owner->flags & FF_PAIRED) || (victim->flags & FF_PAIRED)) {
          fprintf(stderr, "throw interruption player=%d lk=%d target=%d health=%x states=%d/%d fibers=%d/%d flags=%x/%x\n", player, lk, interrupted,
              victim->health, owner->state, victim->state, fighter_fiber_active(owner), fighter_fiber_active(victim), owner->flags, victim->flags);
        }
        check(victim->health == Q16_ONE && owner->state == S_IDLE && victim->state == S_IDLE && !fighter_fiber_active(owner) && !fighter_fiber_active(victim) &&
                  !(owner->flags & FF_PAIRED) && !(victim->flags & FF_PAIRED),
            "interrupting either actor immediately before damage must cancel the paired throw and all stale continuations");
      }
      start_ground_throw(player, lk);
      victim->damage_scale = 0x8000;
      advance(damage_at);
      check(victim->health == Q16_ONE - q16_mul(0x8000, damage), "retail throw damage must retain victim damage scaling");

      start_ground_throw(player, lk);
      advance(release_at);
      fighter_interrupt(owner, fighter_ready);
      check(lk ? fighter_fiber_active(victim) : victim->current_mmove != NULL && !fighter_fiber_active(victim),
          "owner interruption after release must preserve the victim's independent recovery");
      advance(150);
      check(owner->state == S_IDLE && victim->state == S_IDLE && victim->health == Q16_ONE - damage, "released victim must recover without repeated damage");

      start_ground_throw(player, lk);
      advance(release_at);
      battle_react(victim, 67);
      advance(40);
      check(victim->action == ACTION_RETAIL_1003 && !fighter_fiber_active(victim),
          "hit interruption must cancel old victim recovery instead of resuming its getup");
      advance(150);
      check(victim->state == S_IDLE, "interrupted victim must finish the new reaction");

      if (!lk) {
        start_ground_throw(player, false);
        advance(release_at);
        check(victim->body.on_land != NULL, "LP tumble must install its landing impact callback");
        fighter_interrupt(victim, fighter_ready);
        check(victim->body.on_land == NULL && !fighter_fiber_active(victim), "queued interruption must retire the LP landing callback with its move");
        advance(150);
        check(victim->state == S_IDLE && victim->health == Q16_ONE - damage, "cancelled LP recovery must not resume or leave a suspended body");
      }

      start_ground_throw(player, lk);
      owner->wins = 1;
      victim->health = damage / 2;
      advance(damage_at);
      check(g_battle.phase == PHASE_FINISH && g_battle.finish_waiting && victim->health == 0 &&
                (lk ? fighter_fiber_active(victim) : victim->current_mmove != NULL && !fighter_fiber_active(victim)),
          "lethal throw must preserve the victim recovery when the winner enters victory");
      advance(160);
      check(g_battle.phase == PHASE_FINISH && !g_battle.finish_waiting && victim->action == ACTION_RETAIL_1009 && !fighter_fiber_active(victim),
          "lethal throw must settle its victim and open the finish window without a stuck pair");

      start_ground_throw(player, lk);
      advance(release_at);
      battle_reset_match();
      advance(150);
      check(owner->health == Q16_ONE && victim->health == Q16_ONE && !fighter_fiber_active(owner) && !fighter_fiber_active(victim),
          "rematch must cancel both throw recoveries without later damage or callback work");
    }
  }
}

static void cpu_behavior(void) {
  fight(0, 6.f);
  Fighter *fighter = &g_battle.fighters[0];
  fighter->human = false;
  g_battle.random[0] = 0;
  g_battle.random[1] = 0x3000;
  advance(1);
  check(fighter_move(fighter)->clip == 16 && fighter->directions.token[0] == 0,
      "CPU far-distance table must select the character ice entry without synthesizing a human command");
  check(sound_events(SOUND_INDEXED, 4) == 1, "CPU ice must share the same indexed entry cue as human commands");
  advance(16);
  check(objects(PROJECTILE_ICE) == 1, "CPU ice must run the shared character continuation");

  fight(0, 2.5f);
  fighter->human = false;
  g_battle.random[0] = 0;
  g_battle.random[1] = 0x3000;
  advance(1);
  check(fighter->action == 0x405 && objects(PROJECTILE_CLONE) == 1, "CPU medium-distance table must select clone directly");
  for (int level = 1; level <= 3; level += 2) {
    fight(0, 1.f);
    g_battle.cpu_level = level;
    fighter->human = false;
    roster_def(4)->jump(fighter);
    fighter->body.pos[AXIS_Y] -= Q16_ONE;
    battle_world();
    check((fighter->pending != NULL) == (level > 1), "CPU straight jump must enable automatic air attacks only above level one");
    if (level > 1) {
      advance(1);
      check(fighter_move(fighter)->clip == 59 && fighter->body.physics == NULL, "CPU air height/range branch must choose angled LK and clear jump callbacks");
    }
  }
  g_battle.random[0] = 0;
  g_battle.random[1] = 0xffff;
  check(!battle_chance(975), "CPU probability must use a strict per-thousand comparison");
  g_battle.random[0] = g_battle.random[1] = 0;
  check(battle_chance(1), "CPU zero draw must pass a positive chance");

  fight(0, 2.f);
  fighter->human = false;
  g_battle.fighters[1].action = 0x113;
  g_battle.random[0] = g_battle.random[1] = 0;
  battle_cpu_step(fighter);
  check(fighter->pending == NULL && (fighter->held & BUTTON_BIT(BUTTON_DOWN)), "CPU back-weapon response must wait in crouch before countering");
  g_battle.fighters[1].action = 0x2000;
  battle_cpu_step(fighter);
  check((fighter->held & BUTTON_BIT(BUTTON_RUN)) && fighter->cpu.timer == 29, "CPU must approach for close HK after the weapon action clears");
  battle_next_round();
  check(!fighter->human && g_battle.fighters[1].human && fighter->cpu.plan == 0, "new rounds must preserve CPU ownership and discard old plans");

  fight(0, 6.f);
  g_battle.fighters[1].roster = 4;
  check(battle_cpu_action(&g_battle.fighters[1], CPU_SPECIAL_1), "mirror fixture needs the actual clone entry");
  advance(1);
  fighter->human = false;
  g_battle.random[0] = g_battle.random[1] = 0;
  battle_cpu_step(fighter);
  int wait_plan = fighter->cpu.plan;
  check(wait_plan != 0 && fighter->held == 0 && fighter->pending == NULL, "CPU must wait for a distant opposing clone");
  advance(10);
  check(fighter->cpu.plan == wait_plan && fighter->held == 0 && fighter->pending == NULL, "clone wait must survive repeated neutral ticks");
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    g_battle.projectiles[i].kind = 0;
  }
  battle_cpu_step(fighter);
  check(fighter->cpu.plan == 0 && fighter->held == 0, "CPU clone wait must release when the independent clone disappears");
}

static void walls(void) {
  for (int marked = 0; marked < 2; ++marked) {
    fight(0, 1.f);
    Fighter *attacker = &g_battle.fighters[0], *victim = &g_battle.fighters[1];
    attacker->body.pos[AXIS_X] = 0x86666;
    victim->body.pos[AXIS_X] = 0x94ccc;
    battle_world();
    fighter_enter_state(attacker, S_KICK);
    AttackRecord record = {0x20000, -0x18000, 0x20000, 0x30000, 16, 13, 6, marked ? ATTACK_MARK : 0, 0x1999};
    check(battle_contact(attacker, &record) == CONTACT_HIT, "wall check needs a real body contact");
    advance(2);
    check((attacker->state == S_HIT) == !marked, "wall recoil must interrupt the attacker unless the record marks the victim");
    check(victim->health == Q16_ONE - 0x1999, "wall dispatch must preserve victim damage");
    if (!marked) {
      check(attacker->body.vel[AXIS_X] == q16_mul(-0x4ccc, 0xe666), "wall recoil must use .3 retreat and native friction");
      advance(13);
      check(attacker->state == S_IDLE && !(attacker->flags & FF_REACTING), "wall recoil must restore neutral control");
    }
  }
  fight(0, 1.f);
  battle_set_arena(10);
  check(g_battle.arena_radius == 7 * Q16_ONE, "arena selection must use its retail radius");
  Fighter *fighter = &g_battle.fighters[0];
  fighter->body.pos[AXIS_X] = 8 * Q16_ONE;
  fighter->body.vel[AXIS_X] = -0x1000;
  battle_world();
  check(fighter->body.pos[AXIS_X] == 8 * Q16_ONE - 0x1000 && fighter->body.vel[AXIS_X] == -0x1000,
      "wall constraint must permit inward recovery from an outside position");
}

static void jumps(void) {
  fight(0, 8.f);
  Fighter *fighter = &g_battle.fighters[0];
  battle_step((Input[2]){{.held = BUTTON_BIT(BUTTON_UP)}, {0}});
  check(fighter->action == 0x2007 && fighter->sample_override == 0, "straight jump must open at sample zero");
  advance(19);
  check(fighter->sample_override == 17 && fighter->buttons != NULL, "straight jump must hold sample 17 with air commands enabled");
  while (fighter_airborne(fighter)) {
    advance(1);
  }
  check(fighter->action == 0x2008 && fighter->sample_override == 18, "straight jump must enter its landing continuation");
  advance(10);
  check(fighter->state == S_IDLE && fighter->body.root_y == 0, "straight landing must restore control");
  check(fighter->blend_clip == 304 && fighter->blend_sample == 27 && fighter->blend_weight == 0x3333,
      "landing must blend its final pose into standing at the native initial weight");
  advance(4);
  check(fighter->blend_weight == 0xffff && fighter->sample_override == 0, "standing return must hold its target sample through five native weights");
  advance(1);
  check(fighter->blend_duration == 0 && fighter->script == NULL, "standing return must release the pose blend");

  for (int back = 0; back < 2; ++back) {
    fight(0, 8.f);
    Input input[2] = {{.held = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(back ? BUTTON_LEFT : BUTTON_RIGHT)}, {0}};
    battle_step(input);
    check(fighter->action == (back ? 0x200a : 0x2009) && fighter_move(fighter)->clip == 301, "directional jump must select its own action and tuck clip");
    check(fighter->body.gravity == 0x24d && fighter->buttons == NULL, "flip takeoff must use native gravity and delay air attacks");
    advance(6);
    check(fighter->body.spin[AXIS_X] == (back ? -0x4ccc : 0x4ccc) && fighter->body.root_y == 0x3333, "flip must begin its signed rotation after six samples");
    advance(2);
    check(fighter->sample_override == 8, "flip rotation must finish extending to tuck sample eight");
    for (int i = 0; i < 100 && fighter->script_phase != 2; ++i) {
      advance(1);
    }
    check(fighter->script_phase == 2 && fighter->sample_override == 7 && fighter->body.spin[AXIS_X] == 0,
        "falling flip must stop rotating and retract from sample seven");
    for (int i = 0; i < 100 && fighter->state != S_IDLE; ++i) {
      advance(1);
    }
    check(fighter->blend_clip == 301 && fighter->blend_sample == 1 && fighter->blend_weight == 0x3333,
        "directional jump must blend its last retraction pose into standing");
    press(0, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter->blend_duration == 0 && fighter->action == 0x10a, "standing attacks must cancel a jump return blend");
    advance(100);
    check(fighter->state == S_IDLE && fighter->body.root_y == 0 && fighter->body.angle[AXIS_X] == 0, "flip landing must restore body and control");
  }
  fight(0, 8.f);
  press(0, BUTTON_BIT(BUTTON_UP));
  advance(1);
  q16 velocity = fighter->body.vel[AXIS_Y];
  battle_step((Input[2]){{.held = BUTTON_BIT(BUTTON_RIGHT)}, {0}});
  check(fighter->action == 0x2009 && fighter->body.vel[AXIS_Y] == velocity + 0x24d,
      "late direction must convert straight flight without restarting vertical motion");
  advance(6);
  press(0, BUTTON_BIT(BUTTON_HIGH_KICK));
  check(fighter_move(fighter)->clip == 56 && fighter->body.spin[AXIS_X] == 0 && fighter->body.root_y == 0,
      "diagonal air attack must replace flip playback and unwind its transform");
}

static void locomotion(void) {
  fight(0, 8.f);
  Fighter *fighter = &g_battle.fighters[0];
  Input input[2] = {{.held = BUTTON_BIT(BUTTON_RIGHT)}, {0}};
  battle_step(input);
  check(fighter_move(fighter)->clip == 267 && fighter->body.vel[AXIS_X] == 0xf5c, "forward walk must use native gait and speed");
  input[0].held = BUTTON_BIT(BUTTON_LEFT);
  battle_step(input);
  check(fighter_move(fighter)->clip == 272 && fighter->body.vel[AXIS_X] == -q16_mul(0xf5c, 0xb333), "back walk must change gait and use 70 percent speed");
  input[0].held = BUTTON_BIT(BUTTON_RIGHT) | BUTTON_BIT(BUTTON_RUN);
  battle_step(input);
  check(fighter->body.vel[AXIS_X] == 0x2147 && fighter->run_energy == Q16_ONE - 0x51e, "run must use native speed and energy cost");
  for (int i = 0; i < 50; ++i) {
    battle_step(input);
  }
  check(fighter->run_energy == 0 && fighter->run_delay == 40, "running must exhaust its meter");
  advance(39);
  check(fighter->run_energy == 0, "run energy must respect the recovery delay");
  advance(1);
  check(fighter->run_energy == 0xccc, "run energy must regenerate at the native rate");

  fight(0, 4.f);
  draw_scepter(0);
  input[0].held = BUTTON_BIT(BUTTON_RIGHT);
  battle_step(input);
  check(fighter_move(fighter)->clip == 277, "armed forward walk");
  input[0].held = BUTTON_BIT(BUTTON_LEFT);
  battle_step(input);
  check(fighter_move(fighter)->clip == 278, "armed direction change must select the backward gait without leaving walk state");
  press(0, BUTTON_BIT(BUTTON_HIGH_PUNCH));
  check(fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0, "ground strike must stop locomotion");

  fight(0, 4.f);
  g_battle.fighters[0].body.pos[AXIS_X] = g_battle.fighters[1].body.pos[AXIS_X] = 0;
  g_battle.fighters[0].body.pos[AXIS_Z] = -2 * Q16_ONE;
  g_battle.fighters[1].body.pos[AXIS_Z] = 2 * Q16_ONE;
  battle_world();
  press(0, BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_RIGHT));
  check(fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0xe14, "directional jump must follow the pair axis after a quarter turn");
}

static void sub_zero_finish(int player, q16 distance, int arena) {
  fight(player, 3.f);
  g_battle.arena = arena;
  g_battle.fighters[player].wins = 1;
  battle_damage(&g_battle.fighters[1 - player], Q16_ONE);
  advance(1);
  check(g_battle.phase == PHASE_FINISH && g_battle.winner == player, "Sub-Zero deciding KO must open finish window");
  for (int i = 0; i < 180 && g_battle.finish_waiting; ++i) {
    advance(1);
  }
  check(!g_battle.finish_waiting, "Defeat recovery must reach the finish controller");
  g_battle.fighters[0].body.pos[AXIS_X] = -distance / 2;
  g_battle.fighters[1].body.pos[AXIS_X] = distance - distance / 2;
}

static void finish_clock(void) {
  sub_zero_finish(0, 0xcccc, 0);
  g_battle.phase_ticks = 59;
  g_battle.contacted = true;
  advance(1);
  check(g_battle.phase == PHASE_FINISH && !g_battle.contacted, "finish grace must clear the old contact latch after 60 ticks");
  press(0, BUTTON_BIT(BUTTON_HIGH_PUNCH));
  advance(20);
  check(g_battle.phase == PHASE_MATCH_OVER, "a normal hit after the grace period must close the finish window");

  sub_zero_finish(0, 0xcccc, 0);
  g_battle.phase_ticks = 299;
  battle_step((Input[2]){{.held = BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
  check(g_battle.phase == PHASE_MATCH_OVER && g_battle.fighters[0].pending == NULL, "a new press on expiry must not extend the finish clock");
  sub_zero_finish(0, 0xcccc, 0);
  g_battle.phase_ticks = 299;
  fighter_interrupt(&g_battle.fighters[0], roster_def(4)->controls->commands[0].enter);
  advance(1);
  check(g_battle.phase == PHASE_FATALITY, "a finisher accepted before expiry must execute on the next tick");

  fight(0, 3.f);
  g_battle.fighters[0].wins = 1;
  battle_damage(&g_battle.fighters[1], Q16_ONE);
  battle_react(&g_battle.fighters[1], 34);
  advance(1);
  check(g_battle.finish_waiting && fighter_airborne(&g_battle.fighters[1]) && !battle_can_finish(&g_battle.fighters[0]),
      "deciding uppercut KO must preserve airborne defeat recovery");
  for (int i = 0; i < 180 && g_battle.finish_waiting; ++i) {
    advance(1);
  }
  check(!g_battle.finish_waiting && g_battle.phase_ticks == 0 && g_battle.fighters[1].action == 0x1009,
      "finish clock must start when the defeated controller becomes ready");
}

static void sub_zero_finish_command(int player, int id, unsigned extra, int window) {
  Button back = player ? BUTTON_RIGHT : BUTTON_LEFT;
  Button forward = player ? BUTTON_LEFT : BUTTON_RIGHT;
  Button tokens[4] = {forward, back, forward, BUTTON_DOWN};
  Button attack = BUTTON_HIGH_PUNCH;
  int count = 4;
  q16 distance = g_battle.fighters[1].body.pos[AXIS_X] - g_battle.fighters[0].body.pos[AXIS_X];
  if (id == FINISH_SUB_ZERO_2) {
    tokens[0] = tokens[1] = tokens[3] = back;
    tokens[2] = BUTTON_DOWN;
  } else if (id == FINISH_FAN) {
    tokens[0] = BUTTON_DOWN;
    tokens[1] = tokens[2] = tokens[3] = BUTTON_UP;
    attack = BUTTON_HIGH_KICK;
  } else if (id == FINISH_SPIKE) {
    tokens[0] = tokens[1] = tokens[2] = BUTTON_DOWN;
    count = 3;
    attack = BUTTON_LOW_KICK;
  }
  for (int i = 0; i < count; ++i) {
    press(player, BUTTON_BIT(tokens[i]) | (tokens[i] == BUTTON_UP ? BUTTON_BIT(BUTTON_BLOCK) : 0));
    advance(2);
  }
  if (window > count * 4) {
    advance(window - count * 4);
  }
  /* Restore exact range after command-entry positioning, before the button. */
  g_battle.fighters[0].body.pos[AXIS_X] = -distance / 2;
  g_battle.fighters[1].body.pos[AXIS_X] = distance - distance / 2;
  press(player, BUTTON_BIT(attack) | extra);
}

static void fatalities(void) {
  const int ids[] = {FINISH_SUB_ZERO_1, FINISH_SUB_ZERO_2, FINISH_FAN, FINISH_SPIKE};
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  for (int player = 0; player < 2; ++player) {
    for (int kind = 0; kind < 4; ++kind) {
      int id = ids[kind];
      sub_zero_finish(player, id == FINISH_SUB_ZERO_2 ? 0x26666 : 0xcccc, id == FINISH_FAN ? 8 : 0);
      int time = g_battle.time_ticks;
      sub_zero_finish_command(player, id, id == FINISH_SUB_ZERO_1 ? chord : 0, kind == 0 ? 60 : kind == 1 ? 50 : kind == 2 ? 48 : 40);
      check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == id, "Sub-Zero command at inclusive window must select finisher");
      check(g_battle.time_ticks == time && g_battle.fighters[player].wins == 2, "finisher freezes timer and awards no extra win");
      check(g_battle.fighters[0].buttons == NULL && g_battle.fighters[1].buttons == NULL, "finisher must lock both players");
      check(sound_events(SOUND_INDEXED, 4) == (id == FINISH_SUB_ZERO_2), "only the breath fatality must use the indexed ice entry cue");
      if (id == FINISH_SUB_ZERO_1) {
        advance(90);
        check(fighter_move(&g_battle.fighters[player])->clip == 385 && fighter_move(&g_battle.fighters[1 - player])->clip == 387,
            "first fatality must select Sub-Zero paired clips");
      } else if (id == FINISH_SUB_ZERO_2) {
        advance(1);
        check(sound_events(SOUND_INDEXED, 4) == 0, "breath animation must not repeat its entry cue");
        advance(79);
        check(fighter_move(&g_battle.fighters[player])->clip == 552 && fighter_move(&g_battle.fighters[1 - player])->clip == 206,
            "second fatality must run the independent victim animation during breath");
        for (int tick = 0; tick < 150 && fighter_move(&g_battle.fighters[player])->clip != 95; ++tick) {
          advance(1);
        }
        check(fighter_move(&g_battle.fighters[player])->clip == 95 && g_battle.fighters[1 - player].appearance == APPEARANCE_FROZEN,
            "second fatality must freeze before approaching with slide");
        for (int tick = 0; tick < 100 && g_battle.fighters[1 - player].appearance != APPEARANCE_HIDDEN; ++tick) {
          advance(1);
        }
        check(g_battle.fighters[1 - player].appearance == APPEARANCE_HIDDEN && fighter_move(&g_battle.fighters[player])->clip == 293,
            "second fatality must shatter on uppercut");
      }
      advance(700);
      check(
          g_battle.phase == PHASE_MATCH_OVER && g_battle.finisher == id && g_battle.fighters[player].script == NULL, "finisher must complete once and settle");
      if (id == FINISH_FAN) {
        check(g_battle.fighters[1 - player].appearance == APPEARANCE_HIDDEN, "fan must remove victim");
      } else if (id == FINISH_SPIKE) {
        check(g_battle.fighters[1 - player].body.pos[AXIS_Y] == -0x59999, "spikes must retain impaled position");
      } else if (id == FINISH_SUB_ZERO_1) {
        Fighter *owner = &g_battle.fighters[player];
        int sample = owner->sample_override;
        advance(1);
        check(owner->sample_override != sample && owner->sample_override >= 96 && owner->sample_override <= 140,
            "first fatality must keep its retail final loop after the result");
      }
      battle_reset_match();
      check(g_battle.finisher == 0 && g_battle.fighters[0].appearance == APPEARANCE_NORMAL && g_battle.fighters[1].appearance == APPEARANCE_NORMAL,
          "rematch must clear all finisher state");
    }
    for (int kind = 0; kind < 4; ++kind) {
      int id = ids[kind];
      sub_zero_finish(player, id == FINISH_SUB_ZERO_2 ? 0x26666 : 0xcccc, id == FINISH_FAN ? 8 : 0);
      sub_zero_finish_command(player, id, id == FINISH_SUB_ZERO_1 ? chord : 0, kind == 0 ? 61 : kind == 1 ? 51 : kind == 2 ? 49 : 41);
      check(g_battle.phase == PHASE_FINISH, "finisher must reject one tick beyond command window");
      check(sound_events(SOUND_INDEXED, 4) == 0, "rejected finisher must not play the breath entry cue");
    }
    const q16 boundaries[] = {Q16_ONE, 0x1cccc, 0x30000};
    for (int boundary = 0; boundary < 3; ++boundary) {
      int id = boundary == 0 ? FINISH_SUB_ZERO_1 : FINISH_SUB_ZERO_2;
      sub_zero_finish(player, boundaries[boundary], 0);
      g_battle.projectiles[0].kind = PROJECTILE_ICE;
      sub_zero_finish_command(player, id, boundary == 0 ? chord : 0, 0);
      check(g_battle.phase == PHASE_FATALITY && objects(PROJECTILE_ICE) == 0, "inclusive range boundary and object cleanup");
    }
    for (int above = 0; above < 2; ++above) {
      sub_zero_finish(player, above ? 0x30001 : 0x1cccb, 0);
      sub_zero_finish_command(player, FINISH_SUB_ZERO_2, 0, 0);
      check(g_battle.phase == PHASE_FINISH, "second fatality must reject either side of range interval");
    }
    for (int missing = 0; missing < 4; ++missing) {
      sub_zero_finish(player, 0xcccc, 0);
      unsigned extra = missing == 0   ? 0
                       : missing == 1 ? BUTTON_BIT(BUTTON_RUN)
                       : missing == 2 ? BUTTON_BIT(BUTTON_BLOCK)
                                      : chord | BUTTON_BIT(BUTTON_LOW_PUNCH);
      sub_zero_finish_command(player, FINISH_SUB_ZERO_1, extra, 0);
      check(g_battle.phase == PHASE_FINISH, "first fatality requires exact held chord");
    }
    for (int kind = 0; kind < 4; ++kind) {
      int id = ids[kind];
      sub_zero_finish(player, id == FINISH_SUB_ZERO_2 ? 0x18000 : 0x10001, id == FINISH_FAN ? 8 : 0);
      sub_zero_finish_command(player, id, id == FINISH_SUB_ZERO_1 ? chord : 0, 0);
      check(g_battle.phase == PHASE_FINISH, "finisher must reject range outside boundary");
    }
    for (int kind = 2; kind < 4; ++kind) {
      sub_zero_finish(player, 0xcccc, kind == 2 ? 0 : 8);
      sub_zero_finish_command(player, ids[kind], 0, 0);
      check(g_battle.phase == PHASE_FINISH, "stage fatality must require its stage");
    }
    sub_zero_finish(player, 0xcccc, 0);
    sub_zero_finish_command(1 - player, FINISH_SUB_ZERO_1, chord, 0);
    check(g_battle.phase == PHASE_FINISH, "loser cannot start finisher");
    advance(300);
    check(g_battle.phase == PHASE_MATCH_OVER && g_battle.finisher == 0, "finish window must still time out");
  }
}

static void fatality_blends(void) {
  sub_zero_finish(0, 0xcccc, 0);
  sub_zero_finish_command(0, FINISH_SUB_ZERO_1, BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN), 60);
  Fighter *victim = &g_battle.fighters[1];
  for (int i = 0; i < 300 && !(victim->blend_duration > 0 && victim->sample_override == 46); ++i) {
    advance(1);
  }
  check(
      victim->blend_duration > 0 && victim->blend_sample == 48 && victim->sample_override == 46, "first fatality must blend from victim sample 48 back to 46");
  advance(30);
  check(victim->blend_duration > 0 && victim->sample_override == 46 && victim->blend_weight < Q16_ONE, "victim blend must retain its native 0x666 progression");
}

static void fatality_camera(void) {
  for (int player = 0; player < 2; ++player) {
    sub_zero_finish(player, 0xcccc, 0);
    sub_zero_finish_command(player, FINISH_SUB_ZERO_1, BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN), 60);
    while (g_battle.phase_ticks < 8) {
      advance(1);
    }
    check(g_battle.camera.active && g_battle.camera.parameter == Q16_ONE && g_battle.camera.shot == 0, "fatality camera must begin its own path at tick eight");
    advance(101);
    check(g_battle.camera.parameter == 6 * Q16_ONE && g_battle.camera.shot == 0, "first camera path must advance at its recovered .05 rate");
    check(fabsf(g_battle.camera.eye.y + 2.f) < .001f && fabsf(g_battle.camera.eye.z - (player ? 3.f : -3.f)) < .001f,
        "camera records must decode X,Z,Y ordering and mirror with the scene axis");
    advance(49);
    check(g_battle.camera.shot == 0, "camera must hold fifty ticks after its first path independently of the actor clip");
    advance(1);
    check(g_battle.camera.shot == 1 && g_battle.camera.parameter == Q16_ONE && g_battle.camera.duration == 80,
        "orbit must open with its own parameter clock and eighty-tick joint-aim transition");
    advance(120);
    check(g_battle.camera.parameter == Q16_ONE + 120 * 0x45a && isfinite(g_battle.camera.eye.x) && g_battle.camera.eye.y < 0,
        "orbit must follow its recovered slow spline rate above the floor");
    while (g_battle.camera.shake_pulses == 0 && g_battle.phase == PHASE_FATALITY) {
      advance(1);
    }
    check(g_battle.camera.shake_y == 0x2e14 && g_battle.camera.shake_pulses == 2, "victim collapse must start the recovered two camera pulses");
    advance(3);
    check(g_battle.camera.shake_y == 0, "camera pulse must remove its offset after three ticks");
    advance(3);
    check(g_battle.camera.shake_y == 0x2e14, "camera must repeat its second pulse after the three-tick pause");
    advance(6);
    check(g_battle.camera.shake_y == 0 && g_battle.camera.shake_pulses == 0, "camera shake must finish without changing the shot origin");
    g_battle.camera.active = false;
    battle_camera_shake(1, 1, 0x1999);
    battle_step_presentation();
    check(g_battle.camera.shake_y == 0, "shared shake must also work with the normal fight camera");
    battle_next_round();
    check(!g_battle.camera.active && g_battle.camera.shots == NULL && g_battle.camera.shake_pulses == 0, "round reset must discard camera continuations");
  }
}

static void fatality_particles(void) {
  for (int player = 0; player < 2; ++player) {
    sub_zero_finish(player, 0xcccc, 0);
    sub_zero_finish_command(player, FINISH_SUB_ZERO_1, BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN), 60);
    while (g_battle.sprays[0].def == NULL && g_battle.phase == PHASE_FATALITY) {
      advance(1);
    }
    JointSpray *spray = &g_battle.sprays[0];
    check(spray->def != NULL && spray->fighter == 1 - player && spray->ticks == 1, "head removal must start the independent victim blood stream");
    int blood = 0;
    for (int i = 0; i < MAX_PARTICLES; ++i) {
      Particle *p = &g_battle.particles[i];
      if (p->ticks > 0) {
        check(p->visual != NULL && p->visual->animation.first == 0 && p->gravity == 0x51e, "neck stream must use the recovered blood mesh and gravity");
        ++blood;
      }
    }
    check(blood == 2, "neck stream must open with two drops rather than a single burst");
    advance(190);
    check(spray->def != NULL && spray->ticks == 191, "neck stream must continue during the actor's independent animation loop");
    advance(13);
    check(spray->def == NULL, "neck stream must end after its recovered 203-tick emission schedule");
    battle_particles(&g_battle.fighters[1 - player], 2, 1, false);
    Particle *drop = NULL;
    for (int i = 0; i < MAX_PARTICLES; ++i) {
      if (g_battle.particles[i].ticks == 45 && g_battle.particles[i].age == 0) {
        drop = &g_battle.particles[i];
        break;
      }
    }
    check(drop != NULL, "prepare a blood drop at floor contact");
    drop->pos[AXIS_Y] = -1;
    drop->vel[AXIS_Y] = 0x2000;
    battle_step_presentation();
    check(drop->visual->animation.first == 4 && drop->ticks == 19 && drop->pos[AXIS_Y] == -0x7ae,
        "blood must become the recovered floor splash sequence on landing");
    for (int i = 0; i < 19; ++i) {
      battle_step_presentation();
    }
    check(drop->ticks == 0, "floor splash must finish its nine two-tick frames and final tick");
    battle_next_round();
    check(g_battle.sprays[0].def == NULL && g_battle.particles[0].ticks == 0, "round reset must discard emitters and their particles");
  }
}

static void sidesteps(void) {
  for (int player = 0; player < 2; ++player) {
    fight(player, .6f);
    g_battle.fighters[1 - player].body.pos[AXIS_Z] = 2 * Q16_ONE;
    check(fighter_distance(&g_battle.fighters[player]) > 2.f, "range must include sidestep depth");
    press(player, BUTTON_BIT(BUTTON_LOW_PUNCH));
    check(fighter_move(&g_battle.fighters[player])->clip == 294, "sidestep depth must prevent a false close throw");
    for (int down = 0; down < 2; ++down) {
      for (int interval = 2; interval <= 9; ++interval) {
        fight(player, 4.f);
        press(player, BUTTON_BIT(BUTTON_RUN));
        check(g_battle.fighters[player].script == NULL, "first RUN press must not manufacture a double tap");
        advance(interval - 2);
        Input run_input[2] = {0};
        run_input[player].held = BUTTON_BIT(BUTTON_RUN) | (down ? BUTTON_BIT(BUTTON_DOWN) : 0);
        battle_step(run_input);
        run_input[player].held &= ~BUTTON_BIT(BUTTON_RUN);
        battle_step(run_input);
        Fighter *fighter = &g_battle.fighters[player];
        bool accepted = interval >= 3 && interval <= 8;
        check((fighter_move(fighter)->clip == 300) == accepted, "RUN two-sample rule and inclusive eight-tick window");
        if (accepted) {
          check(fighter->sample_override == (down ? 0 : 17) && fighter->sidestep_ready == g_battle.ticks + 80, "sidestep cursor direction/cooldown");
          check(fighter->body.vel[AXIS_X] == (player ? -0x443 : 0x443) && fighter->body.vel[AXIS_Z] == (player ? -1 : 1) * (down ? -0xccc : 0xccc),
              "sidestep must use pair-axis forward and perpendicular motion");
          q16 z = fighter->body.pos[AXIS_Z];
          advance(18);
          check(fighter->state == S_IDLE && fighter->body.vel[AXIS_Z] == 0 && fighter->body.pos[AXIS_Z] != z, "sidestep must travel then restore control");
          check(g_battle.fighters[1 - player].health == Q16_ONE, "sidestep must not damage opponent");
        }
      }
    }
    for (int boundary = -1; boundary <= 1; ++boundary) {
      fight(player, 4.f);
      press(player, BUTTON_BIT(BUTTON_RUN));
      advance(2);
      /* Next press is tick 5; compare just below/at/above its deadline. */
      g_battle.fighters[player].sidestep_ready = g_battle.ticks + 1 + boundary;
      press(player, BUTTON_BIT(BUTTON_RUN));
      check((fighter_move(&g_battle.fighters[player])->clip == 300) == (boundary <= 0), "sidestep cooldown inclusive deadline");
    }
    fight(player, 4.f);
    press(player, BUTTON_BIT(BUTTON_UP));
    press(player, BUTTON_BIT(BUTTON_RUN));
    advance(2);
    press(player, BUTTON_BIT(BUTTON_RUN));
    check(fighter_move(&g_battle.fighters[player])->clip != 300, "airborne owner must reject sidestep");
    fight(player, 4.f);
    g_battle.fighters[1 - player].body.pos[AXIS_Y] -= Q16_ONE;
    fighter_enter_state(&g_battle.fighters[1 - player], S_JUMP);
    press(player, BUTTON_BIT(BUTTON_RUN));
    advance(2);
    press(player, BUTTON_BIT(BUTTON_RUN));
    check(abs(g_battle.fighters[player].body.vel[AXIS_X]) == q16_mul(0x443, 0x4ccc), "airborne victim must scale sidestep motion to 30 percent");
    battle_react(&g_battle.fighters[player], 37);
    advance(100);
    check(g_battle.fighters[player].state == S_IDLE && g_battle.fighters[player].body.vel[AXIS_Z] == 0, "interrupted sidestep must clear motion");

    fight(player, 4.f);
    draw_scepter(player);
    g_battle.fighters[player].disarm(&g_battle.fighters[player]);
    advance(100);
    Projectile *drop = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (g_battle.projectiles[i].kind == PROJECTILE_SWORD) {
        drop = &g_battle.projectiles[i];
      }
    }
    check(drop != NULL && drop->phase == 2, "prepare sidestep versus pickup priority");
    g_battle.fighters[player].body.pos[AXIS_X] += player ? Q16_ONE * 2 : -Q16_ONE * 2;
    press(player, BUTTON_BIT(BUTTON_RUN));
    advance(2);
    g_battle.fighters[player].body.pos[AXIS_X] = drop->pos[AXIS_X];
    g_battle.fighters[player].body.pos[AXIS_Z] = drop->pos[AXIS_Z];
    press(player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RUN));
    check(fighter_move(&g_battle.fighters[player])->clip == 300 && !g_battle.fighters[player].armed && drop->kind != 0,
        "RUN double-tap record must take priority over crouching pickup");
    battle_next_round();
    check(g_battle.fighters[player].sidestep_ready == 0, "new round clears sidestep deadline");
  }
}

int main(void) {
  air_throws();
  cpu_behavior();
  finish_clock();
  walls();
  jumps();
  locomotion();
  fatality_blends();
  fatality_camera();
  fatality_particles();
  weapon_recovery();
  independent_contacts();
  sidesteps();
  fatalities();
  weapons();
  normals();
  sweep_frames();
  ground_throws();
  ice();
  clone_tests();
  slide_tests();
  puts(
      "PASS: shared air throw/gates/paired release/sound/interruption/disarm/recovery; RUN double-tap/motion/windows/cooldown/priority/interruption; Sub-Zero fatalities/both stages/chord/windows/range/rematch; scepter strikes/freeze/air/draw/follow-up/disarm/throw/cross-pickup/reactions; slide chord/gates/contact/recovery/interruption; clone pose/contact/near/cooldown/air/expiry/pool/mirror; ice commands/gates/flight/freeze/thaw/backfire/interruption/reset; Sub-Zero normal selectors, kick records, combos, paired throws, crouch launcher, avoidance and air recovery for both players");
  return 0;
}
