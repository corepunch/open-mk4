#include "game.h"
#include <stdio.h>
#include <stdlib.h>

static void check(bool condition, const char *message) {
  if (!condition) {
    fprintf(stderr, "FAIL Sub-Zero: %s\n", message);
    exit(1);
  }
}

static Battle fight(int player, float distance) {
  Battle battle;
  battle_init(&battle, player == 0 ? 4 : 0, player == 1 ? 4 : 0);
  battle.phase = PHASE_FIGHT;
  battle.fighters[0].body.pos[AXIS_X] = (q16)(-distance * Q16_ONE / 2);
  battle.fighters[1].body.pos[AXIS_X] = (q16)(distance * Q16_ONE / 2);
  return battle;
}

static void advance(Battle *battle, int ticks) {
  for (int i = 0; i < ticks; ++i) {
    battle_step(battle, (Input[2]){0});
  }
}

static void press(Battle *battle, int player, unsigned held) {
  Input input[2] = {0};
  input[player].held = held;
  battle_step(battle, input);
  advance(battle, 1);
}

static void normals(void) {
  for (int player = 0; player < 2; ++player) {
    unsigned back = BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT);
    unsigned buttons[] = {BUTTON_BIT(BUTTON_HIGH_PUNCH), BUTTON_BIT(BUTTON_LOW_PUNCH), BUTTON_BIT(BUTTON_HIGH_KICK), BUTTON_BIT(BUTTON_LOW_KICK),
        back | BUTTON_BIT(BUTTON_HIGH_KICK), back | BUTTON_BIT(BUTTON_LOW_KICK), BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH),
        BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_KICK), BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_KICK)};
    const int clips[] = {296, 294, 71, 70, 313, 63, 260, 315, 52};
    for (int i = 0; i < 9; ++i) {
      Battle battle = fight(player, 4.f);
      press(&battle, player, buttons[i]);
      check(fighter_move(&battle.fighters[player])->clip == clips[i], "stance/character selector chose wrong normal");
      advance(&battle, 120);
      check(battle.fighters[player].state == S_IDLE && battle.fighters[1 - player].health == Q16_ONE, "miss must recover without damage");
    }
    for (int outcome = 0; outcome < 4; ++outcome) {
      Battle battle = fight(player, outcome == 3 ? 4.f : 1.f);
      Input input[2] = {0};
      if (outcome == 1) {
        input[1 - player].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      if (outcome == 2) {
        battle.fighters[1 - player].body.pos[AXIS_Y] -= Q16_ONE;
        fighter_enter_state(&battle, &battle.fighters[1 - player], S_JUMP);
      }
      input[player].held = BUTTON_BIT(BUTTON_HIGH_KICK);
      battle_step(&battle, input);
      input[player].held = 0;
      for (int t = 0; t < 11; ++t) {
        battle_step(&battle, input);
      }
      check(battle.fighters[1 - player].health == Q16_ONE, "kick damaged before first contact");
      battle_step(&battle, input);
      q16 damage = outcome == 3 ? 0 : outcome == 1 ? q16_mul(0x2147, 0x3333) : 0x2147;
      check(battle.fighters[1 - player].health == Q16_ONE - damage, "kick record/chip/air/miss damage");
      advance(&battle, 160);
      check(battle.fighters[1 - player].health == Q16_ONE - damage, "kick must connect once");
      check(battle.fighters[player].state == S_IDLE, "kick recovery must release controller");
    }
    Battle battle = fight(player, .65f);
    press(&battle, player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    advance(&battle, 7);
    check(battle.fighters[player].flags & FF_FOLLOW_UP, "close HP must open its cancel");
    press(&battle, player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter_move(&battle.fighters[player])->clip == 532, "combo HP must select Sub-Zero clip 532");
    advance(&battle, 120);
    check(battle.fighters[player].state == S_IDLE, "combo HP must recover");
    for (int lk = 0; lk < 2; ++lk) {
      battle = fight(player, .6f);
      press(&battle, player, BUTTON_BIT(lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH));
      check(fighter_move(&battle.fighters[player])->clip == (lk ? 37 : 75), "paired throw entry");
      advance(&battle, 41);
      check(battle.fighters[1 - player].health == Q16_ONE, "throw damage occurred before sample 42");
      advance(&battle, 1);
      check(battle.fighters[1 - player].health == Q16_ONE - (lk ? 0x2e14 : 0x2666), "paired throw damage");
      advance(&battle, 180);
      check(battle.fighters[player].state == S_IDLE && battle.fighters[1 - player].state == S_IDLE, "throw must release both controllers");
    }
    battle = fight(player, .65f);
    press(&battle, player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_KICK));
    advance(&battle, 7);
    check(battle.fighters[1 - player].reaction == 26 && battle.fighters[1 - player].health == Q16_ONE - 0x1999, "crouch HK must use its own launcher record");
    advance(&battle, 180);
    check(battle.fighters[1 - player].state == S_IDLE, "crouch HK victim must finish its reaction");
    battle = fight(player, 4.f);
    press(&battle, player, BUTTON_BIT(BUTTON_LOW_KICK));
    advance(&battle, 6);
    check(battle.fighters[player].avoids_high, "low-kick avoidance begins at sample 6");
    battle_react(&battle, &battle.fighters[player], 37);
    advance(&battle, 1);
    check(!battle.fighters[player].avoids_high, "interruption must clear low-kick avoidance");
    for (int button = BUTTON_HIGH_PUNCH; button <= BUTTON_LOW_KICK; ++button) {
      if (button == BUTTON_BLOCK) {
        continue;
      }
      battle = fight(player, 4.f);
      battle.fighters[player].body.pos[AXIS_Y] -= 3 * Q16_ONE;
      fighter_enter_state(&battle, &battle.fighters[player], S_JUMP);
      press(&battle, player, BUTTON_BIT(button));
      check(fighter_move(&battle.fighters[player])->flags & SF_RECORD, "air attack must use script records");
      advance(&battle, 160);
      check(battle.fighters[player].state == S_IDLE && !fighter_airborne(&battle.fighters[player]), "air attack must land and restore controls");
    }
  }
}

