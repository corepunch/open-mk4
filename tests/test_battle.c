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
  battle.fighters[0].body.pos[AXIS_X] = (q16)(-distance * 0.5f * Q16_ONE);
  battle.fighters[1].body.pos[AXIS_X] = (q16)(distance * 0.5f * Q16_ONE);
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
  /* Fighter 0 stands left of fighter 1, so back is left. */
  if (state == S_PUNCH)
    in[0].held |= BUTTON_BIT(BUTTON_HIGH_PUNCH);
  if (state == S_KICK)
    in[0].held |= BUTTON_BIT(BUTTON_HIGH_KICK);
  if (state == S_SWEEP)
    in[0].held |= BUTTON_BIT(BUTTON_LOW_KICK) | BUTTON_BIT(BUTTON_LEFT);
  if (state == S_UPPERCUT)
    in[0].held |= BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_DOWN);
  if (state == S_THROW)
    in[0].held |= BUTTON_BIT(BUTTON_LOW_PUNCH);
  battle_step(battle, in);
  in[0] = (Input){0};
  advance_ticks(battle, 20, in);
}

static void check_roster(void) {
  const CharacterDef *human = roster_def(0);
  const CharacterDef *goro = roster_def(15);
  check(roster_count() == 18, "MK4 roster should be 18 fighters");
  check(human->game == GAME_MK4 && goro->game == GAME_MK4, "Current rows are MK4");
  check(human->controls != roster_def(4)->controls, "Scorpion owns its command and stance tables");
  for (int i = 0; i < roster_count(); ++i)
    check(roster_def(i)->moves != NULL && roster_def(i)->move_count == S_COUNT, "Every fighter owns a move table");
  check(human->moves != goro->moves, "Goro names clips from his own bank");
  check(roster_clip(human, S_IDLE, 77) == 77, "Idle reads the mesh idle clip");
  check(roster_clip(human, S_PUNCH, 0) == 294, "Scorpion punch clip");
  check(roster_clip(human, S_KICK, 0) == 66, "Scorpion kick from the 0x4e5ca4 table");
  check(roster_clip(roster_def(1), S_KICK, 0) == 71, "Raiden kick from the 0x4e5ca4 table");
  check(roster_clip(human, S_BLOCK, 0) == 322 && roster_clip(goro, S_BLOCK, 0) == 595, "Block reads slot 0x53ce30 or Goro's clip");
  check(roster_clip(roster_def(16), S_VICTORY, 0) == roster_clip(roster_def(2), S_VICTORY, 0), "Kitana reads Sonya's character entries");
  check(roster_clip(roster_def(17), S_THROW, 0) == roster_clip(roster_def(7), S_THROW, 0), "Noob Saibot reads Reiko's character entries");
  check(roster_clip(goro, S_PUNCH, 0) == 597, "Goro punch from his 0x4dfb40 script");
  check(goro->moves[S_CROUCH]->first > goro->moves[S_CROUCH]->last, "Goro's duck plays his rise backwards");
  check(roster_clip(roster_def(13), S_THROW, 0) == 83, "Jax throw from script 0x4de4f0");
  check(roster_clip(human, S_VICTORY, 0) == 115 && (human->moves[S_VICTORY]->flags & SF_HOLD), "Scorpion holds his 0x4e6a44 win pose");
  check(human->moves[S_PUNCH]->flags & SF_RECORD, "Scorpion uses script attack records");
  check(human->moves[S_BLOCK]->flags & SF_HOLD, "Block holds its last frame");
  check(human->moves[S_SWEEP]->flags & SF_LOW, "Sweep is a low");
  check(human->moves[S_THROW]->flags & SF_UNBLOCKABLE, "Throw is unblockable");
}

