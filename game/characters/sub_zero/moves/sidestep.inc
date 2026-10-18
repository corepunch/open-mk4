/* 45e820: RUN latch is written after matching, so this sees the prior
 * press. 4692e0 uses shared descriptor 53ce18 (clip 300), not a roster
 * table. Down plays it forwards; otherwise it plays backwards. */

static bool can_sidestep(const Fighter *fighter) {
  return mk4_can_sidestep(fighter);
}

static void sub_zero_sidestep(Fighter *fighter) {
  mk4_sidestep(fighter);
}
