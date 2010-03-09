Description
===========

These are some random C programs to deal with pileup files
in a way that is completely ad hoc and does not use the SAMtools C API.

Usage
=====

piledriver
----------

Lossily convert a 10-column single-chromosome pileup file to a binary format.

The output binary file has one record
for each reference nucleotide position in the chromosome.
Each record is 11 bytes.
The first byte is the reference nucleotide
character taken from the pileup file.
It is one of {A, C, G, T, N}.
The remaining bytes define the uint16_t counts of each of these five
nucleotide symbols in the aligned read column.

Input and output are through stdin and stdout.
The single command line argument
is the number of positions in the reference chromosome,
that is, the length of the reference sequence.

pilesplitter
------------

Split a multiple-chromosome pileup file into multiple single-chromosome
pileup files.

The input is through stdin,
and the two command line arguments define the prefix and suffix
of the created files.
