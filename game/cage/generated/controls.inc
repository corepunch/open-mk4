/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_FINISH_1] = cage_fatality_1,
                                                  [CPU_FINISH_2] = cage_fatality_2,
                                                  [CPU_FINISH_FAN] = cage_fan_fatality,
                                                  [CPU_FINISH_SPIKE] = cage_spike_fatality,
                                              },
    .finish_min = {0x0, 0x0}};
