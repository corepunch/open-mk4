extern void retail_handstand_front(Fighter *fighter);
extern void retail_handstand_rear(Fighter *fighter);
extern void retail_handstand_spin(Fighter *fighter);
extern void retail_handstand_exit(Fighter *fighter);
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
extern void kai_input_478df0(Fighter *fighter);
extern void kai_input_47bd10(Fighter *fighter);
extern void kai_input_47c520(Fighter *fighter);
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
  {RETAIL_NATIVE_FIREBALL, retail_fireball},
  {RETAIL_NATIVE_AIR_FIREBALL, retail_air_fireball},
  {RETAIL_NATIVE_WEAPON_DRAW, retail_weapon_draw},
  {RETAIL_NATIVE_TELEPORT, retail_teleport},
  {RETAIL_NATIVE_TELEPORT_SLAM, retail_teleport_slam},
  {RETAIL_NATIVE_HANDSTAND_SPIN, retail_handstand_spin},
  {RETAIL_NATIVE_HANDSTAND_EXIT, retail_handstand_exit},
  {RETAIL_NATIVE_HANDSTAND_FRONT, retail_handstand_front},
  {RETAIL_NATIVE_HANDSTAND_REAR, retail_handstand_rear},

  {RETAIL_NATIVE_AIR_THROW, retail_air_throw},
  {RETAIL_NATIVE_AIR_THROW_LIFT, retail_air_throw_lift},
  {RETAIL_NATIVE_AIR_THROW_SLAM, retail_air_throw_slam},
  {RETAIL_NATIVE_AIR_THROW_FLIP, retail_air_throw_flip},

  {RETAIL_NATIVE_AIR_HIGH_PUNCH, retail_air_high_punch},
  {RETAIL_NATIVE_AIR_LOW_PUNCH, retail_air_low_punch},
  {RETAIL_NATIVE_AIR_KICK, retail_air_kick},
  {RETAIL_NATIVE_AIR_DIAGONAL_HIGH_KICK, retail_air_diagonal_high_kick},
  {RETAIL_NATIVE_AIR_DIAGONAL_LOW_KICK, retail_air_diagonal_low_kick},

  {RETAIL_NATIVE_NORMAL_LOW_PUNCH, retail_normal_low_punch},
  {RETAIL_NATIVE_NORMAL_LOW_KICK, retail_normal_low_kick},
  {RETAIL_NATIVE_CLOSE_HIGH_KICK, retail_close_high_kick},
  {RETAIL_NATIVE_CLOSE_HIGH_PUNCH, retail_close_high_punch},
  {RETAIL_NATIVE_SPIKE_FINISH, retail_spike_finish},
  {RETAIL_NATIVE_CAGE_FATALITY_1, cage_fatality_1},
  {RETAIL_NATIVE_FUJIN_FIRST_FATALITY, fujin_first_fatality},
  {RETAIL_NATIVE_JAREK_FATALITY_1, jarek_fatality_1},
  {RETAIL_NATIVE_JAX_FATALITY_1, jax_fatality_1},
  {RETAIL_NATIVE_KAI_FATALITY_1, kai_fatality_1},
  {RETAIL_NATIVE_LIU_KANG_FIRST_FATALITY, liu_kang_first_fatality},
  {RETAIL_NATIVE_QUAN_CHI_FIRST_FATALITY, quan_chi_first_fatality},
  {RETAIL_NATIVE_RAIDEN_FIRST_FATALITY, raiden_first_fatality},
  {RETAIL_NATIVE_REIKO_FIRST_FATALITY, reiko_first_fatality},
  {RETAIL_NATIVE_REPTILE_FATALITY_1, reptile_fatality_1},
  {RETAIL_NATIVE_SCORPION_BURN_FATALITY, scorpion_burn_fatality},
  {RETAIL_NATIVE_SHINNOK_FATALITY_1, shinnok_fatality_1},
  {RETAIL_NATIVE_SONYA_FIRST_FATALITY, sonya_first_fatality},
  {RETAIL_NATIVE_SUB_ZERO_FIRST_FATALITY, sub_zero_first_fatality},
  {RETAIL_NATIVE_TANYA_FATALITY_1, tanya_fatality_1},
  {RETAIL_NATIVE_FINISH_FIRST, retail_finish_first},
  {RETAIL_NATIVE_CAGE_FATALITY_2, cage_fatality_2},
  {RETAIL_NATIVE_FUJIN_SECOND_FATALITY, fujin_second_fatality},
  {RETAIL_NATIVE_JAREK_FATALITY_2, jarek_fatality_2},
  {RETAIL_NATIVE_JAX_FATALITY_2, jax_fatality_2},
  {RETAIL_NATIVE_KAI_FATALITY_2, kai_fatality_2},
  {RETAIL_NATIVE_LIU_KANG_SECOND_FATALITY, liu_kang_second_fatality},
  {RETAIL_NATIVE_QUAN_CHI_SECOND_FATALITY, quan_chi_second_fatality},
  {RETAIL_NATIVE_RAIDEN_SECOND_FATALITY, raiden_second_fatality},
  {RETAIL_NATIVE_REIKO_SECOND_FATALITY, reiko_second_fatality},
  {RETAIL_NATIVE_REPTILE_FATALITY_2, reptile_fatality_2},
  {RETAIL_NATIVE_SCORPION_BEAST_FATALITY, scorpion_beast_fatality},
  {RETAIL_NATIVE_SHINNOK_FATALITY_2, shinnok_fatality_2},
  {RETAIL_NATIVE_SONYA_SECOND_FATALITY, sonya_second_fatality},
  {RETAIL_NATIVE_SUB_ZERO_SECOND_FATALITY, sub_zero_second_fatality},
  {RETAIL_NATIVE_TANYA_FATALITY_2, tanya_fatality_2},
  {RETAIL_NATIVE_FINISH_SECOND, retail_finish_second},
  {RETAIL_NATIVE_ROUNDHOUSE, retail_roundhouse},
  {RETAIL_NATIVE_SWEEP, retail_sweep},
  {RETAIL_NATIVE_HIGH_PUNCH, retail_high_punch},
  {RETAIL_NATIVE_CROUCH_LOW_PUNCH, retail_crouch_low_punch},
  {RETAIL_NATIVE_CROUCH_HIGH_KICK, retail_crouch_high_kick},
  {RETAIL_NATIVE_CROUCH_LOW_KICK, retail_crouch_low_kick},
  {RETAIL_NATIVE_LK_THROW, retail_lk_throw},
  {RETAIL_NATIVE_LP_THROW, retail_lp_throw},
  {RETAIL_NATIVE_HIGH_KICK, retail_high_kick},
  {RETAIL_NATIVE_FAN_FINISH, retail_fan_finish},
  {RETAIL_NATIVE_SONYA_CARTWHEEL, sonya_cartwheel},
  {RETAIL_NATIVE_SONYA_LEG_GRAB, sonya_leg_grab},
  {RETAIL_NATIVE_SONYA_RING, sonya_ring},
  {RETAIL_NATIVE_REIKO_FLIP_KICK, reiko_flip_kick},
  {RETAIL_NATIVE_REIKO_STAR, reiko_star},
  {RETAIL_NATIVE_GORO_SPECIAL_PUNCH, goro_special_punch},
  {RETAIL_NATIVE_GORO_SPECIAL_KICK, goro_special_kick},
  {RETAIL_NATIVE_GORO_LEAP_STOMP, goro_leap_stomp},
  {RETAIL_NATIVE_GORO_SPECIAL_UPPERCUT, goro_special_uppercut},
  {RETAIL_NATIVE_GORO_QUAKE_STOMP, goro_quake_stomp},
  {RETAIL_NATIVE_SCORPION_FIRE, scorpion_fire},
  {RETAIL_NATIVE_CAGE_GROIN_PUNCH, cage_groin_punch},
  {RETAIL_NATIVE_JAX_GROUND_POUND, jax_ground_pound},
  {RETAIL_NATIVE_SIDESTEP, mk4_sidestep},
  {RETAIL_NATIVE_REPTILE_INVISIBILITY, reptile_invisibility},
  {RETAIL_NATIVE_SHINNOK_MORPH_SCORPION, shinnok_morph_scorpion},
  {RETAIL_NATIVE_SHINNOK_MORPH_RAIDEN, shinnok_morph_raiden},
  {RETAIL_NATIVE_SHINNOK_MORPH_SONYA, shinnok_morph_sonya},
  {RETAIL_NATIVE_SHINNOK_MORPH_LIU_KANG, shinnok_morph_liu_kang},
  {RETAIL_NATIVE_SHINNOK_MORPH_SUB_ZERO, shinnok_morph_sub_zero},
  {RETAIL_NATIVE_SHINNOK_MORPH_FUJIN, shinnok_morph_fujin},
  {RETAIL_NATIVE_SHINNOK_MORPH_REIKO, shinnok_morph_reiko},
  {RETAIL_NATIVE_SHINNOK_MORPH_JAREK, shinnok_morph_jarek},
  {RETAIL_NATIVE_SHINNOK_MORPH_QUAN_CHI, shinnok_morph_quan_chi},
  {RETAIL_NATIVE_SHINNOK_MORPH_TANYA, shinnok_morph_tanya},
  {RETAIL_NATIVE_SHINNOK_MORPH_REPTILE, shinnok_morph_reptile},
  {RETAIL_NATIVE_SHINNOK_MORPH_JAX, shinnok_morph_jax},
  {RETAIL_NATIVE_SHINNOK_MORPH_CAGE, shinnok_morph_cage},
  {RETAIL_NATIVE_SHINNOK_MORPH_KAI, shinnok_morph_kai},
  {RETAIL_NATIVE_QUAN_CHI_WEAPON_STEAL, quan_chi_weapon_steal},
  {RETAIL_NATIVE_WEAPON_PICKUP, weapon_pickup},
  {RETAIL_NATIVE_FUJIN_WIND_SLAM, fujin_wind_slam},
  {RETAIL_NATIVE_FUJIN_WIND, fujin_wind},
  {RETAIL_NATIVE_KAI_INPUT_478DF0_ENTRY, kai_input_478df0},
  {RETAIL_NATIVE_REIKO_ORBIT, reiko_orbit},
  {RETAIL_NATIVE_TANYA_SPIN_KICK, tanya_spin_kick},
  {RETAIL_NATIVE_JAREK_UPWARD_BALL, jarek_upward_ball},
  {RETAIL_NATIVE_CAGE_CPU_KICK, cage_cpu_kick},
  {RETAIL_NATIVE_CAGE_POWERED_PUNCH, cage_powered_punch},
  {RETAIL_NATIVE_QUAN_CHI_FLIP_KICK, quan_chi_flip_kick},
  {RETAIL_NATIVE_REPTILE_INPUT_47A690_ENTRY, reptile_input_47a690},
  {RETAIL_NATIVE_JAX_DASH_PUNCH, jax_dash_punch},
  {RETAIL_NATIVE_REPTILE_INPUT_47AFE0_ENTRY, reptile_input_47afe0},
  {RETAIL_NATIVE_JAREK_CANNONBALL, jarek_cannonball},
  {RETAIL_NATIVE_JAREK_QUAKE, jarek_quake},
  {RETAIL_NATIVE_TANYA_DRILL_KICK, tanya_drill_kick},
  {RETAIL_NATIVE_KAI_INPUT_47BD10_ENTRY, kai_input_47bd10},
  {RETAIL_NATIVE_FUJIN_DIVE, fujin_dive},
  {RETAIL_NATIVE_KAI_INPUT_47C520_ENTRY, kai_input_47c520},
  {RETAIL_NATIVE_RAIDEN_TELEPORT, raiden_teleport},
  {RETAIL_NATIVE_QUAN_CHI_TELEPORT_STOMP, quan_chi_teleport_stomp},
  {RETAIL_NATIVE_RAIDEN_TORPEDO, raiden_torpedo},
  {RETAIL_NATIVE_SUB_ZERO_SLIDE, sub_zero_slide},
  {RETAIL_NATIVE_SONYA_SQUARE_WAVE, sonya_square_wave},
  {RETAIL_NATIVE_SONYA_BICYCLE, sonya_bicycle},
  {RETAIL_NATIVE_LIU_KANG_BICYCLE_KICK, liu_kang_bicycle_kick},
  {RETAIL_NATIVE_LIU_KANG_FLYING_KICK, liu_kang_flying_kick},
  {RETAIL_NATIVE_FUJIN_TRIPLE_KICK, fujin_triple_kick},
  {RETAIL_NATIVE_SUB_ZERO_CLONE, sub_zero_clone},
  {RETAIL_NATIVE_SCORPION_TELEPORT, scorpion_teleport},
  {RETAIL_NATIVE_FUJIN_WIND_GRAB, fujin_wind_grab},
  {RETAIL_NATIVE_JAX_MISSILE, jax_missile},
  {RETAIL_NATIVE_CAGE_HIGH_ORB, cage_high_orb},
  {RETAIL_NATIVE_CAGE_LOW_ORB, cage_low_orb},
  {RETAIL_NATIVE_JAREK_BLADE, jarek_blade},
  {RETAIL_NATIVE_REPTILE_ACID_SPIT, reptile_acid_spit},
  {RETAIL_NATIVE_KAI_GROUND_SKULL, kai_ground_skull},
  {RETAIL_NATIVE_KAI_RISING_SKULL, kai_rising_skull},
  {RETAIL_NATIVE_QUAN_CHI_SKULL, quan_chi_skull},
  {RETAIL_NATIVE_SCORPION_SPEAR, scorpion_spear},
  {RETAIL_NATIVE_SUB_ZERO_ICE, sub_zero_ice},
  {RETAIL_NATIVE_RAIDEN_LIGHTNING, raiden_lightning},
  {RETAIL_NATIVE_LIU_KANG_LOW_FIREBALL, liu_kang_low_fireball},
  {RETAIL_NATIVE_LIU_KANG_HIGH_FIREBALL, liu_kang_high_fireball},
  {RETAIL_NATIVE_GORO_FIREBALL, goro_fireball},
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const NativeGate gates[] = {
  {RETAIL_NATIVE_JAX_CAN_FATALITY_1, jax_can_fatality_1},
  {RETAIL_NATIVE_KAI_CAN_FIRST_FATALITY, kai_can_first_fatality},
  {RETAIL_NATIVE_REPTILE_CAN_FIRST_FATALITY, reptile_can_first_fatality},
  {RETAIL_NATIVE_TANYA_CAN_FIRST_FATALITY, tanya_can_first_fatality},
  {RETAIL_NATIVE_QUAN_CHI_CAN_FIRST_FATALITY, quan_chi_can_first_fatality},
  {RETAIL_NATIVE_REIKO_CAN_FIRST_FATALITY, reiko_can_first_fatality},
  {RETAIL_NATIVE_FUJIN_CAN_FIRST_FATALITY, fujin_can_first_fatality},
  {RETAIL_NATIVE_LIU_KANG_CAN_FIRST_FATALITY, liu_kang_can_first_fatality},
  {RETAIL_NATIVE_SUB_ZERO_CAN_FIRST_FATALITY, sub_zero_can_first_fatality},
  {RETAIL_NATIVE_SONYA_CAN_FIRST_FATALITY, sonya_can_first_fatality},
  {RETAIL_NATIVE_CAN_SPIKE_FINISH, mk4_can_spike_finish},
  {RETAIL_NATIVE_CAN_FINISH_RANGE, retail_can_finish_range},
  {RETAIL_NATIVE_CAN_FINISH_CLOSE, retail_can_finish_close},
  {RETAIL_NATIVE_CAN_FAN_FINISH, mk4_can_fan_finish},
  {RETAIL_NATIVE_SHINNOK_CAN_MORPH, shinnok_can_morph},
  {RETAIL_NATIVE_REIKO_CAN_STAR, reiko_can_star},
  {RETAIL_NATIVE_REIKO_CAN_ORBIT, reiko_can_orbit},
  {RETAIL_NATIVE_REIKO_CAN_FLIP_KICK, reiko_can_flip_kick},
  {RETAIL_NATIVE_JAX_CAN_MISSILE, jax_can_missile},
  {RETAIL_NATIVE_CAN_SIDESTEP, mk4_can_sidestep},
  {RETAIL_NATIVE_TANYA_CAN_SPIN_KICK, tanya_can_spin_kick},
  {RETAIL_NATIVE_JAX_CAN_DASH_PUNCH, jax_can_dash_punch},
  {RETAIL_NATIVE_REPTILE_CAN_DASH, reptile_can_dash},
  {RETAIL_NATIVE_SONYA_CAN_CARTWHEEL, sonya_can_cartwheel},
  {RETAIL_NATIVE_REPTILE_CAN_SPIT, reptile_can_spit},
  {RETAIL_NATIVE_RAIDEN_CAN_TELEPORT, raiden_can_teleport},
  {RETAIL_NATIVE_SCORPION_CAN_SPEAR, scorpion_can_spear},
  {RETAIL_NATIVE_LIU_KANG_CAN_BICYCLE, liu_kang_can_bicycle},
  {RETAIL_NATIVE_KAI_CAN_GROUND_SKULL, kai_can_ground_skull},
  {RETAIL_NATIVE_KAI_CAN_RISING_SKULL, kai_can_rising_skull},
  {RETAIL_NATIVE_KAI_CAN_HANDSTAND, kai_can_handstand},
  {RETAIL_NATIVE_FUJIN_CAN_WIND_SLAM, fujin_can_wind_slam},
  {RETAIL_NATIVE_FUJIN_CAN_WIND, fujin_can_wind},
  {RETAIL_NATIVE_FUJIN_CAN_WIND_GRAB, fujin_can_wind_grab},
  {RETAIL_NATIVE_SUB_ZERO_CAN_CLONE, sub_zero_can_clone},
  {RETAIL_NATIVE_JAX_AIR_THROW_COMMAND_GATE, mk4_can_air_throw},
  {RETAIL_NATIVE_CAN_AIR_THROW_45ED80, mk4_can_air_throw},
  {RETAIL_NATIVE_FUJIN_CAN_DIVE, fujin_can_dive},
  {RETAIL_NATIVE_HAS_GRAVITY, retail_has_gravity},
  {RETAIL_NATIVE_WEAPON_CAN_DRAW, weapon_can_draw},
  {RETAIL_NATIVE_WEAPON_CAN_PICKUP, weapon_can_pickup},
  {RETAIL_NATIVE_SCORPION_CAN_TELEPORT, scorpion_can_teleport},
  {RETAIL_NATIVE_CAGE_CAN_GROIN_PUNCH, cage_can_groin_punch},
  {RETAIL_NATIVE_SONYA_CAN_LEG_GRAB, sonya_can_leg_grab},
  {RETAIL_NATIVE_SUB_ZERO_CAN_ICE, sub_zero_can_ice},
  {RETAIL_NATIVE_REPTILE_CAN_INVISIBILITY, reptile_can_invisibility},
  {RETAIL_NATIVE_SUB_ZERO_CAN_SLIDE, sub_zero_can_slide},
};
// clang-format on
