#include "game.h"
#include "math_util.h"
#include <math.h>
#include <string.h>

static void finish_round(Battle *battle);

static void set_state(Fighter *fighter, int state) {
    if (fighter->state == state)
        return;
    fighter->state = state;
    fighter->ticks = 0;
    fighter->connected = false;
}

static bool locked(const Fighter *fighter) {
    return (fighter_state(fighter)->flags & STATE_F_LOCK) != 0;
}

static void place(Fighter *fighter, int roster_index, int wins, float x, int facing) {
    *fighter = (Fighter){0};
    fighter->roster = roster_index;
    fighter->wins = wins;
    fighter->health = 100;
    fighter->x = x;
    fighter->facing = facing;
    fighter->state = STATE_IDLE;
}

void battle_init(Battle *battle, int p1, int p2) {
    memset(battle, 0, sizeof *battle);
    battle->wall = 5.f;
    battle->fighters[0].roster = p1;
    battle->fighters[1].roster = p2;
    roster_def(p1);
    roster_def(p2);
    battle_reset_match(battle);
}

void battle_reset_match(Battle *battle) {
    int i;
    for (i = 0; i < 2; ++i)
        battle->fighters[i].wins = 0;
    battle->round = 0;
    battle->ticks = 0;
    battle_next_round(battle);
}

void battle_next_round(Battle *battle) {
    size_t i;
    battle->round++;
    for (i = 0; i < 2; ++i) {
        int roster_index = battle->fighters[i].roster;
        int wins = battle->fighters[i].wins;
        float x = i ? 2.f : -2.f;
        int facing = i ? -1 : 1;
        place(&battle->fighters[i], roster_index, wins, x, facing);
    }
    memset(battle->effects, 0, sizeof battle->effects);
    battle->phase = PHASE_READY;
    battle->phase_ticks = 0;
    battle->time_ticks = 99 * 60;
    battle->winner = -1;
}

static void finish_round(Battle *battle) {
    int i;
    if (battle->fighters[0].health == battle->fighters[1].health)
        battle->winner = -1;
    else if (battle->fighters[0].health > battle->fighters[1].health)
        battle->winner = 0;
    else
        battle->winner = 1;
    if (battle->winner >= 0) {
        battle->fighters[battle->winner].wins++;
        set_state(&battle->fighters[battle->winner], STATE_VICTORY);
    }
    for (i = 0; i < 2; ++i) {
        if (battle->fighters[i].health <= 0)
            set_state(&battle->fighters[i], STATE_KNOCKDOWN);
    }
    if (battle->winner >= 0 && battle->fighters[battle->winner].wins >= 2)
        battle->phase = PHASE_MATCH_OVER;
    else
        battle->phase = PHASE_ROUND_OVER;
    battle->phase_ticks = 0;
}

static bool command_matches(const Fighter *fighter, Input in, const CommandDef *command) {
    if (!input_pressed(in, command->button))
        return false;
    if ((command->when & CMD_GROUNDED) && fighter->height != 0.f)
        return false;
    if ((command->when & CMD_CROUCHING) && !fighter->crouching)
        return false;
    if ((command->when & CMD_STANDING) && fighter->crouching)
        return false;
    return true;
}

static void advance_fighter(Fighter *fighter, const Fighter *other, Input in) {
    const CharacterDef *def = roster_def(fighter->roster);
    const StateDef *state = fighter_state(fighter);
    const Motion *motion = def->motion;
    int dir;
    int c;

    fighter->ticks++;
    if (!locked(fighter) && fighter->height == 0.f)
        fighter->facing = fighter->x <= other->x ? 1 : -1;

    if (state->flags & STATE_F_ATTACK) {
        int total = state->startup + state->active + state->recovery;
        if (fighter->ticks >= total)
            set_state(fighter, fighter->height > 0.f ? STATE_JUMP : STATE_IDLE);
    } else if (state->flags & STATE_F_REACTION) {
        bool landed = !(state->flags & STATE_F_WAIT_LAND) || fighter->height == 0.f;
        if (fighter->ticks >= state->recovery && landed)
            set_state(fighter, STATE_IDLE);
    }

    if (fighter->height > 0.f || fighter->velocity > 0.f) {
        fighter->height += fighter->velocity * TICK_SECONDS;
        fighter->velocity -= motion->gravity * TICK_SECONDS;
        if (fighter->height <= 0.f) {
            fighter->height = 0.f;
            fighter->velocity = 0.f;
            if (fighter->state == STATE_JUMP)
                set_state(fighter, STATE_IDLE);
        }
    }
    if (locked(fighter))
        return;

    fighter->crouching = input_held(in, BUTTON_DOWN) && fighter->height == 0.f;
    if (input_held(in, BUTTON_BLOCK)) {
        set_state(fighter, STATE_BLOCK);
        return;
    }
    for (c = 0; c < def->command_count; ++c) {
        if (!command_matches(fighter, in, &def->commands[c]))
            continue;
        set_state(fighter, def->commands[c].state);
        return;
    }

    if (input_held(in, BUTTON_UP) && fighter->height == 0.f) {
        fighter->velocity = motion->jump_velocity;
        fighter->height = 0.01f;
        set_state(fighter, STATE_JUMP);
    }
    dir = (int)input_held(in, BUTTON_RIGHT) - (int)input_held(in, BUTTON_LEFT);
    if (!fighter->crouching) {
        float speed = input_held(in, BUTTON_RUN) ? motion->run : motion->walk;
        fighter->x += (float)dir * speed * TICK_SECONDS;
    }
    if (fighter->height > 0.f)
        set_state(fighter, STATE_JUMP);
    else if (fighter->crouching)
        set_state(fighter, STATE_CROUCH);
    else if (dir)
        set_state(fighter, STATE_WALK);
    else
        set_state(fighter, STATE_IDLE);
}

