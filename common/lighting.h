#pragma once
#include "shadow.h"

namespace mk4 {
struct SceneLight {
    Vec3 position;
    float ambient=.32f,diffuse=.68f;
    Vec4 homogeneous() const { return {position.x,position.y,position.z,1}; }
    float shade(Vec3 point,Vec3 normal) const;
};
// MK4 world Y increases downward; the light sits five units above the camera.
SceneLight above_camera_light(Vec3 camera);
}
