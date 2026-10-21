/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_SPECIAL_NEAR] = reiko_teleport,
                                                  [CPU_SPECIAL_1] = reiko_teleport,
                                                  [CPU_SPECIAL_2] = reiko_flip_kick,
                                                  [CPU_PROJECTILE] = reiko_star,
                                                  [CPU_FINISH_1] = reiko_first_fatality,
                                                  [CPU_FINISH_2] = reiko_second_fatality,
                                                  [CPU_FINISH_FAN] = reiko_fan_fatality,
                                                  [CPU_FINISH_SPIKE] = reiko_spike_fatality,
                                              },
    .finish_min = {0x0, 0x1cccc},
    .finish_distance = {0xcccc, 0x26666}};
