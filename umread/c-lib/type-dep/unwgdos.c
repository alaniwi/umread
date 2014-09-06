
/*
 *  Modified version of unwgdos to remove the dependence on Cray utilities.
 *  It is for use on machines with native IEEE integer types.
 *
 *  Note API change compared to version taken from xconv: datain is void* and 
 *  second argument is number of bytes (not ints)
 */

/* unwgdos.c is unpack.c file from xconv but with GRIB stuff stripped out */

#include <math.h>

#include "umfileint.h"
#include "stdlib.h"

#define TRUE 1
#define FALSE 0

static int xpnd(int, int32_t *, REAL *, REAL, int, REAL, int, REAL);
static int extrin(int32_t *, int, int, int, int *, int);
static int bit_test(void *, int);
static void move_bits(void *, int, int, void *);

int unwgdos(void *datain, int nbytes, REAL *dataout, int nout, REAL mdi)
{
  int /* len, */ isc, ix, iy;
  REAL prec, base;
  int icx, j;
  int ibit, nop;
  int swap;
  int16_t *datain16;
  int32_t *datain32;
  float32_t *datainr32;
  
  datain16 = (int16_t *) datain;
  datain32 = (int32_t *) datain;
  datainr32 = (float32_t *) datain;
  
  /* Determine if data needs byte swapping */
  
  swap = -1;
  
  ix = datain16[4];
  iy = datain16[5];
  if (ix*iy == nout) swap = 0;
  
  if (swap == -1)
    {
      /* see if data is byte swapped with 4 byte words */
      ix = int16_swap_bytes(datain16[7]);
      iy = int16_swap_bytes(datain16[6]);
      if (ix*iy == nout) swap = 4;
    }
  
  if (swap == -1)
    {
      /* see if data is byte swapped with 8 byte words */       
      ix = int16_swap_bytes(datain16[3]);
      iy = int16_swap_bytes(datain16[2]);
      if (ix*iy == nout) swap = 8;
    }
  
  if (swap == -1)
    {
      error_mesg("WGDOS data header record mismatch ");
      return 1;
    }
  else if (swap == 4)
    {
      swap_bytes_sgl(datain, nbytes / 4);
    }
  else if (swap == 8)
    {
      swap_bytes_dbl(datain, nbytes / 8);
    }
  
  
  /* Below only works for 32 bit integers, therefore there must be a 
     32 bit integer type */
  
  /* Extract scale factor and number of columns and rows from header */
  /* len = datain32[0]; */ /* not used?? */
  isc = datain32[1];
  ix = datain16[4];
  iy = datain16[5];
  
  /* Expand compressed data */
  
  prec = pow(2.0, (double) isc);
  icx = 3;
  
  for (j=0; j<iy; j++)
    {
      /* Extract base, number of bits per value, number of 32 bit words used */
      base = datainr32[icx];
      ibit = datain16[(icx + 1) * 2];
      nop = datain16[(icx + 1) * 2 + 2];
      
#if NATIVE_ORDERING == little_endian
      swap_bytes_sgl(datain32+icx+2, nop);
#endif
      xpnd(ix, datain32+icx+2, dataout, prec, ibit, base, nop, mdi);
      
      icx += nop + 2;
      dataout += ix;
    }
  
  return 0;
}

