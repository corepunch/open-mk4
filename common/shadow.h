#pragma once
#include "mk4_assets.h"

namespace mk4 {
struct Vec4 { float x=0,y=0,z=0,w=0; };
struct Mat4 {
    std::array<float,16> m{};
    Vec4 apply(Vec4) const;
    bool project(Vec3 point,Vec3& projected) const;
};
// Plane is (normal, distance): dot(plane, [position,1]) == 0.
// Light is homogeneous: w=0 directional, w=1 point light.
Mat4 planar_shadow_matrix(Vec4 plane,Vec4 light);
}
