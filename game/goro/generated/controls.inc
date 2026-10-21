/* Native CPU bindings; input records are loaded from retail at runtime. */

static const CpuMoves retail_cpu_moves = {.enter = {
                                              [CPU_SPECIAL_2] = goro_leap_stomp,
                                              [CPU_PROJECTILE] = goro_fireball,
                                              [CPU_FINISH_FAN] = goro_fan_fatality,
                                              [CPU_FINISH_SPIKE] = goro_spike_fatality,
                                          }};
