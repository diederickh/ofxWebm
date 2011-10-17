#!/bin/sh
if [ ! -d libvpx ] ; then
	mkdir libvpx
	cd libvpx
	git clone http://git.chromium.org/webm/libvpx.git .
fi

if [ ! -d yasm ] ; then 
	curl http://www.tortall.net/projects/yasm/releases/yasm-1.1.0.tar.gz -o yasm.tar.gz
	tar -zxvf yasm.tar.gz
	mv yasm-* yasm
fi

if [ ! -d ffmpeg ] ; then
	#curl http://ffmpeg.org/releases/ffmpeg-0.8.5.tar.gz -o ffmpeg.tar.gz
	#tar -zxvf ffmpeg.tar.gz
	#mv ffmpeg-0.* ffmpeg
	mkdir ffmpeg
	cd ffmpeg
	git clone git://git.videolan.org/ffmpeg.git .
fi

if [ ! -d x264 ] ; then
	curl ftp://ftp.videolan.org/pub/x264/snapshots/last_x264.tar.bz2 -o x264.tar.bz2
	bunzip2 x264.tar.bz2
	tar -xvf x264.tar	
	mv x264-* x264
fi

