/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_FINISH_1] = quan_chi_first_fatality,
                                                  [CPU_FINISH_2] = quan_chi_second_fatality,
                                                  [CPU_FINISH_FAN] = quan_chi_fan_fatality,
                                                  [CPU_FINISH_SPIKE] = quan_chi_spike_fatality,
                                              },
    .finish_min = {0x0, 0x0}};