static void ice_command(Battle *battle, int player, int delay, unsigned extra) {
  press(battle, player, BUTTON_BIT(BUTTON_DOWN));
  advance(battle, delay);
  press(battle, player, BUTTON_BIT(player ? BUTTON_LEFT : BUTTON_RIGHT) | BUTTON_BIT(BUTTON_LOW_PUNCH) | extra);
}

static int objects(const Battle *battle, int kind) {
  int count = 0;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    count += battle->projectiles[i].kind == kind;
  }
  return count;
}

static void ice(void) {
  for (int player = 0; player < 2; ++player) {
    Battle battle = fight(player, 2.f);
    Fighter *victim = &battle.fighters[1 - player];
    ice_command(&battle, player, 2, 0);
    check(fighter_move(&battle.fighters[player])->clip == 16, "D,F,LP must enter ice");
    advance(&battle, 14);
    check(objects(&battle, PROJECTILE_ICE) == 0, "ice must wait for startup");
    advance(&battle, 14);
    check(victim->appearance == APPEARANCE_FROZEN && victim->action == 0x1003 && victim->health == Q16_ONE, "ice must freeze without direct damage");
    check(victim->buttons == NULL && (victim->flags & FF_NO_GUARD), "frozen victim must be locked and unable to guard");
    int frozen_age = victim->ticks;
    q16 x = victim->body.pos[AXIS_X];
    int pose = victim->sample_override;
    advance(&battle, 141 - frozen_age);
    check(victim->appearance == APPEARANCE_FROZEN && victim->body.pos[AXIS_X] == x && victim->sample_override == pose,
        "freeze must hold pose and body through tick 141");
    advance(&battle, 1);
    check(victim->appearance == APPEARANCE_NORMAL && victim->state == S_IDLE && victim->buttons != NULL && !(victim->flags & FF_NO_GUARD),
        "ground thaw must restore controls at tick 142");

    battle = fight(player, 4.f);
    ice_command(&battle, player, 2, 0);
    advance(&battle, 16);
    check(objects(&battle, PROJECTILE_ICE) == 1, "ice object must spawn");
    battle_react(&battle, &battle.fighters[player], 37);
    advance(&battle, 1);
    check(objects(&battle, PROJECTILE_ICE) == 1, "independent ice object must survive owner interruption");
    advance(&battle, 180);
    check(objects(&battle, PROJECTILE_ICE) == 0, "ice must expire after contact/bounds");

    for (int gate = 0; gate < 4; ++gate) {
      battle = fight(player, 4.f);
      if (gate == 1) {
        battle.fighters[1 - player].hits = 2;
      }
      if (gate == 2) {
        battle.fighters[player].body.pos[AXIS_Y] -= Q16_ONE;
        fighter_enter_state(&battle, &battle.fighters[player], S_JUMP);
      }
      ice_command(&battle, player, gate == 0 ? 27 : 2, gate == 3 ? BUTTON_BIT(BUTTON_BLOCK) : 0);
      check(fighter_move(&battle.fighters[player])->clip != 16, "ice must reject late/juggle/air/block gates");
    }
    battle = fight(player, 4.f);
    ice_command(&battle, player, 24, 0);
    check(fighter_move(&battle.fighters[player])->clip == 16, "ice accepts inclusive 26-tick command window");
    battle = fight(player, 14.f);
    ice_command(&battle, player, 2, 0);
    advance(&battle, 58);
    check(objects(&battle, PROJECTILE_ICE) == 1 && battle.fighters[player].state == S_IDLE, "ice flight must outlive attacker recovery");
    ice_command(&battle, player, 2, 0);
    check(fighter_move(&battle.fighters[player])->clip == 16 && objects(&battle, PROJECTILE_ICE) == 1, "ice gate must allow another shot when no clone exists");
    battle_next_round(&battle);
    check(objects(&battle, PROJECTILE_ICE) == 0 && battle.fighters[1 - player].appearance == APPEARANCE_NORMAL, "round reset must clear ice and freeze");

    battle = fight(player, 2.f);
    ice_command(&battle, player, 2, 0);
    Input guard[2] = {0};
    guard[1 - player].held = BUTTON_BIT(BUTTON_BLOCK);
    for (int t = 0; t < 45; ++t) {
      battle_step(&battle, guard);
    }
    check(battle.fighters[1 - player].health == Q16_ONE && battle.fighters[1 - player].appearance == APPEARANCE_NORMAL && objects(&battle, PROJECTILE_ICE) == 0,
        "block must consume ice without damage or freeze");

    battle = fight(player, 4.f);
    victim = &battle.fighters[1 - player];
    battle_react(&battle, victim, 67);
    advance(&battle, 1);
    check(victim->appearance == APPEARANCE_FROZEN, "explicit freeze reaction");
    battle_react(&battle, victim, 67);
    advance(&battle, 2);
    check(victim->appearance == APPEARANCE_NORMAL && battle.fighters[player].appearance == APPEARANCE_FROZEN,
        "double freeze must backfire and release original victim");
    battle_react(&battle, &battle.fighters[player], 37);
    advance(&battle, 1);
    check(battle.fighters[player].appearance == APPEARANCE_NORMAL && !(battle.fighters[player].flags & FF_NO_GUARD),
        "new hit must clear frozen presentation and guard restriction");

    battle = fight(player, 4.f);
    victim = &battle.fighters[1 - player];
    victim->body.pos[AXIS_Y] -= Q16_ONE;
    fighter_enter_state(&battle, victim, S_JUMP);
    battle_react(&battle, victim, 67);
    advance(&battle, 143);
    check(victim->body.gravity == 0x147 && fighter_airborne(victim), "air thaw must restore its own gravity");
    advance(&battle, 150);
    check(victim->state == S_IDLE && !fighter_airborne(victim) && victim->appearance == APPEARANCE_NORMAL, "air thaw must land and recover");
  }
}