static void separate_bodies(Battle *battle) {
    Fighter *a = &battle->fighters[0];
    Fighter *b = &battle->fighters[1];
    float sign, mid;
    int i;
    for (i = 0; i < 2; ++i)
        battle->fighters[i].x = clampf(battle->fighters[i].x, -battle->wall, battle->wall);
    if (fabsf(a->height - b->height) >= 1.25f || fabsf(a->x - b->x) >= 0.7f)
        return;
    sign = a->x <= b->x ? 1.f : -1.f;
    mid = (a->x + b->x) * 0.5f;
    mid = clampf(mid, -battle->wall + 0.35f, battle->wall - 0.35f);
    a->x = mid - sign * 0.35f;
    b->x = mid + sign * 0.35f;
}

/* Both attacks are judged from the positions at the start of resolution, so
 * two moves that become active on the same tick can trade. */
static void resolve_hits(Battle *battle, const Fighter snapshot[2]) {
    size_t i;
    for (i = 0; i < 2; ++i) {
        const Fighter *attacker = &snapshot[i];
        const Fighter *defender = &snapshot[1 - i];
        const StateDef *move = fighter_state(attacker);
        Fighter *target;
        float distance;
        bool blocked;
        int damage;
        int reaction;

        if (!(move->flags & STATE_F_ATTACK) || attacker->connected)
            continue;
        if (attacker->ticks < move->startup || attacker->ticks >= move->startup + move->active)
            continue;
        distance = (defender->x - attacker->x) * (float)attacker->facing;
        if (distance < 0.f || distance > move->reach)
            continue;
        if (fabsf(attacker->height - defender->height) > 0.9f)
            continue;
        if ((move->flags & STATE_F_LOW) && defender->height > 0.2f)
            continue;
        if ((move->flags & STATE_F_UNBLOCKABLE)
            && (defender->height > 0.f || defender->state == STATE_KNOCKDOWN || defender->state == STATE_HIT))
            continue;
        if (defender->state == STATE_KNOCKDOWN)
            continue;

        battle->fighters[i].connected = true;
        blocked = defender->state == STATE_BLOCK
            && !(move->flags & STATE_F_UNBLOCKABLE)
            && (!(move->flags & STATE_F_LOW) || defender->crouching);
        target = &battle->fighters[1 - i];
        damage = blocked ? 0 : move->damage;
        target->health = (int)fmaxf(0.f, (float)target->health - (float)damage);
        target->x = clampf(
            target->x + (float)attacker->facing * move->push * (blocked ? 0.4f : 1.f),
            -battle->wall, battle->wall);
        if (!blocked) {
            reaction = (move->flags & STATE_F_LAUNCH) ? STATE_KNOCKDOWN : STATE_HIT;
            set_state(target, reaction);
            target->ticks = fighter_state(target)->recovery - move->stun;
            if (move->flags & STATE_F_LAUNCH) {
                target->velocity = 4.7f;
                target->height = fmaxf(0.01f, target->height);
            }
        }
        battle->effects[i] = (HitEffect){
            (attacker->x + defender->x) * 0.5f,
            defender->height + ((move->flags & STATE_F_LOW) ? 0.25f : 1.2f),
            10,
            blocked
        };
    }
}

void battle_step(Battle *battle, const Input input[2]) {
    int i;
    battle->ticks++;
    battle->phase_ticks++;
    for (i = 0; i < 2; ++i) {
        if (battle->effects[i].ticks > 0)
            battle->effects[i].ticks--;
    }
    if (battle->phase == PHASE_READY) {
        if (battle->phase_ticks >= 90) {
            battle->phase = PHASE_FIGHT;
            battle->phase_ticks = 0;
        }
        return;
    }
    if (battle->phase != PHASE_FIGHT) {
        for (i = 0; i < 2; ++i)
            battle->fighters[i].ticks++;
        if (battle->phase == PHASE_ROUND_OVER && battle->phase_ticks >= 180)
            battle_next_round(battle);
        return;
    }

    battle->time_ticks--;
    for (i = 0; i < 2; ++i)
        advance_fighter(&battle->fighters[i], &battle->fighters[1 - i], input[i]);
    separate_bodies(battle);
    {
        Fighter snapshot[2];
        memcpy(snapshot, battle->fighters, sizeof snapshot);
        resolve_hits(battle, snapshot);
    }
    if (battle->fighters[0].health <= 0 || battle->fighters[1].health <= 0 || battle->time_ticks <= 0)
        finish_round(battle);
}
