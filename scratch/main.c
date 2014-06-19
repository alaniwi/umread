#include <stdio.h>
#include "mypackage.h"

int main()
{
  float a = 1.5, b = 2.5, c;
  double x = 1.1e100, y = 2.2e50, z;
  
  c = 0.;
  multiply_dispatch(&a, &b, &c, single_precision);
  printf("%g * %g = %g\n", a, b, c);

  z = 0.;
  multiply_dispatch(&x, &y, &z, double_precision);
  printf("%lg * %lg = %lg\n", x, y, z);

  z = 0.;
  multiply_dispatch(&x, &y, &z, single_precision);
  printf("This should fail: %lg * %lg = %lg\n", x, y, z);

  return 0;
}
