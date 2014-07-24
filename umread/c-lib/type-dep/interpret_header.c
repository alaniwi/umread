#include "umfileint.h"

Data_type WITH_LEN(get_type)(const INTEGER *int_hdr)
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
      error_mesg("get_type", 
		 "Warning: datatype %d not recognised, assuming real", 
		 int_hdr[INDEX_LBUSER1]);
      return real_type;
    }
}

size_t WITH_LEN(get_data_length) (const INTEGER *int_hdr) 
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


int WITH_LEN(get_type_and_length)(const INTEGER *int_hdr,
				   Data_type *type_rtn,
				   size_t *num_words_rtn)
{
  *type_rtn = WITH_LEN(get_type)(int_hdr);
  *num_words_rtn = WITH_LEN(get_data_length)(int_hdr);
  return 0;
}
