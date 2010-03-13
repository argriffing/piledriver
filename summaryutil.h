#ifndef SUMMARYUTIL_H
#define SUMMARYUTIL_H

#include <stdio.h>
#include <stdint.h>


/*
 * The idea is to summarize rows of a matrix
 * which has many more rows than columns.
 */
struct summary_t
{
  int ncols; /* the number of columns per row */
  int nrows; /* the number of rows added so far */
  int64_t *accum_arr;
  int32_t *min_arr;
  int32_t *max_arr;
};

int init_summary(struct summary_t *p, int ncols);

int add_to_summary(struct summary_t *p, const int32_t *counts);

int fprint_summary(const struct summary_t *p, FILE *fout);

int del_summary(struct summary_t *p);

#endif
