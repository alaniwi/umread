#include "umfileint.h"

Data_type get_type(const INTEGER *int_hdr)
{
  switch (int_hdr[INDEX_LBUSER1])
    {
    case(2):
    case(-2):
    case(3):
    case(-3):
      return int_type;
      /* break; */
    case(1):
    case(-1):
      return real_type;
      /* break; */
    default:
      error_mesg("Warning: datatype %d not recognised, assuming real", 
		 int_hdr[INDEX_LBUSER1]);
      return real_type;
    }
}

/* get data length in words */
size_t get_data_length(const INTEGER *int_hdr) 
{
  size_t datalen;

  if (int_hdr[INDEX_LBPACK] == 0)
    /* unpacked record */
    return int_hdr[INDEX_LBLREC];

  /* packed record */
  datalen=0;
  if (int_hdr[INDEX_LBROW] > 0
      && int_hdr[INDEX_LBNPT] > 0)
    datalen += int_hdr[INDEX_LBROW] * int_hdr[INDEX_LBNPT];
  
  if (int_hdr[INDEX_LBEXT] > 0)
    datalen += int_hdr[INDEX_LBEXT];

  if (datalen == 0)
    return int_hdr[INDEX_LBLREC];
  else
    return datalen;
}


int get_type_and_length_core(const INTEGER *int_hdr,
			     Data_type *type_rtn,
			     size_t *num_words_rtn)
{
  *type_rtn = get_type(int_hdr);
  *num_words_rtn = get_data_length(int_hdr);
  return 0;
}


/* sometimes a variable is included but which has some
 * really essential header elements to missing data flag,
 * so the variable is essentially missing in that any
 * attempt to process the variable is only going to
 * lead to errors
 *
 * pp_var_missing() tests for this.
 *
 * FIXME: expand to test other header elements
 */
int var_is_missing(const INTEGER *int_hdr)
{
  if (int_hdr[INDEX_LBNPT] == INT_MISSING_DATA)
    return 1;

  if (int_hdr[INDEX_LBROW] == INT_MISSING_DATA)
    return 1;

  return 0;
}

int get_var_stash_model(const INTEGER *int_hdr)
{
  return int_hdr[INDEX_LBUSER7];
}

int get_var_stash_section(const INTEGER *int_hdr)
{
  return int_hdr[INDEX_LBUSER4] / 1000;
}

int get_var_stash_item(const INTEGER *int_hdr)
{
  return int_hdr[INDEX_LBUSER4] % 1000;
}

int get_var_compression(const INTEGER *int_hdr)
{
  return (int_hdr[INDEX_LBPACK] / 10) % 10;
}


int get_var_gridcode(const INTEGER *int_hdr)
{
  return int_hdr[INDEX_LBCODE];
}


int get_var_packing(const INTEGER *int_hdr)
{
   return int_hdr[INDEX_LBPACK] % 10;
}


/* get the fill value from the floating point header.
 * caller needs to check that it is actually floating point data!
 */
REAL get_var_real_fill_value(const REAL *real_hdr)
{
  return real_hdr[INDEX_BMDI];
}
