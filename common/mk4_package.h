#pragma once
#include "binary.h"

namespace mk4 {
constexpr size_t package_table_size=0x3000, package_max_entries=1024;
struct PackageEntry { uint32_t hash=0,offset=0,size=0; };
uint32_t name_hash(const std::string& path);
std::vector<PackageEntry> parse_package(View data);
class Package {
    Bytes data_;
    std::vector<PackageEntry> entries_;
public:
    explicit Package(const std::string& path);
    const std::vector<PackageEntry>& entries() const { return entries_; }
    View get(uint32_t hash) const;
    View geo(const std::string& name) const;
};
}
