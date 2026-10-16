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
  for (int reacting = 0; reacting < 2; ++reacting) {
    Battle battle = fight(0, 4.f);
    battle.fighters[0].flags |= FF_FOLLOW_UP;
    Fighter *victim = &battle.fighters[1];
    victim->body.pos[AXIS_Y] -= Q16_ONE;
    fighter_enter_state(&battle, victim, S_JUMP);
    victim->flags |= reacting ? FF_REACTING : 0;
    ice_command(&battle, 0, 2, 0);
    advance(&battle, 8);
    check(objects(&battle, PROJECTILE_ICE) == reacting, "ice command must use the airborne victim reaction flag, not the owner follow-up flag");
  }
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

static void independent_contacts(void) {
  Battle battle = fight(0, 1.f);
  Fighter *owner = &battle.fighters[0];
  Fighter *victim = &battle.fighters[1];
  const AttackRecord record = {Q16_ONE, -Q16_ONE, Q16_ONE, Q16_ONE, 67, 67, 0, 0, 0};
  check(battle_contact_from(&battle, owner, &record, owner->body.pos, owner->facing) == CONTACT_HIT, "object contact must resolve");
  check(!owner->connected, "independent object must not consume the owner's current strike");
  check(battle_contact(&battle, owner, &record) == CONTACT_HIT && owner->connected, "fighter contact must latch its own result");
  owner->connected = false;
  Projectile ray = {.owner = 0, .facing = 1, .heading = {Q16_ONE, 0}};
  ray.pos[AXIS_X] = victim->body.pos[AXIS_X] - Q16_ONE / 2;
  ray.pos[AXIS_Y] = owner->body.pos[AXIS_Y];
  victim->body.pos[AXIS_Z] = Q16_ONE / 5;
  check(battle_projectile_contact(&battle, &ray, &record) == CONTACT_MISS, "projectile bearing gate must reject a nearby sidestepped victim");
  victim->body.pos[AXIS_Z] = 0;
  check(battle_projectile_contact(&battle, &ray, &record) == CONTACT_HIT && !owner->connected, "aligned projectile must contact without latching the owner");
  for (int air = 0; air < 2; ++air) {
    victim->body.pos[AXIS_Y] = victim->body.ground - air * Q16_ONE;
    for (int followup = 0; followup < 2; ++followup) {
      owner->flags = followup ? FF_FOLLOW_UP : 0;
      for (int reacting = 0; reacting < 2; ++reacting) {
        victim->flags = reacting ? FF_REACTING : 0;
        check(fighter_fast_start(&battle, owner) == (air ? reacting : followup), "opcode 45 must select the appropriate fighter's flags");
      }
    }
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
  Battle retreat = fight(0, 4.f);
  clone_command(&retreat, 0);
  advance(&retreat, 8);
  check(retreat.fighters[0].sample_override == 8 && retreat.fighters[0].body.spin[AXIS_X] == -0x6147,
      "clone retreat must finish forward with native angular motion");
  advance(&retreat, 15);
  check(retreat.fighters[0].script_phase == 1 && retreat.fighters[0].body.spin[AXIS_X] == 0 && retreat.fighters[0].body.angle[AXIS_X] == 0,
      "clone retreat must restore orientation and retract after the apex");
  advance(&retreat, 100);
  check(retreat.fighters[0].state == S_IDLE && retreat.fighters[0].body.root_y == 0, "clone landing must restore its root and controls");
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

static unsigned slide_chord(void) {
  return BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_LOW_PUNCH) | BUTTON_BIT(BUTTON_LOW_KICK);
}

static void slide_tests(void) {
  for (int player = 0; player < 2; ++player) {
    for (int outcome = 0; outcome < 4; ++outcome) {
      Battle battle = fight(player, outcome == 2 ? 8.f : 2.f);
      Input input[2] = {0};
      input[player].held = slide_chord();
      if (outcome == 1) {
        input[1 - player].held = BUTTON_BIT(BUTTON_BLOCK);
      }
      if (outcome == 3) {
        battle.fighters[1 - player].body.pos[AXIS_Y] -= 0x3333;
        fighter_enter_state(&battle, &battle.fighters[1 - player], S_JUMP);
      }
      battle_step(&battle, input);
      input[player].held = 0;
      battle_step(&battle, input);
      check(fighter_move(&battle.fighters[player])->clip == 95, "slide chord must enter clip 95");
      for (int t = 0; t < 8; ++t) {
        battle_step(&battle, input);
      }
      check(battle.fighters[1 - player].health == Q16_ONE, "slide damaged before contact window");
      for (int t = 0; t < 18; ++t) {
        battle_step(&battle, input);
      }
      q16 damage = outcome == 2 ? 0 : outcome == 1 ? q16_mul(0x147a, 0x3333) : 0x147a;
      check(battle.fighters[1 - player].health == Q16_ONE - damage, "slide hit/block/miss/air record damage");
      check(battle.fighters[1 - player].reaction == (outcome == 2 ? 0 : outcome == 1 ? 165 : 122), "slide must select explicit victim reaction");
      check((battle.fighters[player].slide_ready != 0) == (outcome == 0 || outcome == 3), "only unblocked slide contact starts cooldown");
      advance(&battle, 180);
      check(battle.fighters[player].state == S_IDLE && battle.fighters[1 - player].state == S_IDLE, "slide must recover both fighters");
      check(!(battle.fighters[player].flags & FF_NO_GUARD), "blocked slide must clear its recovery guard restriction");
      check(battle.fighters[1 - player].health == Q16_ONE - damage, "slide must deal damage once");
    }
    Battle crouch = fight(player, 2.f);
    Input crouch_input[2] = {0};
    crouch_input[player].held = slide_chord();
    crouch_input[1 - player].held = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_DOWN);
    battle_step(&crouch, crouch_input);
    crouch_input[player].held = 0;
    for (int t = 0; t < 12; ++t) {
      battle_step(&crouch, crouch_input);
    }
    check(crouch.fighters[1 - player].reaction == 165 && fighter_move(&crouch.fighters[1 - player])->clip == 255, "crouching slide block must select clip 255");
    const Button triggers[] = {BUTTON_LOW_PUNCH, BUTTON_BLOCK, BUTTON_LOW_KICK};
    for (int k = 0; k < 3; ++k) {
      Battle battle = fight(player, 6.f);
      Input input[2] = {0};
      input[player].held = slide_chord() & ~BUTTON_BIT(triggers[k]);
      for (int t = 0; t < 80; ++t) {
        battle_step(&battle, input);
      }
      input[player].held = slide_chord();
      battle_step(&battle, input);
      advance(&battle, 1);
      check(fighter_move(&battle.fighters[player])->clip == 95, "each LP/BLK/LK press can complete slide chord");
    }
    const Button forbidden[] = {BUTTON_UP, BUTTON_HIGH_PUNCH, BUTTON_HIGH_KICK, BUTTON_RUN};
    for (int k = 0; k < 4; ++k) {
      Battle battle = fight(player, 6.f);
      press(&battle, player, slide_chord() | BUTTON_BIT(forbidden[k]));
      check(fighter_move(&battle.fighters[player])->clip != 95, "slide must reject forbidden extra buttons");
    }
    for (int gate = 0; gate < 3; ++gate) {
      Battle battle = fight(player, 6.f);
      if (gate == 0) {
        battle.fighters[player].slide_ready = 2;
      } else if (gate == 1) {
        battle.fighters[player].body.pos[AXIS_Y] -= Q16_ONE;
        fighter_enter_state(&battle, &battle.fighters[player], S_JUMP);
      } else {
        battle_react(&battle, &battle.fighters[player], 37);
      }
      press(&battle, player, slide_chord());
      check(fighter_move(&battle.fighters[player])->clip != 95, "slide must reject cooldown/air/reaction gates");
    }
    Battle battle = fight(player, 6.f);
    battle.fighters[player].slide_ready = 1;
    press(&battle, player, slide_chord() | BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT));
    check(fighter_move(&battle.fighters[player])->clip == 95, "slide cooldown is inclusive and does not require direction history");
    advance(&battle, 4);
    battle_react(&battle, &battle.fighters[player], 37);
    advance(&battle, 100);
    check(battle.fighters[1 - player].health == Q16_ONE && battle.fighters[player].state == S_IDLE, "interrupted startup must not resume slide or contact");
  }
}

