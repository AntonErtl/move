#include <string.h>
#include <x86intrin.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* the cases n<64 are the same as for memcpy (src is read completely
   before dest is stored), the difference is only for the larger cases */

/* on Zen better use ssememcpy: its about as fast for most cases, and
   for block sizes 5..63 where avxmemcpy uses _mm256_maskload_ps/
   _mm256_maskstore_ps is slow on Zen */

/* the calls to memcpy() here implement unaligned accesses, and are
   compiled by gcc to mov instructions (even if you call this function
   memcpy, too) */
void *avxmemmove(void *dest, const void *src, size_t n)
{
  static unsigned mask1[] = {~0,~0,~0,~0,~0,~0,~0,~0,
                              0, 0, 0, 0, 0, 0, 0, 0};
  if (n<5) {
    if (n<2) {
      if (n==1) {
        *(char *)dest = *(char *)src;
      }
    } else { /* n in 2..4 */
      short temp1, temp2;
      memcpy(&temp1,src,2);
      memcpy(&temp2,src+(n-2),2);
      memcpy(dest,&temp1,2);
      memcpy(dest+(n-2),&temp2,2);
    }
  } else {
    if (n<64) { /* n in 5..63 */
      size_t masklen = (n>>1)&~3; /* actual mask len is 4 bytes longer, 
                                     that's why this case does not cover n=64 */
      __m256i mask = (__m256i)_mm256_loadu_ps((float const*)(((char *)mask1)+28-masklen));
      __m256 p1 = _mm256_maskload_ps((float const *)src, mask);
      __m256 p2 = _mm256_maskload_ps((float const *)(src+(n-masklen-4)), mask);
      _mm256_maskstore_ps((float *)dest, mask, p1);
      _mm256_maskstore_ps((float *)(dest+(n-masklen-4)), mask, p2);
    } else { /* n>=64 */
      void *dlast = dest+n-32;
      uintptr_t off = src-dest;
      __m256i x3 = _mm256_loadu_si256((__m256i *)src);
      __m256i x4 = _mm256_loadu_si256((__m256i *)(dlast+off));
      /* the following test succeeds in all cases where forward stride works.
         this bias towards one case leads to good branch prediction.
         to bias for backwards stride, use "off < n" */
      if (off <= -n) {
        void *d = (void *)(((intptr_t)(dest+32))&~31);
#ifdef NO_UNROLLING
        for (; d<dlast; d+=32) {
          __m256i x = _mm256_loadu_si256((__m256i *)(d+off));
          _mm256_storeu_si256((__m256i *)d, x);
        }
#else
        for (; d<dlast-32; d+=64) {
          __m256i x1 = _mm256_loadu_si256((__m256i *)(d+off));
          __m256i x2 = _mm256_loadu_si256((__m256i *)(d+off+32));
          _mm256_storeu_si256((__m256i *)d, x1);
          _mm256_storeu_si256((__m256i *)(d+32), x2);
        }
        if (d<dlast) {
          __m256i x = _mm256_loadu_si256((__m256i *)(d+off));
          _mm256_storeu_si256((__m256i *)d, x);
        }
#endif
        _mm256_storeu_si256((__m256i *)dest, x3);
        _mm256_storeu_si256((__m256i *)dlast, x4);
      } else {
        void *d = (void *)(((uintptr_t)dlast)&~31);
#ifdef NO_UNROLLING
        for (; d>=dest+32; d-=32) {
          __m256i x = _mm256_loadu_si256((__m256i *)(d+off));
          _mm256_storeu_si256((__m256i *)d, x);
        }
#else
        for (; d>=dest+32; d-=64) {
          __m256i x1 = _mm256_loadu_si256((__m256i *)(d+off));
          __m256i x2 = _mm256_loadu_si256((__m256i *)(d+off-32));
          _mm256_storeu_si256((__m256i *)d, x1);
          _mm256_storeu_si256((__m256i *)(d-32), x2);
        }
        if (d>=dest) {
          __m256i x = _mm256_loadu_si256((__m256i *)(d+off));
          _mm256_storeu_si256((__m256i *)d, x);
        }
#endif
        _mm256_storeu_si256((__m256i *)dest, x3);
        _mm256_storeu_si256((__m256i *)dlast, x4);
      }
    }
  }
  return dest;
}
