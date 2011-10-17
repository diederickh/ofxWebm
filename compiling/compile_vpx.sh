#!/bin/sh
set -x
cd libvpx
dir=${PWD}
inc_dir=${dir}/../compiled/
LDFLAGS="-arch i386 -I${inc_dir}"
CPPFLAGS="-arch i386 -I${inc_dir}"
CFLAGS="-arch i386 -I${inc_dir}/include/libyasm/ "
export PATH=${inc_dir}/bin/:${dir}:${PATH}
./configure --prefix=${dir}/../compiled  --extra-cflags="${CFLAGS}" --target="x86-darwin10-gcc" --enable-static
make
make install
#./configure --prefix=${dir}/../compiled --host=i386-apple-darwin  --extra-cflags="-arch i386" --extra-ldflags="-arch i386" --enable-static
