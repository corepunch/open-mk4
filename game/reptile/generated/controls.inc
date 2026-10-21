/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_SPECIAL_NEAR] = reptile_input_47a690,
                                                  [CPU_SPECIAL_1] = reptile_input_47a690,
                                                  [CPU_SPECIAL_2] = reptile_input_47afe0,
                                                  [CPU_PROJECTILE] = reptile_acid_spit,
                                                  [CPU_FINISH_1] = reptile_fatality_1,
                                                  [CPU_FINISH_2] = reptile_fatality_2,
                                                  [CPU_FINISH_FAN] = reptile_entry_fan,
                                                  [CPU_FINISH_SPIKE] = reptile_entry_spike,
                                              },
    .finish_min = {0x0, 0x1cccc},
    .finish_distance = {0xcccc, 0x23333}};
