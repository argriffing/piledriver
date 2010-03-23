Description
===========

These are some random C programs to deal with pileup files
in a way that is completely ad hoc and does not use the SAMtools C API.

Usage
=====

piledriver
----------

Lossily convert a 10-column single-chromosome pileup file to a binary format.

The output binary file has 32-bit integer counts
for aligned (A, C, G, T, N) nucleotide symbols
at each position in the reference sequence.

Input and output are through stdin and stdout.
The single command line argument
is the number of positions in the reference chromosome,
that is, the length of the reference sequence.

pileup-to-acgtn
---------------

This is very similar to the piledriver binary except
that it should actually compile and its
single parameter is a reference sequence filename
instead of a reference sequence length.

pilesplitter
------------

Split a multiple-chromosome pileup file into multiple
single-chromosome pileup files.

The input is through stdin,
and the two command line arguments define the prefix and suffix
of the created files.

As the input is read, each line is appended to the appropriate output file.
Therefore you might want to make sure that none of the output files
already exists before using this script.

acgtn-to-rabc
-------------

Use a reference sequence to convert a binary file of (A, C, G, T, N)
counts to a file of (R, A, B, C) counts.
ACGTN refers to the four nucleotides plus an ambiguous nucleotide.
RABC refers to a reference nucletide plus three non-reference nucleotides.

The first argument of this script is the binary file
of int32 ACGTN counts per position.
The second argument is a file containing only reference nucleotides.
The final argument is the output filename for the int32 RABC counts.

summarize-acgtn
---------------

Summarize a binary file of rows of counts,
especially when the number of rows is large
and the number of columns is small.

The original purpose of this script is to summarize
counts of (A, C, G, T, N) nucleotide symbols over
aligned reads with respect to a reference sequence.
Counts should be int32,
but the summary uses int64 so overflow is unlikely.
