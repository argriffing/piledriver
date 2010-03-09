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

elementwise-sum
---------------

Add binary files together elementwise.
Elements are assumed to be 32 bit integers.
The first argument to this script is the output file.
The remaining arguments are the input files to be summed elementwise.

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
