#include "game.h"
#include <stdio.h>
#include <stdlib.h>

static int current_character, current_player;

static void check(bool condition, const char *message) {
  if (!condition) {
    fprintf(stderr, "FAIL %s P%d: %s\n", current_character == 1 ? "Raiden" : "Sonya", current_player + 1, message);
    for (int i = 0; i < 2; ++i) {
      Fighter *f = &g_battle.fighters[i];
      fprintf(stderr, "  P%d health=%d state=%d clip=%d sample=%d flags=%x fiber=%d reaction=%d y=%d phase=%d\n", i + 1, f->health, f->state,
          fighter_move(f)->clip, f->sample_override, f->flags, fighter_fiber_active(f), f->reaction, f->body.pos[AXIS_Y], g_battle.phase);
    }
    exit(1);
  }
}

static Fighter *owner(void) {
  return &g_battle.fighters[current_player];
}

static Fighter *victim(void) {
  return &g_battle.fighters[1 - current_player];
}

static void advance(int ticks) {
  for (int i = 0; i < ticks; ++i) {
    battle_step((Input[2]){0});
  }
}

static void setup(float distance) {
  battle_init(current_player == 0 ? current_character : 0, current_player == 1 ? current_character : 0);
  g_battle.phase = PHASE_FIGHT;
  g_battle.fighters[0].body.pos[AXIS_X] = (q16)(-distance * Q16_ONE / 2);
  g_battle.fighters[1].body.pos[AXIS_X] = (q16)(distance * Q16_ONE / 2);
}

static void press(unsigned held) {
  Input input[2] = {0};
  input[current_player].held = held;
  battle_step(input);
  advance(1);
}

static Button forward(void) {
  return current_player ? BUTTON_LEFT : BUTTON_RIGHT;
}

static Button back(void) {
  return current_player ? BUTTON_RIGHT : BUTTON_LEFT;
}

static void command(const Button *directions, int count, Button button) {
  for (int i = 0; i < count; ++i) {
    press(BUTTON_BIT(directions[i]));
    advance(2);
  }
  press(BUTTON_BIT(button));
}

static void normals(void) {
  unsigned buttons[] = {BUTTON_BIT(BUTTON_HIGH_KICK), BUTTON_BIT(BUTTON_LOW_KICK), BUTTON_BIT(back()) | BUTTON_BIT(BUTTON_HIGH_KICK),
      BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_HIGH_KICK)};
  int clips[2][4] = {{71, 70, 64, 52}, {65, 308, 313, 53}};
  for (int i = 0; i < 4; ++i) {
    setup(4.f);
    press(buttons[i]);
    check(fighter_move(owner())->clip == clips[current_character - 1][i], "normal selected the wrong roster stream");
    advance(150);
    check(owner()->state == S_IDLE && !fighter_fiber_active(owner()), "normal miss did not restore control");
    check(victim()->health == Q16_ONE, "normal miss damaged the other fighter");
  }
  for (int blocked = 0; blocked < 2; ++blocked) {
    setup(1.1f);
    Input input[2] = {0};
    input[current_player].held = BUTTON_BIT(BUTTON_LOW_KICK);
    input[1 - current_player].held = blocked ? BUTTON_BIT(BUTTON_BLOCK) : 0;
    battle_step(input);
    input[current_player].held = 0;
    for (int i = 0; i < 100; ++i) {
      battle_step(input);
    }
    q16 damage = blocked ? q16_mul(0x1c28, 0x3333) : 0x1c28;
    check(victim()->health == Q16_ONE - damage, "generated low-kick hit/block record or repeat count");
    check(owner()->state == S_IDLE, "generated low kick did not recover");
  }
}

static void throws(void) {
  for (int lk = 0; lk < 2; ++lk) {
    q16 damage = lk ? (current_character == 1 ? 0x35c2 : 0x2e14) : 0x2666;
    setup(.5f);
    press(BUTTON_BIT(lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH));
    int clip = lk ? (current_character == 1 ? 33 : 43) : (current_character == 1 ? 76 : 78);
    check(fighter_move(owner())->clip == clip, "throw used another fighter's selected root");
    advance(10);
    check(victim()->health == Q16_ONE, "throw damaged before its paired opening");
    advance(250);
    check(victim()->health == Q16_ONE - damage, "paired throw damage");
    check(owner()->state == S_IDLE && victim()->state == S_IDLE, "throw participants did not recover independently");
    check(!(victim()->flags & FF_PAIRED) && !fighter_fiber_active(victim()), "throw left a victim lifetime attached");
    setup(.5f);
    press(BUTTON_BIT(lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH));
    advance(10);
    battle_react(owner(), 37);
    advance(220);
    check(victim()->health == Q16_ONE && victim()->state == S_IDLE, "interrupted throw resumed or stranded the victim");
    setup(.5f);
    owner()->wins = 1;
    victim()->health = 1;
    press(BUTTON_BIT(lk ? BUTTON_LOW_KICK : BUTTON_LOW_PUNCH));
    advance(250);
    check(victim()->health == 0 && !(victim()->flags & FF_PAIRED) && !fighter_fiber_active(victim()), "lethal throw left a paired/fiber lifetime");
  }
}