static void weapon_command(Battle *battle, int player) {
  press(battle, player, BUTTON_BIT(BUTTON_DOWN));
  advance(battle, 2);
  press(battle, player, BUTTON_BIT(player ? BUTTON_LEFT : BUTTON_RIGHT) | BUTTON_BIT(BUTTON_HIGH_KICK));
}

static void draw_scepter(Battle *battle, int player) {
  weapon_command(battle, player);
  check(fighter_move(&battle->fighters[player])->clip == 244, "D,F,HK must draw scepter");
  advance(battle, 21);
  check(!battle->fighters[player].armed, "scepter must wait until draw event");
  advance(battle, 27);
  check(battle->fighters[player].armed && battle->fighters[player].weapon->type == 0x6b && fighter_move(&battle->fighters[player])->clip == 286,
      "draw must equip descriptor 6b and armed stance");
}

static void weapons(void) {
  for (int player = 0; player < 2; ++player) {
    unsigned back = BUTTON_BIT(player ? BUTTON_RIGHT : BUTTON_LEFT);
    for (int attack = 0; attack < 4; ++attack) {
      for (int outcome = 0; outcome < 3; ++outcome) {
        Battle battle = fight(player, 4.f);
        draw_scepter(&battle, player);
        if (outcome != 2) {
          battle.fighters[player].body.pos[AXIS_X] = player ? Q16_ONE / 2 : -Q16_ONE / 2;
          battle.fighters[1 - player].body.pos[AXIS_X] = player ? -Q16_ONE / 2 : Q16_ONE / 2;
        }
        unsigned button = BUTTON_BIT(attack % 2 == 0 ? BUTTON_HIGH_PUNCH : BUTTON_LOW_PUNCH) | (attack >= 2 ? back : 0);
        Input input[2] = {0};
        input[player].held = button;
        input[1 - player].held = outcome == 1 ? BUTTON_BIT(BUTTON_BLOCK) | (attack == 1 ? BUTTON_BIT(BUTTON_DOWN) : 0) : 0;
        battle_step(&battle, input);
        input[player].held = 0;
        battle_step(&battle, input);
        const int clips[] = {231, 250, 237, 228};
        const q16 damages[] = {11796, 9830, 9830, 0};
        check(fighter_move(&battle.fighters[player])->clip == clips[attack], "scepter must select distinct directional strikes");
        for (int i = 0; i < 50; ++i) {
          battle_step(&battle, input);
        }
        q16 expected = outcome == 2 ? 0 : outcome == 1 ? q16_mul(damages[attack], 0x3333) : damages[attack];
        check(battle.fighters[1 - player].health == Q16_ONE - expected, "scepter hit/block/miss must apply one record");
        if (attack == 3 && outcome == 0) {
          check(battle.fighters[1 - player].appearance == APPEARANCE_FROZEN, "back LP must freeze without damage");
        }
        advance(&battle, 180);
        check(battle.fighters[player].state == S_IDLE && battle.fighters[player].armed && battle.fighters[1 - player].state == S_IDLE,
            "weapon strike and victim must recover");
      }
    }
    Battle battle = fight(player, 4.f);
    draw_scepter(&battle, player);
    press(&battle, player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH));
    check(fighter_move(&battle.fighters[player])->clip == 260, "scepter must keep normal crouching LP");
    advance(&battle, 80);
    battle_react(&battle, &battle.fighters[player], 51);
    check(!battle.fighters[player].armed && objects(&battle, PROJECTILE_SWORD) == 1, "scepter launch must drop actual weapon");
    advance(&battle, 180);
    Projectile *drop = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (battle.projectiles[i].kind == PROJECTILE_SWORD) {
        drop = &battle.projectiles[i];
      }
    }
    check(drop != NULL && drop->phase == 2 && drop->weapon->type == 0x6b, "resting weapon must retain scepter descriptor");
    Fighter *other = &battle.fighters[1 - player];
    other->body.pos[AXIS_X] = drop->pos[AXIS_X];
    other->body.pos[AXIS_Z] = drop->pos[AXIS_Z];
    fighter_enter_state(&battle, other, S_CROUCH);
    press(&battle, 1 - player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RUN));
    check(other->armed && other->weapon->type == 0x6b && drop->kind == 0, "Scorpion must pick up Sub-Zero's actual weapon");
    advance(&battle, 20);
    other->body.pos[AXIS_X] = player ? -3 * Q16_ONE : 3 * Q16_ONE;
    battle.fighters[player].body.pos[AXIS_X] = player ? 3 * Q16_ONE : -3 * Q16_ONE;
    press(&battle, 1 - player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter_move(other)->clip == 231, "foreign scepter must use scepter attacks");

    battle = fight(player, 4.f);
    draw_scepter(&battle, player);
    weapon_command(&battle, player);
    check(fighter_move(&battle.fighters[player])->clip == 208, "armed D,F,HK must throw");
    advance(&battle, 23);
    check(!battle.fighters[player].armed, "throw must detach scepter");
    battle.contacts_off = true;
    battle_react(&battle, &battle.fighters[player], 37);
    advance(&battle, 180);
    check(objects(&battle, PROJECTILE_SWORD) == 0, "thrown weapon must leave arena after bounded flight");
    weapon_command(&battle, player);
    check(fighter_move(&battle.fighters[player])->clip != 244, "spent weapon must not draw again");
    battle_init(&battle, 4, 0);
    check(!battle.fighters[0].armed && !battle.fighters[0].weapon_used && battle.fighters[0].weapon == NULL, "round reset clears weapon descriptor");

    battle = fight(player, 4.f);
    draw_scepter(&battle, player);
    press(&battle, player, BUTTON_BIT(BUTTON_UP));
    press(&battle, player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter_move(&battle.fighters[player])->clip == 252, "armed air HP must use shared weapon strike");
    advance(&battle, 180);
    check(battle.fighters[player].state == S_IDLE && battle.fighters[player].armed, "armed air strike must land and retain weapon");

    battle = fight(player, 4.f);
    battle.fighters[player].flags |= FF_FOLLOW_UP;
    weapon_command(&battle, player);
    check(battle.fighters[player].armed && fighter_move(&battle.fighters[player])->clip == 237, "follow-up draw must equip and enter back HP");
    advance(&battle, 90);
    check(battle.fighters[player].state == S_IDLE, "follow-up draw must recover");

    battle = fight(player, 4.f);
    int sword_owner = 1 - player;
    Button forward = sword_owner ? BUTTON_LEFT : BUTTON_RIGHT;
    press(&battle, sword_owner, BUTTON_BIT(forward));
    advance(&battle, 2);
    press(&battle, sword_owner, BUTTON_BIT(forward) | BUTTON_BIT(BUTTON_HIGH_KICK));
    advance(&battle, 50);
    check(battle.fighters[sword_owner].armed, "Scorpion draw for foreign pickup");
    battle.fighters[sword_owner].disarm(&battle, &battle.fighters[sword_owner]);
    advance(&battle, 100);
    drop = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (battle.projectiles[i].kind == PROJECTILE_SWORD) {
        drop = &battle.projectiles[i];
      }
    }
    check(drop != NULL && drop->weapon->type == 0x60, "dropped sword descriptor");
    battle.fighters[player].body.pos[AXIS_X] = drop->pos[AXIS_X];
    battle.fighters[player].body.pos[AXIS_Z] = drop->pos[AXIS_Z];
    fighter_enter_state(&battle, &battle.fighters[player], S_CROUCH);
    press(&battle, player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RUN));
    check(battle.fighters[player].armed && battle.fighters[player].weapon->type == 0x60, "Sub-Zero must pick up sword descriptor");
    advance(&battle, 20);
    battle.fighters[player].body.pos[AXIS_X] = player ? 3 * Q16_ONE : -3 * Q16_ONE;
    battle.fighters[sword_owner].body.pos[AXIS_X] = player ? -3 * Q16_ONE : 3 * Q16_ONE;
    press(&battle, player, BUTTON_BIT(BUTTON_HIGH_PUNCH));
    check(fighter_move(&battle.fighters[player])->clip == 235, "Sub-Zero's foreign sword must retain sword attacks");
    advance(&battle, 80);
    press(&battle, player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH));
    advance(&battle, 4);
    check(fighter_move(&battle.fighters[player])->clip == 224, "foreign sword must enable the sword spin");
    advance(&battle, 20);
    check(battle.fighters[player].state == S_IDLE, "foreign sword spin must release");

    for (int reaction = 150; reaction <= 151; ++reaction) {
      battle = fight(player, 3.f);
      Fighter *victim = &battle.fighters[1 - player];
      victim->hits = 4;
      battle_react(&battle, victim, reaction);
      advance(&battle, 1);
      check(fighter_move(victim)->clip == (reaction == 151 ? 131 : 192), "scepter reaction and freeze hit-cap branch");
      advance(&battle, 180);
      check(victim->state == S_IDLE && victim->appearance == APPEARANCE_NORMAL, "scepter reactions must recover");
    }
  }
}

