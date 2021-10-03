#ifndef VECTOR_KERNELS_HPP
#define VECTOR_KERNELS_HPP

#include <cstdint>

#include <immintrin.h>

#include "zp_eliminator/zp_scalar.hpp"

namespace zp {

template <typename Word, int Width, Word P> struct VecAddOp;

template <uint16_t P> struct VecAddOp<uint16_t, 16, P> {
  using Word = uint16_t;
  using Zp = ZpScalar<P, Word>;
  inline static __m256i run(const __m256i &a, const __m256i &b,
                            const __m256i &p) {
    const __m256i sum = _mm256_add_epi16(a, b);
    const __m256i sum_sub = _mm256_sub_epi16(sum, p);
    const __m256i mask = _mm256_cmpeq_epi16(_mm256_min_epu16(sum, p), p);
    const __m256i res = _mm256_blendv_epi8(sum, sum_sub, mask);
    return res;
  }

  inline static void run(const uint16_t *ap, const uint16_t *bp, uint16_t *cp,
                         size_t N) {
    __m256i p = _mm256_set1_epi16(P);
    for (size_t i = 0; i < N; i += 16) {
      __m256i a = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(ap + i));
      __m256i b = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(bp + i));
      __m256i c = run(a, b, p);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(cp + i), c);
    }
  }

  inline static void run(const Zp *ap, const Zp *bp, Zp *cp, size_t N) {
    static_assert(sizeof(Zp) == sizeof(Word));
    __m256i p = _mm256_set1_epi16(P);
    for (size_t i = 0; i < N; i += 16) {
      __m256i a = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(ap + i));
      __m256i b = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(bp + i));
      __m256i c = run(a, b, p);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(cp + i), c);
    }
  }
};

template <typename Word, int Width, Word P> struct VecSubOp;

template <uint16_t P> struct VecSubOp<uint16_t, 16, P> {
  using Word = uint16_t;
  using Zp = ZpScalar<P, Word>;
  inline static __m256i run(const __m256i &a, const __m256i &b,
                            const __m256i &p, const __m256i &z) {
    const __m256i sub = _mm256_sub_epi16(a, b);
    const __m256i sub_add = _mm256_add_epi16(sub, p);
    const __m256i mask = _mm256_cmpgt_epi16(z, sub);
    const __m256i res = _mm256_blendv_epi8(sub, sub_add, mask);
    return res;
  }

  inline static void run(const uint16_t *ap, const uint16_t *bp, uint16_t *cp,
                         size_t N) {
    __m256i p = _mm256_set1_epi16(P);
    __m256i z = _mm256_setzero_si256();
    for (size_t i = 0; i < N; i += 16) {
      __m256i a = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(ap + i));
      __m256i b = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(bp + i));
      __m256i c = run(a, b, p, z);
      _mm256_storeu_si256(reinterpret_cast<__m256i *>(cp + i), c);
    }
  }

  inline static void run(const Zp *ap, const Zp *bp, Zp *cp, size_t N) {
    static_assert(sizeof(Zp) == sizeof(Word));
    run(reinterpret_cast<const Word *>(ap), reinterpret_cast<const Word *>(bp),
        reinterpret_cast<Word *>(cp), N);
  }
};
} // namespace zp

#endif
