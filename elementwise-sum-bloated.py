import numpy as np

def main(args):
    # everything has type int32
    dt = np.dtype(np.int32)
    # read the first input file into an accumulation matrix
    accum = open(args.input_files[0], 'rb')
    # accumulate the remaining input files
    for input_filename in args.input_files[1:]:
        addend = np.fromfile(input_filename, dt)
        accum += addend
    # write the accumulated matrix to the output file
    accum.tofile(args.output_file)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('output_file')
    parser.add_argument('input_files', nargs='+')
    args = parser.parse_args()
    main(args)
