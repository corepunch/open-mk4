#include "mk4_package.h"
#include <algorithm>
#include <fstream>

namespace mk4 {
Bytes read_file(const std::string& path) {
    std::ifstream f(path,std::ios::binary|std::ios::ate);
    require(bool(f),"Cannot open "+path);
    auto n=f.tellg();require(n>=0 && n<512*1024*1024,"Invalid file size: "+path);
    Bytes b(size_t(n),0);f.seekg(0);
    if(!b.empty())f.read(reinterpret_cast<char*>(b.data()),std::streamsize(b.size()));
    require(bool(f),"Cannot read "+path);return b;
}
uint32_t name_hash(const std::string& path) {
    uint32_t h=0;
    for(size_t i=0;i<path.size();++i) {
        auto c=uint8_t(path[i]);if(c>='a'&&c<='z')c-=32;
        h+=uint32_t(c)<<((i%4)*8);
    }
    return h+uint32_t(path.size());
}
std::vector<PackageEntry> parse_package(View data) {
    require(data.n>=package_table_size,"truncated archive index");
    std::vector<PackageEntry> out;bool padding=false;
    for(size_t p=0;p<package_table_size;p+=12) {
        PackageEntry e{data.u32(p),data.u32(p+4),data.u32(p+8)};
        if(!e.hash&&!e.offset&&!e.size){padding=true;continue;}
        require(!padding && e.hash && (out.empty()||e.hash>out.back().hash),"invalid index ordering or padding");
        require(e.offset>=package_table_size && uint64_t(e.offset)+e.size<=data.n,"entry extends outside archive payload");
        for(const auto& prev:out)
            require(!e.size || !prev.size || uint64_t(e.offset)>=uint64_t(prev.offset)+prev.size ||
                    uint64_t(prev.offset)>=uint64_t(e.offset)+e.size,"overlapping payload entries");
        out.push_back(e);
    }
    require(!out.empty(),"empty archive index");return out;
}
Package::Package(const std::string& path):data_(read_file(path)),entries_(parse_package(view(data_))) {}
View Package::get(uint32_t hash) const {
    auto it=std::lower_bound(entries_.begin(),entries_.end(),hash,[](auto e,uint32_t h){return e.hash<h;});
    require(it!=entries_.end()&&it->hash==hash,"Missing archive entry "+std::to_string(hash));
    return view(data_).sub(it->offset,it->size);
}
View Package::geo(const std::string& name) const { return get(name_hash("c:\\source\\mk4\\win\\geogfx\\"+name)); }
}
