#ifndef PILEUTIL_H
#define PILEUTIL_H

#include <stdio.h>
#include <stdint.h>


#define ACGTN 5
#define COLUMNS 10

typedef int32_t ntcount_t;


int nt_to_index(char nt);

/*
 * @param ref_nt: the ref. nt. at the position
 * @param pile: the bases within reads aligned to the position
 * @param acgtn_counts: counts are accumulated here
 */
int parse_acgtn(char ref_nt, const char *pile, ntcount_t *acgtn_counts);

/*
 * Parse a line of a pileup file.
 * @param line: the input line which is modified strtok-style.
 * @param pref_star: will hold 1 if the reference base is a star
 * @param pref_pos: will hold the one-based reference position
 * @param acgtn_counts: will hold the five acgtn counts
 * @return: 0 if success, -1 if error
 */
int parse_pileup_line(char *line,
    int *pref_star, int *pref_pos, ntcount_t *acgtn_counts);

#endif
