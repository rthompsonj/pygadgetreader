#!/usr/bin/env python

from numpy.distutils.core import setup, Extension

HDF5_PRESENT = 0
HDF5INCL     = "/Users/bob/local/hdf5/include"
HDF5LIB      = "/Users/bob/local/hdf5/lib"

NAME = "readgadget"
FILE = "readgadget.c"

#ext = Extension(NAME,[FILE],
#                include_dirs = [HDF5INCL])

if HDF5_PRESENT:
    setup(name = "readgadget",
          version="0.5",
          ext_modules=[Extension("readgadget",["readgadget.c"], 
                                 include_dirs = [HDF5INCL],
                                 libraries    = ['hdf5'],
                                 library_dirs = [HDF5LIB])])
else:
    setup(name = "readgadget",
          version="0.5",
          ext_modules=[Extension("readgadget",["readgadget.c"])])    