static void weapon_recovery(void) {
  Battle battle = fight(0, 4.f);
  draw_scepter(&battle, 0);
  Fighter *fighter = &battle.fighters[0];
  battle.fighters[1].body.pos[AXIS_X] = fighter->body.pos[AXIS_X] + Q16_ONE;
  fighter->weapon->lp(&battle, fighter);
  for (int i = 0; i < 50 && fighter->blend_duration == 0; ++i) {
    advance(&battle, 1);
  }
  check(fighter->blend_duration == 10 && fighter->blend_clip == 250 && fighter->blend_sample == 31, "scepter LP contact must finish then blend to stance");
  advance(&battle, 9);
  check(fighter->blend_ticks == 9 && fighter->buttons == NULL, "stance blend retains recovery control lock");
  advance(&battle, 1);
  check(fighter->state == S_IDLE && fighter->blend_duration == 0, "stance blend returns control after ten steps");

  battle = fight(0, 4.f);
  fighter = &battle.fighters[0];
  Fighter *victim = &battle.fighters[1];
  fighter->flags |= FF_FOLLOW_UP;
  victim->hits = 4;
  victim->flags |= FF_NO_GUARD;
  weapon_command(&battle, 0);
  check(
      victim->reaction == 120 && victim->damage_scale == 0x4ccc && (victim->flags & FF_NO_GUARD), "follow-up draw must prepare victim and scale a long combo");
  advance(&battle, 10);
  check(!fighter->weapon_travel && fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0, "follow-up draw must omit ordinary back-HP travel");
}

