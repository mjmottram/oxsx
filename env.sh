#!/bin/bash

#Set these for the installation paths
SCONS_DIR=/Users/matthewmottram/snoPlus/software/snoing-install/scons-2.3.4
ROOT_DIR=/Users/matthewmottram/snoPlus/software/snoing-install/root-5.34.30

# location of armadillo includes and libraries. 
# If these are installed to any of the standard /usr/... they may be left blank
# h5c++ -show tells you where the hdf libraries are (-L/.....)
ARMA_LIB=""
ARMA_HEADER=""
GSL_LIB=/usr/local/lib
GSL_HEADER=/usr/local/include
HDF_HEADER="" 
HDF_LIB=""

#####################################################
# Shouldn't need to change anything below this line #
#####################################################
OXSXROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PATH=$ROOT_DIR/bin:/$SCONS_DIR/script:$PATH

#tells python where the utility functions are
PYTHONPATH=$OXSXROOT/util:$SCONS_DIR/engine:$PYTHONPATH
LD_LIBRARY_PATH=$OXSXROOT/build:$ROOT_DIR/lib:$LD_LIBRARY_PATH
DYLD_LIBRARY_PATH=$ROOT_DIR/lib:$ARMA_LIB:$H5_LIB:$DYLD_LIBRARY_PATH

export ARMA_HEADER ARMA_LIB GSL_LIB GSL_HEADER PYTHONPATH LD_LIBRARY_PATH DYLD_LIBRARY_PATH HDF_LIB HDF_HEADER
