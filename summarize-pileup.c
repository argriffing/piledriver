#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "xgutil.h"
#include "pileutil.h"
#include "summaryutil.h"

/*
 * Read the pileup and write the summary.
 */
int summarize(FILE *fin, FILE *fout)
{
  int errcode = 0;
  /* initialize the summary */
  struct summary_t summary;
  init_summary(&summary, ACGTN);
  /* build the summary */
  int ref_star;
  int ref_pos;
  ntcount_t acgtn_counts[ACGTN];
  int pos;
  size_t linesize = 8;
  char *line = malloc(linesize);
  for (pos=0;
      fautogets(&line, &linesize, fin) != NULL;
      pos++)
  {
    if (parse_pileup_line(line, &ref_star, &ref_pos, acgtn_counts) < 0)
    {
      errcode = -1;
      break;
    }
    if (!ref_star)
    {
      add_to_summary(&summary, acgtn_counts);
    }
  }
  /* cleanup */
  if (errcode < 0)
  {
    fprintf(stderr, "the error was on line %d\n", pos+1);
  } else {
    fprint_summary(&summary, fout);
  }
  free(line);
  del_summary(&summary);
  return errcode;
}

int main(int argc, const char **argv)
{
  if (argc != 1)
  {
    fprintf(stderr, "expected no arguments\n");
    return EXIT_FAILURE;
  }
  summarize(stdin, stdout);
  return EXIT_SUCCESS;
}

