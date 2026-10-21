/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_FINISH_1] = reiko_first_fatality,
                                                  [CPU_FINISH_2] = reiko_second_fatality,
                                                  [CPU_FINISH_FAN] = reiko_fan_fatality,
                                                  [CPU_FINISH_SPIKE] = reiko_spike_fatality,
                                              },
    .finish_min = {0x0, 0x1cccc}};
