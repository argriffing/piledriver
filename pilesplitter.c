#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLINE 8192

/*
 * Split the file into multiple parts depending on the first
 * column in the file.
 */
int process(FILE *fin, const char *prefix, const char *suffix)
{
  int errcode = 0;
  char *curr_filename = malloc(MAXLINE);
  char *prev_filename = malloc(MAXLINE);
  curr_filename[0] = 0;
  prev_filename[0] = 0;
  int prefix_length = strlen(prefix);
  int suffix_length = strlen(suffix);
  char line[MAXLINE];
  FILE *fappend = NULL;
  while (fgets(line, MAXLINE, fin) != NULL)
  {
    /* assert that the buffer is long enough to hold the line */
    size_t line_length = strlen(line);
    if (line_length >= MAXLINE-1)
    {
      fprintf(stderr, "a line was too long\n");
      errcode = -1; break;
    }
    /* get the length of the first word */
    int i;
    for (i=0; line[i] && !isspace(line[i]); i++);
    int word_length = i;
    /* assert that the buffer is long enough to hold the filename */
    int filename_length = prefix_length + word_length + suffix_length;
    if (filename_length >= MAXLINE-1)
    {
      fprintf(stderr, "a filename was too long\n");
      errcode = -1; break;
    }
    /* piece together the filename associated with the current position */
    memcpy(curr_filename, prefix, prefix_length);
    memcpy(curr_filename+prefix_length, line, word_length);
    memcpy(curr_filename+prefix_length+word_length, suffix, suffix_length);
    curr_filename[filename_length] = 0;
    /* if the filename has changed then open a new file */
    if (strcmp(curr_filename, prev_filename))
    {
      if (fappend) fclose(fappend);
      fappend = fopen(curr_filename, "a");
    }
    /* swap filenames to keep track of which one is open */
    char *tmp_filename = curr_filename;
    curr_filename = prev_filename;
    prev_filename = tmp_filename;
    /* write the line */
    fwrite(line, sizeof(char), line_length, fappend);
  }
  free(curr_filename);
  free(prev_filename);
  return errcode;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "Expected two arguments: ");
    fprintf(stderr, "the prefix and suffix of the output files.\n");
    return EXIT_FAILURE;
  }
  const char *prefix = argv[1];
  const char *suffix = argv[2];
  if (process(stdin, prefix, suffix) < 0)
  {
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}
