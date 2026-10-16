#include "mk4_assets.h"
#include "mk4_package.h"
#include "binary.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>
#include <stdexcept>

namespace mk4 {
namespace {

[[noreturn]] void bad(const std::string& what) { throw std::runtime_error(what); }
struct Image {
    Bytes image;
    explicit Image(const std::string& name) {
        Bytes raw=read_file(name); auto v=view(raw);
        require(v.u16(0)==0x5a4d,"Executable is not an MZ/PE image");
        size_t pe=v.u32(0x3c); require(v.u32(pe)==0x4550,"Executable is not PE");
        require(v.u16(pe+4)==0x14c && v.u16(pe+24)==0x10b,"Expected 32-bit x86 MK4 executable");
        require(v.u32(pe+52)==0x400000,"Unexpected executable image base");
        // Only headers and initialized sections are needed, not executable code.
        image.resize(0x180000);
        size_t sec=pe+24+v.u16(pe+20);
        for(size_t i=0;i<v.u16(pe+6);++i) {
            size_t h=sec+i*40, va=v.u32(h+12), z=v.u32(h+16), o=v.u32(h+20);
            if(va>=image.size()) continue;
            require(z<=image.size()-va,"Oversized executable section");
            v.check(o,z); std::copy_n(v.p+o,z,image.data()+va);
        }
        // This June 1998 release has two startup tables of (destination VA,
        // source VA). The game stores source/4 at each destination. These
        // are asset-data fixups, not OS PE relocations or executed code.
        auto before=at();
        require(before.u32(0x4fad7c-0x400000)==0 &&
                before.u32(0x4fad7c-0x400000+4)==13 &&
                before.u32(0x4fad7c-0x400000+612*12)==0xffffffff,
                "Unsupported MK4 executable revision (animation table differs)");
        for(auto range: {std::pair<uint32_t,uint32_t>{0x4d3e80,0x4d93f0}, {0x4d93f8,0x4d9d60}})
            for(uint32_t p=range.first;p<range.second;p+=8) {
                uint32_t dst=u32(p), src=u32(p+4);
                require(dst>=0x4d0000 && dst+4<=0x580000 &&
                        (src==0 || (src>=0x4d0000 && src<0x580000)),"Invalid asset pointer fixup");
                uint32_t w=src>>2;
                for(int k=0;k<4;++k) image[dst-0x400000+size_t(k)]=uint8_t(w>>(k*8));
            }
    }
    View at() const { return view(image); }
    uint32_t u32(uint32_t va) const { require(va>=0x400000,"Invalid asset VA"); return at().u32(va-0x400000); }
    int32_t s32(uint32_t va) const { return int32_t(u32(va)); }
    std::string string(uint32_t va) const { require(va>=0x400000,"Invalid string VA"); return at().string(va-0x400000); }
};

std::vector<Mesh> load_meshes(View v) {
    require(v.n>=12 && !std::memcmp(v.p,"0.1v",4) && v.u32(8)==v.n,"Invalid GEO header");
    size_t end=v.u32(4); require(end>=12 && end<v.n,"Invalid GEO texture offset");
    size_t table_end=end;
    std::vector<Mesh> out;
    for(size_t b=12;b+16<=table_end+4;b+=16) {
        uint16_t kind=v.u16(b),count=v.u16(b+2);
        if((kind&0xff)==0x80) { out.push_back({});continue; }
        if((kind&0x3f)>1 || !count) break;
        int64_t vp=int64_t(b)+4+v.s32(b+4), sp=int64_t(b)+8+v.s32(b+8), tp=int64_t(b)+12+v.s32(b+12);
        if(vp<=int64_t(b) || sp<=int64_t(b) || vp>=int64_t(end) || sp>=int64_t(end)) break;
        table_end=std::min(table_end,size_t(std::min(vp,sp)));
        // Header's texture offset is four bytes before the texture count;
        // those four bytes are also the final triangle's trailing UV bytes.
        require(tp>=0 && uint64_t(tp)+size_t(count)*8<=uint64_t(end)+4,"Invalid GEO triangle table");
        Mesh mesh; size_t vi=size_t(vp), ti=0; bool terminated=false;
        for(size_t s=size_t(sp);s+4<=end;s+=4) {
            int n=v.s16(s+2); if(n<0) { terminated=true; break; }
            require(n<8192 && ti+size_t(n)+1<=count,"Invalid GEO strip count");
            std::vector<Vertex> verts;
            for(int k=0;k<n+3;++k) {
                require(vi+12<=size_t(tp),"Invalid GEO vertex stream");
                // GEO stores normal components as Z/X/Y; TransformVertex consumes X/Y/Z.
                verts.push_back({{v.s16(vi)/256.f,v.s16(vi+2)/256.f,v.s16(vi+4)/256.f},
                                 {v.s16(vi+8)/4096.f,v.s16(vi+10)/4096.f,v.s16(vi+6)/4096.f},0,0});
                vi+=12;
            }
            for(int k=0;k<=n;++k,++ti) {
                Triangle t;t.texture=v.u8(size_t(tp)+ti*8);
                size_t uv=size_t(tp)+ti*8;
                for(int j=0;j<3;++j) {
                    t.v[size_t(j)]=verts[size_t(k+j)];
                    t.v[size_t(j)].u=v.u8(uv+2+size_t(j)*2);
                    t.v[size_t(j)].v=v.u8(uv+3+size_t(j)*2);
                }
                mesh.triangles.push_back(t);
            }
        }
        require(terminated && ti==count && vi==size_t(tp),"Inconsistent GEO strips/vertices");
        out.push_back(std::move(mesh)); require(out.size()<1024,"Too many GEO mesh blocks");
    }
    require(!out.empty(),"No GEO block records found"); return out;
}

std::vector<Texture> load_textures(View v) {
    size_t p=size_t(v.u32(4))+4; auto count=v.u32(p); p+=4;
    require(count>0 && count<=16,"Invalid GEO texture count");
    std::vector<Texture> textures;
    for(uint32_t page=0;page<count;++page) {
        Texture t; t.width=v.u16(p); t.height=v.u16(p+2); size_t n=v.u32(p+4); p+=8;
        require(t.width>0 && t.width<=1024 && t.height>0 && t.height<=1024,"Invalid texture dimensions");
        // Stream size is rounded to an even byte count. Several retail files
        // omit that last pad byte; it is not part of the decoded pixel stream.
        require(p<=v.n && n<=v.n-p+1,"Truncated texture stream");
        size_t available=std::min(n,v.n-p);View src=v.sub(p,available); size_t pos=0;
        for(int y=0;y<t.height;++y) {
            int x=0;
            while(x<t.width) {
                auto token=src.u16(pos); pos+=2;
                int run=1; uint16_t c=token;
                if(token&0x8000) { c=token&0x7fff; run=src.u8(pos++); }
                require(run>0 && run<=t.width-x,"Invalid RGB555 texture run");
                // Zero is transparent. Literal and repeated colors use the same RGB555 format.
                uint32_t r=(c>>10)&31,g=(c>>5)&31,b=c&31;
                uint32_t rgba=(c?0xff000000u:0)|((r*255/31)<<16)|((g*255/31)<<8)|(b*255/31);
                for(int k=0;k<run;++k) t.pixels.push_back(rgba);
                x+=run;
            }
        }
        require(pos==available || (pos+1==available && n%2==0),"Texture stream has unexplained trailing data");
        textures.push_back(std::move(t));p+=n;
    }
    return textures;
}

Geometry load_geometry(View v) {
    Geometry g;g.meshes=load_meshes(v);g.textures=load_textures(v);g.sprites.resize(g.meshes.size());
    for(size_t i=0;i<g.meshes.size();++i) {
        size_t p=12+i*16;
        if((v.u16(p)&0xff)==0x80) {
            Sprite sp;sp.texture=v.u8(p+2);sp.width=v.s16(p+4);sp.height=v.s16(p+6);
            sp.origin_x=v.s16(p+8);sp.origin_y=v.s16(p+10);
            sp.u=v.u8(p+12);sp.v=v.u8(p+13);sp.w=v.u8(p+14);sp.h=v.u8(p+15);
            require(size_t(sp.texture)<g.textures.size() && sp.w>0 && sp.h>0 &&
                    sp.u+sp.w<=g.textures[size_t(sp.texture)].width &&
                    sp.v+sp.h<=g.textures[size_t(sp.texture)].height,"Invalid GEO sprite");
            g.sprites[i]=sp;
        } else for(const auto& t:g.meshes[i].triangles)
            require(t.texture>=0 && size_t(t.texture)<g.textures.size(),"Invalid GEO texture page");
    }
    return g;
}

struct Bits {
    View v; size_t bit=24*8;
    uint32_t peek(unsigned n) const {
        require(n<=24 && bit<=v.n*8,"Animation bitstream overrun");
        uint32_t w=0;
        // Prefix lookup may straddle EOF; only the consumed prefix must fit.
        for(unsigned i=0;i<n && bit+i<v.n*8;++i) w|=((v.u8((bit+i)/8)>>((bit+i)%8))&1u)<<i;
        return w;
    }
    uint32_t get(unsigned n) { require(bit<=v.n*8 && n<=v.n*8-bit,"Animation bitstream overrun"); auto w=peek(n);bit+=n;return w; }
    void skip(size_t n) { require(bit<=v.n*8 && n<=v.n*8-bit,"Animation seek overrun"); bit+=n; }
};

// Adapted from mk4decomp's Anim_DecodeBitstream (MIT; see THIRD_PARTY_NOTICES).
// A bounded bit cursor replaces the original's address-dependent DWORD reader.
std::vector<uint32_t> decode_frame(View stream, View nibtable, int frames, int words, int frame) {
    require(words>=3 && words<=24,"Unsupported animation channel count");
    int bones=words-3; std::vector<uint32_t> out(size_t(words),0);
    std::array<int,63> keyed{},delta{},span{},nib{},extra{};
    Bits bits{stream}; int local=frame;
    for(;;) { auto d=bits.get(13); if(local<16)break; local-=16;bits.skip(d); }
    for(int i=0;i<bones*3;++i) {
        int shift=(i%3)>>1;
        keyed[size_t(i)]=int(bits.get(1));
        int value=int(bits.get(unsigned(11-shift)))<<shift;
        if(value&0x400)value-=0x800;
        delta[size_t(i)]=value;
        if(keyed[size_t(i)])span[size_t(i)]=int(bits.get(8));
    }
    int repeats;
    if(local>7) repeats=local-7;
    else {
        int last=frames-1, lim=7;
        if(frame>=(last&~15))lim=std::min(lim,last&15);
        repeats=lim-local;
    }
    std::array<uint32_t,3> scalar{bits.get(8),bits.get(8),bits.get(8)};
    for(int i=0;i<bones*3;++i) if(keyed[size_t(i)]) {
        nib[size_t(i)]=int(bits.get(4));
        if(!nib[size_t(i)]) { int x=int(bits.get(8)); if(!repeats)extra[size_t(i)]=x; }
    }
    auto d=bits.get(12);if(local>7)bits.skip(d);
    while(repeats-->0) {
        for(int i=0;i<3;++i) { auto x=bits.get(8);if(!repeats)scalar[size_t(i)]=x; }
        for(int i=0;i<bones*3;++i) if(keyed[size_t(i)]) {
            auto token=nibtable.u16(bits.peek(8)*2);
            unsigned used=token&15;require(used>0 && used<=8,"Invalid animation prefix code");
            bits.skip(used);nib[size_t(i)]=(nib[size_t(i)]+(token>>7))&15;
            if(!nib[size_t(i)]) { int x=int(bits.get(8)); if(!repeats)extra[size_t(i)]=x; }
        }
    }
    for(int i=0;i<3;++i)out[size_t(i)]=((scalar[size_t(i)]*stream.u32(size_t(i)*8))>>8)+stream.u32(size_t(i)*8+4);
    for(int i=0;i<bones;++i) {
        uint32_t packed=0;
        for(int k=0;k<3;++k) {
            size_t j=size_t(i*3+k);int value=delta[j];
            if(keyed[j])value=nib[j]?span[j]*(nib[j]-1)+delta[j]:int(int8_t(extra[j]))*8;
            packed|=uint32_t((value&0x7ff)>>(k>>1))<<(k*11);
        }
        out[size_t(i+3)]=packed;
    }
    return out;
}

std::vector<Clip> load_clips(const Package& archive, const Image& exe) {
    View ap0=archive.get(0x2da2df95), ap1=archive.get(0x2da3df95);
    std::vector<Clip> clips(612); std::vector<bool> seen(612,false); size_t offset=0;
    for(size_t p=0x200;;p+=4) {
        uint16_t tag=ap0.u16(p); if(tag==0xffff)break;
        int id=tag&0x7fff;size_t size=ap0.u16(p+2);
        require(id<int(clips.size()) && !seen[size_t(id)],"Invalid animation index"); seen[size_t(id)]=true;
        uint32_t desc=0x4fad7c+uint32_t(id)*12;
        int frames=int(exe.u32(desc+4)),words=int(exe.u32(desc+8)>>16);
        require(frames>0 && frames<=600 && words>=3 && words<=24,"Invalid animation descriptor");
        View stream=ap1.sub(offset,size); offset+=size;Clip& clip=clips[size_t(id)];clip.id=id;
        if(!(tag&0x8000))require(size==size_t(words)*size_t(frames)*4,"Invalid uncompressed clip size");
        for(int f=0;f<frames;++f) {
            std::vector<uint32_t> record;
            if(tag&0x8000) {
                try { record=decode_frame(stream,ap0.sub(0,512),frames,words,f); }
                catch(const std::exception& e) { bad("Clip "+std::to_string(id)+", frame "+std::to_string(f)+": "+e.what()); }
            }
            else for(int w=0;w<words;++w)record.push_back(stream.u32(size_t(f*words+w)*4));
            Frame out;out.translation={int32_t(record[0])/65536.f,int32_t(record[1])/65536.f,int32_t(record[2])/65536.f};
            for(int w=3;w<words;++w) {
                uint32_t v=record[size_t(w)];
                out.angles.push_back({float((v&0x7ff)*2),float(((v>>11)&0x7ff)*2),float((v>>22)*4)});
            }
            clip.frames.push_back(std::move(out));
        }
    }
    require(offset==ap1.n && std::all_of(seen.begin(),seen.end(),[](bool x){return x;}),"Incomplete animation pack");
    return clips;
}

std::vector<Bone> load_rig(const Image& exe, uint32_t start, uint32_t slot, size_t meshes) {
    std::vector<Bone> bones;uint32_t cursor=start;
    std::function<void(int,int)> walk=[&](int parent,int depth) {
        require(depth<32 && bones.size()<128,"Invalid skeleton hierarchy");
        uint32_t flags=exe.u32(cursor);require(exe.u32(cursor+4)==slot,"Skeleton asset slot mismatch");
        Bone b;b.parent=parent;b.flags=(flags&0xfffff)|(parent<0?0x09000000u:0x0a000000u);
        // MStackPushTwoEntryChainCall (4058c0) replaces the allocation
        // defaults: root kind 9 (C), all descendants kind 10 (B).
        // ScaledMaskOrStore preserves the other render/animation flags.
        // Missing this post-build pass produces twisted, airborne poses.
        b.kind=int((b.flags>>24)&7);
        int id=exe.s32(cursor)>>20;b.channel=(b.flags&0x100)?id-2:-1;
        b.mesh=int(exe.u32(cursor+24));require(b.mesh>=0 && size_t(b.mesh)<meshes,"Skeleton mesh index out of range: "+std::to_string(b.mesh)+" / "+std::to_string(meshes));
        b.translation={exe.s32(cursor+12)/65536.f,exe.s32(cursor+16)/65536.f,exe.s32(cursor+20)/65536.f};
        int index=int(bones.size());bones.push_back(b);cursor+=28;
        if(!(flags&8))for(;;) { uint32_t f=exe.u32(cursor);walk(index,depth+1);if(f&16)break; }
    };
    walk(-1,0);return bones;
}
} // namespace

Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x,a.y+b.y,a.z+b.z}; }
Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
Vec3 operator*(Vec3 a, float b) { return {a.x*b,a.y*b,a.z*b}; }
Vec3 Mat3::apply(Vec3 v) const { return {m[0]*v.x+m[1]*v.y+m[2]*v.z,m[3]*v.x+m[4]*v.y+m[5]*v.z,m[6]*v.x+m[7]*v.y+m[8]*v.z}; }
Mat3 operator*(const Mat3& a,const Mat3& b) {
    Mat3 c;c.m.fill(0);
    for(int i=0;i<3;++i)for(int j=0;j<3;++j)for(int k=0;k<3;++k)c.m[size_t(i*3+j)]+=a.m[size_t(i*3+k)]*b.m[size_t(k*3+j)];
    return c;
}
Mat3 rotation(int kind,Vec3 bam) {
    constexpr float tau=6.283185307179586f;
    // Direct transform setters negate the 12-bit angles before lookup.
    float a=-bam.x*tau/4096,b=-bam.y*tau/4096,c=-bam.z*tau/4096;
    // AppInit_PostJoy builds this table with FCOS, despite its historical
    // g_sinTable name. The phase-shifted lookup is therefore SIN, not -COS.
    // Keep the original builders' raw lookup formulas to avoid that trap.
    float x=std::cos(a),xr=std::sin(a),y=std::cos(b),yr=std::sin(b),z=std::cos(c),zr=std::sin(c);
    if(kind==1) {
        float p=yr*xr,q=-yr*x;
        return {{z*y,-zr*y,yr,z*p+zr*x,z*x-zr*p,-xr*y,z*q+zr*xr,xr*z-zr*q,y*x}};
    }
    if(kind==2) {
        float r=yr*z,t=zr*yr;
        return {{z*y,xr*r-zr*x,x*r+zr*xr,zr*y,xr*t+z*x,x*t-xr*z,-yr,xr*y,y*x}};
    }
    float t=yr*xr,u=xr*y;
    return {{zr*t+z*y,z*t-zr*y,yr*x,zr*x,z*x,-xr,zr*u-yr*z,z*u+zr*yr,y*x}};
}

