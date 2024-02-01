#!/bin/sh

set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-./build}
BUILD_TYPE=${BUILD_TYPE:-debug}

ln -sf $BUILD_DIR/compile_commands.json

mkdir -p $BUILD_DIR/ \
    && cd $BUILD_DIR/ \
    && cmake \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            -DSDK_INCLUDE_DIR="${HOME}/libreoffice/instdir/sdk/include" \
            -DSDK_LIBRARY_DIR="${HOME}/libreoffice/instdir/sdk/lib" \
            -DSDK_BINARY_DIR="${HOME}/libreoffice/instdir/sdk/bin" \
            -DSDK_PROGRAM_DIR="${HOME}/libreoffice/instdir/program" \
            $SOURCE_DIR \
    && make $*

