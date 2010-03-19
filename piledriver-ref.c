#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "xgutil.h"
#include "pileutil.h"

/*
 * At this site the pileup file does not have data for the reference base
 * and it does not have any aligned reads.
 */
int write_default_data(FILE *fout)
{
  int errcode = 0;
  ntcount_t data[ACGTN] = {0,0,0,0,0};
  fwrite(data, sizeof(data), 1, fout);
  return errcode;
}

int write_informative_data(const ntcount_t *acgtn_counts, FILE *fout)
{
  int errcode = 0;
  fwrite(acgtn_counts, sizeof(ntcount_t), ACGTN, fout);
  return errcode;
}

int process_pileup(FILE *fin, FILE *fseq, FILE *fout)
{
  int pos;
  int errcode = 0;
  size_t linesize = 8;
  char *line = malloc(linesize);
  int i;
  int last_index = -1;
  int current_index;
  int ref_star;
  int ref_pos;
  char ref_nt;
  ntcount_t acgtn_counts[ACGTN];
  for (pos=0;
      fautogets(&line, &linesize, fin) != NULL;
      pos++)
  {
    if (parse_pileup_line(line, &ref_star, &ref_pos, acgtn_counts) < 0)
    {
      errcode = -1; goto end;
    }
    /* skip lines with a star as a reference nucleotide */
    if (ref_star)
    {
      continue;
    }
    /* get the current index from the one-based reference position*/
    current_index = ref_pos - 1;
    if (current_index <= last_index)
    {
      fprintf(stderr, "ref positions must strictly monotonically increase\n");
      errcode = -1; goto end;
    }
    /* if the position is out of bounds then ignore the counts */
    if (current_index >= ref_seq_length)
    {
      if (ref_nt != 'N')
      {
        fprintf(stderr, "out of bounds positions should have ref nt N\n");
        errcode = -1; goto end;
      }
      continue;
    }
    /* write default data for reference positions with no aligned reads */
    for (i=last_index+1; i<current_index; i++)
    {
      write_default_data(fout);
    }
    /* write data for the current position */
    write_informative_data(acgtn_counts, fout);
    last_index = current_index;
  }
  /* write default data for the remaining positions */
  for (i=last_index+1; i<ref_seq_length; i++)
  {
    write_default_data(fout);
  }
  /* cleanup after here */
end:
  if (errcode < 0)
  {
    fprintf(stderr, "the error was on line %d\n", pos+1);
  }
  free(line);
  return errcode;
}

int main(int argc, char *argv[])
{
  int errcode = EXIT_SUCCESS;
  if (argc != 2)
  {
    fprintf(stderr, "expected exactly one argument ");
    fprintf(stderr, "which should be the reference sequence filename\n");
    return EXIT_FAILURE;
  }
  const char *seqfilename = atoi(argv[1]);
  FILE *fseq = fopen(seqfilename);
  if (!fseq)
  {
    perror("fopen");
    return EXIT_FAILURE;
  }
  if (process_pileup(stdin, fseq, stdout) < 0)
  {
    errcode = EXIT_FAILURE;
  } else {
    errcode = EXIT_SUCCESS;
  }
  fclose(fseq);
  return errcode;
}
