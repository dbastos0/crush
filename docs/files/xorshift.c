#include <stdio.h>
#include <stdlib.h>

unsigned int xorshift(void)
{
  static unsigned int y = 2463534242U; /* the seed */
  y = y ^ (y << 13);
  y = y ^ (y >> 17);
  y = y ^ (y << 5);
  return y;
}

int main(void) {
  unsigned int r; 
  for (;;) {
    size_t n;
    r = xorshift();
    n = fwrite(&r, sizeof r, 1, stdout);
    if (n < 1) _exit(0);
  }
}