static void check_original_kicks(void) {
  struct KickFixture {
    int roster, clip, count;
    int frames[30];
  };
  static const struct KickFixture fixtures[] = {
#include "fixtures/original_kicks.inc"
  };
  for (size_t i = 0; i < sizeof fixtures / sizeof fixtures[0]; ++i) {
    const struct KickFixture *expected = &fixtures[i];
    Battle battle = fight(4.f);
    int player = expected->roster == 0 ? 0 : 1;
    Fighter *fighter = &battle.fighters[player];
    Input in[2] = {0};
    in[player].held = BUTTON_BIT(BUTTON_HIGH_KICK);
    battle_step(&battle, in);
    check(fighter->pending != NULL && fighter->state == S_IDLE, "Press must start its move on the next tick");
    in[player] = (Input){0};
    battle_step(&battle, in);
    check(fighter->current_move->clip == expected->clip, "Kick callback chose wrong move");
    for (int tick = 0; tick < expected->count; ++tick) {
      check(fighter->state == S_KICK && fighter->ticks == tick, "Kick returned prematurely");
      check(fighter_frame(fighter, expected->clip == 66 ? 18 : 28) == expected->frames[tick], "Kick sample differs from original VM/native recovery");
      in[player] = (Input){.held = BUTTON_BIT(BUTTON_LEFT)};
      battle_step(&battle, in);
    }
    check(fighter->state == S_WALK && fighter->ticks == 0, "Kick end callback did not release control on original return step");
    check(fighter->current_move == roster_def(expected->roster)->moves[S_WALK], "Movement kept stale kick playback");

    fighter_enter_state(&battle, fighter, S_KICK);
    advance_ticks(&battle, 5, NULL);
    fighter_enter_state(&battle, fighter, S_HIT);
    check(fighter->current_move->sequence == NULL, "Reaction kept interrupted kick sequence");
    advance_ticks(&battle, 30, NULL);
    check(fighter->state == S_IDLE, "Interrupted kick completion overwrote reaction");

    battle = fight(1.f);
    fighter = &battle.fighters[player];
    in[0] = (Input){0};
    in[1] = (Input){0};
    in[player].held = BUTTON_BIT(BUTTON_HIGH_KICK);
    battle_step(&battle, in);
    advance_ticks(&battle, 11, NULL);
    check(battle.fighters[1 - player].health == Q16_ONE, "Kick contact started before retail query");
    advance_ticks(&battle, 1, NULL);
    check(battle.fighters[1 - player].health == Q16_ONE - (player == 0 ? 0x2147 : 12 * Q16_ONE / 100), "Kick missed its first retail contact step");
    advance_ticks(&battle, 2, NULL);
    check(battle.fighters[1 - player].health == Q16_ONE - (player == 0 ? 0x2147 : 12 * Q16_ONE / 100), "Kick connected more than once");
  }
}

static void sequence_event(Battle *battle, Fighter *fighter) {
  (void)battle;
  fighter->health--;
}

static void check_original_scorpion(void) {
  struct ScorpionFixture {
    const char *name;
    int state, result, contact_tick, press_count, presses[8], facing_away, count;
    int clips[256], frames[256];
  };
  static const struct ScorpionFixture fixtures[] = {
#include "fixtures/original_scorpion.inc"
  };
  for (size_t i = 0; i < sizeof fixtures / sizeof fixtures[0]; ++i) {
    const struct ScorpionFixture *expected = &fixtures[i];
    Battle battle = fight(4.f);
    Fighter *fighter = &battle.fighters[0];
    if (expected->facing_away) {
      fighter->facing = -1;
    }
    fighter_enter_state(&battle, fighter, expected->state);
    for (int tick = 0; tick < expected->count; ++tick) {
      int clip = fighter_move(fighter)->clip;
      int count = clip == 294 ? 23 : clip == 295 ? 19 : clip == 293 ? 20 : clip == 63 ? 46 : 18;
      if (clip != expected->clips[tick] || fighter_frame(fighter, count) != expected->frames[tick]) {
        fprintf(stderr, "%s at step %d\n", expected->name, tick);
        check(false, "Scorpion sample differs from original x86");
      }
      check(fighter->state == expected->state, "Scorpion move returned early");
      if (expected->result != 0 && tick == expected->contact_tick) {
        /* Match the oracle's controlled result. Collision is tested separately. */
        fighter_note_contact(fighter, (ContactResult)expected->result);
      }
      /* The oracle writes the LP latch before the pump; mirror that latch. */
      for (int press = 0; press < expected->press_count; ++press) {
        if (expected->presses[press] == tick + 1) {
          fighter->buffered |= BUTTON_BIT(BUTTON_LOW_PUNCH);
        }
      }
      battle_step(&battle, (Input[2]){0});
    }
    check(fighter->state == S_IDLE, "Scorpion move did not return at retail end step");
  }
}

