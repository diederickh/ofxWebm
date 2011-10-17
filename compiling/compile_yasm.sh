#!/bin/sh
cd yasm
dir=${PWD}
./configure --prefix=${dir}/../compiled LDFLAGS="-arch i386" CPPFLAGS="-arch i386" CFLAGS="-m32"
make
make install