static int xpnd(int ix, int32_t *icomp, REAL *field, REAL prec, 
		int ibit, REAL base, int nop, REAL mdi)
{
  int btmap, btmis, btmin, btzer;
  int jword, jbit, j, iscale;
  int *imap, *imis, *imin, *izer;
  
  btmap = FALSE;
  btmis = FALSE;
  btmin = FALSE;
  btzer = FALSE;
  
  /* check if bitmap used for zero values */
  
  if (ibit >= 128)
    {
      btzer = TRUE;
      btmap = TRUE;
      ibit -= 128;
    }
  
  /* check if bitmap used for minimum values */
  
  if (ibit >= 64)
    {
      btmin = TRUE;
      btmap = TRUE;
      ibit -= 64;
    }
  
  /* check if bitmap used for missing data values */
  
  if (ibit >= 32)
    {
      btmis = TRUE;
      btmap = TRUE;
      ibit -= 32;
    }
  
  if (ibit > 32)
    {
      error_mesg("Number of bits used to pack wgdos data = %d must be <= 32 ",
		 ibit);
      return 1;
    }
  
  if (btmap)
    {
      if ( (imap = malloc (ix*sizeof(int))) == NULL )
	{
          error_mesg("Error unable to allocate memory for imap in xpnd ix = %d ",
		     ix);
          return 1;
	}
      
      for (j=0; j<ix; j++)
	imap[j] = 1;
    }
  
  /* Set start position in icomp */
  
  jword = 0;
  jbit = 31;
  
  /* Extract missing data value bitmap */
  
  if (btmis)
    {
      if ( (imis = malloc (ix*sizeof(int))) == NULL )
	{
          error_mesg("Error unable to allocate memory for imis in xpnd ix = %d ",
		     ix);
          return 1;
	}
      
      for (j=0; j<ix; j++)
	{
	  if (bit_test(icomp+jword, jbit))
	    {
	      imis[j] = 1;
	      imap[j] = 0;
	    }
	  else
            imis[j] = 0;
	  
	  if (jbit > 0)
            jbit--;
	  else
	    {
	      jbit = 31;
	      jword++;
	    }
	}
    }
  
  /* Extract minimum value bitmap */
  
  if (btmin)
    {
      if ( (imin = malloc (ix*sizeof(int))) == NULL )
	{
          error_mesg("Error unable to allocate memory for imin in xpnd ix = %d ",
		     ix);
          return 1;
	}
      
      for (j=0; j<ix; j++)
	{
	  if (bit_test(icomp+jword, jbit))
	    {
	      imin[j] = 1;
	      imap[j] = 0;
	    }
	  else
            imin[j] = 0;
	  
	  if (jbit > 0)
            jbit--;
	  else
	    {
	      jbit = 31;
	      jword++;
	    }
	}
    }
  
  /* Extract zero value bitmap */
  
  if (btzer)
    {
      if ( (izer = malloc (ix*sizeof(int))) == NULL )
	{
          error_mesg("Error unable to allocate memory for izer in xpnd ix = %d ",
		     ix);
          return 1;
	}
      
      for (j=0; j<ix; j++)
	{
	  if (bit_test(icomp+jword, jbit))
            izer[j] = 0;
	  else
	    {
	      izer[j] = 1;
	      imap[j] = 0;
	    }
	  
	  if (jbit > 0)
            jbit--;
	  else
	    {
	      jbit = 31;
	      jword++;
	    }
	}
    }
  
  /* If bitmap used reset pointers to beginning of 32 bit boundary */
  
  if (btmap && jbit != 31)
    {
      jbit = 31;
      jword++;
    }
  
  if (ibit > 0)
    {
      /* Unpack scaled values */
      
      for (j=0; j<ix; j++)
	{
	  if (btmap && imap[j] == 0) continue;
	  
	  extrin(icomp+jword, 4, jbit, ibit, &iscale, 0);
	  field[j] = base + iscale*prec;
	  
	  jbit -= ibit;
	  if (jbit < 0)
	    {
	      jword++;
	      jbit += 32;
	    }
	}
      
      /* If minimum value bitmap fill in field with base */
      
      if (btmin)
	{
	  for (j=0; j<ix; j++) 
	    {
	      if (imin[j] == 1) field[j] = base;
	    }
	}
    }
  else if (ibit == 0)
    {
      /* All points in row have same value */
      
      for (j=0; j<ix; j++) field[j] = base;
    }
  
  /* If missing data value bitmap fill in field with mdi */
  
  if (btmis)
    {
      for (j=0; j<ix; j++) 
	{
	  if (imis[j] == 1) field[j] = mdi;
	}
    }
  
  /* If zero value bitmap fill in field with 0.0 */
  
  if (btzer)
    {
      for (j=0; j<ix; j++) 
	{
	  if (izer[j] == 1) field[j] = 0.0;
	}
    }
  
  if (btmap) free(imap);
  if (btmis) free(imis);
  if (btmin) free(imin);
  if (btzer) free(izer);
  
  return 0;
}

int extrin(int32_t *icomp, int iword, int istart, int nbit, int *inum, int isign)
{
  if (isign == 0)
    {
      move_bits(icomp, istart, nbit, inum);
    }
  else if (isign == 1)
    {
      /* move integer without sign bit */
      
      move_bits(icomp, istart-1, nbit-1, inum);
      
      /* move sign bit */
      
      *inum = (*icomp << (31-istart)) & (~0 << 31);
      
      /* set undefined if inum negative */
      
      if (*inum < 0) *inum = (~0 << (nbit-1)) | *inum;
    }
  else if (isign == 2)
    {
      /* move integer without sign bit */
      
      move_bits(icomp, istart-1, nbit-1, inum);
      
      if (bit_test(icomp, istart)) *inum = -*inum;
    }
  
  return 0;
}

static int bit_test(void *iword, int ibit)
{
  unsigned char i;
  unsigned int ui;
  
  ui = *(unsigned int *) iword;
  
  i = (ui >> ibit) & ~(~0 << 1);
  
  if (i == 1)
    return TRUE;
  else
    return FALSE;
}

/*
 *  Move nbits from 32 bit word1 starting at start1 into 32 bit word2.
 *  0 =< nbits <= 32, bits can cross into word1+1.
 */

static void move_bits(void *word1, int start1, int nbits, void *word2)
{
  uint32_t *ui1, *ui2, temp1, temp2;
  
  ui1 = (uint32_t *) word1;
  ui2 = (uint32_t *) word2;
  
  if (start1+1-nbits >= 0)
    {
      /* move bits within one word */
      
      ui2[0] = (ui1[0] >> (start1+1-nbits)) & ~(~0 << nbits);
    }
  else
    {
      /* move bits within two words */
      
      temp1 = (ui1[0] << (nbits-start1-1)) & ~(~0 << nbits);
      temp2 = (ui1[1] >> (32+start1+1-nbits)) & ~(~0 << (nbits-start1-1));
      ui2[0] = temp1 | temp2;
    }
}
