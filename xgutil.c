#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Don't complain if the pointer is NULL.
 */
int fsafeclose(FILE *f)
{
  if (f)
  {
    return fclose(f);
  }
  return 0;
}

/*
 * Automatically extend the buffer to fit the line of text.
 * Return the possibly reallocated buffer,
 * possibly modifying the bufsize argument to give the new buffer size.
 */
char *fautogets(char *buffer, size_t *pbufsize, FILE *fin)
{
  int nempty = *pbufsize; /* number of unfilled bytes in the buffer */
  int nfilled = 0; /* number of filled bytes in the buffer */
  char *p = buffer; /* begin filling here */
  while (fgets(p, nempty, fin) != NULL)
  {
    int npartial = strlen(p);
    nfilled += npartial;
    nempty -= npartial;
    if (nempty == 1)
    {
      /* We have filled up the buffer as much as fgets allows,
       * so if the last character is a newline then we are done.
       */
      if (buffer[nfilled-1] == '\n') break;
      /* Otherwise we need to keep looking. */
      int next_bufsize = (nfilled + nempty)*2;
      buffer = realloc(buffer, next_bufsize);
      nempty = next_bufsize - nfilled;
      p = buffer + nfilled;
    } else {
      /* If we have not filled up the buffer then we are done. */
      break;
    }
  }
  /* set the new buffer size */
  *pbufsize = nfilled + nempty;
  /* return a value depending on whether we found anything */
  if (nfilled)
  {
    return buffer;
  } else {
    return NULL;
  }
}
