#pragma GCC push_options
#pragma GCC target("aes")
#pragma GCC target("avx")

#include <stdint.h>
#include <string.h>
#include <immintrin.h>
#include <wmmintrin.h>

void
AESHash128( const void *p, size_t sz, uint64_t *x1, uint64_t *x2 )
{
  __m128i data[ 8 ], hash, seed = _mm_set_epi64x( *x1 + sz, *x2 );
  size_t i = 0;

  const uint8_t *e = &((const uint8_t *) p)[ sz ];
  const uint8_t *b = (const uint8_t *) p;
  hash = seed;
  if ( b != e ) {
    do {
      uint64_t lo = 0, hi = 0;
      switch ( e - b ) {
	default:
	  hi = *(uint64_t *) (void *) &b[ 8 ];
	  lo = *(uint64_t *) (void *) &b[ 0 ];
	  break;
	case 15: hi |= ( (uint64_t) b[ 14 ] ) << 56; /* FALLTHRU */
	case 14: lo |= ( (uint64_t) b[ 13 ] ) << 48; /* FALLTHRU */
	case 13: hi |= ( (uint64_t) b[ 12 ] ) << 48; /* FALLTHRU */
	case 12:
	  lo |= ( (uint64_t) *(const uint32_t *) (void *) &b[ 8 ] ) << 16;
	  hi |= ( (uint64_t) *(const uint32_t *) (void *) &b[ 4 ] ) << 16;
	  lo |= (uint64_t) *(const uint16_t *) (void *) &b[ 2 ];
	  hi |= (uint64_t) *(const uint16_t *) (void *) &b[ 0 ];
	  break;
	case 11: hi |= ( (uint64_t) b[ 10 ] ) << 40; /* FALLTHRU */
	case 10: lo |= ( (uint64_t) b[ 9 ] ) << 32; /* FALLTHRU */
	case 9: hi |= ( (uint64_t) b[ 8 ] ) << 32; /* FALLTHRU */
	case 8:
	  lo |= (uint64_t) *(const uint32_t *) (void *) &b[ 4 ];
	  hi |= (uint64_t) *(const uint32_t *) (void *) &b[ 0 ];
	  break;
	case 7: hi |= ( (uint64_t) b[ 6 ] ) << 24; /* FALLTHRU */
	case 6: lo |= ( (uint64_t) b[ 5 ] ) << 16; /* FALLTHRU */
	case 5: hi |= ( (uint64_t) b[ 4 ] ) << 16; /* FALLTHRU */
	case 4:
	  lo |= (uint64_t) *(const uint16_t *) (void *) &b[ 2 ];
	  hi |= (uint64_t) *(const uint16_t *) (void *) &b[ 0 ];
	  break;
	case 3: hi |= ( (uint64_t) b[ 2 ] ) << 8; /* FALLTHRU */
	case 2: lo |= ( (uint64_t) b[ 1 ] ); /* FALLTHRU */
	case 1: hi |= ( (uint64_t) b[ 0 ] );
	  break;
      }
      data[ i ] = _mm_xor_si128( _mm_set_epi64x( hi, lo ), seed );
      if ( ++i == 8 ) {
	while ( i > 1 ) {
	  data[ 0 ] = _mm_aesenc_si128( data[ 0 ], data[ --i ] );
	  data[ 0 ] = _mm_aesenc_si128( data[ 0 ], data[ i ] );
        }
	data[ 0 ] = _mm_aesenc_si128( data[ 0 ], seed );
	hash = _mm_aesenc_si128( hash, data[ 0 ] );
	i = 0;
      }
      b += 16;
    } while ( b < e );
    while ( i > 1 ) {
      data[ 0 ] = _mm_aesenc_si128( data[ 0 ], data[ --i ] );
      data[ 0 ] = _mm_aesenc_si128( data[ 0 ], data[ i ] );
    }
  }
  else {
    data[ 0 ] = seed;
  }
  hash = _mm_aesenc_si128( hash, data[ 0 ] );
  hash = _mm_aesenc_si128( hash, seed );
  hash = _mm_aesenc_si128( hash, seed );
  hash = _mm_aesenc_si128( hash, seed );
  *x1 = _mm_extract_epi64( hash, 0 );
  *x2 = _mm_extract_epi64( hash, 1 );
}

uint64_t
AESHash64( const void *p, size_t sz, uint64_t seed )
{
  uint64_t h1 = seed, h2 = seed;
  AESHash128( p, sz, &h1, &h2 );
  return h1;
}

