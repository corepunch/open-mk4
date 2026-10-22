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
  {RETAIL_NATIVE_FIREBALL_498B70, retail_fireball},
  {RETAIL_NATIVE_AIR_FIREBALL_498E70, retail_air_fireball},
  {RETAIL_NATIVE_WEAPON_DRAW_46D1D0, retail_weapon_draw},
  {RETAIL_NATIVE_TELEPORT_47C070, retail_teleport},
  {RETAIL_NATIVE_TELEPORT_SLAM_47C2A0, retail_teleport_slam},
  {RETAIL_NATIVE_HANDSTAND_SPIN_47C850, retail_handstand_spin},
  {RETAIL_NATIVE_HANDSTAND_EXIT_47CCF0, retail_handstand_exit},
  {RETAIL_NATIVE_HANDSTAND_FRONT_47CC10, retail_handstand_front},
  {RETAIL_NATIVE_HANDSTAND_REAR_47CC80, retail_handstand_rear},

  {RETAIL_NATIVE_AIR_THROW_46AA80, retail_air_throw},
  {RETAIL_NATIVE_AIR_THROW_LIFT_46A990, retail_air_throw_lift},
  {RETAIL_NATIVE_AIR_THROW_SLAM_46ABA0, retail_air_throw_slam},
  {RETAIL_NATIVE_AIR_THROW_FLIP_46AE60, retail_air_throw_flip},

  {RETAIL_NATIVE_AIR_HIGH_PUNCH_46E6C0, retail_air_high_punch},
  {RETAIL_NATIVE_AIR_LOW_PUNCH_46EE30, retail_air_low_punch},
  {RETAIL_NATIVE_AIR_KICK_46C7A0, retail_air_kick},
  {RETAIL_NATIVE_AIR_DIAGONAL_HIGH_KICK_46E380, retail_air_diagonal_high_kick},
  {RETAIL_NATIVE_AIR_DIAGONAL_LOW_KICK_46DF50, retail_air_diagonal_low_kick},

  {RETAIL_NATIVE_NORMAL_LOW_PUNCH_46DCA0, retail_normal_low_punch},
  {RETAIL_NATIVE_NORMAL_LOW_KICK_46A7B0, retail_normal_low_kick},
  {RETAIL_NATIVE_CLOSE_HIGH_KICK_46BA60, retail_close_high_kick},
  {RETAIL_NATIVE_CLOSE_HIGH_PUNCH_46BE40, retail_close_high_punch},
  {RETAIL_NATIVE_SPIKE_FINISH_467E70, retail_spike_finish},
  {RETAIL_NATIVE_CAGE_FATALITY_1_448160, cage_fatality_1},
  {RETAIL_NATIVE_FUJIN_FIRST_FATALITY_44F4F0, fujin_first_fatality},
  {RETAIL_NATIVE_JAREK_FATALITY_1_446D00, jarek_fatality_1},
  {RETAIL_NATIVE_JAX_FATALITY_1_4494C0, jax_fatality_1},
  {RETAIL_NATIVE_KAI_FATALITY_1_44AD70, kai_fatality_1},
  {RETAIL_NATIVE_LIU_KANG_FIRST_FATALITY_450930, liu_kang_first_fatality},
  {RETAIL_NATIVE_QUAN_CHI_FIRST_FATALITY_44BCD0, quan_chi_first_fatality},
  {RETAIL_NATIVE_RAIDEN_FIRST_FATALITY_440FC0, raiden_first_fatality},
  {RETAIL_NATIVE_REIKO_FIRST_FATALITY_452730, reiko_first_fatality},
  {RETAIL_NATIVE_REPTILE_FATALITY_1_4539B0, reptile_fatality_1},
  {RETAIL_NATIVE_SCORPION_BURN_FATALITY_443840, scorpion_burn_fatality},
  {RETAIL_NATIVE_SHINNOK_FATALITY_1_455000, shinnok_fatality_1},
  {RETAIL_NATIVE_SONYA_FIRST_FATALITY_43F420, sonya_first_fatality},
  {RETAIL_NATIVE_SUB_ZERO_FIRST_FATALITY_445100, sub_zero_first_fatality},
  {RETAIL_NATIVE_TANYA_FATALITY_1_44D340, tanya_fatality_1},
  {RETAIL_NATIVE_FINISH_FIRST_467CE0, retail_finish_first},
  {RETAIL_NATIVE_CAGE_FATALITY_2_465790, cage_fatality_2},
  {RETAIL_NATIVE_FUJIN_SECOND_FATALITY_43BEA0, fujin_second_fatality},
  {RETAIL_NATIVE_JAREK_FATALITY_2_41DF20, jarek_fatality_2},
  {RETAIL_NATIVE_JAX_FATALITY_2_4669C0, jax_fatality_2},
  {RETAIL_NATIVE_KAI_FATALITY_2_43BFE0, kai_fatality_2},
  {RETAIL_NATIVE_LIU_KANG_SECOND_FATALITY_43C3D0, liu_kang_second_fatality},
  {RETAIL_NATIVE_QUAN_CHI_SECOND_FATALITY_43BE30, quan_chi_second_fatality},
  {RETAIL_NATIVE_RAIDEN_SECOND_FATALITY_4647F0, raiden_second_fatality},
  {RETAIL_NATIVE_REIKO_SECOND_FATALITY_41C360, reiko_second_fatality},
  {RETAIL_NATIVE_REPTILE_FATALITY_2_43BF40, reptile_fatality_2},
  {RETAIL_NATIVE_SCORPION_BEAST_FATALITY_41E960, scorpion_beast_fatality},
  {RETAIL_NATIVE_SHINNOK_FATALITY_2_43C6D0, shinnok_fatality_2},
  {RETAIL_NATIVE_SONYA_SECOND_FATALITY_465CB0, sonya_second_fatality},
  {RETAIL_NATIVE_SUB_ZERO_SECOND_FATALITY_41B3A0, sub_zero_second_fatality},
  {RETAIL_NATIVE_TANYA_FATALITY_2_464F30, tanya_fatality_2},
  {RETAIL_NATIVE_FINISH_SECOND_467CD0, retail_finish_second},
  {RETAIL_NATIVE_ROUNDHOUSE_46F1F0, retail_roundhouse},
  {RETAIL_NATIVE_SWEEP_46EF90, retail_sweep},
  {RETAIL_NATIVE_HIGH_PUNCH_46D290, retail_high_punch},
  {RETAIL_NATIVE_CROUCH_LOW_PUNCH_46B7F0, retail_crouch_low_punch},
  {RETAIL_NATIVE_CROUCH_HIGH_KICK_46B5A0, retail_crouch_high_kick},
  {RETAIL_NATIVE_CROUCH_LOW_KICK_46B710, retail_crouch_low_kick},
  {RETAIL_NATIVE_LK_THROW_42B6E0, retail_lk_throw},
  {RETAIL_NATIVE_LP_THROW_42C590, retail_lp_throw},
  {RETAIL_NATIVE_HIGH_KICK_46A680, retail_high_kick},
  {RETAIL_NATIVE_FAN_FINISH_467E00, retail_fan_finish},
  {RETAIL_NATIVE_SONYA_CARTWHEEL_47B690, sonya_cartwheel},
  {RETAIL_NATIVE_SONYA_LEG_GRAB_47DEE0, sonya_leg_grab},
  {RETAIL_NATIVE_SONYA_RING_49B9A0, sonya_ring},
  {RETAIL_NATIVE_REIKO_FLIP_KICK_4791A0, reiko_flip_kick},
  {RETAIL_NATIVE_REIKO_STAR_496DB0, reiko_star},
  {RETAIL_NATIVE_GORO_SPECIAL_PUNCH_43AF90, goro_special_punch},
  {RETAIL_NATIVE_GORO_SPECIAL_KICK_43B170, goro_special_kick},
  {RETAIL_NATIVE_GORO_LEAP_STOMP_43B320, goro_leap_stomp},
  {RETAIL_NATIVE_GORO_SPECIAL_UPPERCUT_43B640, goro_special_uppercut},
  {RETAIL_NATIVE_GORO_QUAKE_STOMP_43B7E0, goro_quake_stomp},
  {RETAIL_NATIVE_SCORPION_FIRE_468610, scorpion_fire},
  {RETAIL_NATIVE_CAGE_GROIN_PUNCH_468AA0, cage_groin_punch},
  {RETAIL_NATIVE_JAX_GROUND_POUND_468F00, jax_ground_pound},
  {RETAIL_NATIVE_SIDESTEP_469250, mk4_sidestep},
  {RETAIL_NATIVE_REPTILE_INVISIBILITY_4698A0, reptile_invisibility},
  {RETAIL_NATIVE_SHINNOK_MORPH_SCORPION_469A00, shinnok_morph_scorpion},
  {RETAIL_NATIVE_SHINNOK_MORPH_RAIDEN_469A10, shinnok_morph_raiden},
  {RETAIL_NATIVE_SHINNOK_MORPH_SONYA_469A20, shinnok_morph_sonya},
  {RETAIL_NATIVE_SHINNOK_MORPH_LIU_KANG_469A30, shinnok_morph_liu_kang},
  {RETAIL_NATIVE_SHINNOK_MORPH_SUB_ZERO_469A40, shinnok_morph_sub_zero},
  {RETAIL_NATIVE_SHINNOK_MORPH_FUJIN_469A50, shinnok_morph_fujin},
  {RETAIL_NATIVE_SHINNOK_MORPH_REIKO_469A60, shinnok_morph_reiko},
  {RETAIL_NATIVE_SHINNOK_MORPH_JAREK_469A70, shinnok_morph_jarek},
  {RETAIL_NATIVE_SHINNOK_MORPH_QUAN_CHI_469A80, shinnok_morph_quan_chi},
  {RETAIL_NATIVE_SHINNOK_MORPH_TANYA_469A90, shinnok_morph_tanya},
  {RETAIL_NATIVE_SHINNOK_MORPH_REPTILE_469AA0, shinnok_morph_reptile},
  {RETAIL_NATIVE_SHINNOK_MORPH_JAX_469AB0, shinnok_morph_jax},
  {RETAIL_NATIVE_SHINNOK_MORPH_CAGE_469AC0, shinnok_morph_cage},
  {RETAIL_NATIVE_SHINNOK_MORPH_KAI_469AD0, shinnok_morph_kai},
  {RETAIL_NATIVE_QUAN_CHI_WEAPON_STEAL_469C80, quan_chi_weapon_steal},
  {RETAIL_NATIVE_WEAPON_PICKUP_46A020, weapon_pickup},
  {RETAIL_NATIVE_FUJIN_WIND_SLAM_46A5F0, fujin_wind_slam},
  {RETAIL_NATIVE_FUJIN_WIND_46A620, fujin_wind},
  {RETAIL_NATIVE_KAI_INPUT_478DF0, kai_input_478df0},
  {RETAIL_NATIVE_REIKO_ORBIT_478EE0, reiko_orbit},
  {RETAIL_NATIVE_TANYA_SPIN_KICK_479520, tanya_spin_kick},
  {RETAIL_NATIVE_JAREK_UPWARD_BALL_4797F0, jarek_upward_ball},
  {RETAIL_NATIVE_CAGE_CPU_KICK_479B20, cage_cpu_kick},
  {RETAIL_NATIVE_CAGE_POWERED_PUNCH_479EC0, cage_powered_punch},
  {RETAIL_NATIVE_QUAN_CHI_FLIP_KICK_47A370, quan_chi_flip_kick},
  {RETAIL_NATIVE_REPTILE_INPUT_47A690, reptile_input_47a690},
  {RETAIL_NATIVE_JAX_DASH_PUNCH_47AB10, jax_dash_punch},
  {RETAIL_NATIVE_REPTILE_INPUT_47AFE0, reptile_input_47afe0},
  {RETAIL_NATIVE_JAREK_CANNONBALL_47B310, jarek_cannonball},
  {RETAIL_NATIVE_JAREK_QUAKE_47B5D0, jarek_quake},
  {RETAIL_NATIVE_TANYA_DRILL_KICK_47B9F0, tanya_drill_kick},
  {RETAIL_NATIVE_KAI_INPUT_47BD10, kai_input_47bd10},
  {RETAIL_NATIVE_FUJIN_DIVE_47BF60, fujin_dive},
  {RETAIL_NATIVE_KAI_INPUT_47C520, kai_input_47c520},
  {RETAIL_NATIVE_RAIDEN_TELEPORT_47CD00, raiden_teleport},
  {RETAIL_NATIVE_QUAN_CHI_TELEPORT_STOMP_47CF30, quan_chi_teleport_stomp},
  {RETAIL_NATIVE_RAIDEN_TORPEDO_47D5F0, raiden_torpedo},
  {RETAIL_NATIVE_SUB_ZERO_SLIDE_47DC00, sub_zero_slide},
  {RETAIL_NATIVE_SONYA_SQUARE_WAVE_47E180, sonya_square_wave},
  {RETAIL_NATIVE_SONYA_BICYCLE_47E650, sonya_bicycle},
  {RETAIL_NATIVE_LIU_KANG_BICYCLE_KICK_47EAF0, liu_kang_bicycle_kick},
  {RETAIL_NATIVE_LIU_KANG_FLYING_KICK_47EDC0, liu_kang_flying_kick},
  {RETAIL_NATIVE_FUJIN_TRIPLE_KICK_47F010, fujin_triple_kick},
  {RETAIL_NATIVE_SUB_ZERO_CLONE_47F4D0, sub_zero_clone},
  {RETAIL_NATIVE_SCORPION_TELEPORT_47FC90, scorpion_teleport},
  {RETAIL_NATIVE_FUJIN_WIND_GRAB_480360, fujin_wind_grab},
  {RETAIL_NATIVE_JAX_MISSILE_496F80, jax_missile},
  {RETAIL_NATIVE_CAGE_HIGH_ORB_497070, cage_high_orb},
  {RETAIL_NATIVE_CAGE_LOW_ORB_497080, cage_low_orb},
  {RETAIL_NATIVE_JAREK_BLADE_497540, jarek_blade},
  {RETAIL_NATIVE_REPTILE_ACID_SPIT_4988D0, reptile_acid_spit},
  {RETAIL_NATIVE_KAI_GROUND_SKULL_498FC0, kai_ground_skull},
  {RETAIL_NATIVE_KAI_RISING_SKULL_499410, kai_rising_skull},
  {RETAIL_NATIVE_QUAN_CHI_SKULL_499740, quan_chi_skull},
  {RETAIL_NATIVE_SCORPION_SPEAR_49A630, scorpion_spear},
  {RETAIL_NATIVE_SUB_ZERO_ICE_49BC60, sub_zero_ice},
  {RETAIL_NATIVE_RAIDEN_LIGHTNING_49BDD0, raiden_lightning},
  {RETAIL_NATIVE_LIU_KANG_LOW_FIREBALL_49BF50, liu_kang_low_fireball},
  {RETAIL_NATIVE_LIU_KANG_HIGH_FIREBALL_49BF90, liu_kang_high_fireball},
  {RETAIL_NATIVE_GORO_FIREBALL_49CA70, goro_fireball},
};
// clang-format on

