/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_SPECIAL_NEAR] = jarek_cannonball,
                                                  [CPU_SPECIAL_1] = jarek_cannonball,
                                                  [CPU_SPECIAL_2] = jarek_cannonball,
                                                  [CPU_PROJECTILE] = jarek_blade,
                                                  [CPU_FINISH_1] = jarek_fatality_1,
                                                  [CPU_FINISH_2] = jarek_fatality_2,
                                                  [CPU_FINISH_FAN] = jarek_fan_fatality,
                                                  [CPU_FINISH_SPIKE] = jarek_spike_fatality,
                                              },
    .finish_min = {0x0, 0x1cccc},
    .finish_distance = {0xcccc, 0x2cccc}};
