#include <stdint.h>
#include <string.h>
#include "meow_intrinsics.h"
#include "meow_hash.h"
  
void Meow128_test(const void *key, int len, uint32_t seed, void *out) {
  meow_hash h = MeowHash_Accelerated( seed, len, const_cast<void *>( key ) );
  memcpy( out, &h, 128 / 8 );
}

void Meow64_test(const void *key, int len, uint32_t seed, void *out) {
  meow_hash h = MeowHash_Accelerated( seed, len, const_cast<void *>( key ) );
  memcpy( out, &h, 64 / 8 );
}
