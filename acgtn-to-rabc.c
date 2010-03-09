#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ACGTN 5
#define RABC 4

typedef int32_t ntcount_t;

void fsafeclose(FILE *f)
{
  if (f) fclose(f);
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
 * This is supposed to compare nucleotide counts.
 * Used with qsort, this should put larger counts first.
 */
int mycmp(const void *a_in, const void *b_in)
{
  ntcount_t a = * (ntcount_t*) a_in;
  ntcount_t b = * (ntcount_t*) b_in;
  if (a > b)
  {
    return -1;
  } else if (a < b) {
    return 1;
  } else {
    return 0;
  }
}

/*
 * Find the index of the maximum value.
 */
int get_max_index(const ntcount_t *v, int n)
{
  /* so far the first element is best */
  ntcount_t best = v[0];
  int best_index = 0;
  /* see if which of the remaining elements is best if any */
  int i;
  for (i=1; i<n; i++)
  {
    if (v[i] > best)
    {
      best_index = i;
      best = v[i];
    }
  }
  return best_index;
}

/*
 * Reorder the rabc counts so that the reference base count is first.
 * @param ref_nt: the reference nucleotide symbol in {A, C, G, T, N}
 * @param rabc_counts: the (A, C, G, T) counts.
 */
int reorder_rabc(char ref_nt, ntcount_t *rabc_counts)
{
  /* identify which of ACGT should be treated as the reference base */
  int index = nt_to_index(ref_nt);
  if (index < 0)
  {
    /* bad reference nucleotide */
    return -1;
  } else if (index == 4) {
    /* ambiguous reference nucleotide, so pick the highest-count ACGT */
    index = get_max_index(rabc_counts, 4);
  } 
  /* move the reference base to the front of the rabc counts */
  ntcount_t tmp = rabc_counts[0];
  rabc_counts[0] = rabc_counts[index];
  rabc_counts[index] = tmp;
  /* sort the last three counts from biggest to smallest */
  qsort(rabc_counts + 1, 3, sizeof(ntcount_t), mycmp);
  return 0;
}

int process(FILE *fin_acgtn, FILE *fin_ref, FILE *fout)
{
  const int nref_elements_expected = 1;
  const int nacgtn_elements_expected = ACGTN;
  int nref_elements;
  int nacgtn_elements;
  char ref_nt;
  ntcount_t acgtn_counts[ACGTN];
  ntcount_t rabc_counts[RABC];
  int i;
  while (1)
  {
    nacgtn_elements = fread(acgtn_counts,
        sizeof(ntcount_t), nacgtn_elements_expected, fin_acgtn);
    nref_elements = fread(&ref_nt,
        sizeof(char), nref_elements_expected, fin_ref);
    if (!nacgtn_elements && !nref_elements)
    {
      /* we have reached the end of the files */
      return 0;
    } else if (nacgtn_elements == nacgtn_elements_expected &&
        nref_elements == nref_elements_expected) {
      /* we have not yet reached the end of either file */
      for (i=0; i<4; i++)
      {
        rabc_counts[i] = acgtn_counts[i];
      }
      reorder_rabc(ref_nt, rabc_counts);
      fwrite(rabc_counts, sizeof(ntcount_t), RABC, fout);
    } else {
      /* we have reached the end of one file but not the other file */
      return -1;
    }
  }
}

/*
 * Convert a binary file of per-position ACGTN counts
 * to a binary file of RABC counts.
 * ACGTN refers to the four nucleotides plus a wildcard nucleotide.
 * RABC refers to the reference nucleotide and
 * the three non-reference nucleotides.
 * In the conversion, one of the ACGT counts is converted
 * to the R count, the N count is discarded, and the three
 * non-reference ACGT counts are arbitrarily ordered as ABC counts
 * in the output.
 */
int main(int argc, char *argv[])
{
  int errcode = EXIT_SUCCESS;
  if (argc != 4)
  {
    fprintf(stderr, "Expected three arguments:\n");
    fprintf(stderr, "a binary file of ACGTN counts,\n");
    fprintf(stderr, "a reference nucleotide sequence file, and\n");
    fprintf(stderr, "the output file.\n");
    return EXIT_FAILURE;
  }
  FILE *fin_acgtn = NULL;
  FILE *fin_ref = NULL;
  FILE *fout = NULL;
  /* open the binary input file */
  fin_acgtn = fopen(argv[1], "rb");
  if (!fin_acgtn)
  {
    fprintf(stderr, "Failed to open %s for reading.\n", argv[1]);
    errcode = EXIT_FAILURE; goto end;
  }
  /* open the reference sequence input file */
  fin_ref = fopen(argv[2], "rb");
  if (!fin_ref)
  {
    fprintf(stderr, "Failed to open %s for reading.\n", argv[2]);
    errcode = EXIT_FAILURE; goto end;
  }
  /* open the output file for writing binary data */
  fout = fopen(argv[3], "wb");
  if (!fout)
  {
    fprintf(stderr, "Failed to open %s for writing.\n", argv[3]);
    errcode = EXIT_FAILURE; goto end;
  }
  /* do the conversion */
  if (process(fin_acgtn, fin_ref, fout) < 0)
  {
    errcode = EXIT_FAILURE;
  }
end:
  fsafeclose(fin_acgtn);
  fsafeclose(fin_ref);
  fsafeclose(fout);
  return errcode;
}

