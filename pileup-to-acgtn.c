#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "xgutil.h"

typedef int32_t t_ntcount;

/*
 * This is returned by functions which report a nucleotide
 * and complain if a bad nucleotide was found.
 * One such function reads nucleotides from a flat reference sequence file.
 * Another reads nucleotides from a refernce column of a pileup.
 * Another reads nucleotides from non-reference aligned reads.
 * In each case the acgtn index is both computed as part of the validation
 * and is also used by the calling function.
 */
struct acgtn
{
  char nt;
  int index;
};

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

/*
 * At this site the pileup file does not have data for the reference base
 * and it does not have any aligned reads.
 */
int write_default_data(FILE *fout)
{
  int errcode = 0;
  t_ntcount data[5] = {0,0,0,0,0};
  fwrite(data, sizeof(data), 1, fout);
  return errcode;
}

int write_informative_data(const t_ntcount *acgtn_counts, FILE *fout)
{
  int errcode = 0;
  fwrite(acgtn_counts, sizeof(t_ntcount), 5, fout);
  return errcode;
}

/* 
 * Return the number of advanced nucleotides.
 * This is -1 on error, 0 on an attempt to go past the end of the file,
 * and 1 when a nucleotide was advanced.
 * @param fref: the sequence file open for reading
 * @param pacgtn: address of the output for the ref nt and its index
 */
int advance_ref(FILE *fref, struct acgtn *pacgtn)
{
  if (feof(fref))
    return 0;
  while (1)
  {
    int result = fgetc(fref);
    /* if we are at the end then no nucleotide was found */
    if (result == EOF)
    {
      if (feof(fref))
      {
        return 0;
      } else {
        perror("read");
        return -1;
      }
    }
    /* get the result as a character */
    pacgtn->nt = result;
    /* skip past newlines */
    if (pacgtn->nt == '\n')
      continue;
    /* validate the nucleotide and save the index */
    pacgtn->index = nt_to_acgtn_index_silent(pacgtn->nt);
    if (pacgtn->index < 0)
    {
      fprintf(stderr, "invalid nucleotide in the sequence file: %c\n",
          pacgtn->nt);
      return -1;
    }
    /* advancing was successful */
    return 1;
  }
}

int acgtn_init_ref(struct acgtn *pacgtn, const char *nt_string)
{
  /* check the reference nucleotide length */
  int nt_len = strlen(nt_string);
  if (nt_len != 1)
  {
    fprintf(stderr, "the reference nt string has %d characters: %s\n",
        nt_len, nt_string);
    return -1;
  }
  /* check the reference nucleotide value */
  pacgtn->nt = nt_string[0];
  pacgtn->index = nt_to_acgtn_index_silent(pacgtn->nt);
  if (pacgtn->index < 0 && pacgtn->nt != '*')
  {
    fprintf(stderr, "invalid reference nucleotide: %c\n", pacgtn->nt);
    return -1;
  }
  return 0;
}

/*
 * Parse a single column of aligned reads.
 * @param pile: an element of a ten column pileup file
 * @param ref_acgtn_index: defines the reference nucleotide
 * @param acgtn_counts: an accumulator
 */
int parse_pile(const char *pile, int ref_acgtn_index,
    t_ntcount *acgtn_counts)
{
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
      acgtn_counts[ref_acgtn_index]++;
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
      nt_index = nt_to_acgtn_index_silent(*p);
      if (nt_index < 0)
      {
        fprintf(stderr, "invalid non-reference nucleotide: %c\n", *p);
        return -1;
      }
      acgtn_counts[nt_index]++;
      p++;
    }
  }
  return 0;
}

/*
 * Replace the first few instances of '\t' with 0.
 * If there are too many, then the last word may contain unreplaced tabs.
 * When the line has n tabs it is said to have n+1 columns.
 * Return the number of columns found.
 */
int tokenize_line(char *line, char **pwords, int ncols_requested)
{
  int ncols_observed = 0;
  char *p;
  char prev_char = 0;
  for (p=line; ; p++)
  {
    /* check for the beginning of a new, possibly empty, word */
    if (prev_char == 0)
    {
      pwords[ncols_observed++] = p;
      if (ncols_observed == ncols_requested)
        break;
    }
    /* check for the end of the line */
    if (!*p)
      break;
    /* replace a tab with a string termination */
    if (*p == '\t')
      *p = 0;
    /* remember the replacement state */
    prev_char = *p;
  }
  return ncols_observed;
}

/*
 * Return the number of written filler records, or -1 on error.
 */
