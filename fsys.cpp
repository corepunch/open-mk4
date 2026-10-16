/* MK4 PC FILESYS.DAT inspector, extractor and polling tool.
 * C++17 / POSIX (macOS and Linux); no external libraries.
 * Format independently checked against MK4/MK-4.EXE's FSYS routines.
 */
#define _POSIX_C_SOURCE 200809L
#define _FILE_OFFSET_BITS 64
#ifdef __APPLE__
#define _DARWIN_C_SOURCE 1
#endif
#include "common/mk4_package.h"
#include <errno.h>
#include <new>
#include <inttypes.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_ENTRIES PACKAGE_MAX_ENTRIES
#define NAME_SIZE 1024u
#define PATH_SIZE 4096u

struct Entry : PackageEntry {
  char original[NAME_SIZE];
  int ambiguous;
};

typedef struct {
  unsigned char *data;
  size_t size;
  Entry entries[MAX_ENTRIES];
  size_t count;
} Archive;

static volatile sig_atomic_t stopped;
static char error_text[PATH_SIZE + 128];

static void on_signal(int sig) {
  (void)sig;
  stopped = 1;
}

static int fail(const char *message, const char *path) {
  snprintf(error_text, sizeof error_text, "%s: %s", message, path);
  return -1;
}

static int same_stamp(const struct stat *a, const struct stat *b) {
#ifdef __APPLE__
  return a->st_dev == b->st_dev && a->st_ino == b->st_ino && a->st_size == b->st_size && a->st_mtimespec.tv_sec == b->st_mtimespec.tv_sec &&
         a->st_mtimespec.tv_nsec == b->st_mtimespec.tv_nsec && a->st_ctimespec.tv_sec == b->st_ctimespec.tv_sec &&
         a->st_ctimespec.tv_nsec == b->st_ctimespec.tv_nsec;
#else
  return a->st_dev == b->st_dev && a->st_ino == b->st_ino && a->st_size == b->st_size && a->st_mtim.tv_sec == b->st_mtim.tv_sec &&
         a->st_mtim.tv_nsec == b->st_mtim.tv_nsec && a->st_ctim.tv_sec == b->st_ctim.tv_sec && a->st_ctim.tv_nsec == b->st_ctim.tv_nsec;
#endif
}

static int read_snapshot(const char *path, unsigned char **data, size_t *size) {
  struct stat before, after, current;
  unsigned char *p;
  FILE *f = fopen(path, "rb");
  if (!f)
    return fail(strerror(errno), path);
  if (fstat(fileno(f), &before) || !S_ISREG(before.st_mode) || before.st_size < 0 || (uintmax_t)before.st_size > SIZE_MAX) {
    fclose(f);
    return fail("not a readable regular file", path);
  }
  *size = (size_t)before.st_size;
  p = static_cast<unsigned char *>(malloc(*size ? *size : 1));
  if (!p) {
    fclose(f);
    return fail("out of memory", path);
  }
  if (fread(p, 1, *size, f) != *size || fstat(fileno(f), &after) || stat(path, &current) || !same_stamp(&before, &after) || !same_stamp(&after, &current)) {
    free(p);
    fclose(f);
    return fail("could not read a stable snapshot", path);
  }
  if (fclose(f)) {
    free(p);
    return fail(strerror(errno), path);
  }
  *data = p;
  return 0;
}

static int parse_archive(Archive *a) {
  PackageEntry entries[MAX_ENTRIES];
  const char *error;
  if (!parse_package({a->data, a->size}, entries, &a->count, &error))
    return fail(error, "FILESYS.DAT");
  for (size_t i = 0; i < a->count; ++i) {
    a->entries[i] = Entry{};
    static_cast<PackageEntry &>(a->entries[i]) = entries[i];
  }
  return 0;
}

static const char *leaf_name(const char *s) {
  const char *leaf = s;
  for (; *s; ++s)
    if (*s == '\\' || *s == '/')
      leaf = s + 1;
  return leaf;
}

