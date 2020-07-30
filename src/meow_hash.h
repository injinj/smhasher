/* ========================================================================

   Meow - A Fast Non-cryptographic Hash
   (C) Copyright 2018 by Molly Rocket, Inc. (https://mollyrocket.com)
   
   See https://mollyrocket.com/meowhash for details.
*/   

/* based on meow_hash v0.4 (https://github.com/cmuratori/meow_hash)
 * Same algo, except that it uses 128 bit seed and mixes the seed into the
 * initial state and uses 2 AESDEC rounds instead of 1.
 */
static inline __m128i
Meow128_AESDEC_Memx2( __m128i R, const uint8_t *S )
{
  R = _mm_aesdec_si128((R), _mm_loadu_si128((__m128i *)(S)));
  R = _mm_aesdec_si128((R), _mm_loadu_si128((__m128i *)(S)));
  return R;
}

static inline __m128i
Meow128_AESDECx2( __m128i R, __m128i S )
{
  R = _mm_aesdec_si128((R), (S));
  R = _mm_aesdec_si128((R), (S));
  return R;
}

static void
hash_meow128( const void *p, size_t sz, uint64_t *x1, uint64_t *x2 )
{
  static const uint8_t MeowShiftAdjust[] = {0,1,2,3,         4,5,6,7,
                                            8,9,10,11,       12,13,14,15,
                                            128,128,128,128, 128,128,128,128,
                                            128,128,128,128, 128,128,128,0};
  static const uint8_t MeowMaskLen[]     = {255,255,255,255, 255,255,255,255,
                                            255,255,255,255, 255,255,255,255,
                                            0,0,0,0,         0,0,0,0,
                                            0,0,0,0,         0,0,0,0};
  static const uint8_t MeowS0Init[]      = { 0, 1, 2, 3,  4, 5, 6, 7,
                                             8, 9,10,11,  12,13,14,15};
  static const uint8_t MeowS1Init[]      = {16,17,18,19,  20,21,22,23,
                                            24,25,26,27,  28,29,30,31};
  static const uint8_t MeowS2Init[]      = {32,33,34,35,  36,37,38,39,
                                            40,41,42,43,  44,45,46,47};
  static const uint8_t MeowS3Init[]      = {48,49,50,51,  52,53,54,55,
                                            56,57,58,59,  60,61,62,63};
  __m128i S0 = *(__m128i *) MeowS0Init;
  __m128i S1 = *(__m128i *) MeowS1Init;
  __m128i S2 = *(__m128i *) MeowS2Init;
  __m128i S3 = *(__m128i *) MeowS3Init;

  const uint8_t * Source = (const uint8_t *) p;
  size_t          Len    = sz;
  const uint32_t  Len8   = (uint32_t) Len & 15;
  const uint32_t  Len128 = (uint32_t) Len & 48;

  __m128i Mixer = _mm_set_epi64x(*x2 + sz + 1, *x1 - sz);
  S0 ^= Mixer;
  S1 ^= Mixer;
  S2 ^= Mixer;
  S3 ^= Mixer;

  while ( Len >= 64 ) {
    S0 = Meow128_AESDEC_Memx2(S0, Source);
    S1 = Meow128_AESDEC_Memx2(S1, Source + 16);
    S2 = Meow128_AESDEC_Memx2(S2, Source + 32);
    S3 = Meow128_AESDEC_Memx2(S3, Source + 48);

    Len    -= 64;
    Source += 64;
  }

  if ( Len8 ) {
    __m128i         Partial;
    const uint8_t * Overhang = Source + Len128;
    uint32_t        Align    = ((size_t)(intptr_t) Overhang) & 15;

    if ( Align != 0 ) { /* presumes page size is >= 4k */
      uint32_t End = ((size_t)(intptr_t) Overhang) & (size_t) (4096 - 1);

      if ( End <= (4096 - 16) || (End + Len8) > 4096 )
        Align = 0;

      Partial = _mm_shuffle_epi8(
        _mm_loadu_si128(
          (__m128i *)( Overhang - Align ) ),
            _mm_loadu_si128( (__m128i *) &MeowShiftAdjust[ Align ] ) );

      Partial =
        _mm_and_si128( Partial,
          _mm_loadu_si128( (__m128i *) &MeowMaskLen[ 16 - Len8 ] ) );
    }
    else {
      Partial =
        _mm_and_si128( *(__m128i *) Overhang,
          _mm_loadu_si128( (__m128i *) &MeowMaskLen[ 16 - Len8 ] ) );
    }
    S3 = Meow128_AESDECx2(S3, Partial);
  }
  switch ( Len128 ) {
    case 48: S2 = Meow128_AESDEC_Memx2(S2, Source + 32); /* FALLTHRU */
    case 32: S1 = Meow128_AESDEC_Memx2(S1, Source + 16); /* FALLTHRU */
    case 16: S0 = Meow128_AESDEC_Memx2(S0, Source); break;
  }

  S3 = _mm_aesdec_si128(S3, Mixer);
  S2 = _mm_aesdec_si128(S2, Mixer);
  S1 = _mm_aesdec_si128(S1, Mixer);
  S0 = _mm_aesdec_si128(S0, Mixer);

  S2 = _mm_aesdec_si128(S2, S3);
  S0 = _mm_aesdec_si128(S0, S1);

  S2 = _mm_aesdec_si128(S2, Mixer);

  S0 = _mm_aesdec_si128(S0, S2);
  S0 = _mm_aesdec_si128(S0, Mixer);

  *x1 = _mm_extract_epi64(S0, 0);
  *x2 = _mm_extract_epi64(S0, 1);
}

