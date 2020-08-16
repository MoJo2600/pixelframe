Import("env")

print(env.Dump())
#
# This is a small tool to read files from the PlatformIO data directory and write
# them to a flashable BIN image in the LittleFS file format
#
# This script requires the LittleFS-Python mopackage
# https://pypi.org/project/littlefs-python/
#
#
# Author: Guido Henkel
# Date: 1/2/2020
#

import getopt, sys, os
from littlefs import LittleFS
from pathlib import Path

print( "Building LittleFS image…" )

argList = sys.argv[1:]
arxx = { argList[i]: argList[i+1] for i in range(0, len(argList)-1, 2) }
print( arxx )

dataPath = env['PROJECT_DATA_DIR'] # arxx["-c"]                                                               # Grab the source path parameter


fs = LittleFS(block_size=4096, block_count=3072)                                    # Open LittleFS, create a 12MB partition



for root, subdirs, files in os.walk(dataPath):

    print(f"--\nroot = {root}")
    # list_file_path = os.path.join(root, 'my-directory-list.txt')
    # print('list_file_path = ' + list_file_path)

    for subdir in subdirs:
        relative_path = os.path.relpath(f"{root}/{subdir}", dataPath)
        fs.mkdir(relative_path)
        print(f"\t- create directory {relative_path}")

    for filename in files:
        file_path = os.path.join(root, filename)

        relative_path = os.path.relpath(root, dataPath) 
        new_filename = f"{relative_path}/{filename}"
        print(f"\t- copy {filename} to {new_filename}")

        with open(file_path, 'rb') as f:
            data = f.read()

        with fs.open(new_filename, 'w') as fh:                                         # Write the file to the LittleFS partition
            fh.write( data )

fileList = [pth for pth in Path(dataPath).iterdir() if pth.stem[0] != '.']          # Create a list of all the files in the directory

# for root, subdirs, files in os.walk()


for curFile in fileList:
    print( "Adding " + curFile.name )



    # with open( curFile, 'rb' ) as f:                                                # Read the file into a buffer
    #     data = f.read()

    # with fs.open( curFile.name, 'w') as fh:                                         # Write the file to the LittleFS partition
    #     fh.write( data )

outName = argList[-1]                                                               # The last argument is the destination file name
with open(outName, 'wb') as fh:                                                     # Dump the filesystem content to a file
    fh.write(fs.context.buffer)