static int projectiles(void) {
  int count = 0;
  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    count += g_battle.projectiles[i].kind == PROJECTILE_FIREBALL;
  }
  return count;
}

static void specials(void) {
  setup(4.f);
  Button directions[] = {BUTTON_DOWN, current_character == 1 ? back() : forward()};
  command(directions, 2, BUTTON_LOW_PUNCH);
  check(fighter_move(owner())->clip == (current_character == 1 ? 17 : 15), "projectile command gate or roster clip");
  for (int tick = 0; tick < 40 && projectiles() == 0; ++tick) {
    advance(1);
  }
  check(projectiles() == 1, "generated spawn did not create an independent projectile");
  battle_react(owner(), 37);
  advance(110);
  check(victim()->health == Q16_ONE - (current_character == 1 ? 0x23d7 : 0x1eb8), "released projectile did not survive interruption or damaged twice");
  check(victim()->state == S_IDLE && projectiles() == 0, "projectile victim/object did not recover");
  if (current_character == 1) {
    setup(3.f);
    press(BUTTON_BIT(BUTTON_DOWN));
    advance(2);
    press(BUTTON_BIT(BUTTON_UP));
    check(owner()->action == 0x20c && fighter_fiber_active(owner()), "Down/Up did not replace the pending jump with teleport");
    advance(200);
    check(owner()->state == S_IDLE && !fighter_airborne(owner()), "teleport did not return through landing");
    setup(3.f);
    Button torpedo[] = {forward(), forward()};
    command(torpedo, 2, BUTTON_LOW_KICK);
    check(owner()->action == 0x202, "torpedo command");
    advance(200);
    check(owner()->state == S_IDLE && victim()->state == S_IDLE, "torpedo owner or carried victim did not recover");
    setup(3.f);
    command(torpedo, 2, BUTTON_LOW_KICK);
    for (int tick = 0; tick < 60 && !(victim()->flags & FF_PAIRED); ++tick) {
      advance(1);
    }
    check((owner()->flags & FF_PAIRED) && (victim()->flags & FF_PAIRED), "torpedo did not capture its victim before interruption");
    check(victim()->facing == -owner()->facing, "torpedo victim turned its back to Raiden");
    battle_react(victim(), 13);
    q16 released_x = victim()->body.pos[AXIS_X] = current_player ? -6 * Q16_ONE : 6 * Q16_ONE;
    advance(2);
    check(abs(victim()->body.pos[AXIS_X] - released_x) < Q16_ONE && victim()->reaction == 13, "torpedo moved or replaced an interrupted victim");
    advance(200);
    check(owner()->state == S_IDLE && victim()->state == S_IDLE && !fighter_airborne(owner()) && !fighter_airborne(victim()) && !(owner()->flags & FF_PAIRED) &&
              !(victim()->flags & FF_PAIRED),
        "interrupted torpedo pair did not recover independently");
    for (int blocked = 0; blocked < 2; ++blocked) {
      setup(blocked ? 3.f : 16.f);
      command(torpedo, 2, BUTTON_LOW_KICK);
      Input guard[2] = {0};
      guard[1 - current_player].held = blocked ? BUTTON_BIT(BUTTON_BLOCK) : 0;
      for (int t = 0; t < 120; ++t) {
        battle_step(guard);
      }
      advance(100);
      q16 expected = blocked ? q16_mul(0x2666, 0x3333) : 0;
      check(victim()->health == Q16_ONE - expected && owner()->state == S_IDLE, "torpedo miss/block damage or recovery");
    }
  } else {
    setup(3.f);
    Button cartwheel[] = {back(), BUTTON_DOWN, forward()};
    command(cartwheel, 3, BUTTON_LOW_KICK);
    check(fighter_move(owner())->clip == 104, "cartwheel command");
    advance(180);
    check(owner()->state == S_IDLE && victim()->state == S_IDLE, "cartwheel recovery");
    setup(3.f);
    check(battle_cpu_action(owner(), CPU_SPECIAL_1), "native bicycle entry missing");
    advance(220);
    check(owner()->state == S_IDLE && !fighter_airborne(owner()), "bicycle did not land");
    setup(3.f);
    Button square[] = {back(), back(), BUTTON_DOWN};
    command(square, 3, BUTTON_HIGH_KICK);
    check(owner()->action == 0x201, "square-wave command");
    advance(220);
    check(owner()->state == S_IDLE && victim()->state == S_IDLE && !fighter_airborne(owner()), "square-wave launch/recovery");
    setup(.7f);
    press(BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH));
    check(owner()->action == 0x203, "leg-grab chord gate");
    advance(230);
    check(owner()->state == S_IDLE && victim()->state == S_IDLE, "leg-grab native pair did not release");
    for (int blocked = 0; blocked < 2; ++blocked) {
      setup(blocked ? .7f : 8.f);
      press(BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH));
      Input guard[2] = {0};
      guard[1 - current_player].held = blocked ? BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_BLOCK) : 0;
      for (int t = 0; t < 100; ++t) {
        battle_step(guard);
      }
      advance(80);
      q16 expected = blocked ? q16_mul(0x2e14, 0x3333) : 0;
      check(victim()->health == Q16_ONE - expected && owner()->state == S_IDLE, "leg-grab miss/block branch");
      check(!(victim()->flags & FF_PAIRED), "leg-grab miss/block incorrectly paired victim");
    }
    setup(.7f);
    press(BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH));
    advance(5);
    battle_react(owner(), 37);
    advance(140);
    check(victim()->health == Q16_ONE && victim()->state == S_IDLE, "interrupted leg-grab committed damage");
    setup(3.f);
    owner()->body.pos[AXIS_Y] = victim()->body.pos[AXIS_Y] = -3 * Q16_ONE;
    victim()->body.pos[AXIS_X] = owner()->body.pos[AXIS_X] + owner()->facing * 0x8000;
    fighter_enter_state(owner(), S_JUMP);
    fighter_enter_state(victim(), S_JUMP);
    press(BUTTON_BIT(BUTTON_BLOCK));
    check(fighter_move(owner())->clip == 77 && fighter_move(victim())->clip == 161, "Sonya air throw reused the common pair");
    advance(230);
    check(victim()->health == Q16_ONE - 0x2666, "Sonya air throw damage");
    check(owner()->state == S_IDLE && victim()->state == S_IDLE, "Sonya air throw landing/getup");
  }
}

