#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "piledriver.h"

double sum(double *v, int n)
{
  int i;
  double total=0;
  for (i=0; i<n; i++) total += v[i];
  return total;
}

int main(int argc, char *argv[])
{
  printf("%lf\n", lgamma(3.5));
}
