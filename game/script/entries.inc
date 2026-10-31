extern void action_handstand_front(Fighter *fighter);
extern void action_handstand_rear(Fighter *fighter);
extern void action_handstand_spin(Fighter *fighter);
extern void action_handstand_exit(Fighter *fighter);
/* One native implementation per local EXE address. Command patterns,
 * priorities, flags and dispatch targets are read from retail. */
extern void cage_cpu_kick(Fighter *fighter);
extern void cage_fatality_1(Fighter *fighter);
extern void cage_fatality_2(Fighter *fighter);
extern void cage_groin_punch(Fighter *fighter);
extern void cage_high_orb(Fighter *fighter);
extern void cage_low_orb(Fighter *fighter);
extern void cage_powered_punch(Fighter *fighter);
extern void fujin_dive(Fighter *fighter);
extern void fujin_first_fatality(Fighter *fighter);
extern void fujin_second_fatality(Fighter *fighter);
extern void fujin_triple_kick(Fighter *fighter);
extern void fujin_wind(Fighter *fighter);
extern void fujin_wind_grab(Fighter *fighter);
extern void fujin_wind_slam(Fighter *fighter);
extern void goro_fireball(Fighter *fighter);
extern void goro_leap_stomp(Fighter *fighter);
extern void goro_quake_stomp(Fighter *fighter);
extern void goro_special_kick(Fighter *fighter);
extern void goro_special_punch(Fighter *fighter);
extern void goro_special_uppercut(Fighter *fighter);
extern void jarek_blade(Fighter *fighter);
extern void jarek_cannonball(Fighter *fighter);
extern void jarek_fatality_1(Fighter *fighter);
extern void jarek_fatality_2(Fighter *fighter);
extern void jarek_quake(Fighter *fighter);
extern void jarek_upward_ball(Fighter *fighter);
extern void jax_air_throw(Fighter *fighter);
extern void jax_dash_punch(Fighter *fighter);
extern void jax_fatality_1(Fighter *fighter);
extern void jax_fatality_2(Fighter *fighter);
extern void jax_ground_pound(Fighter *fighter);
extern void jax_missile(Fighter *fighter);
extern void kai_fatality_1(Fighter *fighter);
extern void kai_fatality_2(Fighter *fighter);
extern void kai_ground_skull(Fighter *fighter);
extern void kai_lunge_kick(Fighter *fighter);
extern void kai_aerial_punch(Fighter *fighter);
extern void kai_handstand_enter(Fighter *fighter);
extern void kai_rising_skull(Fighter *fighter);
extern void sonya_bicycle(Fighter *fighter);
extern void kitana_air_throw(Fighter *fighter);
extern void sonya_square_wave(Fighter *fighter);
extern void sub_zero_air_throw(Fighter *fighter);
extern void sonya_cartwheel(Fighter *fighter);
extern void sonya_leg_grab(Fighter *fighter);
extern void sonya_ring(Fighter *fighter);
extern void reiko_flip_kick(Fighter *fighter);
extern void reiko_star(Fighter *fighter);
extern void liu_kang_bicycle_kick(Fighter *fighter);
extern void liu_kang_first_fatality(Fighter *fighter);
extern void liu_kang_flying_kick(Fighter *fighter);
extern void liu_kang_high_fireball(Fighter *fighter);
extern void liu_kang_low_fireball(Fighter *fighter);
extern void liu_kang_second_fatality(Fighter *fighter);
extern void mk4_sidestep(Fighter *fighter);
extern void noob_saibot_air_throw(Fighter *fighter);
extern void noob_saibot_teleport(Fighter *fighter);
extern void quan_chi_air_throw(Fighter *fighter);
extern void quan_chi_first_fatality(Fighter *fighter);
extern void quan_chi_flip_kick(Fighter *fighter);
extern void quan_chi_second_fatality(Fighter *fighter);
extern void quan_chi_skull(Fighter *fighter);
extern void quan_chi_teleport_stomp(Fighter *fighter);
extern void quan_chi_weapon_steal(Fighter *fighter);
extern void raiden_first_fatality(Fighter *fighter);
extern void raiden_lightning(Fighter *fighter);
extern void raiden_second_fatality(Fighter *fighter);
extern void raiden_teleport(Fighter *fighter);
extern void raiden_torpedo(Fighter *fighter);
extern void reiko_first_fatality(Fighter *fighter);
extern void reiko_orbit(Fighter *fighter);
extern void reiko_second_fatality(Fighter *fighter);
extern void reiko_teleport(Fighter *fighter);
extern void reptile_acid_spit(Fighter *fighter);
extern void reptile_fatality_1(Fighter *fighter);
extern void reptile_fatality_2(Fighter *fighter);
extern void reptile_dash(Fighter *fighter);
extern void reptile_roll(Fighter *fighter);
extern void reptile_invisibility(Fighter *fighter);
extern void scorpion_air_throw(Fighter *fighter);
extern void scorpion_beast_fatality(Fighter *fighter);
extern void scorpion_burn_fatality(Fighter *fighter);
extern void scorpion_fire(Fighter *fighter);
extern void scorpion_spear(Fighter *fighter);
extern void scorpion_teleport(Fighter *fighter);
extern void shinnok_fatality_1(Fighter *fighter);
extern void shinnok_fatality_2(Fighter *fighter);
extern void shinnok_morph_cage(Fighter *fighter);
extern void shinnok_morph_fujin(Fighter *fighter);
extern void shinnok_morph_jarek(Fighter *fighter);
extern void shinnok_morph_jax(Fighter *fighter);
extern void shinnok_morph_kai(Fighter *fighter);
extern void shinnok_morph_liu_kang(Fighter *fighter);
extern void shinnok_morph_quan_chi(Fighter *fighter);
extern void shinnok_morph_raiden(Fighter *fighter);
extern void shinnok_morph_reiko(Fighter *fighter);
extern void shinnok_morph_reptile(Fighter *fighter);
extern void shinnok_morph_scorpion(Fighter *fighter);
extern void shinnok_morph_sonya(Fighter *fighter);
extern void shinnok_morph_sub_zero(Fighter *fighter);
extern void shinnok_morph_tanya(Fighter *fighter);
extern void sonya_air_throw(Fighter *fighter);
extern void sonya_first_fatality(Fighter *fighter);
extern void sonya_second_fatality(Fighter *fighter);
extern void sub_zero_clone(Fighter *fighter);
extern void sub_zero_first_fatality(Fighter *fighter);
extern void sub_zero_ice(Fighter *fighter);
extern void sub_zero_second_fatality(Fighter *fighter);
extern void sub_zero_slide(Fighter *fighter);
extern void tanya_drill_kick(Fighter *fighter);
extern void tanya_fatality_1(Fighter *fighter);
extern void tanya_fatality_2(Fighter *fighter);
extern void tanya_spin_kick(Fighter *fighter);
extern void weapon_pickup(Fighter *fighter);
extern bool cage_can_groin_punch(const Fighter *fighter);
extern bool fujin_can_dive(const Fighter *fighter);
extern bool fujin_can_first_fatality(const Fighter *fighter);
extern bool fujin_can_wind(const Fighter *fighter);
extern bool fujin_can_wind_grab(const Fighter *fighter);
extern bool fujin_can_wind_slam(const Fighter *fighter);
extern bool jax_can_dash_punch(const Fighter *fighter);
extern bool jax_can_fatality_1(const Fighter *fighter);
extern bool jax_can_missile(const Fighter *fighter);
extern bool kai_can_first_fatality(const Fighter *fighter);
extern bool kai_can_ground_skull(const Fighter *fighter);
extern bool kai_can_handstand(const Fighter *fighter);
extern bool kai_can_rising_skull(const Fighter *fighter);
extern bool liu_kang_can_bicycle(const Fighter *fighter);
extern bool liu_kang_can_first_fatality(const Fighter *fighter);
extern bool mk4_can_air_throw(const Fighter *fighter);
extern bool mk4_can_fan_finish(const Fighter *fighter);
extern bool mk4_can_sidestep(const Fighter *fighter);
extern bool mk4_can_spike_finish(const Fighter *fighter);
extern bool quan_chi_can_first_fatality(const Fighter *fighter);
extern bool raiden_can_teleport(const Fighter *fighter);
extern bool reiko_can_first_fatality(const Fighter *fighter);
extern bool reiko_can_flip_kick(const Fighter *fighter);
extern bool reiko_can_orbit(const Fighter *fighter);
extern bool reiko_can_star(const Fighter *fighter);
extern bool reptile_can_dash(const Fighter *fighter);
extern bool reptile_can_first_fatality(const Fighter *fighter);
extern bool reptile_can_invisibility(const Fighter *fighter);
extern bool reptile_can_spit(const Fighter *fighter);
extern bool scorpion_can_spear(const Fighter *fighter);
extern bool scorpion_can_teleport(const Fighter *fighter);
extern bool shinnok_can_morph(const Fighter *fighter);
extern bool sonya_can_cartwheel(const Fighter *fighter);
extern bool sonya_can_first_fatality(const Fighter *fighter);
extern bool sonya_can_leg_grab(const Fighter *fighter);
extern bool sub_zero_can_clone(const Fighter *fighter);
extern bool sub_zero_can_first_fatality(const Fighter *fighter);
extern bool sub_zero_can_ice(const Fighter *fighter);
extern bool sub_zero_can_slide(const Fighter *fighter);
extern bool tanya_can_first_fatality(const Fighter *fighter);
extern bool tanya_can_spin_kick(const Fighter *fighter);
extern bool weapon_can_draw(const Fighter *fighter);
extern bool weapon_can_pickup(const Fighter *fighter);

