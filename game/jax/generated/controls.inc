/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_FINISH_1] = jax_fatality_1,
                                                  [CPU_FINISH_2] = jax_fatality_2,
                                                  [CPU_FINISH_FAN] = retail_fan_finish,
                                                  [CPU_FINISH_SPIKE] = jax_command_spike,
                                              },
    .finish_min = {0x0, 0x0}};
