#include "mk4_package.h"
#include <assert.h>
#include <stdio.h>

/* A packed named archive with colliding MK4 hashes and a zero-size entry.
 * Named lookup must compare filenames rather than choose a hash collision.
 */
int main(void) {
  uint8_t data[75] = {4, 0, 0, 0, 72};
  memcpy(data + 8, "ABCDE.W", 8);
  data[21] = 73;
  memcpy(data + 25, "EBCDA.W", 8);
  data[38] = 75;
  memcpy(data + 42, "EMPTY.W", 8);
  data[55] = 75;
  memcpy(data + 72, "abc", 3);
  Package p = {0};
  p.bytes.data = data;
  p.bytes.count = sizeof data;
  const char *error;
  assert(parse_package(view(p.bytes), p.entries, &p.count, &error));
  assert(p.count == 3);
  assert(p.entries[0].hash == p.entries[1].hash);
  View a = package_named(&p, "abcde.w"), b = package_named(&p, "EbCdA.W");
  assert(a.n == 1 && a.p[0] == 'a');
  assert(b.n == 2 && !memcmp(b.p, "bc", 2));
  assert(package_named(&p, "empty.w").n == 0);
  assert(package_get(&p, name_hash("EMPTY.W")).n == 0);
  puts("PASS: named package lookup, hash collisions and empty payloads");
  return 0;
}
