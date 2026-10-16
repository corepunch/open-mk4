#include "game.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void check(bool yes, const char *message) {
  if (!yes) {
    fprintf(stderr, "FAIL: %s\n", message);
    exit(1);
  }
}

static Battle fight(float distance) {
  Battle battle;
  battle_init(&battle, 0, 4);
  battle.phase = PHASE_FIGHT;
  battle.fighters[0].x = -distance * 0.5f;
  battle.fighters[1].x = distance * 0.5f;
  return battle;
}

static void advance_ticks(Battle *battle, int n, const Input *in) {
  const Input zero[2] = {0};
  int i;
  for (i = 0; i < n; ++i)
    battle_step(battle, in ? in : zero);
}

static void do_attack(Battle *battle, int state, bool guard, bool crouch) {
  Input in[2] = {0};
  if (guard)
    in[1].held |= BUTTON_BIT(BUTTON_BLOCK);
  if (crouch)
    in[1].held |= BUTTON_BIT(BUTTON_DOWN);
  if (state == S_PUNCH)
    in[0].pressed |= BUTTON_BIT(BUTTON_PUNCH);
  if (state == S_KICK || state == S_SWEEP)
    in[0].pressed |= BUTTON_BIT(BUTTON_KICK);
  if (state == S_UPPERCUT)
    in[0].pressed |= BUTTON_BIT(BUTTON_UPPERCUT);
  if (state == S_THROW)
    in[0].pressed |= BUTTON_BIT(BUTTON_THROW);
  if (state == S_SWEEP)
    in[0].held |= BUTTON_BIT(BUTTON_DOWN);
  battle_step(battle, in);
  in[0] = (Input){0};
  advance_ticks(battle, 19, in);
}

static void check_roster(void) {
  const CharacterDef *human = roster_def(0);
  const CharacterDef *goro = roster_def(15);
  check(roster_count() == 18, "MK4 roster should be 18 fighters");
  check(human->game == GAME_MK4 && goro->game == GAME_MK4, "Current rows are MK4");
  check(human->commands == roster_def(4)->commands, "Humans share one command table");
  for (int i = 0; i < roster_count(); ++i)
    check(roster_def(i)->moves != NULL && roster_def(i)->move_count == S_COUNT,
        "Every fighter owns a move table");
  check(human->moves != goro->moves, "Goro names clips from his own bank");
  check(roster_clip(human, S_IDLE, 77) == 77, "Idle reads the mesh idle clip");
  check(roster_clip(human, S_PUNCH, 0) == 294, "Scorpion punch clip");
  check(roster_clip(human, S_KICK, 0) == 66, "Scorpion kick from the 0x4e5ca4 table");
  check(roster_clip(roster_def(1), S_KICK, 0) == 71, "Raiden kick from the 0x4e5ca4 table");
  check(roster_clip(human, S_BLOCK, 0) == 322 && roster_clip(goro, S_BLOCK, 0) == 595,
      "Block reads slot 0x53ce30 or Goro's clip");
  check(roster_clip(roster_def(16), S_VICTORY, 0) == roster_clip(roster_def(2), S_VICTORY, 0),
      "Kitana reads Sonya's character entries");
  check(roster_clip(roster_def(17), S_THROW, 0) == roster_clip(roster_def(7), S_THROW, 0),
      "Noob Saibot reads Reiko's character entries");
  check(roster_clip(goro, S_PUNCH, 0) == 597, "Goro punch from his 0x4dfb40 script");
  check(goro->moves[S_CROUCH].first > goro->moves[S_CROUCH].last,
      "Goro's duck plays his rise backwards");
  check(roster_clip(roster_def(13), S_THROW, 0) == 83, "Jax throw from script 0x4de4f0");
  check(roster_clip(human, S_VICTORY, 0) == 115 && (human->moves[S_VICTORY].flags & SF_HOLD),
      "Scorpion holds his 0x4e6a44 win pose");
  check(human->moves[S_PUNCH].damage == 7, "Punch damage row");
  check(human->moves[S_BLOCK].flags & SF_HOLD, "Block holds its last frame");
  check(human->moves[S_SWEEP].flags & SF_LOW, "Sweep is a low");
  check(human->moves[S_THROW].flags & SF_UNBLOCKABLE, "Throw is unblockable");
}

