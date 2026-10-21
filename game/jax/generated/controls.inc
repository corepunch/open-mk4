/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter =
                                              {
                                                  [CPU_SPECIAL_NEAR] = jax_dash_punch,
                                                  [CPU_SPECIAL_1] = jax_ground_pound,
                                                  [CPU_SPECIAL_2] = jax_dash_punch,
                                                  [CPU_PROJECTILE] = jax_missile,
                                                  [CPU_FINISH_1] = jax_fatality_1,
                                                  [CPU_FINISH_2] = jax_fatality_2,
                                                  [CPU_FINISH_FAN] = jax_command_fan,
                                                  [CPU_FINISH_SPIKE] = jax_command_spike,
                                              },
    .finish_min = {0x0, 0x0},
    .finish_distance = {0xcccc, 0x10000}};
