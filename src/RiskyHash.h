/* Originally from https://github.com/boazsegev/facil.io */
/*
Copyright: Boaz Segev, 2018-2019
License: MIT

Feel free to copy, use and enjoy according to the license provided.
*/

/* the 64 bit should be equivalent to fio.h version
 * the 128 bit version is a slight modification of the 64 bit */

#include <stdlib.h>
#include <stdint.h>

/** Converts an unaligned network ordered byte stream to a 64 bit number. */
#define fio_str2u64(c)                                                         \
  ((uint64_t)((((uint64_t)((uint8_t *)(c))[0]) << 56) |                        \
              (((uint64_t)((uint8_t *)(c))[1]) << 48) |                        \
              (((uint64_t)((uint8_t *)(c))[2]) << 40) |                        \
              (((uint64_t)((uint8_t *)(c))[3]) << 32) |                        \
              (((uint64_t)((uint8_t *)(c))[4]) << 24) |                        \
              (((uint64_t)((uint8_t *)(c))[5]) << 16) |                        \
              (((uint64_t)((uint8_t *)(c))[6]) << 8) | (((uint8_t *)(c))[7])))

/** 64Bit left rotation, inlined. */
#define fio_lrot64(i, bits)                                                    \
  (((uint64_t)(i) << ((bits)&63UL)) | ((uint64_t)(i) >> ((-(bits)) & 63UL)))

/* The primes used by Risky Hash */
static const uint64_t FIO_RISKY_PRIME_0 = 0xFBBA3FA15B22113B, // 1111101110111010001111111010000101011011001000100001000100111011
                      FIO_RISKY_PRIME_1 = 0xAB137439982B86C9; // 1010101100010011011101000011100110011000001010111000011011001001

/* Risky Hash consumption round */
#define fio_risky_consume(w, i)                                                \
  v[i] ^= (w);                                                                 \
  v[i] = fio_lrot64(v[i], 33) + (w);                                           \
  v[i] *= FIO_RISKY_PRIME_0;

/* compilers could, hopefully, optimize this code for SIMD */
#define fio_risky_consume256(w0, w1, w2, w3)                                   \
  fio_risky_consume(w0, 0);                                                    \
  fio_risky_consume(w1, 1);                                                    \
  fio_risky_consume(w2, 2);                                                    \
  fio_risky_consume(w3, 3);

#define fio_risky_compress(data, len, v) \
  for (size_t i = len >> 5; i; --i) { \
    fio_risky_consume256(fio_str2u64(data), fio_str2u64(data + 8), \
                         fio_str2u64(data + 16), fio_str2u64(data + 24)); \
    data += 32; \
  } \
  switch (len & 24) { \
  case 24: \
    fio_risky_consume(fio_str2u64(data + 16), 2); \
  case 16: \
    fio_risky_consume(fio_str2u64(data + 8), 1); \
  case 8: \
    fio_risky_consume(fio_str2u64(data), 0); \
    data += len & 24; \
  } \
  uint64_t tmp = 0; \
  switch ((len & 7)) { \
  case 7: \
    tmp |= ((uint64_t)data[6]) << 56; \
  case 6: \
    tmp |= ((uint64_t)data[5]) << 48; \
  case 5: \
    tmp |= ((uint64_t)data[4]) << 40; \
  case 4: \
    tmp |= ((uint64_t)data[3]) << 32; \
  case 3: \
    tmp |= ((uint64_t)data[2]) << 24; \
  case 2: \
    tmp |= ((uint64_t)data[1]) << 16; \
  case 1: \
    tmp |= ((uint64_t)data[0]) << 8; \
    fio_risky_consume(tmp, 3); \
  }

uint64_t fio_risky_hash64(const void *data_, size_t len, uint64_t seed) {
  /* The consumption vectors initialized state */
  uint64_t v[4] = {
      seed ^ FIO_RISKY_PRIME_1,
      ~seed + FIO_RISKY_PRIME_1,
      fio_lrot64(seed, 17) ^ FIO_RISKY_PRIME_1,
      fio_lrot64(seed, 33) + FIO_RISKY_PRIME_1,
  };

  const uint8_t *data = (const uint8_t *) data_;
  fio_risky_compress(data, len, v);

  /* merge and mix */
  uint64_t result = fio_lrot64(v[0], 17) + fio_lrot64(v[1], 13) +
                    fio_lrot64(v[2], 47) + fio_lrot64(v[3], 57);
  result += len;
  result += v[0] * FIO_RISKY_PRIME_1;
  result ^= fio_lrot64(result, 13);
  result += v[1] * FIO_RISKY_PRIME_1;
  result ^= fio_lrot64(result, 29);
  result += v[2] * FIO_RISKY_PRIME_1;
  result ^= fio_lrot64(result, 33);
  result += v[3] * FIO_RISKY_PRIME_1;
  result ^= fio_lrot64(result, 51);

  /* irreversible avalanche... I think */
  result ^= (result >> 29) * FIO_RISKY_PRIME_0;
  return result;
}


void fio_risky_hash128(const void *data_, size_t len, uint64_t *h1,  uint64_t *h2) {
  /* The consumption vectors initialized state */
  uint64_t v[4] = {
      h1[0] ^ FIO_RISKY_PRIME_1,
      ~h2[0] + FIO_RISKY_PRIME_1,
      fio_lrot64(h1[0], 17) ^ FIO_RISKY_PRIME_1,
      fio_lrot64(h2[0], 33) + FIO_RISKY_PRIME_1,
  };

  const uint8_t *data = (const uint8_t *) data_;
  fio_risky_compress(data, len, v);

  /* merge and mix */
  h1[0] = fio_lrot64(v[0], 17) + fio_lrot64(v[1], 13) +
          fio_lrot64(v[2], 47) + fio_lrot64(v[3], 57);
  h2[0] = fio_lrot64(v[2], 17) + fio_lrot64(v[3], 13) +
          fio_lrot64(v[0], 47) + fio_lrot64(v[1], 57);
  h1[0] += len;
  h1[0] += v[0] * FIO_RISKY_PRIME_1;
  h1[0] ^= fio_lrot64(h1[0], 13);
  h1[0] += v[1] * FIO_RISKY_PRIME_1;
  h1[0] ^= fio_lrot64(h1[0], 29);
  h1[0] += v[2] * FIO_RISKY_PRIME_1;
  h1[0] ^= fio_lrot64(h1[0], 33);
  h1[0] += v[3] * FIO_RISKY_PRIME_1;
  h1[0] ^= fio_lrot64(h1[0], 51);
  h1[0] ^= (h1[0] >> 29) * FIO_RISKY_PRIME_0;
  h2[0] ^= (h1[0] >> 29) * FIO_RISKY_PRIME_0;
}

#undef fio_risky_consume256
#undef fio_risky_consume