static void clone_command(Battle *battle, int player) {
  press(battle, player, BUTTON_BIT(BUTTON_DOWN));
  press(battle, player, BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT) | BUTTON_BIT(BUTTON_LOW_PUNCH));
}

static void idle_object(Battle *battle, Projectile *p) {
  (void)battle;
  (void)p;
}

static void clone_tests(void) {
  for (int player = 0; player < 2; ++player) {
    Battle battle = fight(player, 2.f);
    clone_command(&battle, player);
    check(objects(&battle, PROJECTILE_CLONE) == 1 && battle.fighters[player].action == 0x405, "clone must capture pose immediately");
    Projectile *clone = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (battle.projectiles[i].kind == PROJECTILE_CLONE) {
        clone = &battle.projectiles[i];
      }
    }
    q16 clone_x = clone->pos[AXIS_X];
    check(clone->clip >= -1 && clone->sample >= 0, "clone must own its captured pose");
    battle_react(&battle, &battle.fighters[player], 37);
    advance(&battle, 2);
    check(objects(&battle, PROJECTILE_CLONE) == 1 && clone->pos[AXIS_X] == clone_x, "clone must remain stationary after owner interruption");
    battle.fighters[1 - player].body.pos[AXIS_X] = clone_x + clone->facing * 0x6000;
    advance(&battle, 2);
    check(objects(&battle, PROJECTILE_CLONE) == 0 && battle.fighters[1 - player].appearance == APPEARANCE_FROZEN, "clone must freeze once on contact");
    check(battle.fighters[1 - player].health == Q16_ONE, "clone freeze must not deal direct damage");
    battle_reset_match(&battle);
    check(battle.fighters[player].clone_ready == 0 && objects(&battle, PROJECTILE_CLONE) == 0, "rematch clears clone state");

    battle = fight(player, .7f);
    clone_command(&battle, player);
    check(objects(&battle, PROJECTILE_CLONE) == 0 && battle.fighters[player].action == 0x406, "near clone must retreat without spawning");
    advance(&battle, 100);
    check(battle.fighters[player].state == S_IDLE, "near branch must land and release controls");

    battle = fight(player, 4.f);
    clone_command(&battle, player);
    advance(&battle, 50);
    check(battle.fighters[player].state == S_IDLE && objects(&battle, PROJECTILE_CLONE) == 1, "retreat must recover while clone remains");
    ice_command(&battle, player, 2, 0);
    check(fighter_move(&battle.fighters[player])->clip != 16, "live clone must gate ice blast");
    advance(&battle, 80);
    check(objects(&battle, PROJECTILE_CLONE) == 0, "clone must expire");
    uint64_t ready = battle.fighters[player].clone_ready;
    battle.ticks = ready - 4;
    clone_command(&battle, player);
    check(battle.fighters[player].action != 0x405, "clone cooldown rejects immediately below boundary");
    advance(&battle, 100);
    clone_command(&battle, player);
    check(objects(&battle, PROJECTILE_CLONE) == 1, "clone cooldown must permit another activation");

    battle = fight(player, 4.f);
    battle.fighters[player].clone_ready = 3;
    clone_command(&battle, player);
    check(objects(&battle, PROJECTILE_CLONE) == 1, "clone accepts cooldown at boundary");
    battle_next_round(&battle);
    battle.phase = PHASE_FIGHT;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      check(battle_spawn_projectile(&battle, &battle.fighters[player], PROJECTILE_SWORD, 200, idle_object) != NULL, "fill object pool");
    }
    clone_command(&battle, player);
    advance(&battle, 100);
    check(battle.fighters[player].state == S_IDLE && objects(&battle, PROJECTILE_CLONE) == 0, "full pool must not strand clone owner");

    battle = fight(player, 4.f);
    battle.fighters[player].body.pos[AXIS_Y] -= Q16_ONE;
    fighter_enter_state(&battle, &battle.fighters[player], S_JUMP);
    clone_command(&battle, player);
    check(objects(&battle, PROJECTILE_CLONE) == 1, "clone command permits air activation");
    advance(&battle, 180);
    check(battle.fighters[player].state == S_IDLE, "air clone must land and recover");
  }
  Battle mirror;
  battle_init(&mirror, 4, 4);
  mirror.phase = PHASE_FIGHT;
  clone_command(&mirror, 0);
  clone_command(&mirror, 1);
  check(objects(&mirror, PROJECTILE_CLONE) == 2, "mirror clones must have independent owners");
  battle_reset_match(&mirror);
  check(objects(&mirror, PROJECTILE_CLONE) == 0, "mirror reset clears both objects");
}

int main(void) {
  normals();
  ice();
  clone_tests();
  puts(
      "PASS: clone pose/contact/near/cooldown/air/expiry/pool/mirror; ice commands/gates/flight/freeze/thaw/backfire/interruption/reset; Sub-Zero normal selectors, kick records, combos, paired throws, crouch launcher, avoidance and air recovery for both players");
  return 0;
}
