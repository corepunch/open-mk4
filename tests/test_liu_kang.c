#include "game.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void check(bool condition, const char *message) {
  if (!condition) {
    fprintf(stderr, "FAIL Liu Kang: %s\n", message);
    exit(1);
  }
}

static void fight(int player, float distance) {
  battle_init(player == 0 ? 3 : 0, player == 1 ? 3 : 0);
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

static void normals(void) {
  for (int player = 0; player < 2; ++player) {
    unsigned back = BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT);
    unsigned buttons[] = {BUTTON_BIT(BUTTON_HIGH_PUNCH), BUTTON_BIT(BUTTON_LOW_PUNCH), BUTTON_BIT(BUTTON_HIGH_KICK), BUTTON_BIT(BUTTON_LOW_KICK),
        back | BUTTON_BIT(BUTTON_HIGH_KICK), back | BUTTON_BIT(BUTTON_LOW_KICK), BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH),
        BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_KICK), BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_KICK)};
    const int clips[] = {296, 294, 66, 67, 64, 62, 260, 315, 54};
    for (int i = 0; i < 9; ++i) {
      fight(player, 4.f);
      press(player, buttons[i]);
      check(fighter_move(&g_battle.fighters[player])->clip == clips[i], "stance/character selector chose wrong normal");
      check(fighter_fiber_active(&g_battle.fighters[player]), "normal must suspend in its actor fiber");
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
    check(fighter_move(&g_battle.fighters[player])->clip == 261, "combo HP must select Liu Kang clip 261");
    advance(120);
    check(g_battle.fighters[player].state == S_IDLE, "combo HP must recover");
    fight(player, .65f);
    press(player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_KICK));
    advance(9);
    check(
        g_battle.fighters[1 - player].reaction == 25 && g_battle.fighters[1 - player].health == Q16_ONE - 0x1eb8, "crouch HK must use its own launcher record");
    advance(180);
    check(g_battle.fighters[1 - player].state == S_IDLE, "crouch HK victim must finish its reaction");
    fight(player, 4.f);
    press(player, BUTTON_BIT(BUTTON_LOW_KICK));
    advance(7);
    check(g_battle.fighters[player].avoids_high, "low-kick avoidance begins after seven ticks");
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

static void throws(void) {
  for (int player = 0; player < 2; ++player) {
    for (int lk = 0; lk < 2; ++lk) {
      fight(player, .5f);
      Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
      press(player, BUTTON_BIT(lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH));
      check(fighter_move(owner)->clip == (lk ? 35 : 79), "throw entry must select Liu Kang pair");
      advance(lk ? 37 : 44);
      check(victim->health == Q16_ONE, "throw cannot damage before its retail release boundary");
      advance(1);
      check(victim->health == Q16_ONE - (lk ? 0x2e14 : 0x2666), "throw damage at owner sample boundary");
      advance(240);
      check(owner->state == S_IDLE && victim->state == S_IDLE, "both throw participants recover");
      check(!(victim->flags & FF_PAIRED) && !fighter_fiber_active(victim), "throw victim must release pair and fiber");
      fight(player, .5f);
      press(player, BUTTON_BIT(lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH));
      advance(20);
      battle_react(owner, 37);
      advance(240);
      check(victim->health == Q16_ONE && victim->state == S_IDLE, "cancelled throw must not wake and damage victim");
      fight(player, .5f);
      victim->health = 1;
      press(player, BUTTON_BIT(lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH));
      advance(130);
      check(victim->health == 0 && !(victim->flags & FF_PAIRED), "lethal throw must release independent victim tail");
      check(!fighter_fiber_active(victim), "lethal victim tail must finish");
      battle_init(3, 0);
      check(!fighter_fiber_active(&g_battle.fighters[0]) && !fighter_fiber_active(&g_battle.fighters[1]), "throw reset frees actor stacks");
    }
  }
}

static void forward_command(int player, Button button) {
  unsigned forward = BUTTON_BIT(player ? BUTTON_LEFT : BUTTON_RIGHT);
  press(player, forward);
  advance(1);
  press(player, forward | BUTTON_BIT(button));
}

