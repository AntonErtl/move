#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

void x(char *dest, char *src, size_t n);

int main(int argc, char **argv)
{
  size_t i,j;
  int nbs = argc-1;
  uint8_t bs[nbs];
  uint8_t randoms[4096];
  char *mem=NULL, *mem1;
  if (argc<2) {
    fprintf(stderr,"Usage: %s <blocksize1>...<blocksizen>",argv[0]);
    exit(1);
  }
  for (i=1; i<argc; i++)
    bs[i-1] = atol(argv[i]);

  for (i=0; i<4096; i++)
    /* use random() without initialization for repeatable results */
    randoms[i] = bs[random()%nbs]; 
  posix_memalign((void **)&mem,32,512+8192);
  mem1 = mem+512+4096;
  memset(mem,1,512+4096);
  for (i=0; i<4096; i++)
    for (j=0; j<4096; j++)
      x(mem1+j,mem+i,randoms[j]);
  return 0;
}
