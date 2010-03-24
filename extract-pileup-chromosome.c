#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "xgutil.h"

int starts_with_word(const char *line, const char *word)
{
  int i = 0;
  for (i=0; word[i]; i++)
  {
    if (!line[i])
    {
      /* the line is shorter than the word */
      return 0;
    }
    if (line[i] != word[i])
    {
      /* there is a mismatch within the word */
      return 0;
    }
  }
  if (!line[i])
  {
    /* the line exactly equals the word */
    return 1;
  }
  if (isspace(line[i]))
  {
    /* the matching word is followed by a space */
    return 1;
  }
  /* the word is a prefix of a longer initial word in the line */
  return 0;
}

/*
 * Go through the pileup file line by line,
 * writing only the lines which match the requested
 * chromosome name.
 */
int process(FILE *fin, FILE *fout, const char *requested_chromosome)
{
  size_t linesize = 8;
  char *line = malloc(linesize);
  while (fautogets(&line, &linesize, fin) != NULL)
  {
    if (starts_with_word(line, requested_chromosome))
    {
      size_t line_length = strlen(line);
      fwrite(line, sizeof(char), line_length, fout);
    }
  }
  free(line);
  return 0;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Expected one argument: ");
    fprintf(stderr, "the name of the requested chromosome.\n");
    return EXIT_FAILURE;
  }
  const char *requested_chromosome = argv[1];
  if (process(stdin, stdout, requested_chromosome) < 0)
  {
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}