static void sequence_end(Battle *battle, Fighter *fighter) {
  fighter_enter_state(battle, fighter, S_IDLE);
}

static void check_sequence_events(void) {
  static const MoveSegment segments[] = {
      {0, 0, 1, 1},
      {2, 1, 0, 1},
  };
  static const MoveEvent events[] = {
      {0, sequence_event},
      {2, sequence_event},
  };
  static const MoveSequence sequence = {segments, 2, events, 2, 3, sequence_end, NULL};
  static const MoveDef move = {.clip = 294, .flags = SF_LOCK, .sequence = &sequence};
  Battle battle = fight(4.f);
  Fighter *fighter = &battle.fighters[0];
  fighter_start_move(&battle, fighter, S_PUNCH, &move);
  check(fighter->health == Q16_ONE - 1, "Entry frame event missing");
  for (int i = 0; i < 5; ++i) {
    check(fighter_frame(fighter, 23) == 0, "Unexpected entry sample");
  }
  check(fighter->health == Q16_ONE - 1, "Rendering/sample reads repeated a frame event");
  advance_ticks(&battle, 1, NULL);
  check(fighter->health == Q16_ONE - 1, "Event fired on empty frame");
  advance_ticks(&battle, 1, NULL);
  check(fighter->health == Q16_ONE - 2, "Repeated sample lost its own frame event");
  advance_ticks(&battle, 1, NULL);
  check(fighter->state == S_IDLE, "Sequence did not transition through its end callback");
  advance_ticks(&battle, 5, NULL);
  check(fighter->health == Q16_ONE - 2, "Completed sequence kept firing events");
}

static void check_scorpion_contact_branches(void) {
  for (int blocked = 0; blocked < 2; ++blocked) {
    Battle battle = fight(1.f);
    Fighter *fighter = &battle.fighters[0];
    fighter_enter_state(&battle, fighter, S_UPPERCUT);
    Input in[2] = {0};
    if (blocked) {
      in[1].held = BUTTON_BIT(BUTTON_BLOCK);
    }
    advance_ticks(&battle, 7, in);
    check(fighter->contact == (blocked ? CONTACT_BLOCK : CONTACT_HIT) && fighter->contact_tick == 7,
        "Battle did not deliver uppercut contact to its continuation");
    int end = blocked ? 31 : 61;
    advance_ticks(&battle, end - 8, in);
    check(fighter->state == S_UPPERCUT, "Contact recovery ended before retail return");
    advance_ticks(&battle, 1, in);
    check(fighter->state == S_IDLE, "Contact recovery did not return on retail step");
  }
  {
    Battle battle = fight(4.f);
    Fighter *fighter = &battle.fighters[0];
    fighter_enter_state(&battle, fighter, S_PUNCH);
    advance_ticks(&battle, 7, NULL);
    battle_step(&battle, (Input[2]){{.held = BUTTON_BIT(BUTTON_LOW_PUNCH)}, {0}});
    battle_step(&battle, (Input[2]){0});
    check(fighter_move(fighter)->clip == 295, "Jab repeat failed at its native branch");
    fighter_enter_state(&battle, fighter, S_HIT);
    advance_ticks(&battle, 30, NULL);
    check(fighter->state == S_IDLE && fighter->pending == NULL && fighter->buffered == 0, "Interrupted jab retained a continuation or buffered repeat");
  }
}

