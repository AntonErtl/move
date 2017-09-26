#include <string.h>

void *ssememcpy(void *dest, const void *src, size_t n);

void x(char *dest, char *src, size_t n)
{
  ssememcpy(dest,src,n);
}
