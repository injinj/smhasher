#include "RiskyHash.h"
  
void RiskyHash128_test(const void *key, int len, uint32_t seed, void *out) {
  uint64_t h1 = seed, h2 = seed;
  fio_risky_hash128(key, len, &h1, &h2);
  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}

void RiskyHash64_test(const void *key, int len, uint32_t seed, void *out) {
  ((uint64_t*)out)[0] = fio_risky_hash64(key, len, seed);

  /* use the first half of 128b:
  uint64_t h1 = seed, h2 = seed;
  fio_risky_hash128(key, len, &h1, &h2);
  ((uint64_t*)out)[0] = h1; */

  /* use the second half of 128b:
  uint64_t h1 = seed, h2 = seed;
  fio_risky_hash128(key, len, &h1, &h2);
  ((uint64_t*)out)[0] = h2; */
}
