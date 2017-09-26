#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

void x(char *dest, char *src, size_t n);

int main(int argc, char **argv)
{
  size_t blocksize;
  int align;
  size_t i,j,k;
  char *mem=NULL, *mem1;
  if (argc!=3) {
    fprintf(stderr,"Usage: %s <blocksize> <align (0|1)>",argv[0]);
    exit(1);
  }
  blocksize=atol(argv[1]);
  posix_memalign((void **)&mem,32,2*blocksize+8192);
  mem1 = mem+blocksize+4096;
  memset(mem,1,blocksize+4096);
  align=atoi(argv[2]);
  if (align)
    for (k=0; k<1024; k++)
      for (i=0; i<4096; i+=32)
        for (j=0; j<4096; j+=32)
          x(mem1+j,mem+i,blocksize);
  else
    for (i=0; i<4096; i++)
      for (j=0; j<4096; j++)
        x(mem1+j,mem+i,blocksize);
  return 0;
}
