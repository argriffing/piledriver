#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

typedef int32_t ntcount_t;

int summarize(FILE *fin, int ncols)
{
  int64_t *accum_arr = malloc(ncols*sizeof(size_t));
  ntcount_t *small_arr = malloc(ncols*sizeof(ntcount_t));
  ntcount_t *min_arr = malloc(ncols*sizeof(ntcount_t));
  ntcount_t *max_arr = malloc(ncols*sizeof(ntcount_t));
  /* initialize the arrays */
  int i;
  for (i=0; i<ncols; i++)
  {
    accum_arr[i] = 0;
    min_arr[i] = 0;
    max_arr[i] = 0;
  }
  /* fill the arrays */
  int nelements;
  size_t pos;
  for (pos=0;
      (nelements = fread(small_arr, sizeof(ntcount_t), ncols, fin)) != 0;
      pos++)
  {
    if (nelements < ncols)
    {
      printf("warning: ragged matrix\n");
      break;
    }
    for (i=0; i<ncols; i++)
    {
      accum_arr[i] += small_arr[i];
      if (pos==0 || small_arr[i] > max_arr[i])
      {
        max_arr[i] = small_arr[i];
      }
      if (pos==0 || small_arr[i] < min_arr[i])
      {
        min_arr[i] = small_arr[i];
      }
    }
  }
  /* compute and display the grand total */
  int64_t grand_total = 0;
  for (i=0; i<ncols; i++)
  {
    grand_total += accum_arr[i];
  }
  printf("grand total:\n");
  printf("\t%0" PRId64 "\n", grand_total);
  /* show the accumulation row */
  printf("\n");
  printf("accumulation:\n");
  for (i=0; i<ncols; i++)
  {
    printf("\t%0" PRId64 "\n", accum_arr[i]);
  }
  /* show the min and max rows */
  printf("\n");
  printf("min:\n");
  for (i=0; i<ncols; i++)
  {
    printf("\t%0" PRId32 "\n", min_arr[i]);
  }
  printf("\n");
  printf("max:\n");
  for (i=0; i<ncols; i++)
  {
    printf("\t%0" PRId32 "\n", max_arr[i]);
  }
  free (accum_arr);
  free (small_arr);
  free (min_arr);
  free (max_arr);
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "expected one argument: the number of columns\n");
    return EXIT_FAILURE;
  }
  int ncols = atoi(argv[1]);
  if (ncols < 1)
  {
    fprintf(stderr, "expected a positive number of columns\n");
    return EXIT_FAILURE;
  }
  summarize(stdin, ncols);
  return EXIT_SUCCESS;
}

