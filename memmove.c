#include <string.h>

void x(char *dest, char *src, size_t n)
{
  memmove(dest,src,n);
}
