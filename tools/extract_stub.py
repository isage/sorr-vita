#!/usr/bin/env python3

import os
import sys
from struct import *

# From core/bgdrtm/src/dcbr.c

files = []

verbose = False
if len(sys.argv) == 3:
    if "-v" in sys.argv:
        verbose = True
    else:
        sys.stderr.write("You should provide a single filename as argument\n")
        sys.exit(1)
elif len(sys.argv) != 2:
    sys.stderr.write("You should provide a single filename as argument\n")
    sys.exit(1)

if not os.path.isfile(sys.argv[1]):
    sys.stdout.write("The file you gave me ({}) doesn't seem to be readable/exist\n".format(sys.argv[1]))
    sys.exit(2)

# Read the DCB file looking for files
with open(sys.argv[1], 'rb') as f:
    #################################################################
    # First phase: Determine file names, sizes and offsets          #
    #################################################################
    # Determine the number of files included in the DCB
    f.seek(16, 0)
    nfiles, = unpack('I', f.read(4))

    # Determine the offset to the files section in bytes
    f.seek(116, 0)
    offset, = unpack('I', f.read(4))

    if nfiles == 0:
        sys.stdout.write("This DCB file doesn't seem to contain any included files, quitting\n")
        sys.exit(0)

    # Read the files
    f.seek(offset, 0)
    for i in range(nfiles):
        # Read the filename length
        sname, = unpack('I', f.read(4))
        # Read one bit with the file flags (and ignore it)
        f.seek(1, 1)
        # Read the file size and the offset to the file
        sfile, ofile = unpack('II', f.read(8))
        # Read the file name. We know its size, but it's a NULL-terminated string, so remove the
        # last character and convert to string
        fname = f.read(sname)[:-1].decode('cp850')

        files.append({'name': fname, 'size': sfile, 'offset': ofile})

    ##################################################################
    ## Second phase: Dump the files                                  #
    ##################################################################
    for dcbfile in files:
        sys.stdout.write("{} ({} bytes)".format(dcbfile['name'], dcbfile['size']))
        if verbose:
            sys.stdout.write(" Offset: {}".format(dcbfile['offset']))

        sys.stdout.write('...')

        # Create the file directory, if it doesn't exist...
        if not os.path.isdir(os.path.dirname(dcbfile['name'])):
            os.makedirs(os.path.dirname(dcbfile['name']))

        # Seek to the file
        f.seek(dcbfile['offset'], 0)
        with open(dcbfile['name'], 'wb') as o:
            o.write(f.read(dcbfile['size']))

        sys.stdout.write('\n')

    #################################################################
    # Third phase: Dump a stripped version of the DCB               #
    #################################################################
    # This is not fully orthodox, since we're leaving the filenames embedded filenames
    # and offsets inside the DCB file, but all the DCB sections can be found by stored offsets
    # and removing that section would force me to change all of the individual offsets
    # and I don't want to do that...
    f.seek(0, 0)
    stripped_filename = sys.argv[1].split('.')[0] + '_stripped.dcb'
    with open(stripped_filename, 'wb') as o:
        # Write the first 16 bytes
        o.write(f.read(16))
        # Write a uint32_t 0 (no of embedded files) after that
        o.write(pack('I', 0))
        f.seek(4, 1)
        # Write the next 96 bytes
        o.write(f.read(96))
        # Write a uint32_t 0 (offset to the files) after that
        o.write(pack('I', 0))
        f.seek(4, 1)
        o.write(f.read(offset-100))

