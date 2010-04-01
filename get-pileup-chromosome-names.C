#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <set>

extern "C"
{
  #include "xgutil.h"
}

using namespace std;


/*
 * Read the pileup and write the summary.
 */
int summarize(FILE *fin, FILE *fout)
{
  set<string> names;
  int pos;
  size_t linesize = 8;
  char *line = (char *) malloc(linesize);
  for (pos=0; fautogets(&line, &linesize, fin) != NULL; pos++)
  {
    /* clobber the first space character with a zero */
    char *p;
    for (p=line; *p; ++p)
    {
      if (isspace(*p))
      {
        *p = 0;
        break;
      }
    }
    /* create the chromosome name string */
    string name(line);
    /* add the name to the set */
    names.insert(name);
  }
  /* print the names */
  set<string>::iterator it;
  for (it=names.begin(); it != names.end(); ++it)
    fprintf(fout, "%s\n", it->c_str());
  /* cleanup */
  free(line);
  return 0;
}

int main(int argc, const char **argv)
{
  if (argc != 1)
  {
    fprintf(stderr, "%s: expected no arguments\n", argv[0]);
    return EXIT_FAILURE;
  }
  summarize(stdin, stdout);
  return EXIT_SUCCESS;
}

