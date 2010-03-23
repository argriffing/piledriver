#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "xgutil.h"
#include "pileutil.h"


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

int nt_to_acgtn_index_silent(char nt)
{
  switch (nt)
  {
    case 'a':
    case 'A':
      return 0;
    case 'c':
    case 'C':
      return 1;
    case 'g':
    case 'G':
      return 2;
    case 't':
    case 'T':
      return 3;
    case 'n':
    case 'N':
      return 4;
    default:
      return -1;
  }
}

int parse_acgtn(char ref_nt, const char *pile, ntcount_t *acgtn_counts)
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

int parse_pileup_line(char *line, struct ref_t *pref, ntcount_t *acgtn_counts)
{
  pref->is_star = 0;
  pref->is_N = 0;
  pref->pos = -1;
  /* array of word start pointers in the input line */
  char *s_words[COLUMNS+1];
  /* parse the tab separated words */
  int nwords = parse_tab_separation(line, s_words, COLUMNS+1);
  /* complain if too few words were found */
  if (nwords < COLUMNS)
  {
    fprintf(stderr, "expected %d tabs per line ", COLUMNS-1);
    fprintf(stderr, "but found %d\n", nwords-1);
    return -1;
  }
  /* get the length of the reference nucleotide column */
  int ref_word_length = strlen(s_words[2]);
  /* read the reference nucleotide */

  const char *ref_word = s_words[2];
  /* ignore this pileup line if the reference base is an asterisk */
  if (!strcmp(ref_word, "*"))
  {
    /*
     * At this position there is an insertion into aligned reads
     * with respect to the reference sequence.
     * This causes trouble in a few ways.
     * First, it causes the position sequence to be non-strictly-monotonic.
     * Second, these rows sometimes have more than
     * the nominal number of columns.
     */
    pref->is_star = 1;
    return 0;
  }
  /* complain if too many words were found */
  if (nwords > COLUMNS)
  {
    fprintf(stderr, "found more than %d tabs per line\n", COLUMNS-1);
    return -1;
  }
  /* read the base-one reference sequence position */
  pref->pos = atoi(s_words[1]);
  /* get the ACGTN counts */
  int i;
  for (i=0; i<ACGTN; i++) acgtn_counts[i] = 0;
  if (parse_acgtn(ref_nt, s_words[8], acgtn_counts) < 0)
  {
    return -1;
  }
  /* success */
  return 0;
}
