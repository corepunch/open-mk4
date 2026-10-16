#include "shadow.h"
#include <math.h>
Vec4 mat4_apply(Mat4 m,Vec4 p) {
    return (Vec4){m.m[0]*p.x+m.m[1]*p.y+m.m[2]*p.z+m.m[3]*p.w,
        m.m[4]*p.x+m.m[5]*p.y+m.m[6]*p.z+m.m[7]*p.w,
        m.m[8]*p.x+m.m[9]*p.y+m.m[10]*p.z+m.m[11]*p.w,
        m.m[12]*p.x+m.m[13]*p.y+m.m[14]*p.z+m.m[15]*p.w};
}
bool mat4_project(Mat4 matrix,Vec3 point,Vec3 *projected) {
    Vec4 p=mat4_apply(matrix,(Vec4){point.x,point.y,point.z,1});
    if(!isfinite(p.w)||fabsf(p.w)<1e-6f)return false;
    *projected=(Vec3){p.x/p.w,p.y/p.w,p.z/p.w};
    return isfinite(projected->x)&&isfinite(projected->y)&&isfinite(projected->z);
}
bool planar_shadow_matrix(Vec4 plane,Vec4 light,Mat4 *matrix) {
    const float p[]={plane.x,plane.y,plane.z,plane.w},l[]={light.x,light.y,light.z,light.w};
    float dot=0;for(int i=0;i<4;++i)dot+=p[i]*l[i];
    if(!isfinite(dot)||fabsf(dot)<1e-6f||plane.x*plane.x+plane.y*plane.y+plane.z*plane.z<1e-12f)return false;
    for(size_t row=0;row<4;++row)for(size_t col=0;col<4;++col)matrix->m[row*4+col]=(row==col?dot:0)-l[row]*p[col];
    return true;
}
