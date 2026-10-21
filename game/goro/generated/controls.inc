/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter = {
                                              [CPU_FINISH_FAN] = retail_fan_finish,
                                              [CPU_FINISH_SPIKE] = goro_spike_fatality,
                                          }};
