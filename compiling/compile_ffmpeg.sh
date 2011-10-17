#!/bin/sh
set -x
cd ffmpeg
dir=${PWD}
bd=${dir}/../compiled/
if [ ! -d ${bd} ] ; then
	mkdir -p ${bd}
fi

export PATH=${bd}/bin:${dir}/rtmpdump/trunk/:${PATH} # because we need msgfmt! (from gettext)

#cd rtmpdump/trunk
#make clean
#make SYS=darwin
#make install
#exit


#cd pkgconfig
#./configure --prefix=${bd}
#make
#make install

./configure --prefix=${bd} \
	--disable-ffmpeg \
	--disable-ffserver \
	--disable-ffplay \
	--disable-librtmp \
	--enable-nonfree \
	--enable-pthreads \
	--enable-libx264 \
	--disable-bzlib \
	--disable-zlib \
	--enable-gpl \
	--arch=i386 \
	--extra-cflags="-m32 -arch i386 -I${bd}/include/ -L${bd}/lib/ -L{$bd}compiled/include/libyasm/" \
	--extra-ldflags="-m32 -arch i386 -I${bd}/include/ -L${bd}/lib/"
make clean	
make V=1
make install 

#LDFLAGS="-arch i386" \
#CPPFLAGS="-arch i386" \
#CFLAGS="-arch i386" 

