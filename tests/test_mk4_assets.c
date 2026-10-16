#include "mk4_assets.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void check(bool ok,const char *message) {require(ok,message);}
int main(void) {
    struct Fixture {int kind;Vec3 angles;struct {int values[9];} q12;};
    const struct Fixture fixtures[]={
#include "fixtures/original_matrices.inc"
    };
    for(size_t f=0;f<sizeof fixtures/sizeof *fixtures;++f) {
        Mat3 m=rotation(fixtures[f].kind,fixtures[f].angles);
        for(size_t i=0;i<9;++i)check(fabsf(m.m[i]-(float)fixtures[f].q12.values[i]/4096)<.002f,"Rotation differs from original x86 builder");
    }
    Assets assets={0};assets_load(&assets,"MK4/FILESYS.DAT","MK4/MK-4.EXE");
    check(assets.characters.count==18 && assets.clips.count==612,"Incomplete roster/pack");
    size_t frames=0,poses=0;for(size_t i=0;i<assets.clips.count;++i)frames+=assets.clips.data[i].frames.count;
    check(frames==36723,"Unexpected animation frame count");
    for(size_t c=0;c<assets.characters.count;++c) {
        const Character *ch=&assets.characters.data[c];const Clip *clip=&assets.clips.data[ch->idle];
        bool channels[MK4_MAX_BONES]={0};int channel_count=0,head=-1,left=-1,right=-1,count=(int)clip->frames.data[0].angles.count;
        for(size_t i=0;i<ch->bones.count;++i) {
            const Bone *b=&ch->bones.data[i];check(b->parent<(int)i,"Skeleton is not in parent order");
            check(b->kind==(b->parent<0?1:2),"Missing character transform initialization pass");
            if(b->channel>=0) {
                check(b->channel<count && b->channel<MK4_MAX_BONES,"Animation channel out of range");
                check(!channels[b->channel],"Duplicate animation channel");channels[b->channel]=true;++channel_count;
            }
            if(b->channel==0)head=(int)i;if(b->channel==13)left=(int)i;if(b->channel==14)right=(int)i;
        }
        check(channel_count==count,"Animation/skeleton channels differ");
        for(int i=0;i<count;++i)check(channels[i],"Missing animation channel");
        check(head>=0 && left>=0 && right>=0,"Missing head/feet");
        for(size_t f=0;f<clip->frames.count;++f) {
            Pose p[MK4_MAX_BONES];pose(ch,clip,(double)f,p);++poses;
            float feet=(p[left].position.y+p[right].position.y)*.5f;
            if(feet-p[head].position.y<=.8f)fatal("%s frame %zu head/feet height %f",ch->name,f,(double)(feet-p[head].position.y));
            for(size_t i=0;i<ch->bones.count;++i) {
                const Pose *bone=&p[i];check(isfinite(bone->position.x+bone->position.y+bone->position.z),"Invalid posed position");
                for(int row=0;row<3;++row)for(int col=0;col<3;++col) {
                    float dot=0;for(int k=0;k<3;++k)dot+=bone->rotation.m[row*3+k]*bone->rotation.m[col*3+k];
                    check(fabsf(dot-(row==col?1.f:0.f))<.0001f,"Joint matrix is not orthonormal");
                }
            }
        }
        Pose first[MK4_MAX_BONES],loop[MK4_MAX_BONES];pose(ch,clip,0,first);pose(ch,clip,(double)clip->frames.count,loop);
        for(size_t i=0;i<ch->bones.count;++i)for(size_t k=0;k<9;++k)check(first[i].rotation.m[k]==loop[i].rotation.m[k],"Clip does not wrap at its frame count");
    }
    assets_free(&assets);check(!assets.characters.data && !assets.clips.data,"Asset ownership was not released");
    printf("PASS: %zu original matrix fixtures; %zu upright idle poses; 18 complete rigs; 612 clips / %zu frames\n",sizeof fixtures/sizeof *fixtures,poses,frames);
    return 0;
}
