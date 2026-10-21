extern void retail_handstand_front(Fighter *fighter);
extern void retail_handstand_rear(Fighter *fighter);
extern void retail_handstand_spin(Fighter *fighter);
extern void retail_handstand_exit(Fighter *fighter);
/* Temporary native ABI bridge while the implementations are consolidated.
 * Command patterns, priorities, flags and dispatch targets are read from retail. */
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
extern void kai_input_478df0(Fighter *fighter);
extern void kai_input_47bd10(Fighter *fighter);
extern void kai_input_47c520(Fighter *fighter);
extern void kai_rising_skull(Fighter *fighter);
extern void kitana_bicycle(Fighter *fighter);
extern void kitana_command_shadow_low(Fighter *fighter);
extern void kitana_command_shadow_up(Fighter *fighter);
extern void kitana_air_throw(Fighter *fighter);
extern void kitana_square_wave(Fighter *fighter);
extern void sub_zero_air_throw(Fighter *fighter);
extern void kitana_cartwheel(Fighter *fighter);
extern void kitana_leg_grab(Fighter *fighter);
extern void kitana_ring(Fighter *fighter);
extern void noob_saibot_flip_kick(Fighter *fighter);
extern void noob_saibot_star(Fighter *fighter);
extern void liu_kang_bicycle_kick(Fighter *fighter);
extern void liu_kang_first_fatality(Fighter *fighter);
extern void liu_kang_flying_kick(Fighter *fighter);
extern void liu_kang_high_fireball(Fighter *fighter);
extern void liu_kang_low_fireball(Fighter *fighter);
extern void liu_kang_second_fatality(Fighter *fighter);
extern void mk4_sidestep(Fighter *fighter);
extern void noob_saibot_air_throw(Fighter *fighter);
extern void noob_saibot_command_498b70(Fighter *fighter);
extern void noob_saibot_command_498e70(Fighter *fighter);
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
extern void reiko_flip_kick(Fighter *fighter);
extern void reiko_orbit(Fighter *fighter);
extern void reiko_second_fatality(Fighter *fighter);
extern void reiko_star(Fighter *fighter);
extern void reiko_teleport(Fighter *fighter);
extern void reptile_acid_spit(Fighter *fighter);
extern void reptile_fatality_1(Fighter *fighter);
extern void reptile_fatality_2(Fighter *fighter);
extern void reptile_input_47a690(Fighter *fighter);
extern void reptile_input_47afe0(Fighter *fighter);
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
extern void sonya_bicycle(Fighter *fighter);
extern void sonya_cartwheel(Fighter *fighter);
extern void sonya_first_fatality(Fighter *fighter);
extern void sonya_leg_grab(Fighter *fighter);
extern void sonya_ring(Fighter *fighter);
extern void sonya_second_fatality(Fighter *fighter);
extern void sonya_square_wave(Fighter *fighter);
extern void sub_zero_clone(Fighter *fighter);
extern void sub_zero_first_fatality(Fighter *fighter);
extern void sub_zero_ice(Fighter *fighter);
extern void sub_zero_second_fatality(Fighter *fighter);
extern void sub_zero_slide(Fighter *fighter);
extern void tanya_air_fireball(Fighter *fighter);
extern void tanya_drill_kick(Fighter *fighter);
extern void tanya_fatality_1(Fighter *fighter);
extern void tanya_fatality_2(Fighter *fighter);
extern void tanya_fireball(Fighter *fighter);
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
  {0x46d1d0, 0x3ffff, retail_weapon_draw},
  {0x47c070, 0x3ffff, retail_teleport},
  {0x47c2a0, 0x3ffff, retail_teleport_slam},
  {0x47c850, 0x3ffff, retail_handstand_spin},
  {0x47ccf0, 0x3ffff, retail_handstand_exit},
  {0x47cc10, 0x3ffff, retail_handstand_front},
  {0x47cc80, 0x3ffff, retail_handstand_rear},

  {0x46aa80, 0x3ffff, retail_air_throw},
  {0x46a990, 0x3ffff, retail_air_throw_lift},
  {0x46aba0, 0x3ffff, retail_air_throw_slam},
  {0x46ae60, 0x3ffff, retail_air_throw_flip},

  {0x46e6c0, 0x3ffff, retail_air_high_punch},
  {0x46ee30, 0x3ffff, retail_air_low_punch},
  {0x46c7a0, 0x3ffff, retail_air_kick},
  {0x46e380, 0x3ffff, retail_air_diagonal_high_kick},
  {0x46df50, 0x3ffff, retail_air_diagonal_low_kick},

  {0x46dca0, 0x3ffff, retail_normal_low_punch},
  {0x46a7b0, 0x3ffff, retail_normal_low_kick},
  {0x46ba60, 0x3ffff, retail_close_high_kick},
  {0x46be40, 0x3ffff, retail_close_high_punch},
  {0x467e70, 0x3ffff, retail_spike_finish},
  {0x448160, 0x3ffff, cage_fatality_1},
  {0x44f4f0, 0x3ffff, fujin_first_fatality},
  {0x446d00, 0x3ffff, jarek_fatality_1},
  {0x4494c0, 0x3ffff, jax_fatality_1},
  {0x44ad70, 0x3ffff, kai_fatality_1},
  {0x450930, 0x3ffff, liu_kang_first_fatality},
  {0x44bcd0, 0x3ffff, quan_chi_first_fatality},
  {0x440fc0, 0x3ffff, raiden_first_fatality},
  {0x452730, 0x3ffff, reiko_first_fatality},
  {0x4539b0, 0x3ffff, reptile_fatality_1},
  {0x443840, 0x3ffff, scorpion_burn_fatality},
  {0x455000, 0x3ffff, shinnok_fatality_1},
  {0x43f420, 0x3ffff, sonya_first_fatality},
  {0x445100, 0x3ffff, sub_zero_first_fatality},
  {0x44d340, 0x3ffff, tanya_fatality_1},
  {0x467ce0, 0x3ffff, retail_finish_first},
  {0x465790, 0x3ffff, cage_fatality_2},
  {0x43bea0, 0x3ffff, fujin_second_fatality},
  {0x41df20, 0x3ffff, jarek_fatality_2},
  {0x4669c0, 0x3ffff, jax_fatality_2},
  {0x43bfe0, 0x3ffff, kai_fatality_2},
  {0x43c3d0, 0x3ffff, liu_kang_second_fatality},
  {0x43be30, 0x3ffff, quan_chi_second_fatality},
  {0x4647f0, 0x3ffff, raiden_second_fatality},
  {0x41c360, 0x3ffff, reiko_second_fatality},
  {0x43bf40, 0x3ffff, reptile_fatality_2},
  {0x41e960, 0x3ffff, scorpion_beast_fatality},
  {0x43c6d0, 0x3ffff, shinnok_fatality_2},
  {0x465cb0, 0x3ffff, sonya_second_fatality},
  {0x41b3a0, 0x3ffff, sub_zero_second_fatality},
  {0x464f30, 0x3ffff, tanya_fatality_2},
  {0x467cd0, 0x3ffff, retail_finish_second},
  {0x46f1f0, 0x3ffffu, retail_roundhouse},
  {0x46ef90, 0x3ffff, retail_sweep},
  {0x46d290, 0x3ffff, retail_high_punch},
  {0x46b7f0, 0x3ffff, retail_crouch_low_punch},
  {0x46b5a0, 0x3ffff, retail_crouch_high_kick},
  {0x46b710, 0x3ffff, retail_crouch_low_kick},
  {0x42b6e0, 0x3ffff, retail_lk_throw},
  {0x42c590, 0x3ffff, retail_lp_throw},
  {0x46a680, 0x3ffff, retail_high_kick},
  {0x467e00, 0x3ffff, retail_fan_finish},
  {0x47b690, 0x10000, kitana_cartwheel},
  {0x47dee0, 0x10000, kitana_leg_grab},
  {0x49b9a0, 0x10000, kitana_ring},
  {0x4791a0, 0x20000, noob_saibot_flip_kick},
  {0x496db0, 0x20000, noob_saibot_star},
  {0x43af90, 0x8000, goro_special_punch},
  {0x43b170, 0x8000, goro_special_kick},
  {0x43b320, 0x8000, goro_leap_stomp},
  {0x43b640, 0x8000, goro_special_uppercut},
  {0x43b7e0, 0x8000, goro_quake_stomp},
  {0x468610, 0x1, scorpion_fire},
  {0x468aa0, 0x4000, cage_groin_punch},
  {0x468f00, 0x2000, jax_ground_pound},
  {0x469250, 0x3ffff, mk4_sidestep},
  {0x4698a0, 0x400, reptile_invisibility},
  {0x469a00, 0x40, shinnok_morph_scorpion},
  {0x469a10, 0x40, shinnok_morph_raiden},
  {0x469a20, 0x40, shinnok_morph_sonya},
  {0x469a30, 0x40, shinnok_morph_liu_kang},
  {0x469a40, 0x40, shinnok_morph_sub_zero},
  {0x469a50, 0x40, shinnok_morph_fujin},
  {0x469a60, 0x40, shinnok_morph_reiko},
  {0x469a70, 0x40, shinnok_morph_jarek},
  {0x469a80, 0x40, shinnok_morph_quan_chi},
  {0x469a90, 0x40, shinnok_morph_tanya},
  {0x469aa0, 0x40, shinnok_morph_reptile},
  {0x469ab0, 0x40, shinnok_morph_jax},
  {0x469ac0, 0x40, shinnok_morph_cage},
  {0x469ad0, 0x40, shinnok_morph_kai},
  {0x469c80, 0x100, quan_chi_weapon_steal},
  {0x46a020, 0x3ffff, weapon_pickup},
  {0x46a5f0, 0x20, fujin_wind_slam},
  {0x46a620, 0x20, fujin_wind},
  {0x478df0, 0x800, kai_input_478df0},
  {0x478ee0, 0x80, reiko_orbit},
  {0x4791a0, 0x80, reiko_flip_kick},
  {0x479520, 0x200, tanya_spin_kick},
  {0x4797f0, 0x1000, jarek_upward_ball},
  {0x479b20, 0x4000, cage_cpu_kick},
  {0x479ec0, 0x4000, cage_powered_punch},
  {0x47a370, 0x100, quan_chi_flip_kick},
  {0x47a690, 0x400, reptile_input_47a690},
  {0x47ab10, 0x2000, jax_dash_punch},
  {0x47afe0, 0x400, reptile_input_47afe0},
  {0x47b310, 0x1000, jarek_cannonball},
  {0x47b5d0, 0x1000, jarek_quake},
  {0x47b690, 0x4, sonya_cartwheel},
  {0x47b9f0, 0x200, tanya_drill_kick},
  {0x47bd10, 0x800, kai_input_47bd10},
  {0x47bf60, 0x20, fujin_dive},
  {0x47c520, 0x800, kai_input_47c520},
  {0x47cd00, 0x2, raiden_teleport},
  {0x47cf30, 0x100, quan_chi_teleport_stomp},
  {0x47d5f0, 0x2, raiden_torpedo},
  {0x47dc00, 0x10, sub_zero_slide},
  {0x47dee0, 0x4, sonya_leg_grab},
  {0x47e180, 0x10000, kitana_square_wave},
  {0x47e180, 0x4, sonya_square_wave},
  {0x47e650, 0x10000, kitana_bicycle},
  {0x47e650, 0x4, sonya_bicycle},
  {0x47eaf0, 0x8, liu_kang_bicycle_kick},
  {0x47edc0, 0x8, liu_kang_flying_kick},
  {0x47f010, 0x20, fujin_triple_kick},
  {0x47f4d0, 0x10, sub_zero_clone},
  {0x47fc90, 0x1, scorpion_teleport},
  {0x480360, 0x20, fujin_wind_grab},
  {0x496db0, 0x80, reiko_star},
  {0x496f80, 0x2000, jax_missile},
  {0x497070, 0x4000, cage_high_orb},
  {0x497080, 0x4000, cage_low_orb},
  {0x497540, 0x1000, jarek_blade},
  {0x4988d0, 0x400, reptile_acid_spit},
  {0x498b70, 0x10000, kitana_command_shadow_low},
  {0x498b70, 0x20000, noob_saibot_command_498b70},
  {0x498b70, 0x200, tanya_fireball},
  {0x498e70, 0x10000, kitana_command_shadow_up},
  {0x498e70, 0x20000, noob_saibot_command_498e70},
  {0x498e70, 0x200, tanya_air_fireball},
  {0x498fc0, 0x800, kai_ground_skull},
  {0x499410, 0x800, kai_rising_skull},
  {0x499740, 0x100, quan_chi_skull},
  {0x49a630, 0x1, scorpion_spear},
  {0x49b9a0, 0x4, sonya_ring},
  {0x49bc60, 0x10, sub_zero_ice},
  {0x49bdd0, 0x2, raiden_lightning},
  {0x49bf50, 0x8, liu_kang_low_fireball},
  {0x49bf90, 0x8, liu_kang_high_fireball},
  {0x49ca70, 0x8000, goro_fireball},
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const NativeGate gates[] = {
  {0x45e1b0, jax_can_fatality_1},
  {0x45e220, kai_can_first_fatality},
  {0x45e250, reptile_can_first_fatality},
  {0x45e2a0, tanya_can_first_fatality},
  {0x45e2f0, quan_chi_can_first_fatality},
  {0x45e360, reiko_can_first_fatality},
  {0x45e3b0, fujin_can_first_fatality},
  {0x45e410, liu_kang_can_first_fatality},
  {0x45e460, sub_zero_can_first_fatality},
  {0x45e4b0, sonya_can_first_fatality},
  {0x45e4f0, mk4_can_spike_finish},
  {0x45e530, retail_can_finish_range},
  {0x45e570, retail_can_finish_close},
  {0x45e5a0, mk4_can_fan_finish},
  {0x45e6a0, shinnok_can_morph},
  {0x45e6d0, reiko_can_star},
  {0x45e710, reiko_can_orbit},
  {0x45e7a0, reiko_can_flip_kick},
  {0x45e7e0, jax_can_missile},
  {0x45e820, mk4_can_sidestep},
  {0x45e8d0, tanya_can_spin_kick},
  {0x45e910, jax_can_dash_punch},
  {0x45e950, reptile_can_dash},
  {0x45e990, sonya_can_cartwheel},
  {0x45e9d0, reptile_can_spit},
  {0x45ea10, raiden_can_teleport},
  {0x45ea60, scorpion_can_spear},
  {0x45eab0, liu_kang_can_bicycle},
  {0x45eb10, kai_can_ground_skull},
  {0x45eb40, kai_can_rising_skull},
  {0x45eb80, kai_can_handstand},
  {0x45ebf0, fujin_can_wind_slam},
  {0x45ec10, fujin_can_wind},
  {0x45ec60, fujin_can_wind_grab},
  {0x45ece0, sub_zero_can_clone},
  {0x45ed70, mk4_can_air_throw},
  {0x45ed80, mk4_can_air_throw},
  {0x45eeb0, fujin_can_dive},
  {0x45ef20, retail_has_gravity},
  {0x45ef50, weapon_can_draw},
  {0x45efc0, weapon_can_pickup},
  {0x45f0c0, scorpion_can_teleport},
  {0x45f120, cage_can_groin_punch},
  {0x45f1b0, sonya_can_leg_grab},
  {0x45f1f0, sub_zero_can_ice},
  {0x45f260, reptile_can_invisibility},
  {0x45f270, sub_zero_can_slide},
};
// clang-format on
