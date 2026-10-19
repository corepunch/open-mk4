#ifndef MK4_ASSETS_H
#define MK4_ASSETS_H
#include "binary.h"
#include "mk4_audio.h"
#define MK4_MAX_BONES 128
#define MK4_ROSTER_COUNT 18
#define MK4_CLIP_COUNT 612
#define MK4_ARENA_COUNT 11

typedef struct Vec3 {
  float x, y, z;
} Vec3;

Vec3 vec_add(Vec3 a, Vec3 b);
Vec3 vec_sub(Vec3 a, Vec3 b);
Vec3 vec_scale(Vec3 a, float scale);

typedef struct Mat3 {
  float m[9];
} Mat3;

Mat3 mat3_identity(void);
Vec3 mat3_apply(Mat3 matrix, Vec3 vector);
Mat3 mat3_mul(Mat3 a, Mat3 b);
Mat3 rotation(int kind, Vec3 bam);

typedef struct Vertex {
  Vec3 p, normal;
  float u, v;
} Vertex;

typedef struct Triangle {
  Vertex v[3];
  int texture;
} Triangle;

ARRAY_TYPE(Triangles, Triangle);

typedef struct Mesh {
  Triangles triangles;
  uint16_t flags;
} Mesh;

ARRAY_TYPE(Meshes, Mesh);
ARRAY_TYPE(Pixels, uint32_t);

typedef struct Texture {
  int width, height;
  Pixels pixels;
} Texture;

ARRAY_TYPE(Textures, Texture);

typedef struct Bone {
  int parent, channel, mesh, kind;
  int id; /* retail joint lookup ID; negative IDs are absent from contact bounds */
  uint32_t flags;
  Vec3 translation;
} Bone;

ARRAY_TYPE(Bones, Bone);
ARRAY_TYPE(Angles, Vec3);

typedef struct Frame {
  Vec3 translation;
  Angles angles;
} Frame;

ARRAY_TYPE(Frames, Frame);

typedef struct Clip {
  int id;
  Frames frames;
} Clip;

ARRAY_TYPE(Clips, Clip);

typedef struct Character {
  char name[1024], file[1024];
  Meshes meshes;
  Texture texture;
  Textures material_pages; /* optional shared multi-page material view for scene models */
  Bones bones;
  int idle;
  float scale, front_yaw;
} Character;

ARRAY_TYPE(Characters, Character);

typedef struct Pose {
  Mat3 rotation;
  Vec3 position;
} Pose;

typedef struct Sprite {
  int texture, u, v, w, h, width, height, origin_x, origin_y;
} Sprite;

ARRAY_TYPE(Sprites, Sprite);

typedef struct Geometry {
  Meshes meshes;
  Textures textures;
  Sprites sprites;
} Geometry;

typedef struct Arena {
  char name[1024], file[1024];
  Geometry geometry;
  Bones nodes;
} Arena;

ARRAY_TYPE(Arenas, Arena);

typedef struct Hud {
  Geometry font, effects, effects2, fujin_wind, title_scene, select_scene;
  Geometry jarek_fatality; /* ja_fat_g.geo: Jarek fatality props, e.g. 446fa0 rig 50831c */
  int names[MK4_ROSTER_COUNT];
} Hud;

typedef struct Assets {
  Characters characters;
  Characters cut_characters; /* closed-surface rigs; share normal meshes/textures when the file matches */
  Characters scene_models;   /* shared immutable geometry; independently owned accessory rigs */
  Character liu_dragon;      /* fatality I creature, separate from the roster */
  Character scorpion_beast;  /* fatality II model, separate from the roster */
  Character shinnok_hand;    /* rig 5083c4 / nf_fat_g.geo, both Shinnok fatalities */
  Clips clips;
  Arenas arenas;
  Hud hud;
  SoundBank audio;
} Assets;

enum {
  SCENE_MODEL_SPINE,
  SCENE_MODEL_ICE_CLONE,
  SCENE_MODEL_SWORD,
  SCENE_MODEL_SCEPTER,
  SCENE_MODEL_DRAGON_SWORD,
  SCENE_MODEL_RAIDEN_STAFF,
  SCENE_MODEL_SONYA_WEAPON,
  SCENE_MODEL_AXE,
  SCENE_MODEL_KAI_WEAPON,
  SCENE_MODEL_CAGE_WEAPON,
  SCENE_MODEL_JAREK_WEAPON,
  SCENE_MODEL_SHINNOK_WEAPON,
  SCENE_MODEL_FUJIN_WEAPON,
  SCENE_MODEL_REPTILE_WEAPON,
  SCENE_MODEL_TANYA_WEAPON,
  SCENE_MODEL_FLAYED_BODY,
  SCENE_MODEL_WIND_PIECES /* Fourteen closed part rigs in4ea420 order. */
};

const Texture *character_texture(const Character *character, int page);

/* Initialize Assets to zero before loading. The owner frees all decoded data. */
void assets_load(Assets *assets, const char *archive, const char *executable);
void assets_load_battle(Assets *assets, const char *archive, const char *executable);
void assets_free(Assets *assets);
/* Output is caller-owned and holds at least ch->bones.count entries. */
void pose(const Character *ch, const Clip *clip, double frame, Pose output[MK4_MAX_BONES]);
void pose_static(const Character *ch, Pose output[MK4_MAX_BONES]);
void pose_blend(const Character *ch, const Clip *from, int sample, const Clip *to, int target, float fraction, Pose output[MK4_MAX_BONES]);
void pose_blend_frames(const Character *ch, const Clip *from, double from_frame, const Clip *to, double to_frame, float fraction, Pose output[MK4_MAX_BONES]);
#endif
