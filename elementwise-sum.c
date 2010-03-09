#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * At each step, read one value from each infile and then
 * add the values together, and write the sum to fout.
 * If at any step a value cannot be read, then return.
 */
int process(int ninputs, FILE **infiles, FILE *fout)
{
  int errcode = 0;
  int i;
  int32_t accum;
  int32_t addend;
  size_t nelements;
  while (1)
  {
    accum = 0;
    for (i=0; i<ninputs; i++)
    {
      nelements = fread(&addend, sizeof(addend), 1, infiles[i]);
      if (nelements)
      {
        accum += addend;
      } else {
        return 0;
      }
    }
    nelements = fwrite(&accum, sizeof(accum), 1, fout);
  }
  return errcode;
}

/*
 * The first argument is the output file.
 * The remaining arguments are binary files containing int32 counts.
 * These int32 counts will be summed elementwise.
 * I guess overflow will wrap.
 */
int main(int argc, char *argv[])
{
  int errcode = EXIT_SUCCESS;
  if (argc < 3)
  {
    fprintf(stderr, "Expected at least two arguments: ");
    fprintf(stderr, "an output file and at least one input file.\n");
    return EXIT_FAILURE;
  }
  /* open the output file for writing binary data */
  FILE *fout = fopen(argv[1], "wb");
  if (!fout)
  {
    fprintf(stderr, "Failed to open %s for writing.", argv[0]);
    return EXIT_FAILURE;
  }
  /* open the input files for reading the binary data */
  int i;
  int ninputs = argc-2;
  FILE **infiles = malloc(ninputs*sizeof(FILE*));
  for (i=0; i<ninputs; i++) infiles[i] = NULL;
  for (i=0; i<ninputs; i++)
  {
    infiles[i] = fopen(argv[2+i], "rb");
  }
  /* do the summation */
  if (process(ninputs, infiles, fout) < 0)
  {
    errcode = EXIT_FAILURE;
  }
  /* clean up */
  if (fout) fclose(fout);
  for (i=0; i<ninputs; i++)
  {
    if (infiles[i]) fclose(infiles[i]);
  }
  free(infiles);
  return errcode;
}

