#include "shadow.h"
#include <cmath>
#include <stdexcept>

namespace mk4 {
Vec4 Mat4::apply(Vec4 p) const {
    return {m[0]*p.x+m[1]*p.y+m[2]*p.z+m[3]*p.w,
            m[4]*p.x+m[5]*p.y+m[6]*p.z+m[7]*p.w,
            m[8]*p.x+m[9]*p.y+m[10]*p.z+m[11]*p.w,
            m[12]*p.x+m[13]*p.y+m[14]*p.z+m[15]*p.w};
}
bool Mat4::project(Vec3 point,Vec3& projected) const {
    auto p=apply({point.x,point.y,point.z,1});
    if(!std::isfinite(p.w)||std::abs(p.w)<1e-6f)return false;
    projected={p.x/p.w,p.y/p.w,p.z/p.w};
    return std::isfinite(projected.x)&&std::isfinite(projected.y)&&std::isfinite(projected.z);
}
Mat4 planar_shadow_matrix(Vec4 plane,Vec4 light) {
    const float p[]={plane.x,plane.y,plane.z,plane.w},l[]={light.x,light.y,light.z,light.w};
    float dot=0;for(int i=0;i<4;++i)dot+=p[i]*l[i];
    if(!std::isfinite(dot)||std::abs(dot)<1e-6f||plane.x*plane.x+plane.y*plane.y+plane.z*plane.z<1e-12f)
        throw std::invalid_argument("Shadow light must intersect the receiver plane");
    Mat4 matrix;
    for(size_t row=0;row<4;++row)for(size_t col=0;col<4;++col)
        matrix.m[row*4+col]=(row==col?dot:0)-l[row]*p[col];
    return matrix;
}
}