/* docs/scorpion/input-and-commands.md: two-sample press rule, history, latency. */
static void check_retail_input(void) {
  Battle battle = fight(4.f);
  Fighter *fighter = &battle.fighters[0];
  Input tap[2] = {{BUTTON_BIT(BUTTON_DOWN)}, {0}};
  battle_step(&battle, tap);
  check(fighter->directions.token[0] == BUTTON_DOWN + 1 && fighter->directions.tick[0] == battle.ticks, "Direction press not stamped");
  battle_step(&battle, (Input[2]){0});
  battle_step(&battle, tap);
  check(fighter->directions.token[1] == 0, "A one-tick release must not repeat a press");
  battle_step(&battle, (Input[2]){0});
  battle_step(&battle, (Input[2]){0});
  battle_step(&battle, tap);
  check(fighter->directions.token[1] == BUTTON_DOWN + 1, "A two-tick release must allow a new press");
  battle_step(&battle, (Input[2]){0});
  battle_step(&battle, (Input[2]){0});
  battle_step(&battle, (Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH) | BUTTON_BIT(BUTTON_HIGH_KICK)}, {0}});
  check(fighter->latch[BUTTON_HIGH_PUNCH] == battle.ticks && fighter->latch[BUTTON_HIGH_KICK] == 0, "Only HP, LP, BLK, LK and RUN latch presses");
  check(fighter->pending != NULL && fighter->buttons == NULL && (fighter->flags & FF_BUSY), "First handled press must lock the fighter");
  battle_step(&battle, (Input[2]){0});
  check(fighter->state == S_PUNCH && fighter->ticks == 0, "Interrupted entry must start on the next tick");
}

static void command(Battle *battle, int player, Button first, Button second, Button attack) {
  Input input[2] = {0};
  input[player].held = BUTTON_BIT(first);
  battle_step(battle, input);
  advance_ticks(battle, 2, NULL);
  input[player].held = BUTTON_BIT(second);
  battle_step(battle, input);
  advance_ticks(battle, 2, NULL);
  input[player].held = BUTTON_BIT(attack);
  battle_step(battle, input);
  battle_step(battle, (Input[2]){0});
}