static void jumps(void) {
  Battle battle = fight(0, 8.f);
  Fighter *fighter = &battle.fighters[0];
  battle_step(&battle, (Input[2]){{.held = BUTTON_BIT(BUTTON_UP)}, {0}});
  check(fighter->action == 0x2007 && fighter->sample_override == 0, "straight jump must open at sample zero");
  advance(&battle, 3);
  check(fighter->sample_override == 17 && fighter->buttons != NULL, "straight jump must hold sample 17 with air commands enabled");
  while (fighter_airborne(fighter)) {
    advance(&battle, 1);
  }
  check(fighter->action == 0x2008 && fighter->sample_override == 18, "straight jump must enter its landing continuation");
  advance(&battle, 10);
  check(fighter->state == S_IDLE && fighter->body.root_y == 0, "straight landing must restore control");

  for (int back = 0; back < 2; ++back) {
    battle = fight(0, 8.f);
    Input input[2] = {{.held = BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(back ? BUTTON_LEFT : BUTTON_RIGHT)}, {0}};
    battle_step(&battle, input);
    check(fighter->action == (back ? 0x200a : 0x2009) && fighter_move(fighter)->clip == 301, "directional jump must select its own action and tuck clip");
    check(fighter->body.gravity == 0x24d && fighter->buttons == NULL, "flip takeoff must use native gravity and delay air attacks");
    advance(&battle, 6);
    check(fighter->body.spin[AXIS_X] == (back ? -0x4ccc : 0x4ccc) && fighter->body.root_y == 0x3333, "flip must begin its signed rotation after six samples");
    for (int i = 0; i < 100 && fighter->script_phase != 2; ++i) {
      advance(&battle, 1);
    }
    check(fighter->script_phase == 2 && fighter->sample_override == 7 && fighter->body.spin[AXIS_X] == 0,
        "falling flip must stop rotating and retract from sample seven");
    advance(&battle, 100);
    check(fighter->state == S_IDLE && fighter->body.root_y == 0 && fighter->body.angle[AXIS_X] == 0, "flip landing must restore body and control");
  }
  battle = fight(0, 8.f);
  press(&battle, 0, BUTTON_BIT(BUTTON_UP));
  advance(&battle, 4);
  q16 velocity = fighter->body.vel[AXIS_Y];
  battle_step(&battle, (Input[2]){{.held = BUTTON_BIT(BUTTON_RIGHT)}, {0}});
  check(fighter->action == 0x2009 && fighter->body.vel[AXIS_Y] == velocity + 0x24d,
      "late direction must convert straight flight without restarting vertical motion");
  advance(&battle, 6);
  press(&battle, 0, BUTTON_BIT(BUTTON_HIGH_KICK));
  check(fighter_move(fighter)->clip == 55 && fighter->body.spin[AXIS_X] == 0 && fighter->body.root_y == 0,
      "diagonal air attack must replace flip playback and unwind its transform");
}