// clang-format off: Keep table entries on one row, including long rows.
static const NativeGate gates[] = {
  {RETAIL_NATIVE_JAX_CAN_FATALITY_1_45E1B0, jax_can_fatality_1},
  {RETAIL_NATIVE_KAI_CAN_FIRST_FATALITY_45E220, kai_can_first_fatality},
  {RETAIL_NATIVE_REPTILE_CAN_FIRST_FATALITY_45E250, reptile_can_first_fatality},
  {RETAIL_NATIVE_TANYA_CAN_FIRST_FATALITY_45E2A0, tanya_can_first_fatality},
  {RETAIL_NATIVE_QUAN_CHI_CAN_FIRST_FATALITY_45E2F0, quan_chi_can_first_fatality},
  {RETAIL_NATIVE_REIKO_CAN_FIRST_FATALITY_45E360, reiko_can_first_fatality},
  {RETAIL_NATIVE_FUJIN_CAN_FIRST_FATALITY_45E3B0, fujin_can_first_fatality},
  {RETAIL_NATIVE_LIU_KANG_CAN_FIRST_FATALITY_45E410, liu_kang_can_first_fatality},
  {RETAIL_NATIVE_SUB_ZERO_CAN_FIRST_FATALITY_45E460, sub_zero_can_first_fatality},
  {RETAIL_NATIVE_SONYA_CAN_FIRST_FATALITY_45E4B0, sonya_can_first_fatality},
  {RETAIL_NATIVE_CAN_SPIKE_FINISH_45E4F0, mk4_can_spike_finish},
  {RETAIL_NATIVE_CAN_FINISH_RANGE_45E530, retail_can_finish_range},
  {RETAIL_NATIVE_CAN_FINISH_CLOSE_45E570, retail_can_finish_close},
  {RETAIL_NATIVE_CAN_FAN_FINISH_45E5A0, mk4_can_fan_finish},
  {RETAIL_NATIVE_SHINNOK_CAN_MORPH_45E6A0, shinnok_can_morph},
  {RETAIL_NATIVE_REIKO_CAN_STAR_45E6D0, reiko_can_star},
  {RETAIL_NATIVE_REIKO_CAN_ORBIT_45E710, reiko_can_orbit},
  {RETAIL_NATIVE_REIKO_CAN_FLIP_KICK_45E7A0, reiko_can_flip_kick},
  {RETAIL_NATIVE_JAX_CAN_MISSILE_45E7E0, jax_can_missile},
  {RETAIL_NATIVE_CAN_SIDESTEP_45E820, mk4_can_sidestep},
  {RETAIL_NATIVE_TANYA_CAN_SPIN_KICK_45E8D0, tanya_can_spin_kick},
  {RETAIL_NATIVE_JAX_CAN_DASH_PUNCH_45E910, jax_can_dash_punch},
  {RETAIL_NATIVE_REPTILE_CAN_DASH_45E950, reptile_can_dash},
  {RETAIL_NATIVE_SONYA_CAN_CARTWHEEL_45E990, sonya_can_cartwheel},
  {RETAIL_NATIVE_REPTILE_CAN_SPIT_45E9D0, reptile_can_spit},
  {RETAIL_NATIVE_RAIDEN_CAN_TELEPORT_45EA10, raiden_can_teleport},
  {RETAIL_NATIVE_SCORPION_CAN_SPEAR_45EA60, scorpion_can_spear},
  {RETAIL_NATIVE_LIU_KANG_CAN_BICYCLE_45EAB0, liu_kang_can_bicycle},
  {RETAIL_NATIVE_KAI_CAN_GROUND_SKULL_45EB10, kai_can_ground_skull},
  {RETAIL_NATIVE_KAI_CAN_RISING_SKULL_45EB40, kai_can_rising_skull},
  {RETAIL_NATIVE_KAI_CAN_HANDSTAND_45EB80, kai_can_handstand},
  {RETAIL_NATIVE_FUJIN_CAN_WIND_SLAM_45EBF0, fujin_can_wind_slam},
  {RETAIL_NATIVE_FUJIN_CAN_WIND_45EC10, fujin_can_wind},
  {RETAIL_NATIVE_FUJIN_CAN_WIND_GRAB_45EC60, fujin_can_wind_grab},
  {RETAIL_NATIVE_SUB_ZERO_CAN_CLONE_45ECE0, sub_zero_can_clone},
  {RETAIL_NATIVE_CAN_AIR_THROW_45ED70, mk4_can_air_throw},
  {RETAIL_NATIVE_CAN_AIR_THROW_45ED80, mk4_can_air_throw},
  {RETAIL_NATIVE_FUJIN_CAN_DIVE_45EEB0, fujin_can_dive},
  {RETAIL_NATIVE_HAS_GRAVITY_45EF20, retail_has_gravity},
  {RETAIL_NATIVE_WEAPON_CAN_DRAW_45EF50, weapon_can_draw},
  {RETAIL_NATIVE_WEAPON_CAN_PICKUP_45EFC0, weapon_can_pickup},
  {RETAIL_NATIVE_SCORPION_CAN_TELEPORT_45F0C0, scorpion_can_teleport},
  {RETAIL_NATIVE_CAGE_CAN_GROIN_PUNCH_45F120, cage_can_groin_punch},
  {RETAIL_NATIVE_SONYA_CAN_LEG_GRAB_45F1B0, sonya_can_leg_grab},
  {RETAIL_NATIVE_SUB_ZERO_CAN_ICE_45F1F0, sub_zero_can_ice},
  {RETAIL_NATIVE_REPTILE_CAN_INVISIBILITY_45F260, reptile_can_invisibility},
  {RETAIL_NATIVE_SUB_ZERO_CAN_SLIDE_45F270, sub_zero_can_slide},
};
// clang-format on
