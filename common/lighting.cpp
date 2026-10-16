#include "lighting.h"
#include <algorithm>
#include <cmath>

namespace mk4 {
float SceneLight::shade(Vec3 point,Vec3 normal) const {
    Vec3 to_light=position-point;
    float n2=normal.x*normal.x+normal.y*normal.y+normal.z*normal.z;
    float l2=to_light.x*to_light.x+to_light.y*to_light.y+to_light.z*to_light.z;
    if(n2<1e-12f||l2<1e-12f)return ambient;
    float lambert=(normal.x*to_light.x+normal.y*to_light.y+normal.z*to_light.z)/std::sqrt(n2*l2);
    return std::clamp(ambient+diffuse*std::max(0.f,lambert),0.f,1.f);
}
SceneLight above_camera_light(Vec3 camera) { return {camera+Vec3{0,-5,0}}; }
}
