#ifndef MK4_PACKAGE_H
#define MK4_PACKAGE_H
#include "binary.h"
#ifdef __cplusplus
extern "C" {
#endif
#define PACKAGE_TABLE_SIZE 0x3000u
#define PACKAGE_MAX_ENTRIES 1024u
typedef struct PackageEntry { uint32_t hash, offset, size; } PackageEntry;
typedef struct Package { Bytes bytes; PackageEntry entries[PACKAGE_MAX_ENTRIES]; size_t count; } Package;
uint32_t name_hash(const char *path);
bool parse_package(View data, PackageEntry entries[PACKAGE_MAX_ENTRIES], size_t *count, const char **error);
void package_load(Package *package, const char *path);
void package_free(Package *package);
View package_get(const Package *package, uint32_t hash);
View package_geo(const Package *package, const char *name);
#ifdef __cplusplus
}
#endif
#endif