void Assets::load(const std::string& archive_path,const std::string& executable) {
    Package archive(archive_path);Image exe(executable);clips=load_clips(archive,exe);
    characters.clear();
    for(int i=0;i<18;++i) {
        uint32_t rec=0x4eb2e8+uint32_t(i)*32,slot=exe.u32(rec)*4;
        uint32_t asset=exe.u32(slot), name=exe.u32(asset);
        Character ch;ch.name=exe.string(exe.u32(0x4ee1f0+uint32_t(i)*4));ch.file=exe.string(name);
        for(auto& c:ch.name)if(c>='a'&&c<='z')c-=32;
        auto data=archive.geo(ch.file);
        try { ch.meshes=load_meshes(data); }
        catch(const std::exception& e) { bad(ch.name+" mesh: "+e.what()); }
        try { ch.texture=load_textures(data).front(); }
        catch(const std::exception& e) { bad(ch.name+" texture: "+e.what()); }
        try { ch.bones=load_rig(exe,exe.u32(rec+12)*4,slot,ch.meshes.size()); }
        catch(const std::exception& e) { bad(ch.name+" skeleton: "+e.what()); }
        ch.scale=float(exe.u32(rec+28))/65536.f;
        // StateDispatchTable selects these default stance descriptors. The
        // two special roster IDs are remapped exactly as in the game.
        int stance_index=i==16?2:i==17?7:i;
        uint32_t stance=exe.u32(0x53d408+uint32_t(stance_index)*4)*4;
        require(stance>=0x4fad7c && (stance-0x4fad7c)%12==0 &&
                stance<0x4fad7c+612*12,"Invalid default stance descriptor");
        ch.idle=int((stance-0x4fad7c)/12);
        auto initial=pose(ch,clips[size_t(ch.idle)],0);
        for(size_t n=0;n<ch.bones.size();++n)if(ch.bones[n].channel==0) {
            Vec3 face=initial[n].rotation.apply({0,0,-1});
            ch.front_yaw=std::atan2(-face.x,face.z);
            break;
        }
        characters.push_back(std::move(ch));
    }
}

