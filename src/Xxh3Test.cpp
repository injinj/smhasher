#include "xxh3.h"
  
void XXH3128_test(const void *key, int len, uint32_t seed, void *out) {
  XXH128_hash_t res = XXH3_128bits_withSeed(key, len, seed);
  ((uint64_t*)out)[0] = res.low64;
  ((uint64_t*)out)[1] = res.high64;
}

void XXH364_test(const void *key, int len, uint32_t seed, void *out) {
  ((uint64_t*)out)[0] = XXH3_64bits_withSeed(key, len, seed);
}
