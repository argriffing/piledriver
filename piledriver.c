#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "xgutil.h"

#define ACGTN 5
#define COLUMNS 10

typedef int32_t ntcount_t;

/*
This is an example of a 10-column pileup file.

seq1  60  T  T  66  0  99  13  ...........^~.^~.   9<<55<;<<<<<<
seq1  61  G  G  72  0  99  15  .............^~.^y. (;975&;<<<<<<<<
seq1  62  T  T  72  0  99  15  .$..............    <;;,55;<<<<<<<<
seq1  63  G  G  72  0  99  15  .$.............^~.  4;2;<7:+<<<<<<<
seq1  64  G  G  69  0  99  14  ..............  9+5<;;;<<<<<<<
seq1  65  A  A  69  0  99  14  .$............. <5-2<;;<<<<<<;
seq1  66  C  C  66  0  99  13  .............   &*<;;<<<<<<8<
seq1  67  C  C  69  0  99  14  .............^~.    ,75<.4<<<<<-<<
seq1  68  C  C  69  0  99  14  ..............  576<;7<<<<<8<<

Column Definition
------- ----------------------------
1 Chromosome
2 Position (1-based)
3 Reference base at that position
4 Consensus bases
5 Consensus quality
6 SNP quality
7 Maximum mapping quality
8 Coverage (# reads aligning over that position)
9 Bases within reads
10 Quality values
*/

/*
 * This is a strtok-like function which
 * modifies its input buffer by changing '\t' to 0.
 * Return the number of words actually found.
 */
int parse_tab_separation(char *s, char *s_words[], int maxwords)
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

int write_informative_data(ntcount_t acgtn_counts[], FILE *fout)
{
  int errcode = 0;
  fwrite(acgtn_counts, sizeof(ntcount_t), ACGTN, fout);
  return errcode;
}

int nt_to_index(char nt)
{
  switch (nt)
  {
    case 'a':
    case 'A': return 0;
    case 'c':
    case 'C': return 1;
    case 'g':
    case 'G': return 2;
    case 't':
    case 'T': return 3;
    case 'n':
    case 'N': return 4;
    default:
      fprintf(stderr, "bad nucleotide: %c\n", nt);
      return -1;
  }
}

/*
 * @param ref_nt: the ref. nt. at the position
 * @param pile: the bases within reads aligned to the position
 * @param acgtn_counts: counts are accumulated here
 */
int parse_acgtn(char ref_nt, const char *pile, ntcount_t acgtn_counts[])
{
  int ref_index = nt_to_index(ref_nt);
  if (ref_index < 0) return -1;
  int nt_index;
  int indel_length;
  const char *p = pile;
  while (*p)
  {
    if (*p == '+' || *p == '-')
    {
      /* indel */
      p++;
      indel_length = atoi(p);
      while (isdigit(*p)) p++;
      p += indel_length;
    } else if (*p == '.' || *p == ',') {
      /* reference nucleotide */
      acgtn_counts[ref_index]++;
      p++;
    } else if (*p == '^') {
      /* begin a new aligned read with a given mapping quality */
      p += 2;
    } else if (*p == '$') {
      /* end an aligned read */
      p++;
    } else if (*p == '*') {
      /* this might be some kind of indel related thing */
      p++;
    } else {
      /* this should be a non-reference nucleotide */
      nt_index = nt_to_index(*p);
      if (nt_index < 0) return -1;
      acgtn_counts[nt_index]++;
      p++;
    }
  }
  return 0;
}

int process(size_t ref_seq_length, FILE *fin, FILE *fout)
{
  int pos;
  int errcode = 0;
  size_t linesize = 8;
  char *line = malloc(linesize);
  char *next_line;
  int i;
  int last_index = -1;
  int current_index;
  char *s_words[10];
  char ref_nt;
  ntcount_t acgtn_counts[ACGTN];
  for (pos=0;
      (next_line = fautogets(line, &linesize, fin)) != NULL;
      pos++)
  {
    line = next_line;
    size_t line_length = strlen(line);
    /* parse the tab separated words */
    int nwords = parse_tab_separation(line, s_words, COLUMNS+1);
    /* complain if too few words were found */
    if (nwords < COLUMNS)
    {
      fprintf(stderr, "expected %d tabs per line ", COLUMNS-1);
      fprintf(stderr, "but found %d\n", nwords-1);
      errcode = -1; goto end;
    }
    /* read the reference nucleotide */
    ref_nt = s_words[2][0];
    /* ignore this pileup line if the reference base is an asterisk */
    if (ref_nt == '*')
    {
      /*
       * At this position there is an insertion into aligned reads
       * with respect to the reference sequence.
       * This causes trouble in a few ways.
       * First, it causes the position sequence to be non-strictly-monotonic.
       * Second, these rows sometimes have more than
       * the nominal number of columns.
       * Therefore we skip such a line in the pileup file.
       */
      continue;
    }
    /* complain if too many words were found */
    if (nwords > COLUMNS)
    {
      fprintf(stderr, "found more than %d tabs per line\n", COLUMNS-1);
      errcode = -1; goto end;
    }
    /* read the reference sequence position */
    current_index = atoi(s_words[1]) - 1;
    if (current_index <= last_index)
    {
      fprintf(stderr, "ref positions must strictly monotonically increase\n");
      errcode = -1; goto end;
    }
    /* If the position goes beyond the reference sequence length,
     * then make sure that the reference base is N and do not write.
     */
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
    /* get the ACGTN counts */
    for (i=0; i<ACGTN; i++) acgtn_counts[i] = 0;
    if (parse_acgtn(ref_nt, s_words[8], acgtn_counts) < 0)
    {
      errcode = -1; goto end;
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
  if (argc != 2)
  {
    fprintf(stderr, "expected exactly one argument ");
    fprintf(stderr, "which should be the length of the reference sequence\n");
    return EXIT_FAILURE;
  }
  size_t ref_seq_length = atoi(argv[1]);
  if (process(ref_seq_length, stdin, stdout) < 0)
  {
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}
