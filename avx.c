#include <string.h>

void *avxmemcpy(void *dest, const void *src, size_t n);

void x(char *dest, char *src, size_t n)
{
  avxmemcpy(dest,src,n);
}
