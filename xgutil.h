#ifndef XGUTIL
#define XGUTIL

#include <stdio.h>

/*
 * Don't complain if the pointer is NULL.
 */
int fsafeclose(FILE *f);

/*
 * This is a strtok-like function which
 * modifies its input buffer by changing '\t' to 0.
 * Return the number of words actually found.
 */
int parse_tab_separation(char *s, char *s_words[], int maxwords);

/*
 * Automatically extend the buffer to fit the line of text.
 * The buffer may be reallocated and its size changed.
 * Return NULL if fgets fails right away.
 */
char *fautogets(char **pbuffer, size_t *pbufsize, FILE *fin);

#endif
