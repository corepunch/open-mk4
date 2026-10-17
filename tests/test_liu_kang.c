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

int main(void) {
  normals();
  throws();
  puts("Liu Kang tests passed");
  return 0;
}