static void check_specials(void) {
  for (int player = 0; player < 2; ++player) {
    Battle battle = fight(4.f);
    if (player == 1) {
      battle_init(&battle, 4, 0);
      battle.phase = PHASE_FIGHT;
      battle.fighters[0].body.pos[AXIS_X] = -2 * Q16_ONE;
      battle.fighters[1].body.pos[AXIS_X] = 2 * Q16_ONE;
    }
    Fighter *fighter = &battle.fighters[player], *victim = &battle.fighters[1 - player];
    command(&battle, player, player ? BUTTON_RIGHT : BUTTON_LEFT, player ? BUTTON_RIGHT : BUTTON_LEFT, BUTTON_LOW_PUNCH);
    check(fighter->current_move->clip == 14 && fighter->script != NULL, "B,B,LP must enter spear for either player");
    advance_ticks(&battle, 45, NULL);
    check(victim->health == Q16_ONE - 0xccc && fighter_distance(&battle, fighter) < 0.9f, "Spear must damage once and pull into follow-up range");
    check(victim->action == 0x1008 && victim->damage_scale == 0x9999, "Spear must leave scaled combo stun");
    advance_ticks(&battle, 100, NULL);
    check(fighter->state == S_IDLE && victim->state == S_IDLE, "Spear must release both fighters");
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      check(battle.projectiles[i].kind == 0, "Spear leaked a projectile");
    }
  }
  {
    Battle battle = fight(4.f);
    command(&battle, 0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
    Input guard[2] = {{0}, {BUTTON_BIT(BUTTON_BLOCK)}};
    advance_ticks(&battle, 60, guard);
    check(battle.fighters[1].health == Q16_ONE - q16_mul(0xccc, 0x3333), "Blocked spear must only chip");
    check(fighter_distance(&battle, &battle.fighters[0]) > 3.f, "Blocked spear must not pull");
  }
  {
    Battle battle = fight(4.f);
    command(&battle, 0, BUTTON_LEFT, BUTTON_LEFT, BUTTON_LOW_PUNCH);
    advance_ticks(&battle, 27, NULL);
    fighter_enter_state(&battle, &battle.fighters[0], S_HIT);
    advance_ticks(&battle, 90, NULL);
    check(battle.fighters[1].state == S_IDLE, "Interrupted spear must release the victim");
  }
  for (int guard = 0; guard < 2; ++guard) {
    Battle battle = fight(2.f);
    command(&battle, 0, BUTTON_DOWN, BUTTON_RIGHT, BUTTON_LOW_PUNCH);
    check(battle.fighters[0].current_move->clip == 448, "D,F,LP must enter fire");
    Input input[2] = {{0}, {guard ? BUTTON_BIT(BUTTON_BLOCK) : 0}};
    advance_ticks(&battle, 60, input);
    check(battle.fighters[1].health == Q16_ONE - 0x1eb8, "Fire must deal one unblockable 12-percent hit");
    check(battle.fighters[0].state == S_IDLE, "Fire must recover");
  }
  for (int guard = 0; guard < 2; ++guard) {
    Battle battle = fight(4.f);
    command(&battle, 0, BUTTON_DOWN, BUTTON_LEFT, BUTTON_HIGH_PUNCH);
    check(battle.fighters[0].current_move->clip == 312, "D,B,HP must enter teleport");
    Input input[2] = {{0}, {guard ? BUTTON_BIT(BUTTON_BLOCK) : 0}};
    advance_ticks(&battle, 12, input);
    check(fighter_x(&battle.fighters[0]) > fighter_x(&battle.fighters[1]), "Teleport must cross to the far side");
    advance_ticks(&battle, 100, input);
    check(battle.fighters[0].state == S_IDLE && !(battle.fighters[0].flags & FF_TELEPORTING), "Teleport must land and clear its state");
    check(battle.fighters[1].health == Q16_ONE - (guard ? q16_mul(0x170a, 0x3333) : 0x170a), "Teleport hit/block damage");
  }
}

static void draw_sword(Battle *battle) {
  command(battle, 0, BUTTON_RIGHT, BUTTON_RIGHT, BUTTON_HIGH_KICK);
  check(battle->fighters[0].current_move->clip == 244, "F,F,HK must draw the sword");
  advance_ticks(battle, 21, NULL);
  check(!battle->fighters[0].armed, "Sword equipped before the script create event");
  advance_ticks(battle, 27, NULL);
  check(battle->fighters[0].armed && battle->fighters[0].current_move->clip == 286, "Draw must equip and return to armed stance");
}