static int fireballs(void) {
  int count = 0;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    count += g_battle.projectiles[i].kind == PROJECTILE_FIREBALL;
  }
  return count;
}

static void pool_object(Projectile *object) {
  (void)object;
}

static void specials(void) {
  for (int player = 0; player < 2; ++player) {
    for (int low = 0; low < 2; ++low) {
      fight(player, 4.f);
      Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
      forward_command(player, low ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH);
      check(fighter_move(owner)->clip == (low ? 19 : 18), "fireball command selects height");
      advance(9);
      check(fireballs() == 0 && victim->health == Q16_ONE, "fireball waits for release sample");
      advance(1);
      check(fireballs() == 1, "fireball spawns after double-step startup");
      battle_react(owner, 37);
      advance(30);
      check(victim->health == Q16_ONE - 0x1999, "released fireball survives owner interruption and damages once");
      advance(100);
      check(victim->state == S_IDLE && fireballs() == 0, "fireball victim and object recover");
    }
    fight(player, 4.f);
    forward_command(player, BUTTON_HIGH_PUNCH);
    Input guard[2] = {0};
    guard[1 - player].held = BUTTON_BIT(BUTTON_DOWN);
    for (int t = 0; t < 100; ++t) {
      battle_step(guard);
    }
    check(g_battle.fighters[1 - player].health == Q16_ONE, "high fireball passes crouching victim");
    for (int blocked = 0; blocked < 2; ++blocked) {
      fight(player, 3.f);
      forward_command(player, BUTTON_HIGH_KICK);
      Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
      check(fighter_move(owner)->clip == 92, "flying kick command entry");
      Input input[2] = {0};
      input[1 - player].held = blocked ? BUTTON_BIT(BUTTON_BLOCK) : 0;
      for (int t = 0; t < 170; ++t) {
        battle_step(input);
      }
      q16 damage = blocked ? q16_mul(0x23d7, 0x3333) : 0x23d7;
      check(victim->health == Q16_ONE - damage, "flying kick hit/block record");
      check(owner->state == S_IDLE && !fighter_airborne(owner), "flying kick settles and recovers");
    }
    fight(player, 6.f);
    Fighter *owner = &g_battle.fighters[player];
    owner->body.pos[AXIS_Y] -= 3 * Q16_ONE;
    owner->body.gravity = 0x28f;
    fighter_enter_state(owner, S_JUMP);
    forward_command(player, BUTTON_HIGH_PUNCH);
    check(fighter_move(owner)->clip == 20, "HP fireball selects air script");
    advance(6);
    check(fireballs() == 1, "air fireball release");
    advance(150);
    check(owner->state == S_IDLE && !fighter_airborne(owner), "air fireball restores landing controller");
    fight(player, 6.f);
    forward_command(player, BUTTON_LOW_PUNCH);
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      check(battle_spawn_projectile(&g_battle.fighters[player], PROJECTILE_ICE, 80, pool_object) != NULL, "fill object pool");
    }
    advance(11);
    check(fireballs() == 0, "full pool safely rejects fireball allocation");
    advance(100);
    check(g_battle.fighters[player].state == S_IDLE, "pool failure cannot strand fireball owner");
  }
}

static void draw_sword(int player) {
  unsigned back = BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT);
  unsigned forward = BUTTON_BIT(player ? BUTTON_LEFT : BUTTON_RIGHT);
  press(player, back);
  advance(1);
  press(player, forward | BUTTON_BIT(BUTTON_LOW_KICK));
  advance(60);
}

