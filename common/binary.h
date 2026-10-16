#ifndef MK4_BINARY_H
#define MK4_BINARY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#define MK4_NORETURN [[noreturn]]
extern "C" {
#else
#define MK4_NORETURN _Noreturn
#endif

/* Internal command-line applications fail with a diagnostic on invalid assets.
 * The package validator itself returns errors so the watcher can retry. */
MK4_NORETURN void fatal(const char *format, ...);
void require(bool condition, const char *message);
void *checked_alloc(size_t count, size_t size);
void *array_resize(void *data, size_t *count, size_t *capacity, size_t n, size_t size);
#define ARRAY_TYPE(Name, Type)                                                                                                                                 \
  typedef struct Name {                                                                                                                                        \
    Type *data;                                                                                                                                                \
    size_t count, capacity;                                                                                                                                    \
  } Name
#define RESIZE(a, n) (a).data = array_resize((a).data, &(a).count, &(a).capacity, (n), sizeof *(a).data)
#define PUSH(a, ...)                                                                                                                                           \
  do {                                                                                                                                                         \
    RESIZE(a, (a).count + 1);                                                                                                                                  \
    (a).data[(a).count - 1] = (__VA_ARGS__);                                                                                                                   \
  } while (0)
#define FREE_ARRAY(a)                                                                                                                                          \
  do {                                                                                                                                                         \
    free((a).data);                                                                                                                                            \
    memset(&(a), 0, sizeof(a));                                                                                                                                \
  } while (0)
ARRAY_TYPE(Bytes, uint8_t);

typedef struct View {
  const uint8_t *p;
  size_t n;
} View;

View view(Bytes bytes);
void view_check(View v, size_t offset, size_t size);
uint8_t view_u8(View v, size_t offset);
uint16_t view_u16(View v, size_t offset);
int16_t view_s16(View v, size_t offset);
uint32_t view_u32(View v, size_t offset);
int32_t view_s32(View v, size_t offset);
View view_sub(View v, size_t offset, size_t size);
void view_string(View v, size_t offset, char *output, size_t capacity);
Bytes read_file(const char *path);
#ifdef __cplusplus
}
#endif
#endif
