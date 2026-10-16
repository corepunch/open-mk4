#ifndef MK4_MATH_UTIL_H
#define MK4_MATH_UTIL_H
#include <math.h>
#include <stddef.h>

static inline int mini(int a, int b) {
  return a < b ? a : b;
}

static inline int maxi(int a, int b) {
  return a > b ? a : b;
}

static inline size_t minz(size_t a, size_t b) {
  return a < b ? a : b;
}

static inline size_t maxz(size_t a, size_t b) {
  return a > b ? a : b;
}

static inline float clampf(float x, float lo, float hi) {
  return fminf(hi, fmaxf(lo, x));
}

static inline double clampd(double x, double lo, double hi) {
  return fmin(hi, fmax(lo, x));
}
#endif