static void locomotion(void) {
  Battle battle = fight(0, 8.f);
  Fighter *fighter = &battle.fighters[0];
  Input input[2] = {{.held = BUTTON_BIT(BUTTON_RIGHT)}, {0}};
  battle_step(&battle, input);
  check(fighter_move(fighter)->clip == 267 && fighter->body.vel[AXIS_X] == 0xf5c, "forward walk must use native gait and speed");
  input[0].held = BUTTON_BIT(BUTTON_LEFT);
  battle_step(&battle, input);
  check(fighter_move(fighter)->clip == 272 && fighter->body.vel[AXIS_X] == -q16_mul(0xf5c, 0xb333), "back walk must change gait and use 70 percent speed");
  input[0].held = BUTTON_BIT(BUTTON_RIGHT) | BUTTON_BIT(BUTTON_RUN);
  battle_step(&battle, input);
  check(fighter->body.vel[AXIS_X] == 0x2147 && fighter->run_energy == Q16_ONE - 0x51e, "run must use native speed and energy cost");
  for (int i = 0; i < 50; ++i) {
    battle_step(&battle, input);
  }
  check(fighter->run_energy == 0 && fighter->run_delay == 40, "running must exhaust its meter");
  advance(&battle, 39);
  check(fighter->run_energy == 0, "run energy must respect the recovery delay");
  advance(&battle, 1);
  check(fighter->run_energy == 0xccc, "run energy must regenerate at the native rate");

  battle = fight(0, 4.f);
  draw_scepter(&battle, 0);
  input[0].held = BUTTON_BIT(BUTTON_RIGHT);
  battle_step(&battle, input);
  check(fighter_move(fighter)->clip == 277, "armed forward walk");
  input[0].held = BUTTON_BIT(BUTTON_LEFT);
  battle_step(&battle, input);
  check(fighter_move(fighter)->clip == 278, "armed direction change must select the backward gait without leaving walk state");
  press(&battle, 0, BUTTON_BIT(BUTTON_HIGH_PUNCH));
  check(fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0, "ground strike must stop locomotion");

  battle = fight(0, 4.f);
  battle.fighters[0].body.pos[AXIS_X] = battle.fighters[1].body.pos[AXIS_X] = 0;
  battle.fighters[0].body.pos[AXIS_Z] = -2 * Q16_ONE;
  battle.fighters[1].body.pos[AXIS_Z] = 2 * Q16_ONE;
  battle_world(&battle);
  press(&battle, 0, BUTTON_BIT(BUTTON_UP) | BUTTON_BIT(BUTTON_RIGHT));
  check(fighter->body.vel[AXIS_X] == 0 && fighter->body.vel[AXIS_Z] == 0xe14, "directional jump must follow the pair axis after a quarter turn");
}