// clang-format off: Keep table entries on one row, including long rows.
static const NativeAction entries[] = {
  {native_fireball, action_fireball},
  {native_air_fireball, action_air_fireball},
  {native_weapon_draw, action_weapon_draw},
  {native_teleport, action_teleport},
  {native_teleport_slam, action_teleport_slam},
  {native_handstand_spin, action_handstand_spin},
  {native_handstand_exit, action_handstand_exit},
  {native_handstand_front, action_handstand_front},
  {native_handstand_rear, action_handstand_rear},

  {native_air_throw, action_air_throw},
  {native_air_throw_lift, action_air_throw_lift},
  {native_air_throw_slam, action_air_throw_slam},
  {native_air_throw_flip, action_air_throw_flip},

  {native_air_high_punch, action_air_high_punch},
  {native_air_low_punch, action_air_low_punch},
  {native_air_kick, action_air_kick},
  {native_air_diagonal_high_kick, action_air_diagonal_high_kick},
  {native_air_diagonal_low_kick, action_air_diagonal_low_kick},

  {native_normal_low_punch, action_normal_low_punch},
  {native_normal_low_kick, action_normal_low_kick},
  {native_close_high_kick, action_close_high_kick},
  {native_close_high_punch, action_close_high_punch},
  {native_spike_finish, action_spike_finish},
  {native_cage_fatality_1, cage_fatality_1},
  {native_fujin_first_fatality, fujin_first_fatality},
  {native_jarek_fatality_1, jarek_fatality_1},
  {native_jax_fatality_1, jax_fatality_1},
  {native_kai_fatality_1, kai_fatality_1},
  {native_liu_kang_first_fatality, liu_kang_first_fatality},
  {native_quan_chi_first_fatality, quan_chi_first_fatality},
  {native_raiden_first_fatality, raiden_first_fatality},
  {native_reiko_first_fatality, reiko_first_fatality},
  {native_reptile_fatality_1, reptile_fatality_1},
  {native_scorpion_burn_fatality, scorpion_burn_fatality},
  {native_shinnok_fatality_1, shinnok_fatality_1},
  {native_sonya_first_fatality, sonya_first_fatality},
  {native_sub_zero_first_fatality, sub_zero_first_fatality},
  {native_tanya_fatality_1, tanya_fatality_1},
  {native_finish_first, action_finish_first},
  {native_cage_fatality_2, cage_fatality_2},
  {native_fujin_second_fatality, fujin_second_fatality},
  {native_jarek_fatality_2, jarek_fatality_2},
  {native_jax_fatality_2, jax_fatality_2},
  {native_kai_fatality_2, kai_fatality_2},
  {native_liu_kang_second_fatality, liu_kang_second_fatality},
  {native_quan_chi_second_fatality, quan_chi_second_fatality},
  {native_raiden_second_fatality, raiden_second_fatality},
  {native_reiko_second_fatality, reiko_second_fatality},
  {native_reptile_fatality_2, reptile_fatality_2},
  {native_scorpion_beast_fatality, scorpion_beast_fatality},
  {native_shinnok_fatality_2, shinnok_fatality_2},
  {native_sonya_second_fatality, sonya_second_fatality},
  {native_sub_zero_second_fatality, sub_zero_second_fatality},
  {native_tanya_fatality_2, tanya_fatality_2},
  {native_finish_second, action_finish_second},
  {native_roundhouse, action_roundhouse},
  {native_sweep, action_sweep},
  {native_high_punch, action_high_punch},
  {native_crouch_low_punch, action_crouch_low_punch},
  {native_crouch_high_kick, action_crouch_high_kick},
  {native_crouch_low_kick, action_crouch_low_kick},
  {native_lk_throw, action_lk_throw},
  {native_lp_throw, action_lp_throw},
  {native_high_kick, action_high_kick},
  {native_fan_finish, action_fan_finish},
  {native_sonya_cartwheel, sonya_cartwheel},
  {native_sonya_leg_grab, sonya_leg_grab},
  {native_sonya_ring, sonya_ring},
  {native_reiko_flip_kick, reiko_flip_kick},
  {native_reiko_star, reiko_star},
  {native_goro_special_punch, goro_special_punch},
  {native_goro_special_kick, goro_special_kick},
  {native_goro_leap_stomp, goro_leap_stomp},
  {native_goro_special_uppercut, goro_special_uppercut},
  {native_goro_quake_stomp, goro_quake_stomp},
  {native_scorpion_fire, scorpion_fire},
  {native_cage_groin_punch, cage_groin_punch},
  {native_jax_ground_pound, jax_ground_pound},
  {native_sidestep, mk4_sidestep},
  {native_reptile_invisibility, reptile_invisibility},
  {native_shinnok_morph_scorpion, shinnok_morph_scorpion},
  {native_shinnok_morph_raiden, shinnok_morph_raiden},
  {native_shinnok_morph_sonya, shinnok_morph_sonya},
  {native_shinnok_morph_liu_kang, shinnok_morph_liu_kang},
  {native_shinnok_morph_sub_zero, shinnok_morph_sub_zero},
  {native_shinnok_morph_fujin, shinnok_morph_fujin},
  {native_shinnok_morph_reiko, shinnok_morph_reiko},
  {native_shinnok_morph_jarek, shinnok_morph_jarek},
  {native_shinnok_morph_quan_chi, shinnok_morph_quan_chi},
  {native_shinnok_morph_tanya, shinnok_morph_tanya},
  {native_shinnok_morph_reptile, shinnok_morph_reptile},
  {native_shinnok_morph_jax, shinnok_morph_jax},
  {native_shinnok_morph_cage, shinnok_morph_cage},
  {native_shinnok_morph_kai, shinnok_morph_kai},
  {native_quan_chi_weapon_steal, quan_chi_weapon_steal},
  {native_weapon_pickup, weapon_pickup},
  {native_fujin_wind_slam, fujin_wind_slam},
  {native_fujin_wind, fujin_wind},
  {native_kai_lunge_kick_entry, kai_lunge_kick},
  {native_reiko_orbit, reiko_orbit},
  {native_tanya_spin_kick, tanya_spin_kick},
  {native_jarek_upward_ball, jarek_upward_ball},
  {native_cage_cpu_kick, cage_cpu_kick},
  {native_cage_powered_punch, cage_powered_punch},
  {native_quan_chi_flip_kick, quan_chi_flip_kick},
  {native_reptile_dash_entry, reptile_dash},
  {native_jax_dash_punch, jax_dash_punch},
  {native_reptile_roll_entry, reptile_roll},
  {native_jarek_cannonball, jarek_cannonball},
  {native_jarek_quake, jarek_quake},
  {native_tanya_drill_kick, tanya_drill_kick},
  {native_kai_aerial_punch_entry, kai_aerial_punch},
  {native_fujin_dive, fujin_dive},
  {native_kai_handstand_entry, kai_handstand_enter},
  {native_raiden_teleport, raiden_teleport},
  {native_quan_chi_teleport_stomp, quan_chi_teleport_stomp},
  {native_raiden_torpedo, raiden_torpedo},
  {native_sub_zero_slide, sub_zero_slide},
  {native_sonya_square_wave, sonya_square_wave},
  {native_sonya_bicycle, sonya_bicycle},
  {native_liu_kang_bicycle_kick, liu_kang_bicycle_kick},
  {native_liu_kang_flying_kick, liu_kang_flying_kick},
  {native_fujin_triple_kick, fujin_triple_kick},
  {native_sub_zero_clone, sub_zero_clone},
  {native_scorpion_teleport, scorpion_teleport},
  {native_fujin_wind_grab, fujin_wind_grab},
  {native_jax_missile, jax_missile},
  {native_cage_high_orb, cage_high_orb},
  {native_cage_low_orb, cage_low_orb},
  {native_jarek_blade, jarek_blade},
  {native_reptile_acid_spit, reptile_acid_spit},
  {native_kai_ground_skull, kai_ground_skull},
  {native_kai_rising_skull, kai_rising_skull},
  {native_quan_chi_skull, quan_chi_skull},
  {native_scorpion_spear, scorpion_spear},
  {native_sub_zero_ice, sub_zero_ice},
  {native_raiden_lightning, raiden_lightning},
  {native_liu_kang_low_fireball, liu_kang_low_fireball},
  {native_liu_kang_high_fireball, liu_kang_high_fireball},
  {native_goro_fireball, goro_fireball},
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const NativeGate gates[] = {
  {native_jax_can_fatality_1, jax_can_fatality_1},
  {native_kai_can_first_fatality, kai_can_first_fatality},
  {native_reptile_can_first_fatality, reptile_can_first_fatality},
  {native_tanya_can_first_fatality, tanya_can_first_fatality},
  {native_quan_chi_can_first_fatality, quan_chi_can_first_fatality},
  {native_reiko_can_first_fatality, reiko_can_first_fatality},
  {native_fujin_can_first_fatality, fujin_can_first_fatality},
  {native_liu_kang_can_first_fatality, liu_kang_can_first_fatality},
  {native_sub_zero_can_first_fatality, sub_zero_can_first_fatality},
  {native_sonya_can_first_fatality, sonya_can_first_fatality},
  {native_can_spike_finish, mk4_can_spike_finish},
  {native_can_finish_range, action_can_finish_range},
  {native_can_finish_close, action_can_finish_close},
  {native_can_fan_finish, mk4_can_fan_finish},
  {native_shinnok_can_morph, shinnok_can_morph},
  {native_reiko_can_star, reiko_can_star},
  {native_reiko_can_orbit, reiko_can_orbit},
  {native_reiko_can_flip_kick, reiko_can_flip_kick},
  {native_jax_can_missile, jax_can_missile},
  {native_can_sidestep, mk4_can_sidestep},
  {native_tanya_can_spin_kick, tanya_can_spin_kick},
  {native_jax_can_dash_punch, jax_can_dash_punch},
  {native_reptile_can_dash, reptile_can_dash},
  {native_sonya_can_cartwheel, sonya_can_cartwheel},
  {native_reptile_can_spit, reptile_can_spit},
  {native_raiden_can_teleport, raiden_can_teleport},
  {native_scorpion_can_spear, scorpion_can_spear},
  {native_liu_kang_can_bicycle, liu_kang_can_bicycle},
  {native_kai_can_ground_skull, kai_can_ground_skull},
  {native_kai_can_rising_skull, kai_can_rising_skull},
  {native_kai_can_handstand, kai_can_handstand},
  {native_fujin_can_wind_slam, fujin_can_wind_slam},
  {native_fujin_can_wind, fujin_can_wind},
  {native_fujin_can_wind_grab, fujin_can_wind_grab},
  {native_sub_zero_can_clone, sub_zero_can_clone},
  {native_jax_air_throw_command_gate, mk4_can_air_throw},
  {native_can_player_pair_air_throw, mk4_can_air_throw},
  {native_fujin_can_dive, fujin_can_dive},
  {native_has_gravity, action_has_gravity},
  {native_weapon_can_draw, weapon_can_draw},
  {native_weapon_can_pickup, weapon_can_pickup},
  {native_scorpion_can_teleport, scorpion_can_teleport},
  {native_cage_can_groin_punch, cage_can_groin_punch},
  {native_sonya_can_leg_grab, sonya_can_leg_grab},
  {native_sub_zero_can_ice, sub_zero_can_ice},
  {native_reptile_can_invisibility, reptile_can_invisibility},
  {native_sub_zero_can_slide, sub_zero_can_slide},
};
// clang-format on
