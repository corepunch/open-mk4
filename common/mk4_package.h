#ifndef MK4_PACKAGE_H
#define MK4_PACKAGE_H
#include "binary.h"
#ifdef __cplusplus
extern "C" {
#endif
#define PACKAGE_TABLE_SIZE 0x3000u
#define PACKAGE_MAX_ENTRIES 1024u

typedef struct PackageEntry {
  uint32_t hash, offset, size;
  char name[14]; /* War Gods directory name; empty for hashed MK4 entries. */
} PackageEntry;

typedef struct Package {
  Bytes bytes;
  PackageEntry entries[PACKAGE_MAX_ENTRIES];
  size_t count;
} Package;

uint32_t name_hash(const char *path);
bool parse_package(View data, PackageEntry entries[PACKAGE_MAX_ENTRIES], size_t *count, const char **error);
void package_load(Package *package, const char *path);
void package_free(Package *package);
View package_get(const Package *package, uint32_t hash);
View package_named(const Package *package, const char *name);
View package_geo(const Package *package, const char *name);
#ifdef __cplusplus
}
#endif
#endif