static void check_weapon(void) {
  for (int guard = 0; guard < 2; ++guard) {
    Battle battle = fight(1.3f);
    draw_sword(&battle);
    Input in[2] = {{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {guard ? BUTTON_BIT(BUTTON_BLOCK) : 0}};
    battle_step(&battle, in);
    in[0].held = 0;
    advance_ticks(&battle, 45, in);
    check(battle.fighters[1].health == Q16_ONE - (guard ? q16_mul(0x3333, 0x3333) : 0x3333), "Sword slash damage/chip must apply once");
    check(battle.fighters[0].armed && battle.fighters[0].state == S_IDLE, "Slash must retain sword and recover");
  }
  {
    Battle battle = fight(4.f);
    draw_sword(&battle);
    command(&battle, 0, BUTTON_RIGHT, BUTTON_RIGHT, BUTTON_HIGH_KICK);
    check(battle.fighters[0].current_move->clip == 208, "Drawing while armed must throw");
    advance_ticks(&battle, 120, NULL);
    check(!battle.fighters[0].armed && battle.fighters[0].weapon_used, "Throw must detach and prevent a new sword");
    check(battle.fighters[1].health == Q16_ONE - 0x4000, "Thrown sword damage must be 25 percent");
    int resting = -1;
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      if (battle.projectiles[i].kind == PROJECTILE_SWORD && battle.projectiles[i].phase == 2) {
        resting = i;
      }
    }
    check(resting >= 0, "Connected thrown sword must fall and become pickable");
    battle.fighters[0].body.pos[AXIS_X] = battle.projectiles[resting].pos[AXIS_X] - Q16_ONE / 2;
    battle_step(&battle, (Input[2]){{BUTTON_BIT(BUTTON_DOWN)}, {0}});
    battle_step(&battle, (Input[2]){{BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_RUN)}, {0}});
    battle_step(&battle, (Input[2]){{BUTTON_BIT(BUTTON_DOWN)}, {0}});
    check(battle.fighters[0].armed && battle.projectiles[resting].kind == 0, "Crouch+RUN must attach a nearby resting sword");
    advance_ticks(&battle, 20, NULL);
    battle_react(&battle, &battle.fighters[0], 34);
    check(!battle.fighters[0].armed, "Launch reaction must knock out the held sword");
    advance_ticks(&battle, 100, NULL);
    battle_next_round(&battle);
    check(!battle.fighters[0].armed && !battle.fighters[0].weapon_used, "New round must reset weapon availability");
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
      check(battle.projectiles[i].kind == 0, "Round reset must remove world weapons");
    }
  }
  {
    Battle battle = fight(4.f);
    draw_sword(&battle);
    battle_step(&battle, (Input[2]){{BUTTON_BIT(BUTTON_DOWN)}, {0}});
    battle_step(&battle, (Input[2]){{BUTTON_BIT(BUTTON_DOWN) | BUTTON_BIT(BUTTON_LOW_PUNCH)}, {0}});
    Input hold[2] = {{BUTTON_BIT(BUTTON_LOW_PUNCH)}, {0}};
    advance_ticks(&battle, 20, hold);
    check(battle.fighters[0].current_move->clip == 224, "Crouching armed LP must enter held spin");
    advance_ticks(&battle, 2, NULL);
    check(battle.fighters[0].state == S_IDLE, "Releasing LP must end spin");
  }
}

static void check_air_attacks(void) {
  const Button button[] = {BUTTON_HIGH_PUNCH, BUTTON_LOW_PUNCH, BUTTON_HIGH_KICK, BUTTON_HIGH_KICK, BUTTON_LOW_KICK};
  const int clip[] = {97, 96, 327, 55, 58};
  for (int i = 0; i < 5; ++i) {
    Battle battle = fight(8.f);
    Fighter *fighter = &battle.fighters[0];
    battle_step(&battle, (Input[2]){{BUTTON_BIT(BUTTON_UP) | (i >= 3 ? BUTTON_BIT(BUTTON_RIGHT) : 0)}, {0}});
    advance_ticks(&battle, 5, NULL);
    battle_step(&battle, (Input[2]){{BUTTON_BIT(button[i])}, {0}});
    advance_ticks(&battle, 1, NULL);
    check(fighter_move(fighter)->clip == clip[i] && fighter_sample(fighter) == 0, "Air button must select its own attack script");
    bool landing = false;
    for (int tick = 0; tick < 100 && fighter->state != S_IDLE; ++tick) {
      battle_step(&battle, (Input[2]){0});
      landing |= fighter_move(fighter)->clip == 304 && fighter_sample(fighter) >= 18;
    }
    check(landing && fighter->state == S_IDLE && !fighter_airborne(fighter), "Air miss must play landing and restore controls");
    check(battle.fighters[1].health == Q16_ONE, "Distant air attack must miss");
  }
  for (int followup = 0; followup < 3; ++followup) {
    Battle battle = fight(1.f);
    Fighter *fighter = &battle.fighters[0];
    fighter_enter_state(&battle, fighter, S_JUMP);
    fighter->body.pos[AXIS_Y] = fighter->body.ground - 0x14ccc;
    fighter->body.gravity = 1;
    Fighter *victim = &battle.fighters[1];
    fighter_enter_state(&battle, victim, S_JUMP);
    victim->body.pos[AXIS_Y] = victim->body.ground - 0x14ccc;
    victim->body.gravity = 1;
    battle_step(&battle, (Input[2]){{BUTTON_BIT(BUTTON_HIGH_PUNCH)}, {0}});
    advance_ticks(&battle, 6, NULL);
    check(battle.fighters[1].health == Q16_ONE, "Jump HP must not hit before its strike hold");
    advance_ticks(&battle, 1, NULL);
    check(battle.fighters[1].health == Q16_ONE - 5898 && fighter->body.gravity == 0 && fighter->buttons != NULL,
        "High jump HP contact must deal nine percent and open its frozen follow-up");
    q16 y = fighter->body.pos[AXIS_Y];
    Button next = followup == 0 ? BUTTON_HIGH_PUNCH : followup == 1 ? BUTTON_LOW_PUNCH : BUTTON_HIGH_KICK;
    battle_step(&battle, (Input[2]){{BUTTON_BIT(next)}, {0}});
    advance_ticks(&battle, 1, NULL);
    check(fighter->body.pos[AXIS_Y] == y, "Air follow-up must preserve the contact freeze");
    check(fighter_move(fighter)->clip == (followup == 0 ? 97 : followup == 1 ? 96 : 327), "Jump HP allows LP or kick, but cannot repeat HP");
    advance_ticks(&battle, 110, NULL);
    check(fighter->state == S_IDLE && !fighter_airborne(fighter), "Air contact/follow-up must eventually fall and recover");
  }
}