static void weapons(void) {
  for (int player = 0; player < 2; ++player) {
    fight(player, 4.f);
    draw_sword(player);
    Fighter *owner = &g_battle.fighters[player], *victim = &g_battle.fighters[1 - player];
    check(owner->armed && owner->weapon->type == 0x66, "B,F,LK draws Liu Kang weapon descriptor");
    check(owner->weapon->model == SCENE_MODEL_DRAGON_SWORD, "weapon uses its own recovered rig");
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    advance(9);
    check(fighter_move(owner)->clip == 216, "armed HP changes into repeat clip");
    advance(40);
    check(owner->state == S_IDLE && owner->armed, "armed HP miss returns to weapon stance");
    for (int mode = 0; mode < 3; ++mode) {
      unsigned back = BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT);
      press(player, (mode ? back : 0) | BUTTON_BIT(mode == 1 ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH));
      check(fighter_move(owner)->clip == (mode == 0 ? 240 : mode == 1 ? 242 : 227), "weapon stance/back selector");
      advance(100);
      check(owner->state == S_IDLE && !fighter_fiber_active(owner), "weapon attack miss restores control");
    }
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    advance(3);
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    advance(6);
    check(fighter_move(owner)->clip == 216 && fighter_fiber_active(owner), "buffered HP extends armed repeat program");
    advance(100);
    victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + owner->facing * (q16)(1.4f * Q16_ONE);
    press(player, BUTTON_BIT(BUTTON_LOW_PUNCH));
    advance(5);
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    advance(10);
    check(fighter_move(owner)->clip == 237, "LP contact plus buffered HP selects second weapon strike");
    advance(160);
    check(owner->state == S_IDLE && victim->state == S_IDLE, "weapon chain and victim recover");
    victim->health = Q16_ONE;
    /* Keep the owner far enough away to avoid LP's ground-throw gate. */
    victim->body.pos[AXIS_X] = owner->body.pos[AXIS_X] + owner->facing * (q16)(1.2f * Q16_ONE);
    press(player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    advance(9);
    check(victim->health == Q16_ONE - 0x11eb && victim->reaction == 60, "armed HP first contact uses weapon[15]");
    battle_react(owner, 16);
    advance(100);
    check(victim->health == Q16_ONE - 0x11eb, "interrupted weapon sequence cannot damage later");
    check(!owner->armed, "weapon hit reaction disarms through shared lifecycle");
    fight(player, 4.f);
    draw_sword(player);
    draw_sword(player); /* Armed command throws the actual weapon identity. */
    check(!g_battle.fighters[player].armed, "armed draw command throws weapon");
    bool identity = false;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      const Projectile *p = &g_battle.projectiles[i];
      identity |= p->kind == PROJECTILE_SWORD && p->weapon->type == 0x66;
    }
    check(identity, "thrown weapon retains Liu Kang identity");
    fight(player, 4.f);
    press(player, BUTTON_BIT(BUTTON_RUN));
    advance(1);
    press(player, BUTTON_BIT(BUTTON_RUN));
    check(fighter_move(&g_battle.fighters[player])->clip == 300, "RUN double tap sidestep");
    advance(24);
    check(g_battle.fighters[player].state == S_IDLE, "sidestep recovers");
  }
}

static void finish_setup(int player, q16 distance, int arena) {
  fight(player, 3.f);
  g_battle.arena = arena;
  g_battle.fighters[player].wins = 1;
  battle_damage(&g_battle.fighters[1 - player], Q16_ONE);
  advance(1);
  for (int tick = 0; tick < 180 && g_battle.finish_waiting; ++tick) {
    advance(1);
  }
  check(battle_can_finish(&g_battle.fighters[player]), "deciding KO opens real finish controller");
  g_battle.fighters[0].body.pos[AXIS_X] = -distance / 2;
  g_battle.fighters[1].body.pos[AXIS_X] = distance - distance / 2;
}