static int safe_leaf(const char *s) {
  size_t i;
  if (!s[0] || s[0] == '.' || strlen(s) > 180)
    return 0;
  for (i = 0; s[i]; ++i)
    if (!((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9') || s[i] == '_' || s[i] == '-' || s[i] == '.'))
      return 0;
  return 1;
}

static void try_name(Archive *a, const char *name) {
  char normalized[NAME_SIZE];
  size_t i, n = strlen(name), lo = 0, hi = a->count;
  uint32_t hash;
  Entry *e;
  if (n < 4 || n >= sizeof normalized || name[1] != ':' || name[2] != '\\' || !safe_leaf(leaf_name(name)))
    return;
  for (i = 0; i <= n; ++i) {
    unsigned char c = (unsigned char)name[i];
    if (c && (c < 32 || c > 126))
      return;
    normalized[i] = (char)((c >= 'A' && c <= 'Z') ? c + 'a' - 'A' : c);
  }
  hash = name_hash(normalized);
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    if (a->entries[mid].hash < hash)
      lo = mid + 1;
    else
      hi = mid;
  }
  if (lo == a->count || a->entries[lo].hash != hash)
    return;
  e = &a->entries[lo];
  if (e->original[0] && strcmp(e->original, normalized))
    e->ambiguous = 1;
  else
    memcpy(e->original, normalized, n + 1);
}

static void recover_names(Archive *a, const unsigned char *exe, size_t size) {
  size_t i = 0;
  unsigned int n;
  char candidate[NAME_SIZE];
  /* This format string is present in the supplied executable. */
  for (n = 0; n < 10000; ++n) {
    snprintf(candidate, sizeof candidate, "c:\\source\\mk4\\win\\sound\\pcmk%04u.esf", n);
    try_name(a, candidate);
  }
  while (i < size) {
    size_t start = i, length;
    while (i < size && exe[i] >= 32 && exe[i] <= 126)
      ++i;
    length = i - start;
    if (length >= 4 && length < sizeof candidate) {
      memcpy(candidate, exe + start, length);
      candidate[length] = 0;
      if (safe_leaf(candidate) && length > 4 && !strcmp(candidate + length - 4, ".geo")) {
        char full[NAME_SIZE];
        snprintf(full, sizeof full, "c:\\source\\mk4\\win\\geogfx\\%s", candidate);
        try_name(a, full);
      } else
        try_name(a, candidate);
    }
    if (i < size)
      ++i;
  }
}

static const char *entry_type(const Archive *a, const Entry *e) {
  const unsigned char *p = a->data + e->offset;
  if (e->size >= 8 && !memcmp(p, "ESF\006", 4))
    return "esf";
  if (e->size >= 12 && !memcmp(p, "0.1v", 4))
    return "geo";
  if (e->original[0] && !e->ambiguous && strstr(leaf_name(e->original), ".tga"))
    return "tga";
  return "bin";
}

static void entry_path(const Archive *a, const Entry *e, char *out, size_t size) {
  const char *type = entry_type(a, e);
  const char *dir = !strcmp(type, "esf") ? "sound" : !strcmp(type, "geo") ? "geogfx" : "other";
  if (e->original[0] && !e->ambiguous)
    snprintf(out, size, "%s/%08" PRIx32 "_%s", dir, e->hash, leaf_name(e->original));
  else
    snprintf(out, size, "%s/%08" PRIx32 ".%s", dir, e->hash, type);
}

static int join_path(char *out, size_t size, const char *dir, const char *name) {
  int n = snprintf(out, size, "%s/%s", dir, name);
  return n < 0 || (size_t)n >= size ? fail("output path too long", dir) : 0;
}

static int make_dirs(const char *path) {
  char buf[PATH_SIZE];
  size_t i, n = strlen(path);
  struct stat st;
  if (n >= sizeof buf)
    return fail("directory path too long", path);
  memcpy(buf, path, n + 1);
  for (i = 1; i <= n; ++i) {
    if (buf[i] && buf[i] != '/')
      continue;
    buf[i] = 0;
    /* System path prefixes may be aliases (macOS /var -> /private/var).
         * Refuse symlinks at the output root and at each generated subdirectory.
         */
    if ((mkdir(buf, 0755) && errno != EEXIST) || (i == n ? lstat(buf, &st) : stat(buf, &st)) || !S_ISDIR(st.st_mode))
      return fail("cannot create directory (output directory must not be a symlink)", buf);
    buf[i] = path[i];
  }
  return 0;
}

/* Compare content so repeated extraction does not touch unchanged files. */
static int file_matches(const char *path, const unsigned char *data, size_t size) {
  unsigned char buf[65536];
  size_t pos = 0;
  struct stat st;
  FILE *f;
  if (lstat(path, &st) || !S_ISREG(st.st_mode) || (uintmax_t)st.st_size != size)
    return 0;
  f = fopen(path, "rb");
  if (!f)
    return 0;
  while (pos < size) {
    size_t n = size - pos;
    if (n > sizeof buf)
      n = sizeof buf;
    if (fread(buf, 1, n, f) != n || memcmp(buf, data + pos, n)) {
      fclose(f);
      return 0;
    }
    pos += n;
  }
  fclose(f);
  return 1;
}

/* Same-directory rename publishes each complete file atomically. */
static int write_atomic(const char *path, const unsigned char *data, size_t size) {
  char temp[PATH_SIZE];
  int fd, n;
  FILE *f;
  if (file_matches(path, data, size))
    return 0;
  n = snprintf(temp, sizeof temp, "%s.tmp.XXXXXX", path);
  if (n < 0 || (size_t)n >= sizeof temp)
    return fail("temporary path too long", path);
  fd = mkstemp(temp);
  if (fd < 0)
    return fail(strerror(errno), path);
  f = fdopen(fd, "wb");
  if (!f) {
    close(fd);
    unlink(temp);
    return fail(strerror(errno), path);
  }
  if (fwrite(data, 1, size, f) != size) {
    fclose(f);
    unlink(temp);
    return fail("write failed", path);
  }
  if (fclose(f)) {
    unlink(temp);
    return fail("close failed", path);
  }
  if (rename(temp, path)) {
    unlink(temp);
    return fail(strerror(errno), path);
  }
  return 1;
}

static int process_archive(Archive *a, const char *output, int list) {
  size_t i, known = 0, written = 0;
  char relative[256], path[PATH_SIZE];
  /* Worst case: two 1024-byte names plus metadata per entry. */
  size_t capacity = a->count * (NAME_SIZE + 512) + 256, used;
  char *manifest = static_cast<char *>(malloc(capacity));
  if (!manifest)
    return fail("out of memory", "manifest");
  used = (size_t)snprintf(manifest, capacity, "index\thash\toffset\tsize\ttype\tpath\tname_status\toriginal_path_candidate\n");
  for (i = 0; i < a->count; ++i) {
    Entry *e = &a->entries[i];
    const char *status = e->ambiguous ? "ambiguous" : e->original[0] ? "candidate" : "unknown";
    int n;
    entry_path(a, e, relative, sizeof relative);
    if (e->original[0] && !e->ambiguous)
      ++known;
    n = snprintf(manifest + used, capacity - used, "%zu\t%08" PRIx32 "\t%" PRIu32 "\t%" PRIu32 "\t%s\t%s\t%s\t%s\n", i, e->hash, e->offset, e->size,
        entry_type(a, e), relative, status, e->original);
    if (n < 0 || (size_t)n >= capacity - used) {
      free(manifest);
      return fail("manifest too large", output);
    }
    used += (size_t)n;
  }
  if (list) {
    int rc = fwrite(manifest, 1, used, stdout) == used ? 0 : fail("listing write failed", "stdout");
    free(manifest);
    return rc;
  }
  if (make_dirs(output)) {
    free(manifest);
    return -1;
  }
  for (i = 0; i < a->count; ++i) {
    char parent[PATH_SIZE], *slash;
    int rc;
    Entry *e = &a->entries[i];
    entry_path(a, e, relative, sizeof relative);
    if (join_path(path, sizeof path, output, relative)) {
      free(manifest);
      return -1;
    }
    memcpy(parent, path, strlen(path) + 1);
    slash = strrchr(parent, '/');
    *slash = 0;
    if (make_dirs(parent)) {
      free(manifest);
      return -1;
    }
    rc = write_atomic(path, a->data + e->offset, e->size);
    if (rc < 0) {
      free(manifest);
      return -1;
    }
    written += (size_t)rc;
  }
  if (join_path(path, sizeof path, output, "manifest.tsv") || write_atomic(path, (const unsigned char *)manifest, used) < 0) {
    free(manifest);
    return -1;
  }
  free(manifest);
  printf("%zu entries, %zu filename candidates; %zu files updated in %s\n", a->count, known, written, output);
  fflush(stdout);
  return 0;
}

static void usage(FILE *f) {
  fprintf(f, "Usage: fsys ARCHIVE [--list | -o DIRECTORY] [--exe GAME.EXE] [--watch SECONDS]\n"
             "  --list          List entries as TSV without extracting\n"
             "  -o DIRECTORY    Output directory (default: extracted)\n"
             "  --exe GAME.EXE  Recover filename candidates from executable strings\n"
             "  --watch SECONDS Poll every 0.05..86400 seconds; Ctrl-C stops\n"
             "Watch mode requires two identical snapshots before extraction, including startup.\n"
             "Unchanged files are preserved; files removed from the archive are retained.\n");
}

int main(int argc, char **argv) {
  const char *input = NULL, *output = "extracted", *exe_path = NULL;
  unsigned char *exe = NULL, *previous = NULL, *published = NULL;
  size_t exe_size = 0, previous_size = 0, published_size = 0;
  double interval = 0;
  int list = 0, output_given = 0, i, result = 0, reported_error = 0;
  Archive *a = new (std::nothrow) Archive{};
  if (!a) {
    fprintf(stderr, "out of memory\n");
    return 1;
  }
  for (i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
      usage(stdout);
      delete a;
      return 0;
    } else if (!strcmp(argv[i], "--list"))
      list = 1;
    else if (!strcmp(argv[i], "-o") && i + 1 < argc) {
      output = argv[++i];
      output_given = 1;
    } else if (!strcmp(argv[i], "--exe") && i + 1 < argc)
      exe_path = argv[++i];
    else if (!strcmp(argv[i], "--watch") && i + 1 < argc) {
      char *end;
      interval = strtod(argv[++i], &end);
      if (!argv[i][0] || *end || !(interval >= 0.05 && interval <= 86400))
        goto bad_args;
    } else if (argv[i][0] == '-' || input)
      goto bad_args;
    else
      input = argv[i];
  }
  if (!input || (list && (interval || output_given)) || !output[0])
    goto bad_args;
  if (exe_path && read_snapshot(exe_path, &exe, &exe_size)) {
    fprintf(stderr, "%s\n", error_text);
    delete a;
    return 1;
  }
  signal(SIGINT, on_signal);
  signal(SIGTERM, on_signal);
  if (interval) {
    printf("Watching %s every %.3g seconds (Ctrl-C to stop)\n", input, interval);
    fflush(stdout);
  }
  do {
    int rc = read_snapshot(input, &a->data, &a->size);
    if (!rc)
      rc = parse_archive(a);
    if (rc) {
      if (!reported_error)
        fprintf(stderr, "%s\n", error_text);
      reported_error = 1;
      /* Invalid/missing snapshots break the consecutive-stability check. */
      free(previous);
      previous = NULL;
      previous_size = 0;
      if (!interval)
        result = 1;
    } else {
      int stable = !interval || (previous && previous_size == a->size && !memcmp(previous, a->data, a->size));
      int changed = !published || published_size != a->size || memcmp(published, a->data, a->size);
      if (reported_error) {
        fprintf(stderr, "Archive readable again\n");
        reported_error = 0;
      }
      if (stable && changed) {
        recover_names(a, exe, exe_size);
        if (process_archive(a, output, list)) {
          fprintf(stderr, "%s\n", error_text);
          result = 1;
          free(a->data);
          a->data = NULL;
          break;
        }
        free(published);
        published = a->data;
        published_size = a->size;
        a->data = NULL;
      }
      if (interval) {
        /* Keep an independent last snapshot; published may be the same version. */
        unsigned char *p = static_cast<unsigned char *>(realloc(previous, a->size));
        if (!p) {
          fprintf(stderr, "out of memory\n");
          result = 1;
          free(a->data);
          a->data = NULL;
          break;
        }
        previous = p;
        previous_size = a->size;
        memcpy(previous, a->data ? a->data : published, a->size);
      }
    }
    free(a->data);
    a->data = NULL;
    if (interval && !stopped) {
      struct timespec delay;
      delay.tv_sec = (time_t)interval;
      delay.tv_nsec = (long)((interval - (double)delay.tv_sec) * 1e9);
      while (!stopped && nanosleep(&delay, &delay) && errno == EINTR) {
      }
    }
  } while (interval && !stopped);
  free(previous);
  free(published);
  free(exe);
  delete a;
  return result;
bad_args:
  usage(stderr);
  delete a;
  return 2;
}