int main(void) {
  check_air_attacks();
  check_weapon();
  check_specials();
  check_retail_input();
  check_roster();
  check_original_kicks();
  check_original_scorpion();
  check_sequence_events();
  check_scorpion_contact_branches();
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
    check(battle.fighters[1].health == Q16_ONE - 0xa3d, "Punch must connect once");
    advance_ticks(&battle, 30, NULL);
    check(battle.fighters[1].health == Q16_ONE - 0xa3d, "Repeated hit during recovery");
  }
  {
    Battle battle = fight(3);
    do_attack(&battle, S_KICK, false, false);
    check(battle.fighters[1].health == Q16_ONE, "Out-of-range attack connects");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_PUNCH, true, false);
    check(battle.fighters[1].health == Q16_ONE - q16_mul(0xa3d, 0x3333) && battle.effects[0].blocked, "Blocking fails");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_SWEEP, true, false);
    check(battle.fighters[1].health == Q16_ONE - 0x23d7, "Standing guard should lose to sweep");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_SWEEP, true, true);
    check(battle.fighters[1].health == Q16_ONE - q16_mul(0x23d7, 0x3333), "Crouch guard fails against sweep");
  }
  {
    Battle battle = fight(0.6f);
    do_attack(&battle, S_THROW, true, false);
    check(battle.fighters[1].health == Q16_ONE - 14 * Q16_ONE / 100, "Throw should beat guard");
  }
  {
    Battle battle = fight(1.f);
    battle.fighters[1].body.pos[AXIS_Y] -= 2 * Q16_ONE;
    do_attack(&battle, S_PUNCH, false, false);
    check(battle.fighters[1].health == Q16_ONE, "Ground punch hits above its vertical reach");
  }
  {
    Battle battle = fight(1.f);
    do_attack(&battle, S_UPPERCUT, false, false);
    check(battle.fighters[1].health == Q16_ONE - 0x28f5 && battle.fighters[1].state == S_KNOCKDOWN && fighter_airborne(&battle.fighters[1]),
        "Uppercut does not launch");
    advance_ticks(&battle, 100, NULL);
    check(!fighter_airborne(&battle.fighters[1]) && battle.fighters[1].state == S_IDLE, "Knockdown never recovers");
  }
  {
    Battle battle = fight(1.f);
    Input in[2] = {0};
    /* Same recovered startup: Sub-Zero's jab still uses approximate timing. */
    battle.fighters[1].roster = 0;
    in[0].held = in[1].held = BUTTON_BIT(BUTTON_HIGH_PUNCH);
    battle_step(&battle, in);
    advance_ticks(&battle, 9, NULL);
    check(battle.fighters[0].health == Q16_ONE && battle.fighters[1].health == Q16_ONE - 0xa3d, "Script contacts use stable fighter order");
  }
  {
    Battle battle = fight(1.f);
    battle.fighters[1].health = 0xa3d;
    do_attack(&battle, S_PUNCH, false, false);
    check(battle.phase == PHASE_ROUND_OVER && battle.winner == 0 && battle.fighters[0].wins == 1, "KO/round award failed");
    advance_ticks(&battle, 180, NULL);
    check(battle.phase == PHASE_READY && battle.round == 2 && battle.fighters[1].health == Q16_ONE && battle.fighters[0].wins == 1, "Next round reset failed");
    battle.phase = PHASE_FIGHT;
    battle.fighters[0].body.pos[AXIS_X] = -Q16_ONE / 2;
    battle.fighters[1].body.pos[AXIS_X] = Q16_ONE / 2;
    battle.fighters[1].health = 0xa3d;
    do_attack(&battle, S_PUNCH, false, false);
    check(battle.phase == PHASE_MATCH_OVER && battle.fighters[0].wins == 2, "Best of three match failed");
    advance_ticks(&battle, 300, NULL);
    check(battle.phase == PHASE_MATCH_OVER, "Match result should wait for rematch");
    battle_reset_match(&battle);
    check(battle.round == 1 && battle.fighters[0].wins == 0, "Rematch failed");
  }
  {
    Battle battle = fight(1.f);
    battle.fighters[1].health = 0x28f5;
    do_attack(&battle, S_UPPERCUT, false, false);
    check(battle.phase == PHASE_ROUND_OVER, "Lethal uppercut must end the round");
    advance_ticks(&battle, 100, NULL);
    check(!fighter_airborne(&battle.fighters[1]) && fighter_sample(&battle.fighters[1]) == 109,
        "KO must continue the uppercut landing sequence and hold the final pose");
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
    check(battle.winner == -1 && battle.fighters[0].wins == 0 && battle.fighters[1].wins == 0, "Draw should not award wins");
  }
  {
    Battle battle = fight(1.f);
    Input in[2] = {0};
    in[0].held = BUTTON_BIT(BUTTON_UP);
    battle_step(&battle, in);
    advance_ticks(&battle, 90, NULL);
    check(!fighter_airborne(&battle.fighters[0]) && battle.fighters[0].body.vel[AXIS_Y] == 0, "Jump never lands");
  }
  {
    Battle battle = fight(1.f);
    Input in[2] = {0};
    int i;
    in[0].held = BUTTON_BIT(BUTTON_RIGHT);
    in[1].held = BUTTON_BIT(BUTTON_LEFT);
    advance_ticks(&battle, 600, in);
    check(fabsf(fighter_x(&battle.fighters[0]) - fighter_x(&battle.fighters[1])) >= 0.499f, "Bodies overlap");
    in[0].held = BUTTON_BIT(BUTTON_LEFT);
    advance_ticks(&battle, 600, in);
    for (i = 0; i < 2; ++i)
      check(fabsf(fighter_x(&battle.fighters[i])) <= q16_float(battle.arena_radius), "Arena wall violated");
  }
  {
    Battle battle = fight(1.f);
    int changes = 0, old = 2 * Q16_ONE, i;
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
    check(changes > 4 && battle.phase == PHASE_MATCH_OVER, "CPU match does not progress to completion");
  }
  puts(
      "PASS: air attack selection/miss/landing/contact/follow-ups; sword draw/slash/chip/throw/drop/pickup/spin/reset; Scorpion special commands/pull/stun/chip/interruption/teleport/fire; retail press rule/history/latency; 27 original Scorpion branches; original kicks; move callbacks/events/interruption; roster tables; countdown; range; single hits; block; sweep; throw; air reach; launch; ordered contacts; KO; rounds; timeout; draw; rematch; jump; walls; CPU match");
  return 0;
}
