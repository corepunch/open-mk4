#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace mk4 {
using Bytes = std::vector<uint8_t>;
inline void require(bool yes, const std::string& what) { if (!yes) throw std::runtime_error(what); }
struct View {
    const uint8_t* p=nullptr; size_t n=0;
    void check(size_t o,size_t z) const { require(o<=n && z<=n-o,"Truncated asset data"); }
    uint8_t u8(size_t o) const { check(o,1);return p[o]; }
    uint16_t u16(size_t o) const { check(o,2);return uint16_t(p[o])|uint16_t(p[o+1])<<8; }
    int16_t s16(size_t o) const { return int16_t(u16(o)); }
    uint32_t u32(size_t o) const { check(o,4);return uint32_t(p[o])|uint32_t(p[o+1])<<8|uint32_t(p[o+2])<<16|uint32_t(p[o+3])<<24; }
    int32_t s32(size_t o) const { return int32_t(u32(o)); }
    View sub(size_t o,size_t z) const { check(o,z);return {p+o,z}; }
    std::string string(size_t o) const {
        std::string s;
        while(o<n && u8(o)) { require(s.size()<1024,"Unterminated asset string");s+=char(u8(o++)); }
        require(o<n,"Unterminated asset string");return s;
    }
};
inline View view(const Bytes& b) { return {b.data(),b.size()}; }
Bytes read_file(const std::string& path);
}
