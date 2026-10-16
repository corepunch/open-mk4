#ifndef MK4_LIGHTING_H
#define MK4_LIGHTING_H
#include "shadow.h"

typedef struct SceneLight {
  Vec3 position;
  float ambient, diffuse;
} SceneLight;

Vec4 light_homogeneous(SceneLight light);
float light_shade(SceneLight light, Vec3 point, Vec3 normal);
SceneLight above_camera_light(Vec3 camera);
#endif
