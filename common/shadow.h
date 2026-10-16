#ifndef MK4_SHADOW_H
#define MK4_SHADOW_H
#include "mk4_assets.h"

typedef struct Vec4 {
  float x, y, z, w;
} Vec4;

typedef struct Mat4 {
  float m[16];
} Mat4;

Vec4 mat4_apply(Mat4 matrix, Vec4 point);
bool mat4_project(Mat4 matrix, Vec3 point, Vec3 *projected);
/* Returns false for an invalid receiver or a parallel light. */
bool planar_shadow_matrix(Vec4 plane, Vec4 light, Mat4 *output);
#endif
