#include "AESHash.h"
  
void AESHash128_test(const void *key, int len, uint32_t seed, void *out) {
  uint64_t h1 = seed, h2 = seed;
  AESHash128(key, len, &h1, &h2);
  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}

void AESHash64_test(const void *key, int len, uint32_t seed, void *out) {
  uint64_t out2[ 2 ];
  AESHash128_test((const char *)key,len,seed, out2);
  ((uint64_t*)out)[0] = out2[0];
}
