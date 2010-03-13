#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "summaryutil.h"


int init_summary(struct summary_t *p, int ncols)
{
  p->nrows = 0;
  p->ncols = ncols;
  p->accum_arr = malloc(ncols*sizeof(int64_t));
  p->min_arr = malloc(ncols*sizeof(int32_t));
  p->max_arr = malloc(ncols*sizeof(int32_t));
  int i;
  for (i=0; i<ncols; i++)
  {
    p->accum_arr[i] = 0;
    p->min_arr[i] = 0;
    p->max_arr[i] = 0;
  }
  return 0;
}

int add_to_summary(struct summary_t *p, const int32_t *counts)
{
  int i;
  for (i=0; i<p->ncols; i++)
  {
    p->accum_arr[i] += counts[i];
    if (p->nrows==0 || counts[i] > p->max_arr[i])
    {
      p->max_arr[i] = counts[i];
    }
    if (p->nrows==0 || counts[i] < p->min_arr[i])
    {
      p->min_arr[i] = counts[i];
    }
  }
  p->nrows++;
  return 0;
}

int fprint_summary(const struct summary_t *p, FILE *fout)
{
  int i;
  /* compute and display the grand total */
  int64_t grand_total = 0;
  for (i=0; i<p->ncols; i++)
  {
    grand_total += p->accum_arr[i];
  }
  fprintf(fout, "grand total:\n");
  fprintf(fout, "\t%0" PRId64 "\n", grand_total);
  /* show the accumulation row */
  fprintf(fout, "\n");
  fprintf(fout, "accumulation:\n");
  for (i=0; i<p->ncols; i++)
  {
    fprintf(fout, "\t%0" PRId64 "\n", p->accum_arr[i]);
  }
  /* show the min and max rows */
  fprintf(fout, "\n");
  fprintf(fout, "min:\n");
  for (i=0; i<p->ncols; i++)
  {
    fprintf(fout, "\t%0" PRId32 "\n", p->min_arr[i]);
  }
  fprintf(fout, "\n");
  fprintf(fout, "max:\n");
  for (i=0; i<p->ncols; i++)
  {
    fprintf(fout, "\t%0" PRId32 "\n", p->max_arr[i]);
  }
  return 0;
}

int del_summary(struct summary_t *p)
{
  p->nrows = 0;
  free(p->accum_arr);
  free(p->min_arr);
  free(p->max_arr);
  p->accum_arr = NULL;
  p->min_arr = NULL;
  p->max_arr = NULL;
  return 0;
}
