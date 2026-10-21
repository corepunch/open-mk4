/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_SPECIAL_NEAR] = kai_rising_skull,
                                                  [CPU_SPECIAL_1] = kai_ground_skull,
                                                  [CPU_PROJECTILE] = kai_rising_skull,
                                                  [CPU_FINISH_1] = kai_fatality_1,
                                                  [CPU_FINISH_2] = kai_fatality_2,
                                                  [CPU_FINISH_FAN] = kai_fan_fatality,
                                                  [CPU_FINISH_SPIKE] = kai_spike_fatality,
                                              },
    .finish_min = {0x0, 0x0},
    .finish_distance = {0xcccc, 0x1cccc}};
