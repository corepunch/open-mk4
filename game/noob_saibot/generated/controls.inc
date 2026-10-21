/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter = {
                                              [CPU_SPECIAL_NEAR] = noob_saibot_teleport,
                                              [CPU_SPECIAL_1] = noob_saibot_teleport,
                                              [CPU_SPECIAL_2] = noob_saibot_flip_kick,
                                              [CPU_PROJECTILE] = noob_saibot_star,
                                              [CPU_FINISH_FAN] = noob_saibot_fan_fatality,
                                              [CPU_FINISH_SPIKE] = noob_saibot_spike_fatality,
                                          }};