static Battle sub_zero_finish(int player, q16 distance, int arena) {
  Battle battle = fight(player, 3.f);
  battle.arena = arena;
  battle.fighters[player].wins = 1;
  battle_damage(&battle, &battle.fighters[1 - player], Q16_ONE);
  advance(&battle, 1);
  check(battle.phase == PHASE_FINISH && battle.winner == player, "Sub-Zero deciding KO must open finish window");
  battle.fighters[0].body.pos[AXIS_X] = -distance / 2;
  battle.fighters[1].body.pos[AXIS_X] = distance - distance / 2;
  return battle;
}

static void sub_zero_finish_command(Battle *battle, int player, int id, unsigned extra, int window) {
  Button back = player ? BUTTON_RIGHT : BUTTON_LEFT;
  Button forward = player ? BUTTON_LEFT : BUTTON_RIGHT;
  Button tokens[4] = {forward, back, forward, BUTTON_DOWN};
  Button attack = BUTTON_HIGH_PUNCH;
  int count = 4;
  q16 distance = battle->fighters[1].body.pos[AXIS_X] - battle->fighters[0].body.pos[AXIS_X];
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
    press(battle, player, BUTTON_BIT(tokens[i]));
    advance(battle, 2);
  }
  if (window > count * 4) {
    advance(battle, window - count * 4);
  }
  /* Restore exact range after command-entry positioning, before the button. */
  battle->fighters[0].body.pos[AXIS_X] = -distance / 2;
  battle->fighters[1].body.pos[AXIS_X] = distance - distance / 2;
  press(battle, player, BUTTON_BIT(attack) | extra);
}

static void fatalities(void) {
  const int ids[] = {FINISH_SUB_ZERO_1, FINISH_SUB_ZERO_2, FINISH_FAN, FINISH_SPIKE};
  unsigned chord = BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN);
  for (int player = 0; player < 2; ++player) {
    for (int kind = 0; kind < 4; ++kind) {
      int id = ids[kind];
      Battle battle = sub_zero_finish(player, id == FINISH_SUB_ZERO_2 ? 0x26666 : 0xcccc, id == FINISH_FAN ? 8 : 0);
      int time = battle.time_ticks;
      sub_zero_finish_command(&battle, player, id, id == FINISH_SUB_ZERO_1 ? chord : 0, kind == 0 ? 60 : kind == 1 ? 50 : kind == 2 ? 48 : 40);
      check(battle.phase == PHASE_FATALITY && battle.finisher == id, "Sub-Zero command at inclusive window must select finisher");
      check(battle.time_ticks == time && battle.fighters[player].wins == 2, "finisher freezes timer and awards no extra win");
      check(battle.fighters[0].buttons == NULL && battle.fighters[1].buttons == NULL, "finisher must lock both players");
      if (id == FINISH_SUB_ZERO_1) {
        advance(&battle, 90);
        check(fighter_move(&battle.fighters[player])->clip == 385 && fighter_move(&battle.fighters[1 - player])->clip == 387,
            "first fatality must select Sub-Zero paired clips");
      } else if (id == FINISH_SUB_ZERO_2) {
        advance(&battle, 80);
        check(fighter_move(&battle.fighters[player])->clip == 552 && fighter_move(&battle.fighters[1 - player])->clip == 206,
            "second fatality must run the independent victim animation during breath");
        advance(&battle, 135);
        check(fighter_move(&battle.fighters[player])->clip == 95 && battle.fighters[1 - player].appearance == APPEARANCE_FROZEN,
            "second fatality must freeze before approaching with slide");
        advance(&battle, 50);
        check(battle.fighters[1 - player].appearance == APPEARANCE_HIDDEN && fighter_move(&battle.fighters[player])->clip == 293,
            "second fatality must shatter on uppercut");
      }
      advance(&battle, 700);
      check(battle.phase == PHASE_MATCH_OVER && battle.finisher == id && battle.fighters[player].script == NULL, "finisher must complete once and settle");
      if (id == FINISH_FAN) {
        check(battle.fighters[1 - player].appearance == APPEARANCE_HIDDEN, "fan must remove victim");
      } else if (id == FINISH_SPIKE) {
        check(battle.fighters[1 - player].body.pos[AXIS_Y] == -0x59999, "spikes must retain impaled position");
      } else if (id == FINISH_SUB_ZERO_1) {
        Fighter *owner = &battle.fighters[player];
        int sample = owner->sample_override;
        advance(&battle, 1);
        check(owner->sample_override != sample && owner->sample_override >= 96 && owner->sample_override <= 140,
            "first fatality must keep its retail final loop after the result");
      }
      battle_reset_match(&battle);
      check(battle.finisher == 0 && battle.fighters[0].appearance == APPEARANCE_NORMAL && battle.fighters[1].appearance == APPEARANCE_NORMAL,
          "rematch must clear all finisher state");
    }
    for (int kind = 0; kind < 4; ++kind) {
      int id = ids[kind];
      Battle battle = sub_zero_finish(player, id == FINISH_SUB_ZERO_2 ? 0x26666 : 0xcccc, id == FINISH_FAN ? 8 : 0);
      sub_zero_finish_command(&battle, player, id, id == FINISH_SUB_ZERO_1 ? chord : 0, kind == 0 ? 61 : kind == 1 ? 51 : kind == 2 ? 49 : 41);
      check(battle.phase == PHASE_FINISH, "finisher must reject one tick beyond command window");
    }
    const q16 boundaries[] = {Q16_ONE, 0x1cccc, 0x30000};
    for (int boundary = 0; boundary < 3; ++boundary) {
      int id = boundary == 0 ? FINISH_SUB_ZERO_1 : FINISH_SUB_ZERO_2;
      Battle battle = sub_zero_finish(player, boundaries[boundary], 0);
      battle.projectiles[0].kind = PROJECTILE_ICE;
      sub_zero_finish_command(&battle, player, id, boundary == 0 ? chord : 0, 0);
      check(battle.phase == PHASE_FATALITY && objects(&battle, PROJECTILE_ICE) == 0, "inclusive range boundary and object cleanup");
    }
    for (int above = 0; above < 2; ++above) {
      Battle battle = sub_zero_finish(player, above ? 0x30001 : 0x1cccb, 0);
      sub_zero_finish_command(&battle, player, FINISH_SUB_ZERO_2, 0, 0);
      check(battle.phase == PHASE_FINISH, "second fatality must reject either side of range interval");
    }
    for (int missing = 0; missing < 4; ++missing) {
      Battle battle = sub_zero_finish(player, 0xcccc, 0);
      unsigned extra = missing == 0   ? 0
                       : missing == 1 ? BUTTON_BIT(BUTTON_RUN)
                       : missing == 2 ? BUTTON_BIT(BUTTON_BLOCK)
                                      : chord | BUTTON_BIT(BUTTON_LOW_PUNCH);
      sub_zero_finish_command(&battle, player, FINISH_SUB_ZERO_1, extra, 0);
      check(battle.phase == PHASE_FINISH, "first fatality requires exact held chord");
    }
    for (int kind = 0; kind < 4; ++kind) {
      int id = ids[kind];
      Battle battle = sub_zero_finish(player, id == FINISH_SUB_ZERO_2 ? 0x18000 : 0x10001, id == FINISH_FAN ? 8 : 0);
      sub_zero_finish_command(&battle, player, id, id == FINISH_SUB_ZERO_1 ? chord : 0, 0);
      check(battle.phase == PHASE_FINISH, "finisher must reject range outside boundary");
    }
    for (int kind = 2; kind < 4; ++kind) {
      Battle battle = sub_zero_finish(player, 0xcccc, kind == 2 ? 0 : 8);
      sub_zero_finish_command(&battle, player, ids[kind], 0, 0);
      check(battle.phase == PHASE_FINISH, "stage fatality must require its stage");
    }
    Battle battle = sub_zero_finish(player, 0xcccc, 0);
    sub_zero_finish_command(&battle, 1 - player, FINISH_SUB_ZERO_1, chord, 0);
    check(battle.phase == PHASE_FINISH, "loser cannot start finisher");
    advance(&battle, 300);
    check(battle.phase == PHASE_MATCH_OVER && battle.finisher == 0, "finish window must still time out");
  }
}

