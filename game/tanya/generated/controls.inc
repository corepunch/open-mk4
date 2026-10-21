/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_SPECIAL_NEAR] = tanya_drill_kick,
                                                  [CPU_SPECIAL_1] = tanya_drill_kick,
                                                  [CPU_SPECIAL_2] = tanya_drill_kick,
                                                  [CPU_PROJECTILE] = tanya_fireball,
                                                  [CPU_FINISH_1] = tanya_fatality_1,
                                                  [CPU_FINISH_2] = tanya_fatality_2,
                                                  [CPU_FINISH_FAN] = tanya_fan_fatality,
                                                  [CPU_FINISH_SPIKE] = tanya_spike_fatality,
                                              },
    .finish_min = {0x0, 0x0},
    .finish_distance = {0xe666, 0xe666}};
