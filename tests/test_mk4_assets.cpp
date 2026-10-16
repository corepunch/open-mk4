#include "mk4_assets.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <set>

static void check(bool ok,const char* message) { if(!ok)throw std::runtime_error(message); }
int main() {
    try {
        struct Fixture { int kind;mk4::Vec3 angles;std::array<int,9> q12; };
        const Fixture fixtures[]={
#include "fixtures/original_matrices.inc"
        };
        for(const auto& f:fixtures) {
            auto m=mk4::rotation(f.kind,f.angles);
            for(size_t i=0;i<9;++i)check(std::abs(m.m[i]-float(f.q12[i])/4096)<.002f,
                                      "Rotation differs from original x86 builder");
        }
        mk4::Assets assets;assets.load("MK4/FILESYS.DAT","MK4/MK-4.EXE");
        check(assets.characters.size()==18 && assets.clips.size()==612,"Incomplete roster/pack");
        size_t frames=0,poses=0;
        for(auto& c:assets.clips)frames+=c.frames.size();
        check(frames==36723,"Unexpected animation frame count");
        for(const auto& ch:assets.characters) {
            std::set<int> channels;int head=-1,left=-1,right=-1;
            const auto& clip=assets.clips[size_t(ch.idle)];
            int count=int(clip.frames[0].angles.size());
            for(size_t i=0;i<ch.bones.size();++i) {
                const auto& b=ch.bones[i];
                check(b.parent<int(i),"Skeleton is not in parent order");
                check(b.kind==(b.parent<0?1:2),"Missing character transform initialization pass");
                if(b.channel>=0)check(channels.insert(b.channel).second,"Duplicate animation channel");
                if(b.channel==0)head=int(i);
                if(b.channel==13)left=int(i);
                if(b.channel==14)right=int(i);
            }
            check(int(channels.size())==count && *channels.begin()==0 && *channels.rbegin()==count-1,
                  "Animation/skeleton channels differ");
            check(head>=0 && left>=0 && right>=0,"Missing head/feet");
            for(size_t f=0;f<clip.frames.size();++f) {
                auto p=mk4::pose(ch,clip,double(f));++poses;
                float feet=(p[size_t(left)].position.y+p[size_t(right)].position.y)*.5f;
                if(feet-p[size_t(head)].position.y<=.8f)throw std::runtime_error(ch.name+" frame "+std::to_string(f)+" head/feet height "+std::to_string(feet-p[size_t(head)].position.y));
                for(auto& bone:p) {
                    check(std::isfinite(bone.position.x+bone.position.y+bone.position.z),"Invalid posed position");
                    for(int row=0;row<3;++row)for(int col=0;col<3;++col) {
                        float dot=0;for(int k=0;k<3;++k)dot+=bone.rotation.m[size_t(row*3+k)]*bone.rotation.m[size_t(col*3+k)];
                        check(std::abs(dot-(row==col?1.f:0.f))<.0001f,"Joint matrix is not orthonormal");
                    }
                }
            }
            auto first=mk4::pose(ch,clip,0),loop=mk4::pose(ch,clip,double(clip.frames.size()));
            for(size_t i=0;i<first.size();++i)for(size_t k=0;k<9;++k)
                check(first[i].rotation.m[k]==loop[i].rotation.m[k],"Clip does not wrap at its frame count");
        }
        std::printf("PASS: %zu original matrix fixtures; %zu upright idle poses; 18 complete rigs; 612 clips / %zu frames\n",
                    sizeof(fixtures)/sizeof(fixtures[0]),poses,frames);
    }catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}
}
