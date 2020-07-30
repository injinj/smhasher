#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <wmmintrin.h>
#include "meow_hash.h"
  
void Meow128_test(const void *key, int len, uint32_t seed, void *out) {
  /*meow_hash h = MeowHash_Accelerated( seed, len, const_cast<void *>( key ) );
  memcpy( out, &h, 128 / 8 );*/
  uint64_t h[ 2 ] = { seed, seed };
  hash_meow128( key, len, &h[ 0 ], &h[ 1 ] );
  memcpy( out, h, 128 / 8 );
}

void Meow64_test(const void *key, int len, uint32_t seed, void *out) {
  /*meow_hash h = MeowHash_Accelerated( seed, len, const_cast<void *>( key ) );
  memcpy( out, &h, 64 / 8 );*/
  uint64_t h1 = seed, h2 = seed;
  hash_meow128( key, len, &h1, &h2 );
  memcpy( out, &h1, 64 / 8 );
}
