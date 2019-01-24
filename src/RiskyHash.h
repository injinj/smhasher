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
static const uint64_t RISKY_PRIME_0 = 0xFBBA3FA15B22113B, // 1111101110111010001111111010000101011011001000100001000100111011
                      RISKY_PRIME_1 = 0xAB137439982B86C9; // 1010101100010011011101000011100110011000001010111000011011001001

/* Risky Hash consumption round */
#define fio_risky_consume(s, w)                                                \
  (s) ^= (w);                                                                  \
  (s) = fio_lrot64((s), 33) + (w);                                             \
  (s) *= RISKY_PRIME_0;

#define fio_risky_compress(data, len, v0, v1, v2, v3) \
  for (size_t i = len >> 5; i; --i) { \
    fio_risky_consume(v0, fio_str2u64(data));\
    fio_risky_consume(v1, fio_str2u64(data + 8)); \
    fio_risky_consume(v2, fio_str2u64(data + 16)); \
    fio_risky_consume(v3, fio_str2u64(data + 24)); \
    data += 32; \
  } \
  switch (len & 24) { \
  case 24: \
    fio_risky_consume(v2, fio_str2u64(data + 16)); \
  case 16: \
    fio_risky_consume(v1, fio_str2u64(data + 8)); \
  case 8: \
    fio_risky_consume(v0, fio_str2u64(data)); \
    data += len & 24; \
  } \
  uint64_t tmp = 0; \
  switch ((len & 7)) { \
  case 7: \
    tmp |= ((uint64_t)data[6]) << 8; \
  case 6: \
    tmp |= ((uint64_t)data[5]) << 16; \
  case 5: \
    tmp |= ((uint64_t)data[4]) << 24; \
  case 4: \
    tmp |= ((uint64_t)data[3]) << 32; \
  case 3: \
    tmp |= ((uint64_t)data[2]) << 40; \
  case 2: \
    tmp |= ((uint64_t)data[1]) << 48; \
  case 1: \
    tmp |= ((uint64_t)data[0]) << 56; \
    /* ((len >> 3) & 3) is a 0...3 value indicating consumption vector */ \
    switch ((len >> 3) & 3) { \
    case 3: \
      fio_risky_consume(v3, tmp); \
      break; \
    case 2: \
      fio_risky_consume(v2, tmp); \
      break; \
    case 1: \
      fio_risky_consume(v1, tmp); \
      break; \
    case 0: \
      fio_risky_consume(v0, tmp); \
      break; \
    } \
  }

uint64_t fio_risky_hash64(const void *data_, size_t len, uint64_t seed) {
  /* The consumption vectors initialized state */
  register uint64_t v0 = seed ^ RISKY_PRIME_1;
  register uint64_t v1 = ~seed + RISKY_PRIME_1;
  register uint64_t v2 =
      fio_lrot64(seed, 17) ^ ((~RISKY_PRIME_1) + RISKY_PRIME_0);
  register uint64_t v3 = fio_lrot64(seed, 33) + (~RISKY_PRIME_1);

  /* reading position */
  const uint8_t *data = (uint8_t *)data_;
  fio_risky_compress(data, len, v0, v1, v2, v3);

  /* merge and mix */
  uint64_t result = fio_lrot64(v0, 17) + fio_lrot64(v1, 13) +
                    fio_lrot64(v2, 47) + fio_lrot64(v3, 57);

  len ^= (len << 33);
  result += len;

  result += v0 * RISKY_PRIME_1;
  result ^= fio_lrot64(result, 13);
  result += v1 * RISKY_PRIME_1;
  result ^= fio_lrot64(result, 29);
  result += v2 * RISKY_PRIME_1;
  result ^= fio_lrot64(result, 33);
  result += v3 * RISKY_PRIME_1;
  result ^= fio_lrot64(result, 51);

  /* irreversible avalanche... I think */
  result ^= (result >> 29) * RISKY_PRIME_0;
  return result;
}


void fio_risky_hash128(const void *data_, size_t len, uint64_t *h1,  uint64_t *h2) {
  /* The consumption vectors initialized state */
  register uint64_t v0 = h1[0] ^ RISKY_PRIME_1;
  register uint64_t v1 = ~h2[0] + RISKY_PRIME_1;
  register uint64_t v2 =
      fio_lrot64(h1[0], 17) ^ ((~RISKY_PRIME_1) + RISKY_PRIME_0);
  register uint64_t v3 = fio_lrot64(h2[0], 33) + (~RISKY_PRIME_1);

  const uint8_t *data = (const uint8_t *) data_;
  fio_risky_compress(data, len, v0, v1, v2, v3);

  /* merge and mix */
  h1[0] = fio_lrot64(v0, 17) + fio_lrot64(v1, 13) +
          fio_lrot64(v2, 47) + fio_lrot64(v3, 57);
  h2[0] = fio_lrot64(v2, 17) + fio_lrot64(v3, 13) +
          fio_lrot64(v0, 47) + fio_lrot64(v1, 57);
  h1[0] += len;
  h1[0] += v0 * RISKY_PRIME_1;
  h1[0] ^= fio_lrot64(h1[0], 13);
  h1[0] += v1 * RISKY_PRIME_1;
  h1[0] ^= fio_lrot64(h1[0], 29);
  h1[0] += v2 * RISKY_PRIME_1;
  h1[0] ^= fio_lrot64(h1[0], 33);
  h1[0] += v3 * RISKY_PRIME_1;
  h1[0] ^= fio_lrot64(h1[0], 51);
  h1[0] ^= (h1[0] >> 29) * RISKY_PRIME_0;
  h2[0] ^= (h1[0] >> 29) * RISKY_PRIME_0;
}

#undef fio_risky_compress
#undef fio_risky_consume
