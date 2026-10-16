#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace mk4 {
struct Vec3 { float x=0, y=0, z=0; };
Vec3 operator+(Vec3 a, Vec3 b);
Vec3 operator-(Vec3 a, Vec3 b);
Vec3 operator*(Vec3 a, float b);
struct Mat3 {
    std::array<float,9> m{1,0,0,0,1,0,0,0,1};
    Vec3 apply(Vec3 v) const;
};
Mat3 operator*(const Mat3& a, const Mat3& b);
Mat3 rotation(int kind, Vec3 bam);
struct Vertex { Vec3 p, normal; float u=0, v=0; };
struct Triangle { std::array<Vertex,3> v; int texture=0; };
struct Mesh { std::vector<Triangle> triangles; };
struct Texture { int width=0, height=0; std::vector<uint32_t> pixels; };
struct Bone {
    int parent=-1, channel=-1, mesh=-1, kind=0;
    uint32_t flags=0;
    Vec3 translation;
};
struct Frame { Vec3 translation; std::vector<Vec3> angles; };
struct Clip { int id=0; std::vector<Frame> frames; };
struct Character {
    std::string name, file;
    std::vector<Mesh> meshes;
    Texture texture;
    std::vector<Bone> bones;
    int idle=0;
    float scale=1, front_yaw=0;
};
struct Pose { Mat3 rotation; Vec3 position; };
struct Sprite { int texture=0,u=0,v=0,w=0,h=0,width=0,height=0,origin_x=0,origin_y=0; };
struct Geometry {
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
    std::vector<Sprite> sprites; // Same indices as GEO block records; empty for meshes.
};
struct Arena { std::string name,file; Geometry geometry; std::vector<Bone> nodes; };
struct Hud { Geometry font,effects; std::array<int,18> names{}; };
struct Assets {
    std::vector<Character> characters;
    std::vector<Clip> clips;
    std::vector<Arena> arenas;
    Hud hud;
    void load_battle(const std::string& archive,const std::string& executable);
    void load(const std::string& archive, const std::string& executable);
};
std::vector<Pose> pose(const Character&, const Clip&, double frame);
} // namespace mk4
