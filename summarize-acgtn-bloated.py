import sys

import numpy as np
import argparse

# numpy dtype for int32_t is np.dtype(np.int32)

if __name__ == '__main__':

    # read the command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('--ncols', default=5, type=int,
            help='number of columns')
    args = parser.parse_args()

    # read the data from stdin
    M = np.fromfile(sys.stdin, dtype=np.dtype(np.int32))

    # reshape the data into a matrix
    count = len(M)
    nrows, r = divmod(count, args.ncols)
    if r:
        print 'warning: ragged matrix'
        print
    shape = (nrows, args.ncols)
    M = np.reshape(M, shape)

    # summarize the matrix
    row_total = np.sum(M, axis=0)
    row_min = np.min(M, axis=0)
    row_max = np.max(M, axis=0)
    grand_total = np.sum(row_total)

    # show the summary
    print 'grand total:'
    print '\t%s' % grand_total
    print
    print 'accumulation:'
    for x in row_total:
        print '\t%s' % x
    print
    print 'min:'
    for x in row_min:
        print '\t%s' % x
    print
    print 'max:'
    for x in row_max:
        print '\t%s' % x