int write_internal_filler(FILE *fseq, FILE *fout,
    int last_index, int current_index)
{
  int nwritten = 0;
  while (last_index + nwritten < current_index - 1 && !feof(fseq))
  {
    struct acgtn acgtn_seq;
    int count = advance_ref(fseq, &acgtn_seq);
    if (count < 0)
      return -1;
    if (count > 0)
    {
      write_default_data(fout);
      nwritten++;
    }
  }
  return nwritten;
}

/*
 * Return the number of written filler records, or -1 on error.
 */
int write_terminal_filler(FILE *fseq, FILE *fout)
{
  int nwritten = 0;
  while (!feof(fseq))
  {
    struct acgtn acgtn_seq;
    int count = advance_ref(fseq, &acgtn_seq);
    if (count < 0)
      return -1;
    if (count > 0)
    {
      write_default_data(fout);
      nwritten++;
    }
  }
  return nwritten;
}

/*
 * Note that the number of columns in a ten column pileup file is not
 * always ten columns.  If the reference nucleotide is an asterisk,
 * this indicates that there is a gap in the reference sequence
 * relative to the reads, and in these positions there might be
 * extra columns for a reason I do not fully understand.
 *
 * Therefore a pileup line is parsed in two steps.
 * First it is parsed up to the reference nucleotide (3 columns).
 * If the reference nucleteotide is an asterisk then the parsing is stopped.
 * Otherwise the remaining (7) columns are parsed.
 */

int process_pileup(FILE *fin, FILE *fseq, FILE *fout)
{
  int errcode = 0;
  size_t linesize = 8;
  char *line = malloc(linesize);
  int last_index = -1;
  int current_index;
  int pileup_line_index; /* required for detailed error reporting */
  for (pileup_line_index=0; ; ++pileup_line_index)
  {
    /* if no lines remain then break */
    if (fautogets(&line, &linesize, fin) == NULL)
      break;
    /* get nine columns and the rest */
    char *cols[10];
    int ncols = tokenize_line(line, cols, 10);
    if (ncols < 10)
    {
      fprintf(stderr, "expected at least ten columns\n");
      errcode = -1; goto end;
    }
    /* get the reference info */
    struct acgtn acgtn_ref;
    if (acgtn_init_ref(&acgtn_ref, cols[2]) < 0)
    {
      errcode = -1; goto end;
    }
    /* if the reference nt is an asterisk then skip this line */
    if (acgtn_ref.nt == '*')
      continue;
    /* at this point the line should have exactly ten columns */
    if (strchr(cols[9], '\t') != NULL)
    {
      fprintf(stderr, "a row with ref nt %c should have 10 columns\n",
          acgtn_ref.nt);
      errcode = -1; goto end;
    }
    /* get the 1-based position */
    int pos = atoi(cols[1]);
    if (pos < 1)
    {
      fprintf(stderr, "invalid reference position: %s\n", cols[1]);
      return -1;
    }
    /* check for strict monotonicity */
    current_index = pos - 1;
    if (current_index <= last_index)
    {
      fprintf(stderr,
          "non-* ref positions must strictly monotonically increase\n");
      errcode = -1; goto end;
    }
    /* write some filler */
    int nfiller = write_internal_filler(
        fseq, fout, last_index, current_index);
    if (nfiller < 0)
    {
      errcode = -1; goto end;
    }
    last_index += nfiller;
    /* get the reference sequence nucleotide at the current position */
    struct acgtn acgtn_seq;
    int count = advance_ref(fseq, &acgtn_seq);
    if (count < 0)
    {
      errcode = -1; goto end;
    }
    if (count == 0)
    {
      /* the pileup reference is beyond the sequence reference */
      if (acgtn_ref.nt != 'N')
      {
        fprintf(stderr, "out of bounds positions should have ref nt N\n");
        errcode = -1; goto end;
      }
    } else {
      /* the pileup reference is within the sequence reference */
      ++last_index;
      if (tolower(acgtn_seq.nt) != tolower(acgtn_ref.nt))
      {
        fprintf(stderr, "the pileup reference and the seq reference ");
        fprintf(stderr, "nucleotides do not match\n");
        errcode = -1; goto end;
      }
      /* get the acgtn counts at the position */
      t_ntcount acgtn_counts[5] = {0, 0, 0, 0, 0};
      if (parse_pile(cols[8], acgtn_ref.index, acgtn_counts) < 0)
      {
        errcode = -1; goto end;
      }
      /* write the counts */
      write_informative_data(acgtn_counts, fout);
    }
  }
  /* write default data for the remaining positions */
  write_terminal_filler(fseq, fout);
  /* cleanup after here */
end:
  if (errcode < 0)
  {
    fprintf(stderr, "the error was on pileup line %d\n", pileup_line_index+1);
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
  const char *seqfilename = argv[1];
  FILE *fseq = fopen(seqfilename, "r");
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
