#include "umfileint.h"

#define DO_SWAP(x, y) {t = p[x]; p[x] = p[y]; p[y] = t;}

void swap_bytes_sgl(void *ptr, size_t num_words)
{
  int i;
  char *p;
  char t;

  p = (char*) ptr;
  for (i = 0; i < num_words; i++)
    {
      DO_SWAP(3, 0);
      DO_SWAP(2, 1);
      p += 4;  
    }
}

void swap_bytes_dbl(void *ptr, size_t num_words)
{
  int i;
  char *p;
  char t;

  p = (char*) ptr;
  for (i = 0; i < num_words; i++)
    {
      DO_SWAP(7, 0);
      DO_SWAP(6, 1);
      DO_SWAP(5, 2);
      DO_SWAP(4, 3);
      p += 8;      
    }
}


/* for a 16-bit int, return the int that has the two bytes swapped */
int16_t int16_swap_bytes(int16_t x)
{
  int8_t *in, out[2];
  in = (int8_t *) &x;
  out[0] = in[1];
  out[1] = in[0];
  return *(int16_t *) out;
}