static void weapons_and_scenes(void) {
  setup(4.f);
  check(battle_cpu_action(owner(), CPU_DRAW), "weapon draw entry missing");
  advance(70);
  check(owner()->armed && owner()->weapon->type == (current_character == 1 ? 0x61 : 0x62), "generated draw equipped another character's weapon");
  for (int button = 0; button < 2; ++button) {
    press(BUTTON_BIT(back()) | BUTTON_BIT(button ? BUTTON_LOW_PUNCH : BUTTON_HIGH_PUNCH));
    advance(120);
    check(owner()->state == S_IDLE && owner()->armed && !fighter_fiber_active(owner()), "armed back attack did not recover");
  }
  for (int which = 0; which < 4; ++which) {
    setup(2.f);
    owner()->wins = 1;
    battle_damage(victim(), Q16_ONE);
    advance(1);
    for (int t = 0; t < 180 && g_battle.finish_waiting; ++t) {
      advance(1);
    }
    check(battle_can_finish(owner()), "deciding KO did not open the finish controller");
    g_battle.arena = which == 2 ? 8 : 0;
    check(battle_cpu_action(owner(), which == 0 ? CPU_FINISH_1 : which == 1 ? CPU_FINISH_2 : which == 2 ? CPU_FINISH_FAN : CPU_FINISH_SPIKE), "finisher entry");
    advance(900);
    check(g_battle.phase == PHASE_MATCH_OVER, "generated scene or native teardown did not complete");
    check(owner()->wins == 2, "scene awarded an extra win");
    int result_age = g_battle.phase_ticks;
    advance(360);
    check(g_battle.phase == PHASE_MATCH_OVER && g_battle.phase_ticks == result_age + 360 && owner()->body.physics == NULL && victim()->body.physics == NULL,
        "completed scene must retire its coordinator and never resubmit completion");
    battle_reset_match();
    check(!fighter_fiber_active(owner()) && !fighter_fiber_active(victim()) && g_battle.finisher == 0, "reset left generated scene stacks alive");
  }
}

int main(void) {
  for (current_character = 1; current_character <= 2; ++current_character) {
    for (current_player = 0; current_player < 2; ++current_player) {
      normals();
      throws();
      specials();
      weapons_and_scenes();
    }
  }
  puts("PASS: generated Raiden/Sonya commands, contact branches, pairs, projectile interruption, air throw, weapons, scenes and reset in both slots");
  return 0;
}
