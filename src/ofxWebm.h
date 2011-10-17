#pragma once
//#include "ofxMPEG.h"

// vp8
#define VPX_CODEC_DISABLE_COMPAT 1
#include "include/vpx/vpx_encoder.h"
#include "include/vpx/vp8cx.h"

// swscale
extern "C" {
	#include "include/libswscale/swscale.h"
}

#define interface (vpx_codec_vp8_cx())
#define fourcc    0x30385056

#include <string>
#include "ofMain.h"
using std::string;
// webm output
//#include "mkvmuxer.hpp"
//#include "mkvwriter.hpp"

class ofxWebm {
public:	
	ofxWebm();
	~ofxWebm();
	void setup(int w, int h);
	void addFrame();
	void addFrameUsingPixels(unsigned char* pixels);
	
	
	// ivf test
	
	
private:
	void setBasePath();
	string getBasePath();
	void setNumberOfWebmFiles();
	
	string getNewMovieFilePath();
	string getFFMPEG();
	int width;
	int height;
	
	uint8_t* out_buffer;
	uint8_t* in_buffer;
//	AVFrame* in_pic;
//	AVFrame* out_pic;
	vpx_image_t* in_image;
	vpx_image_t* out_image;
	
	vpx_codec_err_t res;
	vpx_codec_enc_cfg_t cfg;
	vpx_codec_ctx_t codec;
//	vpx_image_t raw;
	FILE* fp;
	int flags;
	int frame_cnt;
	string base_path;
	int number_of_webm_files;
	ofImage grabber_img;
	// webm export
	//mkvmuxer::MkvWriter mkv_writer;
	//mkvmuxer::Segment* mkv_segment;
	
	// ivf test
};