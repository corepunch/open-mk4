/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_FINISH_1] = retail_finish_first,
                                                  [CPU_FINISH_2] = retail_finish_second,
                                                  [CPU_FINISH_FAN] = retail_fan_finish,
                                                  [CPU_FINISH_SPIKE] = fujin_entry_spike,
                                              },
    .finish_min = {0x1cccc, 0x1cccc}};