void Assets::load_battle(const std::string& archive_path,const std::string& executable) {
    Package archive(archive_path);Image exe(executable);arenas.clear();
    const char* names[]={"GORO'S LAIR","THE WELL","ELDER GODS","THE TOMB","WIND WORLD",
                         "REPTILE'S LAIR","SHAOLIN TEMPLE","THE LIVING FOREST","THE PRISON","ICE PIT","THE SKULL"};
    // Actual June-1998 stage table. The upstream revision uses different VAs.
    const int records[]={0,1,2,3,4,5,6,7,8,9,10};
    for(size_t i=0;i<sizeof(records)/sizeof(records[0]);++i) {
        uint32_t rec=0x4dd960+uint32_t(records[i])*64,slot=exe.u32(rec)*4;
        Arena a;a.name=names[i];a.file=exe.string(exe.u32(exe.u32(slot)));
        try {
            a.geometry=load_geometry(archive.geo(a.file));
            a.nodes=load_rig(exe,exe.u32(rec+12)*4,slot,a.geometry.meshes.size());
            // Stage nodes have no character animation channels. Static bind
            // translations place the arena geometry; callbacks/effects are separate.
            for(auto& n:a.nodes)n.channel=-1;
        } catch(const std::exception& e) { bad(a.file+": "+e.what()); }
        arenas.push_back(std::move(a));
    }
    hud.font=load_geometry(archive.geo("mk4font_.geo"));
    hud.effects=load_geometry(archive.geo("shared_g.geo"));
    // Sprite names were checked against the decoded font atlas, not inferred from roster order.
    const char* names_by_sprite[]={"LIU KANG","RAIDEN","SONYA","SCORPION","SUB ZERO","FUJIN","JAREK","KAI",
                                  "TANYA","MILEENA","NOOB SAIBOT","QUAN CHI","REPTILE","SHINNOK","JAX","CAGE","REIKO","GORO","KITANA"};
    for(size_t i=0;i<characters.size();++i) {
        hud.names[i]=-1;
        for(int j=0;j<19;++j)if(characters[i].name==names_by_sprite[j])hud.names[i]=j;
        require(hud.names[i]>=0,"Missing HUD name for "+characters[i].name);
    }
}

