#include "renderer.h"
#include "shadow.h"
#include "lighting.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static void check(bool ok,const char *message) {require(ok,message);}
static bool closef(float a,float b) {return fabsf(a-b)<.0001f;}
static Mat4 projection(Vec4 plane,Vec4 light) {Mat4 m;check(planar_shadow_matrix(plane,light,&m),"Shadow matrix construction failed");return m;}
static void triangle(Canvas *cv,float z,const Texture *texture,TrianglePass pass) {
    const ScreenVertex vertices[]={{0,0,z,0,0,1,1},{8,0,z,0,0,1,1},{0,8,z,0,0,1,1}};
    canvas_triangle(cv,vertices,texture,(Rect){0,0,8,8},false,pass);
}
static void matrix_checks(void) {
    Vec3 out;Mat4 directional=projection((Vec4){0,1,0,0},(Vec4){-.5f,-1,-.75f,0});
    check(mat4_project(directional,(Vec3){2,-2,3},&out),"Directional projection failed");
    check(closef(out.x,3)&&closef(out.y,0)&&closef(out.z,4.5f),"Directional ray did not meet floor");
    check(mat4_project(directional,(Vec3){2,0,3},&out)&&closef(out.x,2)&&closef(out.z,3),"Floor contact moved");
    Vec3 low,high;check(mat4_project(directional,(Vec3){0,-1,0},&low)&&mat4_project(directional,(Vec3){0,-3,0},&high),"Airborne projection failed");
    check(closef(high.x,3*low.x)&&closef(high.z,3*low.z),"Airborne shadow does not move with caster height");
    Mat4 offset=projection((Vec4){0,1,0,.0078125f},(Vec4){-.5f,-1,-.75f,0});
    check(mat4_project(offset,(Vec3){0,-2,0},&out)&&closef(out.y,-.0078125f),"Quantized arena floor height was lost");
    Mat4 point=projection((Vec4){0,1,0,0},(Vec4){0,-4,0,1});
    check(mat4_project(point,(Vec3){1,-2,1},&out)&&closef(out.x,2)&&closef(out.y,0)&&closef(out.z,2),"Point-light homogeneous divide incorrect");
    check(!mat4_project(point,(Vec3){1,-4,1},&out),"Point at light height should project to infinity");
    Mat4 tilted=projection((Vec4){1,2,-1,-3},(Vec4){-2,-4,1,0});
    const Vec3 points[]={{2,-2,3},{-1,5,2},{4,0,-1}};
    for(size_t i=0;i<3;++i) {
        check(mat4_project(tilted,points[i],&out),"Tilted plane projection failed");
        check(closef(out.x+2*out.y-out.z,3),"Projected point is not on the receiver plane");
        Vec3 ray=vec_sub(out,points[i]);check(closef(ray.x*(-4)-ray.y*(-2),0)&&closef(ray.y-ray.z*(-4),0),"Projection did not follow light direction");
    }
    Mat4 rejected;check(!planar_shadow_matrix((Vec4){0,1,0,0},(Vec4){1,0,0,0},&rejected),"Parallel light was not rejected");
}
static void lighting_checks(void) {
    SceneLight light=above_camera_light((Vec3){0,-2.8f,7.5f});
    check(closef(light.position.x,0)&&closef(light.position.y,-7.8f)&&closef(light.position.z,7.5f),"Light is not above camera");
    Vec3 surface={1,-1,0},toward=vec_sub(light.position,surface);
    check(closef(light_shade(light,surface,toward),1),"Surface facing light is not fully lit");
    check(closef(light_shade(light,surface,vec_scale(toward,-1)),light.ambient),"Back face should receive ambient only");
    check(closef(light_shade(light,surface,vec_scale(toward,7)),light_shade(light,surface,toward)),"Lighting depends on normal magnitude");
    check(closef(light_shade(light,surface,(Vec3){0}),light.ambient),"Missing normal should have finite ambient lighting");
    Vec3 point={1,-2,0},shadow;Mat4 matrix=projection((Vec4){0,1,0,0},light_homogeneous(light));
    check(mat4_project(matrix,point,&shadow)&&shadow.x>point.x&&shadow.z<point.z,"Shadow points toward above-camera light");
    Vec3 ray=vec_sub(shadow,point),from_light=vec_sub(point,light.position);
    check(closef(ray.x*from_light.y-ray.y*from_light.x,0)&&closef(ray.z*from_light.y-ray.y*from_light.z,0),"Lighting and shadow projection disagree");
    SceneLight moved=above_camera_light((Vec3){3,-2.8f,12});check(closef(moved.position.x,3)&&closef(moved.position.z,12),"Light does not follow camera movement/zoom");
    Mat4 shifted=projection((Vec4){0,1,0,0},light_homogeneous(moved));
    check(mat4_project(shifted,point,&shadow)&&shadow.x<point.x,"Shadow did not respond to camera light movement");
    Vec3 front={0,0,1},back=mat3_apply(yaw_matrix(3.14159265358979323846f),front);
    check(light_shade(light,surface,front)>light_shade(light,surface,back),"Rotated normals do not change shading");
    Canvas cv;canvas_init(&cv,8,8);canvas_clear(&cv);uint32_t pixel=0xffc8c8c8;Texture texture={1,1,{&pixel,1,1}};
    const ScreenVertex vertices[]={{0,0,.1f,0,0,0,1},{8,0,.1f,0,0,1,.5f},{0,8,.1f,0,0,0,1}};
    canvas_triangle(&cv,vertices,&texture,(Rect){0,0,8,8},false,PASS_COLOR);
    check(cv.pixels[9]==0xff141414,"Vertex lighting is not perspective-correct or clamps ambient to 0.4");canvas_free(&cv);
}
static void stencil_checks(void) {
    Canvas cv;canvas_init(&cv,8,8);canvas_clear(&cv);
    uint32_t opaque=0xffc8c8c8,clear=0;const Texture white={1,1,{&opaque,1,1}},transparent={1,1,{&clear,1,1}};const size_t at=9;
    triangle(&cv,.1f,&white,PASS_SHADOW_RECEIVER);check(cv.stencil[at]==CANVAS_RECEIVER_BIT,"Visible floor did not mark stencil");
    float depth[64];uint32_t color[64];memcpy(depth,cv.depth,sizeof depth);memcpy(color,cv.pixels,sizeof color);
    triangle(&cv,.1f,&white,PASS_SHADOW_MASK);
    check(!memcmp(cv.pixels,color,sizeof color)&&!memcmp(cv.depth,depth,sizeof depth),"Shadow mask changed color/depth");
    for(int i=0;i<5;++i)triangle(&cv,.1f,&white,PASS_SHADOW_MASK);canvas_resolve_shadows(&cv,128);
    check(cv.pixels[at]==0xff636363,"Shadow overlap was blended more than once");
    uint32_t once[64];memcpy(once,cv.pixels,sizeof once);canvas_resolve_shadows(&cv,128);check(!memcmp(cv.pixels,once,sizeof once),"Resolved stencil coverage was not consumed");
    check(!memcmp(cv.depth,depth,sizeof depth),"Shadow resolve modified scene depth");
    canvas_clear(&cv);triangle(&cv,.1f,&white,PASS_COLOR);triangle(&cv,.1f,&white,PASS_SHADOW_MASK);canvas_resolve_shadows(&cv,128);
    check(cv.pixels[at]==0xffc8c8c8,"Nonreceiver surface received shadow");
    canvas_clear(&cv);triangle(&cv,.1f,&white,PASS_SHADOW_RECEIVER);triangle(&cv,.3f,&white,PASS_COLOR);triangle(&cv,.1f,&white,PASS_SHADOW_MASK);canvas_resolve_shadows(&cv,128);
    check(cv.pixels[at]==0xffc8c8c8&&cv.stencil[at]==0,"Foreground prop did not occlude receiver");
    canvas_clear(&cv);triangle(&cv,.1f,&white,PASS_SHADOW_RECEIVER);triangle(&cv,.05f,&white,PASS_SHADOW_MASK);canvas_resolve_shadows(&cv,128);
    check(cv.pixels[at]==0xffc8c8c8,"Hidden shadow passed the depth test");
    triangle(&cv,.1f,&transparent,PASS_SHADOW_MASK);canvas_resolve_shadows(&cv,128);check(cv.pixels[at]==0xffc8c8c8,"Transparent caster texel cast a shadow");
    triangle(&cv,.0999f,&white,PASS_SHADOW_MASK);canvas_resolve_shadows(&cv,128);check(cv.pixels[at]==0xff636363,"Coplanar floor bias failed");
    canvas_clear(&cv);for(size_t i=0;i<64;++i)check(cv.stencil[i]==0,"Stencil was not reset between frames");
    triangle(&cv,.1f,&transparent,PASS_SHADOW_RECEIVER);check(cv.stencil[at]==0,"Transparent floor texel marked receiver");canvas_free(&cv);
}
int main(void) {
    matrix_checks();lighting_checks();stencil_checks();
    puts("PASS: camera light; Lambert shading; animated normals; perspective lighting; matching light/shadow rays; directional/point shadow matrices; homogeneous divide; tilted planes; airborne casters; stencil union; receiver/depth/alpha tests; single blend; frame reset");return 0;
}
