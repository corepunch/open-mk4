#include "renderer.h"
#include "shadow.h"
#include "lighting.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>

namespace {
using namespace mk4;
void check(bool ok,const char* message){if(!ok)throw std::runtime_error(message);}
bool close(float a,float b){return std::abs(a-b)<.0001f;}
std::array<ScreenVertex,3> triangle(float z){return {{{0,0,z,0,0,1},{8,0,z,0,0,1},{0,8,z,0,0,1}}};}
void matrix_checks(){
    Vec3 out;
    auto directional=planar_shadow_matrix({0,1,0,0},{-.5f,-1,-.75f,0});
    check(directional.project({2,-2,3},out),"Directional projection failed");
    check(close(out.x,3)&&close(out.y,0)&&close(out.z,4.5f),"Directional ray did not meet floor");
    check(directional.project({2,0,3},out)&&close(out.x,2)&&close(out.z,3),"Floor contact moved");
    Vec3 low,high;directional.project({0,-1,0},low);directional.project({0,-3,0},high);
    check(close(high.x,3*low.x)&&close(high.z,3*low.z),"Airborne shadow does not move with caster height");
    auto offset=planar_shadow_matrix({0,1,0,.0078125f},{-.5f,-1,-.75f,0});
    check(offset.project({0,-2,0},out)&&close(out.y,-.0078125f),"Quantized arena floor height was lost");
    auto point=planar_shadow_matrix({0,1,0,0},{0,-4,0,1});
    check(point.project({1,-2,1},out)&&close(out.x,2)&&close(out.y,0)&&close(out.z,2),"Point-light homogeneous divide incorrect");
    check(!point.project({1,-4,1},out),"Point at light height should project to infinity");
    auto tilted=planar_shadow_matrix({1,2,-1,-3},{-2,-4,1,0});
    for(Vec3 p: {Vec3{2,-2,3},Vec3{-1,5,2},Vec3{4,0,-1}}){
        check(tilted.project(p,out),"Tilted plane projection failed");
        check(close(out.x+2*out.y-out.z,3),"Projected point is not on the receiver plane");
        auto ray=out-p;check(close(ray.x*(-4)-ray.y*(-2),0)&&close(ray.y-ray.z*(-4),0),"Projection did not follow light direction");
    }
    bool rejected=false;try{planar_shadow_matrix({0,1,0,0},{1,0,0,0});}catch(const std::invalid_argument&){rejected=true;}
    check(rejected,"Parallel light was not rejected");
}
void lighting_checks(){
    auto light=above_camera_light({0,-2.8f,7.5f});
    check(close(light.position.x,0)&&close(light.position.y,-7.8f)&&close(light.position.z,7.5f),"Light is not above camera");
    Vec3 surface{1,-1,0},toward=light.position-surface;
    check(close(light.shade(surface,toward),1),"Surface facing light is not fully lit");
    check(close(light.shade(surface,toward*-1),light.ambient),"Back face should receive ambient only");
    check(close(light.shade(surface,toward*7),light.shade(surface,toward)),"Lighting depends on normal magnitude");
    check(close(light.shade(surface,{}),light.ambient),"Missing normal should have finite ambient lighting");
    Vec3 point{1,-2,0},shadow;
    auto projection=planar_shadow_matrix({0,1,0,0},light.homogeneous());
    check(projection.project(point,shadow)&&shadow.x>point.x&&shadow.z<point.z,"Shadow points toward above-camera light");
    Vec3 ray=shadow-point,from_light=point-light.position;
    check(close(ray.x*from_light.y-ray.y*from_light.x,0)&&close(ray.z*from_light.y-ray.y*from_light.z,0),"Lighting and shadow projection disagree");
    auto moved=above_camera_light({3,-2.8f,12});
    check(close(moved.position.x,3)&&close(moved.position.z,12),"Light does not follow camera movement/zoom");
    auto shifted=planar_shadow_matrix({0,1,0,0},moved.homogeneous());
    check(shifted.project(point,shadow)&&shadow.x<point.x,"Shadow did not respond to camera light movement");
    Vec3 front{0,0,1},back=yaw_matrix(3.14159265358979323846f).apply(front);
    check(light.shade(surface,front)>light.shade(surface,back),"Rotated normals do not change shading");
    Canvas cv(8,8);cv.clear();auto vertices=triangle(.1f);
    vertices[0].light=0;vertices[1].light=1;vertices[1].q=.5f;vertices[2].light=0;
    cv.triangle(vertices,Texture{1,1,{0xffc8c8c8}},{0,0,8,8});
    check(cv.pixels[9]==0xff141414,"Vertex lighting is not perspective-correct or clamps ambient to 0.4");
}
void stencil_checks(){
    Canvas cv(8,8);cv.clear();const Texture white{1,1,{0xffc8c8c8}},transparent{1,1,{0}};Rect clip{0,0,8,8};size_t at=1*8+1;
    cv.triangle(triangle(.1f),white,clip,false,TrianglePass::ShadowReceiver);
    check(cv.stencil[at]==Canvas::receiver_bit,"Visible floor did not mark stencil");
    const auto depth=cv.depth;const auto color=cv.pixels;
    cv.triangle(triangle(.1f),white,clip,false,TrianglePass::ShadowMask);
    check(cv.pixels==color&&cv.depth==depth,"Shadow mask changed color/depth");
    // Repeated triangles model self-overlap and two fighters sharing a shadow.
    for(int i=0;i<5;++i)cv.triangle(triangle(.1f),white,clip,false,TrianglePass::ShadowMask);
    cv.resolve_shadows(128);
    check(cv.pixels[at]==0xff636363,"Shadow overlap was blended more than once");
    auto once=cv.pixels;cv.resolve_shadows(128);check(cv.pixels==once,"Resolved stencil coverage was not consumed");
    check(cv.depth==depth,"Shadow resolve modified scene depth");
    cv.clear();cv.triangle(triangle(.1f),white,clip);cv.triangle(triangle(.1f),white,clip,false,TrianglePass::ShadowMask);cv.resolve_shadows(128);
    check(cv.pixels[at]==0xffc8c8c8,"Nonreceiver surface received shadow");
    cv.clear();cv.triangle(triangle(.1f),white,clip,false,TrianglePass::ShadowReceiver);
    cv.triangle(triangle(.3f),white,clip);cv.triangle(triangle(.1f),white,clip,false,TrianglePass::ShadowMask);cv.resolve_shadows(128);
    check(cv.pixels[at]==0xffc8c8c8&&cv.stencil[at]==0,"Foreground prop did not occlude receiver");
    cv.clear();cv.triangle(triangle(.1f),white,clip,false,TrianglePass::ShadowReceiver);
    cv.triangle(triangle(.05f),white,clip,false,TrianglePass::ShadowMask);cv.resolve_shadows(128);
    check(cv.pixels[at]==0xffc8c8c8,"Hidden shadow passed the depth test");
    cv.triangle(triangle(.1f),transparent,clip,false,TrianglePass::ShadowMask);cv.resolve_shadows(128);
    check(cv.pixels[at]==0xffc8c8c8,"Transparent caster texel cast a shadow");
    cv.triangle(triangle(.0999f),white,clip,false,TrianglePass::ShadowMask);cv.resolve_shadows(128);
    check(cv.pixels[at]==0xff636363,"Coplanar floor bias failed");
    cv.clear();check(std::all_of(cv.stencil.begin(),cv.stencil.end(),[](auto x){return x==0;}),"Stencil was not reset between frames");
    cv.triangle(triangle(.1f),transparent,clip,false,TrianglePass::ShadowReceiver);
    check(cv.stencil[at]==0,"Transparent floor texel marked receiver");
}
}
int main(){try{matrix_checks();lighting_checks();stencil_checks();std::puts("PASS: camera light; Lambert shading; animated normals; perspective lighting; matching light/shadow rays; directional/point shadow matrices; homogeneous divide; tilted planes; airborne casters; stencil union; receiver/depth/alpha tests; single blend; frame reset");}
catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
