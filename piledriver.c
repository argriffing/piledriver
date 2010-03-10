#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define ACGTN 5

const size_t MAXLINE = 8192;

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

int parse_tab_separation(char *s, char *s_words[])
{
  int word_index = 0;
  s_words[word_index++] = s;
  size_t i=0;
  char *p = s;
  for (p=s; *p; p++)
  {
    if (*p == '\t')
    {
      if (word_index > 10-1)
      {
        fprintf(stderr, "expected only 9 tabs per line\n");
        return -1;
      }
      *p = 0;
      s_words[word_index++] = p+1;
    }
  }
  if (word_index != 10)
  {
    fprintf(stderr, "expected 9 tabs per line\n");
    return -1;
  }
  return 0;
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
  int pos=0;
  int errcode = 0;
  char *s = malloc(MAXLINE);
  int i;
  int last_index = -1;
  int current_index;
  char *s_words[10];
  char ref_nt;
  ntcount_t acgtn_counts[ACGTN];
  while (fgets(s, MAXLINE, fin) != NULL)
  {
    size_t line_length = strlen(s);
    if (line_length == MAXLINE-1)
    {
      fprintf(stderr, "a line was too long\n");
      errcode = -1; break;
    }
    /* parse the tab separated words */
    if (parse_tab_separation(s, s_words) < 0)
    {
      fprintf(stderr, "this error was on error on line %d\n", pos+1);
      errcode = -1; break;
    }
    /* read the reference sequence position */
    current_index = atoi(s_words[1]) - 1;
    if (current_index <= last_index)
    {
      fprintf(stderr, "reference positions should monotonically increase\n");
      errcode = -1; break;
    }
    /* read the reference nucleotide */
    ref_nt = s_words[2][0];
    /* If the position goes beyond the reference sequence length,
     * then make sure that the reference base is N and do not write.
     */
    if (current_index >= ref_seq_length)
    {
      if (ref_nt != 'N')
      {
        fprintf(stderr, "out of bounds positions should have ref nt N\n");
        errcode = -1; break;
      }
      pos += 1;
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
      errcode = -1; break;
    }
    /* write data for the current position */
    write_informative_data(acgtn_counts, fout);
    last_index = current_index;
    pos++;
  }
  /* write default data for the remaining positions */
  for (i=last_index+1; i<ref_seq_length; i++)
  {
    write_default_data(fout);
  }
  /* clean up */
  free(s);
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
