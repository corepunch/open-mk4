/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_SPECIAL_NEAR] = fujin_triple_kick,
                                                  [CPU_SPECIAL_1] = fujin_wind,
                                                  [CPU_SPECIAL_2] = fujin_triple_kick,
                                                  [CPU_FINISH_1] = fujin_first_fatality,
                                                  [CPU_FINISH_2] = fujin_second_fatality,
                                                  [CPU_FINISH_FAN] = fujin_entry_fan,
                                                  [CPU_FINISH_SPIKE] = fujin_entry_spike,
                                              },
    .finish_min = {0x1cccc, 0x1cccc},
    .finish_distance = {0x20000, 0x20000}};
