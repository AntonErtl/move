#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

void *testmemcpy(void *dest, const void *src, size_t n);

int main()
{
  size_t i,j,k;
  int ok=1;
  char *dest1 = mmap(NULL, 3*4096, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1,0);
  char *ref1  = mmap(NULL, 3*4096, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1,0);
  char *src1  = mmap(NULL, 3*4096, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1,0);
  mprotect(dest1+4096, 4096, PROT_READ|PROT_WRITE);
  mprotect( ref1+4096, 4096, PROT_READ|PROT_WRITE);
  mprotect( src1+4096, 4096, PROT_READ|PROT_WRITE);
  FILE *rf = fopen("/dev/urandom","r");
  fread(src1+4096,4096,1,rf);
  for (i=0; i<4096; i++) {
    for (j=0; j<258 && j<4096-i; j++) {
      for (k=0; k<32; k++) {
        memset(dest1+4096,0,4096);
        memset(ref1+4096,0,4096);
        testmemcpy(dest1+4096+i,src1+8192-j-k,j);
        memcpy    ( ref1+4096+i,src1+8192-j-k,j);
        if (memcmp(dest1+4096,ref1+4096,4096)) {
          printf("difference for i=%ld, j=%ld, k=%ld\n",i,j,k);
          ok = 0;
        }
      }
    }
  }
  if (ok)
    printf("no differences found\n");
  return !ok;
}
        
