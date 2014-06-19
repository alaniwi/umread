#include "mypackage.h"

void multiply_dispatch(const void *x, 
		       const void *y, 
		       void *answer, 
		       data_type length)
{
  if (length == single_precision)
    multiply_sgl(x, y, answer);
  else if (length == double_precision)
    multiply_dbl(x, y, answer);
}
