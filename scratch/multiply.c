#include <stdio.h>

#include "mypackage.h"
#include "datatype.h"

void FUNC(multiply)(const REAL *a, const REAL *b, REAL *answer) {

  if (COMPILED_TYPE == single_precision)
    puts("single precision version of multiply called");
  else if (COMPILED_TYPE == double_precision)
    puts("double precision version of multiply called");

  *answer = *a * *b;
}
