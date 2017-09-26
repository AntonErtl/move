#include <string.h>

void x(char *dest, char *src, size_t n)
{
  asm("cld");
  asm volatile ("rep movsb"
                : /* no outputs */
                : "D" (dest),
                  "S" (src),
                  "c" (n)
                : "memory");
}