int main(void) {
  check_roster();
  {
    Battle battle;
    battle_init(&battle, 0, 4);
    advance_ticks(&battle, 89, NULL);
    check(battle.phase == PHASE_READY, "Countdown ends too soon");
    advance_ticks(&battle, 1, NULL);
    check(battle.phase == PHASE_FIGHT, "Countdown never starts fight");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_PUNCH, false, false);
    check(battle.fighters[1].health == 93, "Punch must connect once");
    advance_ticks(&battle, 30, NULL);
    check(battle.fighters[1].health == 93, "Repeated hit during recovery");
  }
  {
    Battle battle = fight(3);
    do_attack(&battle, S_KICK, false, false);
    check(battle.fighters[1].health == 100, "Out-of-range attack connects");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_PUNCH, true, false);
    check(battle.fighters[1].health == 100 && battle.effects[0].blocked, "Blocking fails");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_SWEEP, true, false);
    check(battle.fighters[1].health == 90, "Standing guard should lose to sweep");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_SWEEP, true, true);
    check(battle.fighters[1].health == 100, "Crouch guard fails against sweep");
  }
  {
    Battle battle = fight(0.8f);
    do_attack(&battle, S_THROW, true, false);
    check(battle.fighters[1].health == 86, "Throw should beat guard");
  }
  {
    Battle battle = fight(1.f);
    battle.fighters[1].height = 1.5f;
    do_attack(&battle, S_PUNCH, false, false);
    check(battle.fighters[1].health == 100, "Ground punch hits above its vertical reach");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_UPPERCUT, false, false);
    check(battle.fighters[1].health == 82 && battle.fighters[1].state == S_KNOCKDOWN &&
              battle.fighters[1].height > 0,
        "Uppercut does not launch");
    advance_ticks(&battle, 100, NULL);
    check(battle.fighters[1].height == 0 && battle.fighters[1].state == S_IDLE,
        "Knockdown never recovers");
  }
  {
    Battle battle = fight(1.f);
    Input in[2] = {0};
    in[0].pressed = in[1].pressed = BUTTON_BIT(BUTTON_PUNCH);
    battle_step(&battle, in);
    advance_ticks(&battle, 6, NULL);
    check(battle.fighters[0].health == 93 && battle.fighters[1].health == 93,
        "Simultaneous attacks must trade");
  }
  {
    Battle battle = fight(1.f);
    battle.fighters[1].health = 7;
    do_attack(&battle, S_PUNCH, false, false);
    check(battle.phase == PHASE_ROUND_OVER && battle.winner == 0 && battle.fighters[0].wins == 1,
        "KO/round award failed");
    advance_ticks(&battle, 180, NULL);
    check(battle.phase == PHASE_READY && battle.round == 2 && battle.fighters[1].health == 100 &&
              battle.fighters[0].wins == 1,
        "Next round reset failed");
    battle.phase = PHASE_FIGHT;
    battle.fighters[0].x = -0.5f;
    battle.fighters[1].x = 0.5f;
    battle.fighters[1].health = 7;
    do_attack(&battle, S_PUNCH, false, false);
    check(battle.phase == PHASE_MATCH_OVER && battle.fighters[0].wins == 2,
        "Best of three match failed");
    advance_ticks(&battle, 300, NULL);
    check(battle.phase == PHASE_MATCH_OVER, "Match result should wait for rematch");
    battle_reset_match(&battle);
    check(battle.round == 1 && battle.fighters[0].wins == 0, "Rematch failed");
  }
  {
    Battle battle = fight(1.f);
    battle.time_ticks = 1;
    battle.fighters[0].health = 75;
    battle.fighters[1].health = 50;
    battle_step(&battle, (Input[2]){0});
    check(battle.winner == 0 && battle.phase == PHASE_ROUND_OVER, "Timeout winner incorrect");
  }
  {
    Battle battle = fight(1.f);
    battle.time_ticks = 1;
    battle_step(&battle, (Input[2]){0});
    check(battle.winner == -1 && battle.fighters[0].wins == 0 && battle.fighters[1].wins == 0,
        "Draw should not award wins");
  }
  {
    Battle battle = fight(1.f);
    Input in[2] = {0};
    in[0].held = BUTTON_BIT(BUTTON_UP);
    battle_step(&battle, in);
    advance_ticks(&battle, 90, NULL);
    check(battle.fighters[0].height == 0 && battle.fighters[0].velocity == 0, "Jump never lands");
  }
  {
    Battle battle = fight(1.f);
    Input in[2] = {0};
    int i;
    in[0].held = BUTTON_BIT(BUTTON_RIGHT);
    in[1].held = BUTTON_BIT(BUTTON_LEFT);
    advance_ticks(&battle, 600, in);
    check(fabsf(battle.fighters[0].x - battle.fighters[1].x) >= 0.699f, "Bodies overlap");
    in[0].held = BUTTON_BIT(BUTTON_LEFT);
    advance_ticks(&battle, 600, in);
    for (i = 0; i < 2; ++i)
      check(fabsf(battle.fighters[i].x) <= battle.wall, "Arena wall violated");
  }
  {
    Battle battle = fight(1.f);
    int changes = 0, old = 200, i;
    for (i = 0; i < 18000 && battle.phase != PHASE_MATCH_OVER; ++i) {
      Input in[2];
      int sum;
      input_cpu(&battle, 0, &in[0]);
      input_cpu(&battle, 1, &in[1]);
      battle_step(&battle, in);
      sum = battle.fighters[0].health + battle.fighters[1].health;
      if (sum < old)
        ++changes;
      old = sum;
    }
    check(changes > 4 && battle.phase == PHASE_MATCH_OVER,
        "CPU match does not progress to completion");
  }
  puts(
      "PASS: roster tables; countdown; range; single hits; block; sweep; throw; air reach; launch; trades; KO; rounds; timeout; draw; rematch; jump; walls; CPU match");
  return 0;
}
