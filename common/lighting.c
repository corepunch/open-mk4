#include "lighting.h"
#include "math_util.h"
Vec4 light_homogeneous(SceneLight light) {return (Vec4){light.position.x,light.position.y,light.position.z,1};}
float light_shade(SceneLight light,Vec3 point,Vec3 normal) {
    Vec3 to_light=vec_sub(light.position,point);
    float n2=normal.x*normal.x+normal.y*normal.y+normal.z*normal.z;
    float l2=to_light.x*to_light.x+to_light.y*to_light.y+to_light.z*to_light.z;
    if(n2<1e-12f||l2<1e-12f)return light.ambient;
    float lambert=(normal.x*to_light.x+normal.y*to_light.y+normal.z*to_light.z)/sqrtf(n2*l2);
    return clampf(light.ambient+light.diffuse*fmaxf(0.f,lambert),0.f,1.f);
}
SceneLight above_camera_light(Vec3 camera) {return (SceneLight){vec_add(camera,(Vec3){0,-5,0}),.32f,.68f};}
