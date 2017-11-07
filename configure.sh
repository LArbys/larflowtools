#!/bin/bash

if [ -z ${LARFLOW_BASEDIR+x} ]; then
    export LARFLOW_BASEDIR=$PWD
fi

# SPECIFY WHERE OPENCV IS
export OPENCV_LIBDIR=/usr/local/lib
export OPENCV_INCDIR=/usr/local/include

# uncomment this if you want to include headers using #include "larcv/PackageName/Header.h"
#export LARCV_PREFIX_INCDIR=1

suffix=so
if [[ `uname` == 'Darwin' ]]; then
    suffix=dylib
    # setting opencv paths to use location used by homebrew.
    # change this here, if you used another location
    export OPENCV_LIBDIR=/usr/local/opt/opencv3/lib
    export OPENCV_INCDIR=/usr/local/opt/opencv3/include    
fi

if [ ! -f $OPENCV_LIBDIR/libopencv_core.${suffix} ]; then
    echo "OPENCV LIBRARY NOT FOUND. Set env variable OPENCV_LIBDIR to where libopencv_core.${suffix} lives"
    return 1
fi

if [ ! -f $OPENCV_INCDIR/opencv/cv.h ]; then
    echo "OPENCV INCLUDES NOT FOUND. Please set env variable OPENCV_INCDIR to where opencv/cv.h lives"
    return 1
fi

# setup environment variables
source $LARFLOW_BASEDIR/setup.sh

# setup larlite
source $LARFLOW_BASEDIR/larlite/config/setup.sh

# setup laropencv
#source $LARFLOW_BASEDIR/LArOpenCV/setup_laropencv.sh

# setup Geo2D
#source $LARFLOW_BASEDIR/Geo2D/config/setup.sh

# setup LArCV
source $LARFLOW_BASEDIR/LArCV/configure.sh

# setup larlitecv
#source $LARFLOW_BASEDIR/larlitecv/configure.sh

