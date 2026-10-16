#include "draw.h"
#include "sprite_font.h"
#include "shadow.h"
#include "lighting.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <vector>

namespace mk4::game {
namespace {
constexpr float pi=3.14159265358979323846f;
struct Camera {
    float x,z,focal;int w,h;
    Vec3 position() const { return {x,-2.8f,z}; }
    Vec3 camera(Vec3 p) const {float y=p.y+2.8f,d=z-p.z;return {p.x-x,y*.978f-d*.208f,y*.208f+d*.978f};}
    ScreenVertex project(Vertex v,float light=1) const {return {float(w)*.5f+v.p.x*focal/v.p.z,float(h)*.47f+v.p.y*focal/v.p.z,1.f/v.p.z,v.u,v.v,light,1.f/v.p.z};}
};
void render_triangle(Canvas& cv,const Camera& camera,Triangle t,const Texture& texture,float brightness,
                     TrianglePass pass=TrianglePass::Color,const SceneLight* light=nullptr) {
    struct LitVertex { Vertex v;float light; };
    std::vector<LitVertex> in,out;
    for(auto v:t.v){float shade=brightness*(light?light->shade(v.p,v.normal):1.f);v.p=camera.camera(v.p);in.push_back({v,shade});}
    // Clip positions, UVs and vertex lighting together before projection.
    for(size_t i=0;i<in.size();++i) {
        auto a=in[i],b=in[(i+1)%in.size()];bool va=a.v.p.z>=.4f,vb=b.v.p.z>=.4f;
        if(va)out.push_back(a);
        if(va!=vb){float u=(.4f-a.v.p.z)/(b.v.p.z-a.v.p.z);LitVertex v;
            v.v.p=a.v.p+(b.v.p-a.v.p)*u;v.v.u=a.v.u+(b.v.u-a.v.u)*u;v.v.v=a.v.v+(b.v.v-a.v.v)*u;
            v.light=a.light+(b.light-a.light)*u;out.push_back(v);}
    }
    for(size_t i=1;i+1<out.size();++i){
        std::array<ScreenVertex,3> v{camera.project(out[0].v,out[0].light),
            camera.project(out[i].v,out[i].light),camera.project(out[i+1].v,out[i+1].light)};
        cv.triangle(v,texture,{0,0,cv.w,cv.h},false,pass);
    }
}
float draw_arena(Canvas& cv,const Camera& camera,const Arena& arena,const SceneLight& light) {
    float floor_y=std::numeric_limits<float>::infinity();
    Clip bind;bind.frames.push_back({});Character rig;rig.bones=arena.nodes;auto nodes=pose(rig,bind,0);
    for(size_t i=0;i<arena.nodes.size();++i){const auto& n=arena.nodes[i];if(n.flags&4)continue;
        for(auto t:arena.geometry.meshes[size_t(n.mesh)].triangles){for(auto& v:t.v){v.p=nodes[i].position+nodes[i].rotation.apply(v.p);v.normal=nodes[i].rotation.apply(v.normal);}
            // Only the actual, visible ground can receive planar shadows.
            bool floor=true;for(const auto& v:t.v)floor&=std::abs(v.p.y)<.05f;
            if(floor)for(const auto& v:t.v)floor_y=std::min(floor_y,v.p.y);
            render_triangle(cv,camera,t,arena.geometry.textures[size_t(t.texture)],1,
                            floor?TrianglePass::ShadowReceiver:TrianglePass::Color,&light);}}
    return std::isfinite(floor_y)?floor_y:0.f;
}
std::vector<Triangle> fighter_geometry(const Assets& assets,const Fighter& fighter) {
    std::vector<Triangle> triangles;
    const auto& ch=assets.characters[size_t(fighter.character)];int id=animation(ch,fighter.action);
    const Clip* clip=&assets.clips[size_t(id)];int channels=0;for(auto& b:ch.bones)channels=std::max(channels,b.channel+1);
    if(int(clip->frames.front().angles.size())!=channels)clip=&assets.clips[size_t(ch.idle)];
    double frame=double(fighter.ticks)*.5;
    if(const auto* m=battle_move(fighter.action)) {
        double t=double(std::max(0,fighter.ticks));
        double progress=t<m->startup?.4*t/m->startup:
            t<m->startup+m->active?.4+.2*(t-m->startup)/m->active:
            .6+.4*(t-m->startup-m->active)/m->recovery;
        frame=std::clamp(progress,0.,1.)*double(clip->frames.size()-1);
    }
    else if(fighter.action==ACTION_KNOCKDOWN)frame=std::min(double(std::max(0,fighter.ticks))*.65,double(clip->frames.size()-1));
    else if(fighter.action==ACTION_BLOCK||fighter.action==ACTION_CROUCH||fighter.action==ACTION_HIT)frame=std::min(frame,double(clip->frames.size()-1));
    auto bones=pose(ch,*clip,frame);auto initial=pose(ch,*clip,0);
    float front=ch.front_yaw;
    // Most original actions use a different absolute heading from the idle.
    // Keep the action facing its opponent by calibrating its initial head basis.
    for(size_t n=0;n<ch.bones.size();++n)if(ch.bones[n].channel==0){auto face=initial[n].rotation.apply({0,0,-1});front=std::atan2(-face.x,face.z);break;}
    auto turn=yaw_matrix(front+float(fighter.facing)*pi*.5f);
    Vec3 root=clip->frames.front().translation;
    // Horizontal root motion belongs to the combat simulation; retain animation Y.
    double wrapped=std::fmod(std::max(0.,frame),double(clip->frames.size()));
    auto current=clip->frames[size_t(wrapped)].translation;root.x=current.x;root.z=current.z;
    for(size_t n=0;n<ch.bones.size();++n){const auto& b=ch.bones[n];if(b.flags&4)continue;
        for(auto t:ch.meshes[size_t(b.mesh)].triangles){for(auto& v:t.v){auto p=turn.apply(bones[n].position+bones[n].rotation.apply(v.p)-root);
                v.p={p.x+fighter.x,p.y-fighter.height,p.z};
                v.normal=turn.apply(bones[n].rotation.apply(v.normal));}
            triangles.push_back(t);}}
    return triangles;
}
void round_intro(Canvas& cv,const Hud& hud,int round) {
    const auto& word=hud.effects.sprites[138];
    constexpr int scale=2,gap=18;
    SpriteFont font(hud.font);auto number=std::to_string(round);
    int digit=round>=1&&round<=3?138+round:-1;
    int digits_width=digit>=0?hud.effects.sprites[size_t(digit)].width*scale:font.width(number,scale);
    int x=(cv.w-word.width*scale-gap-digits_width)/2;
    draw_sprite(cv,hud.effects,138,{x,205,word.width*scale,word.height*scale});
    x+=word.width*scale+gap;
    if(digit>=0){const auto& s=hud.effects.sprites[size_t(digit)];draw_sprite(cv,hud.effects,digit,{x,205,s.width*scale,s.height*scale});}
    else font.draw(cv,x,216,number,scale);
}
void bar(Canvas& cv,const Geometry& font,int health,Rect r,bool right) {
    // The complete red/gold frame is sprite 25 in MK4FONT, including
    // its authored bevel. Empty health covers only the interior.
    const auto& s=font.sprites[25];
    cv.blit(font.textures[size_t(s.texture)],{s.u,s.v,s.w,8},r,right);
    int inner=r.w-6,missing=inner*(100-health)/100;
    cv.rect({right?r.x+3:r.x+r.w-3-missing,r.y+3,missing,r.h-6},0xff292320);

}
}
int animation(const Character& ch,Action a){
    if(ch.idle==587) {
        // Goro has his own original 21-channel action bank.
        switch(a){case ACTION_WALK:return 590;case ACTION_CROUCH:return 603;case ACTION_JUMP:return 602;
            case ACTION_PUNCH:return 596;case ACTION_KICK:return 598;case ACTION_SWEEP:return 600;
            case ACTION_UPPERCUT:return 593;case ACTION_THROW:return 597;case ACTION_BLOCK:return 595;
            case ACTION_HIT:return 605;case ACTION_KNOCKDOWN:return 581;case ACTION_VICTORY:return 608;default:return 587;}
    }
    switch(a){case ACTION_WALK:return 267;case ACTION_CROUCH:return 219;case ACTION_JUMP:return 301;
        case ACTION_PUNCH:return 216;case ACTION_KICK:return 65;case ACTION_SWEEP:return 101;
        case ACTION_UPPERCUT:return 212;case ACTION_THROW:return 23;case ACTION_BLOCK:return 251;
        case ACTION_HIT:return 247;case ACTION_KNOCKDOWN:return 166;case ACTION_VICTORY:return 293;default:return ch.idle;}
}
void draw_sprite(Canvas& cv,const Geometry& geo,int index,Rect dst,bool flip) {
    if(index<0||size_t(index)>=geo.sprites.size())return;
    const auto& s=geo.sprites[size_t(index)];if(!s.w||!s.h)return;
    cv.blit(geo.textures[size_t(s.texture)],{s.u,s.v,s.w,s.h},dst,flip);
}
void draw_battle(Canvas& cv,const Assets& assets,const Battle& battle,int arena,bool cpu,bool paused) {
    cv.clear();const auto& a=battle.fighters[0];const auto& b=battle.fighters[1];
    Camera camera{(a.x+b.x)*.25f,std::max(7.5f,std::abs(a.x-b.x)*.85f+3.f),float(cv.w)*.95f,cv.w,cv.h};
    const auto light=above_camera_light(camera.position());
    const float floor_y=draw_arena(cv,camera,assets.arenas[size_t(arena)],light);
    // Pose once, then render the exact same world-space caster in both passes.
    const std::array<std::vector<Triangle>,2> fighters={fighter_geometry(assets,a),fighter_geometry(assets,b)};
    const auto shadow=planar_shadow_matrix({0,1,0,-floor_y},light.homogeneous());
    for(size_t i=0;i<fighters.size();++i) {
        const auto& texture=assets.characters[size_t(battle.fighters[i].character)].texture;
        for(auto triangle:fighters[i]) {
            bool valid=true;
            for(auto& vertex:triangle.v)valid&=shadow.project(vertex.p,vertex.p);
            if(valid)render_triangle(cv,camera,triangle,texture,1,TrianglePass::ShadowMask);
        }
    }
    cv.resolve_shadows(104);
    for(size_t i=0;i<fighters.size();++i) {
        const auto& fighter=battle.fighters[i];
        const auto& texture=assets.characters[size_t(fighter.character)].texture;
        for(auto triangle:fighters[i])render_triangle(cv,camera,triangle,texture,fighter.action==ACTION_HIT?.72f:1.f,TrianglePass::Color,&light);
    }
    for(const auto& e:battle.effects)if(e.ticks>0){auto p=camera.project({camera.camera({e.x,-e.height,.2f}),{},0,0});
        // Shared effects atlas supplies the original blue/white impact glow.
        cv.blit(assets.hud.effects.textures[2],{1,1,23,23},{int(p.x)-26,int(p.y)-26,52,52});}
    constexpr uint32_t gold=0xffdcc798;
    const SpriteFont font(assets.hud.font);
    cv.rect({0,0,cv.w,122},0xff141517);cv.line(32,121,cv.w-32,121,0xff66533a);
    int bw=(cv.w-250)/2;Rect left{38,60,bw,18},right{cv.w-38-bw,60,bw,18};
    bar(cv,assets.hud.font,a.health,left,false);bar(cv,assets.hud.font,b.health,right,true);
    for(int i=0;i<2;++i){const auto& f=battle.fighters[size_t(i)];int idx=assets.hud.names[size_t(f.character)];
        auto s=assets.hud.font.sprites[size_t(idx)];int x=i?cv.w-38-s.w*2:38;
        draw_sprite(cv,assets.hud.font,idx,{x,31,s.w*2,s.h*2});
        for(int win=0;win<2;++win)cv.rect({i?cv.w-52-win*21:38+win*21,92,13,8},win<f.wins?gold:0xff3b3830);}
    char timer[16];std::snprintf(timer,sizeof timer,"%02d",std::max(0,(battle.time_ticks+59)/60));
    font.centered(cv,43,timer,2);font.centered(cv,96,"ROUND "+std::to_string(battle.round),.65f);
    cv.rect({0,cv.h-96,cv.w,96},0xff141517);
    font.draw(cv,28,cv.h-79,assets.arenas[size_t(arena)].name);
    const std::string mode=cpu?"P1 VS CPU / F3 TWO PLAYERS":"LOCAL TWO PLAYERS / F3 CPU";
    font.draw(cv,cv.w-28-font.width(mode,.6f),cv.h-78,mode,.6f);
    font.draw(cv,28,cv.h-49,"P1  A/D MOVE  W JUMP  S DUCK  J PUNCH  K KICK  L BLOCK  U UPPERCUT  O THROW  H RUN",.6f);
    font.draw(cv,28,cv.h-29,"P2  ARROWS  NUM 1/2/3/4/5  RSHIFT RUN    F1/F2 FIGHTERS  F4 ARENA  R RESET  SPACE PAUSE",.6f);
    if(battle.phase==PHASE_READY){round_intro(cv,assets.hud,battle.round);
        if(battle.phase_ticks>40)draw_sprite(cv,assets.hud.effects,137,{cv.w/2-181,285,362,126});}
    if(battle.phase==PHASE_ROUND_OVER||battle.phase==PHASE_MATCH_OVER){
        std::string message=battle.winner<0?"DRAW":assets.characters[size_t(battle.fighters[size_t(battle.winner)].character)].name+" WINS";
        cv.rect({cv.w/2-350,205,700,100},0xff141517);font.centered(cv,224,message,1.5f);
        font.centered(cv,267,battle.phase==PHASE_MATCH_OVER?"MATCH COMPLETE / ENTER TO REMATCH":"NEXT ROUND",.75f);}
    if(paused){cv.rect({cv.w/2-130,cv.h/2-35,260,66},0xff141517);font.centered(cv,cv.h/2-15,"PAUSED",1.5f);}
}
}