std::vector<Pose> pose(const Character& ch,const Clip& clip,double frame) {
    require(!clip.frames.empty(),"Empty animation clip");
    double f=std::fmod(std::max(0.,frame),double(clip.frames.size()));
    size_t a=size_t(f),b=(a+1)%clip.frames.size();float t=float(f-double(a));
    const auto& f0=clip.frames[a];const auto& f1=clip.frames[b];std::vector<Pose> out;
    for(const auto& bone:ch.bones) {
        Pose p;Vec3 angle{};
        if(bone.channel>=0 && size_t(bone.channel)<f0.angles.size()) {
            Vec3 v0=f0.angles[size_t(bone.channel)],v1=f1.angles[size_t(bone.channel)];
            auto blend=[&](float x,float y){return x+std::remainder(y-x,4096.f)*t;};
            angle={blend(v0.x,v1.x),blend(v0.y,v1.y),blend(v0.z,v1.z)};
        }
        // Unanimated nodes with zero rotation inherit their parent's matrix.
        p.rotation=bone.channel>=0?rotation(bone.kind,angle):Mat3{};
        p.position=bone.translation;
        if(bone.parent<0)p.position=p.position+f0.translation*(1-t)+f1.translation*t;
        if(bone.parent>=0) {
            const Pose& par=out[size_t(bone.parent)];
            p.position=par.position+par.rotation.apply(p.position);
            p.rotation=par.rotation*p.rotation;
        }
        out.push_back(p);
    }
    return out;
}
} // namespace mk4
