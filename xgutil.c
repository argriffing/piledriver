#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int fsafeclose(FILE *f)
{
  if (f)
  {
    return fclose(f);
  }
  return 0;
}

int parse_tab_separation(char *s, char **s_words, int maxwords)
{
  /* if the buffer is empty then there are no words */
  if (!*s) return 0;
  /* otherwise there are some words */
  int word_index = 0;
  s_words[word_index++] = s;
  char *p;
  for (p=s; *p; p++)
  {
    if (*p == '\t')
    {
      /* terminate the previous word */
      *p = 0;
      /* if the prev word was the final requested word then we've finished */
      if (word_index == maxwords) break;
      /* otherwise move towards the next tab or buffer termination */
      s_words[word_index++] = p+1;
    }
  }
  return word_index;
}

char *fautogets(char **pbuffer, size_t *pbufsize, FILE *fin)
{
  char *buffer = *pbuffer; /* the buffer to be filled */
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
  /* set the new buffer */
  *pbuffer = buffer;
  /* return a value depending on whether we found anything */
  if (nfilled)
  {
    return *pbuffer;
  } else {
    return NULL;
  }
}
