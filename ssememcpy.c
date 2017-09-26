#include <string.h>
#include <emmintrin.h>
#include <stddef.h>
#include <stdint.h>

/* the calls to memcpy() here implement unaligned accesses, and are
   compiled by gcc to mov instructions (even if you call this function
   memcpy, too) */
void *ssememcpy(void *dest, const void *src, size_t n)
{
  if (n<9) {
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
    } else { /* n in 5..8 */
        int temp1, temp2;
        memcpy(&temp1,src,4);
        memcpy(&temp2,src+(n-4),4);
        memcpy(dest,&temp1,4);
        memcpy(dest+(n-4),&temp2,4);
    }
  } else {
    if (n<33) {
      if (n<17) { /* 9..16 */
        long temp1, temp2;
        memcpy(&temp1,src,8);
        memcpy(&temp2,src+(n-8),8);
        memcpy(dest,&temp1,8);
        memcpy(dest+(n-8),&temp2,8);
      } else { /* 17..32 */
        __m128i temp1, temp2;
        temp1 = _mm_loadu_si128(src);
        temp2 = _mm_loadu_si128(src+(n-16));
        _mm_storeu_si128(dest, temp1);
        _mm_storeu_si128(dest+(n-16), temp2);
      }
    } else { /* n>32 */
      __m128i x = _mm_loadu_si128((__m128i *)src);
      ptrdiff_t off = src-dest;
      void *dlast = dest+n-16;
      _mm_storeu_si128((__m128i *)dest, x);
      void *d = (void *)(((intptr_t)(dest+16))&~15);
      for (; d<dlast-16; d+=32) {
        __m128i x1 = _mm_loadu_si128((__m128i *)(d+off));
        __m128i x2 = _mm_loadu_si128((__m128i *)(d+off+16));
        _mm_storeu_si128((__m128i *)d, x1);
        _mm_storeu_si128((__m128i *)(d+16), x2);
      }
      if (d<dlast) {
        x = _mm_loadu_si128((__m128i *)(d+off));
        _mm_storeu_si128((__m128i *)d, x);
      }
      x = _mm_loadu_si128((__m128i *)(dlast+off));
      _mm_storeu_si128((__m128i *)dlast, x);
    }
  }
  return dest;
}