static void finish_command(int player, int id, unsigned extra, int window) {
  Button forward = player ? BUTTON_LEFT : BUTTON_RIGHT;
  Button back = player ? BUTTON_RIGHT : BUTTON_LEFT;
  Button tokens[4] = {forward, forward, forward, BUTTON_DOWN};
  Button attack = BUTTON_BLOCK;
  int count = 4;
  q16 distance = g_battle.fighters[1].body.pos[AXIS_X] - g_battle.fighters[0].body.pos[AXIS_X];
  if (id == FINISH_LIU_KANG_2) {
    tokens[1] = tokens[2] = BUTTON_DOWN;
    tokens[3] = BUTTON_UP;
    attack = BUTTON_HIGH_PUNCH;
  } else if (id == FINISH_FAN || id == FINISH_SPIKE) {
    tokens[2] = back;
    count = 3;
    attack = id == FINISH_FAN ? BUTTON_LOW_PUNCH : BUTTON_HIGH_KICK;
  }
  for (int i = 0; i < count; ++i) {
    press(player, BUTTON_BIT(tokens[i]) | (tokens[i] == BUTTON_UP ? BUTTON_BIT(BUTTON_BLOCK) : 0));
    advance(2);
  }
  if (window > count * 4) {
    advance(window - count * 4);
  }
  g_battle.fighters[0].body.pos[AXIS_X] = -distance / 2;
  g_battle.fighters[1].body.pos[AXIS_X] = distance - distance / 2;
  press(player, BUTTON_BIT(attack) | extra);
}

static void fatalities(void) {
  const int ids[] = {FINISH_LIU_KANG_1, FINISH_LIU_KANG_2, FINISH_FAN, FINISH_SPIKE};
  unsigned chord = BUTTON_BIT(BUTTON_HIGH_KICK) | BUTTON_BIT(BUTTON_LOW_KICK);
  for (int player = 0; player < 2; ++player) {
    for (int kind = 0; kind < 4; ++kind) {
      int id = ids[kind];
      finish_setup(player, kind == 0 ? 0x26666 : 0xcccc, id == FINISH_FAN ? 8 : 0);
      finish_command(player, id, kind == 0 ? chord : 0, kind == 0 ? 60 : kind == 1 ? 50 : 40);
      check(g_battle.phase == PHASE_FATALITY && g_battle.finisher == id, "inclusive finisher command chooses correct scene");
      if (kind == 0) {
        advance(130);
        check(g_battle.fighters[player].appearance == APPEARANCE_DRAGON && fighter_move(&g_battle.fighters[player])->clip == 418,
            "first fatality uses original dragon model and clip");
        advance(160);
        check(fighter_move(&g_battle.fighters[1 - player])->clip == 419, "dragon victim owns independent clip");
      } else if (kind == 1) {
        advance(100);
        check(fighter_move(&g_battle.fighters[player])->clip == 537 && fighter_move(&g_battle.fighters[1 - player])->clip == 539,
            "second fatality paired opening");
        advance(120);
        check(g_battle.fighters[1 - player].appearance == APPEARANCE_HIDDEN && g_battle.fighters[1 - player].reaction == 177,
            "finisher projectile commits victim explosion");
      }
      advance(800);
      check(g_battle.phase == PHASE_MATCH_OVER && !fighter_fiber_active(&g_battle.fighters[player]), "finisher completes and retires actor stack");
      check(g_battle.fighters[player].wins == 2, "finisher must not award an extra win");
      battle_reset_match();
      check(g_battle.finisher == 0 && g_battle.fighters[player].appearance == APPEARANCE_NORMAL, "rematch clears finisher appearance and ownership");
      finish_setup(player, kind == 0 ? 0x26666 : 0xcccc, id == FINISH_FAN ? 8 : 0);
      finish_command(player, id, kind == 0 ? chord : 0, kind == 0 ? 61 : kind == 1 ? 51 : 41);
      check(g_battle.phase == PHASE_FINISH, "finisher rejects expired direction window");
    }
    finish_setup(player, 0x26666, 0);
    finish_command(player, FINISH_LIU_KANG_1, 0, 0);
    check(g_battle.phase == PHASE_FINISH, "first fatality requires HK+LK with BLOCK");
    finish_setup(player, 0xcccc, 0);
    finish_command(player, FINISH_LIU_KANG_1, chord, 0);
    check(g_battle.phase == PHASE_FINISH, "dragon fatality rejects close range");
    finish_setup(player, 0xcccc, 1);
    finish_command(player, FINISH_FAN, 0, 0);
    check(g_battle.phase == PHASE_FINISH, "fan requires its retail stage");
  }
}

int main(void) {
  normals();
  throws();
  specials();
  weapons();
  fatalities();
  puts("Liu Kang tests passed");
  return 0;
}