static void fatality_blends(void) {
  Battle battle = sub_zero_finish(0, 0xcccc, 0);
  sub_zero_finish_command(&battle, 0, FINISH_SUB_ZERO_1, BUTTON_BIT(BUTTON_BLOCK) | BUTTON_BIT(BUTTON_RUN), 60);
  Fighter *victim = &battle.fighters[1];
  for (int i = 0; i < 300 && victim->scene.index != 8; ++i) {
    advance(&battle, 1);
  }
  check(victim->scene.index == 8 && victim->blend_sample == 48 && victim->sample_override == 46, "first fatality must blend from victim sample 48 back to 46");
  advance(&battle, 30);
  check(victim->scene.index == 8 && victim->blend_weight < Q16_ONE, "victim blend must retain its native 0x666 progression");
}

static void sidesteps(void) {
  for (int player = 0; player < 2; ++player) {
    Battle range = fight(player, .6f);
    range.fighters[1 - player].body.pos[AXIS_Z] = 2 * Q16_ONE;
    check(fighter_distance(&range, &range.fighters[player]) > 2.f, "range must include sidestep depth");
    press(&range, player, BUTTON_BIT(BUTTON_LOW_PUNCH));
    check(fighter_move(&range.fighters[player])->clip == 294, "sidestep depth must prevent a false close throw");
    for (int down = 0; down < 2; ++down) {
      for (int interval = 2; interval <= 9; ++interval) {
        Battle battle = fight(player, 4.f);
        press(&battle, player, BUTTON_BIT(BUTTON_RUN));
        check(battle.fighters[player].script == NULL, "first RUN press must not manufacture a double tap");
        advance(&battle, interval - 2);
        Input run_input[2] = {0};
        run_input[player].held = BUTTON_BIT(BUTTON_RUN) | (down ? BUTTON_BIT(BUTTON_DOWN) : 0);
        battle_step(&battle, run_input);
        run_input[player].held &= ~BUTTON_BIT(BUTTON_RUN);
        battle_step(&battle, run_input);
        Fighter *fighter = &battle.fighters[player];
        bool accepted = interval >= 3 && interval <= 8;
        check((fighter_move(fighter)->clip == 300) == accepted, "RUN two-sample rule and inclusive eight-tick window");
        if (accepted) {
          check(fighter->sample_override == (down ? 0 : 17) && fighter->sidestep_ready == battle.ticks + 80, "sidestep cursor direction/cooldown");
          check(fighter->body.vel[AXIS_X] == (player ? -0x443 : 0x443) && fighter->body.vel[AXIS_Z] == (player ? -1 : 1) * (down ? -0xccc : 0xccc),
              "sidestep must use pair-axis forward and perpendicular motion");
          q16 z = fighter->body.pos[AXIS_Z];
          advance(&battle, 18);
          check(fighter->state == S_IDLE && fighter->body.vel[AXIS_Z] == 0 && fighter->body.pos[AXIS_Z] != z, "sidestep must travel then restore control");
          check(battle.fighters[1 - player].health == Q16_ONE, "sidestep must not damage opponent");
        }
      }
    }
    for (int boundary = -1; boundary <= 1; ++boundary) {
      Battle battle = fight(player, 4.f);
      press(&battle, player, BUTTON_BIT(BUTTON_RUN));
      advance(&battle, 2);
      /* Next press is tick 5; compare just below/at/above its deadline. */
      battle.fighters[player].sidestep_ready = battle.ticks + 1 + boundary;
      press(&battle, player, BUTTON_BIT(BUTTON_RUN));
      check((fighter_move(&battle.fighters[player])->clip == 300) == (boundary <= 0), "sidestep cooldown inclusive deadline");
    }
    Battle battle = fight(player, 4.f);
    press(&battle, player, BUTTON_BIT(BUTTON_UP));
    press(&battle, player, BUTTON_BIT(BUTTON_RUN));
    advance(&battle, 2);
    press(&battle, player, BUTTON_BIT(BUTTON_RUN));
    check(fighter_move(&battle.fighters[player])->clip != 300, "airborne owner must reject sidestep");
    battle = fight(player, 4.f);
    battle.fighters[1 - player].body.pos[AXIS_Y] -= Q16_ONE;
    fighter_enter_state(&battle, &battle.fighters[1 - player], S_JUMP);
    press(&battle, player, BUTTON_BIT(BUTTON_RUN));
    advance(&battle, 2);
    press(&battle, player, BUTTON_BIT(BUTTON_RUN));
    check(abs(battle.fighters[player].body.vel[AXIS_X]) == q16_mul(0x443, 0x4ccc), "airborne victim must scale sidestep motion to 30 percent");
    battle_react(&battle, &battle.fighters[player], 37);
    advance(&battle, 100);
    check(battle.fighters[player].state == S_IDLE && battle.fighters[player].body.vel[AXIS_Z] == 0, "interrupted sidestep must clear motion");

    battle = fight(player, 4.f);
    draw_scepter(&battle, player);
    battle.fighters[player].disarm(&battle, &battle.fighters[player]);
    advance(&battle, 100);
    Projectile *drop = NULL;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (battle.projectiles[i].kind == PROJECTILE_SWORD) {
        drop = &battle.projectiles[i];
      }
    }
    check(drop != NULL && drop->phase == 2, "prepare sidestep versus pickup priority");
    battle.fighters[player].body.pos[AXIS_X] += player ? Q16_ONE * 2 : -Q16_ONE * 2;
    press(&battle, player, BUTTON_BIT(BUTTON_RUN));
    advance(&battle, 2);
    battle.fighters[player].body.pos[AXIS_X] = drop->pos[AXIS_X];
    battle.fighters[player].body.pos[AXIS_Z] = drop->pos[AXIS_Z];
    press(&battle, player, BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RUN));
    check(fighter_move(&battle.fighters[player])->clip == 300 && !battle.fighters[player].armed && drop->kind != 0,
        "RUN double-tap record must take priority over crouching pickup");
    battle_next_round(&battle);
    check(battle.fighters[player].sidestep_ready == 0, "new round clears sidestep deadline");
  }
}

int main(void) {
  jumps();
  locomotion();
  fatality_blends();
  weapon_recovery();
  independent_contacts();
  sidesteps();
  fatalities();
  weapons();
  normals();
  ice();
  clone_tests();
  slide_tests();
  puts(
      "PASS: RUN double-tap/motion/windows/cooldown/priority/interruption; Sub-Zero fatalities/both stages/chord/windows/range/rematch; scepter strikes/freeze/air/draw/follow-up/disarm/throw/cross-pickup/reactions; slide chord/gates/contact/recovery/interruption; clone pose/contact/near/cooldown/air/expiry/pool/mirror; ice commands/gates/flight/freeze/thaw/backfire/interruption/reset; Sub-Zero normal selectors, kick records, combos, paired throws, crouch launcher, avoidance and air recovery for both players");
  return 0;
}
