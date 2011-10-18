#include "ofxWebm.h"
#include "ofMain.h"
#include <set>
#include "Poco/Glob.h"
#include "Poco/Path.h"
#include <iostream>

using Poco::Path;

static void mem_put_le16(char *mem, unsigned int val) {
    mem[0] = val;
    mem[1] = val>>8;
}
 
static void mem_put_le32(char *mem, unsigned int val) {
    mem[0] = val;
    mem[1] = val>>8;
    mem[2] = val>>16;
    mem[3] = val>>24;
}

static void die_codec(vpx_codec_ctx_t *ctx, const char *s) {                  //
    const char *detail = vpx_codec_error_detail(ctx);                         //
                                                                              //
    printf("%s: %s\n", s, vpx_codec_error(ctx));                              //
    if(detail)                                                                //
        printf("    %s\n",detail);                                            //
    exit(EXIT_FAILURE);                                                       //
}
                                                                             //
static void write_ivf_frame_header(FILE *outfile,
                                   const vpx_codec_cx_pkt_t *pkt)
{
    char             header[12];
    vpx_codec_pts_t  pts;
 
    if(pkt->kind != VPX_CODEC_CX_FRAME_PKT)
        return;
 
    pts = pkt->data.frame.pts;
    mem_put_le32(header, pkt->data.frame.sz);
    mem_put_le32(header+4, pts&0xFFFFFFFF);
    mem_put_le32(header+8, pts >> 32);
 
    if(fwrite(header, 1, 12, outfile));
}

static void write_ivf_file_header(FILE *outfile,
                                  const vpx_codec_enc_cfg_t *cfg,
                                  int frame_cnt) {
    char header[32];
 
    if(cfg->g_pass != VPX_RC_ONE_PASS && cfg->g_pass != VPX_RC_LAST_PASS)
        return;
    header[0] = 'D';
    header[1] = 'K';
    header[2] = 'I';
    header[3] = 'F';
    mem_put_le16(header+4,  0);                   /* version */
    mem_put_le16(header+6,  32);                  /* headersize */
    mem_put_le32(header+8,  fourcc);              /* headersize */
    mem_put_le16(header+12, cfg->g_w);            /* width */
    mem_put_le16(header+14, cfg->g_h);            /* height */
    mem_put_le32(header+16, cfg->g_timebase.den); /* rate */
    mem_put_le32(header+20, cfg->g_timebase.num); /* scale */
    mem_put_le32(header+24, frame_cnt);           /* length */
    mem_put_le32(header+28, 0);                   /* unused */
 
    if(fwrite(header, 1, 32, outfile));
}
 


//---------------------------------------------------------------------------- 

ofxWebm::ofxWebm() 
:number_of_webm_files(0)
,enable_recording(true)
{
	setNumberOfWebmFiles();
	setBasePath();
}

ofxWebm::~ofxWebm() {
	if(frame_cnt > 0) {
		if(!fseek(fp, 0, SEEK_SET)) {
			write_ivf_file_header(fp, &cfg, frame_cnt-1);
		}
		string output_file = getNewMovieFilePath() +".webm";
		string input_file =  getNewMovieFilePath() +".ivf";
		string cmd = getFFMPEG() +" -i " +input_file +" -vcodec copy " +output_file;
		system(cmd.c_str());	
	}

	vpx_img_free(in_image);
	vpx_img_free(out_image);
	fclose(fp);	
	vpx_codec_destroy(&codec);
}

void ofxWebm::setup(int w, int h) {
	flags = 0;
	frame_cnt = 0;
	width = w;
	height = h;
	
	// Populate encoder configuration                                      
    res = vpx_codec_enc_config_default(interface, &cfg, 0);                  
    if(res) {                                                               
        printf("Error: Failed to get config: %s\n", vpx_codec_err_to_string(res));
		exit(1);
    }    
	
	// set bitrate and size
	//cfg.rc_target_bitrate = width * height * cfg.rc_target_bitrate / cfg.g_w / cfg.g_h;
	cfg.rc_target_bitrate = 4048;
	cfg.rc_min_quantizer = 1;
	cfg.rc_max_quantizer = 1;

	cfg.kf_min_dist = 1;
	cfg.kf_max_dist = 1;
	cfg.g_w = width;
	cfg.g_h = height;
	
	// set codec (vp8)
	if(vpx_codec_enc_init(&codec, interface, &cfg, 0)) {
		die_codec(&codec, "Failed to initialize");
	}
	
	if(vpx_codec_control(&codec, VP8E_SET_SHARPNESS, 0) != VPX_CODEC_OK) {
		die_codec(&codec, "Failed to set sharpness");
	}
	if(vpx_codec_control(&codec, VP8E_SET_TUNING, VP8_TUNE_SSIM) != VPX_CODEC_OK) {
		die_codec(&codec, "Failed to set sharpness");
	}
	if(vpx_codec_control(&codec, VP8E_SET_CQ_LEVEL, 5) != VPX_CODEC_OK) {
		die_codec(&codec, "Failed to set sharpness");
	}

	string ivf_filename =  getNewMovieFilePath() +".ivf";
    if(!(fp = fopen(ivf_filename.c_str(), "wb"))) {
      printf("Error: canot open output file.\n");
	  exit(1);
	}

	// test with using vpx image only
	in_image = vpx_img_alloc(NULL, VPX_IMG_FMT_RGB24, width, height, 0);
	out_image = vpx_img_alloc(NULL, VPX_IMG_FMT_I420, width, height, 0);
	
	// output ivf
	//---------------------
	write_ivf_file_header(fp, &cfg, 0);
}

// auto add a new frame
void ofxWebm::addFrame() {
	grabber_img.grabScreen(0,0,ofGetWidth(), ofGetHeight());
	addFrameUsingPixels(grabber_img.getPixels());
}

// create a buffer in yuv format
void ofxWebm::addFrameUsingPixels(unsigned char* pixels) {
	if(!enable_recording) {
		return ;
	}
	struct SwsContext* convert_context = sws_getContext(
		width
		,height
		,PIX_FMT_RGB24
		,width
		,height
		,PIX_FMT_YUV420P
		,SWS_FAST_BILINEAR
		,NULL
		,NULL
		,NULL
	);
	
	if(convert_context == NULL) {
		printf("Error: cannot get convert context\n");
		exit(1);
	}
	
	
	// fill the buffer.
	in_image = vpx_img_wrap(in_image, VPX_IMG_FMT_RGB24, width, height, 0, pixels);
	
	int line_size = width * 3;
	int output_slice_h = sws_scale(
		 convert_context
		,in_image->planes
		,in_image->stride
		,0
		,height
		,out_image->planes
		,out_image->stride
	);	
	
	// now, out_pic->data contains the YUV version of our RGB frame.
	// ----------------------------------------------------------------
	if(vpx_codec_encode(&codec, out_image, frame_cnt, 1,flags, VPX_DL_REALTIME)) {
		die_codec(&codec, "Failed to encode frame");
	}	
	int got_data = 0;
	vpx_codec_iter_t iter = NULL;
	const vpx_codec_cx_pkt_t* pkt;
	while( (pkt = vpx_codec_get_cx_data(&codec, &iter)) ) {
		got_data = 1;
		switch(pkt->kind) {
			case VPX_CODEC_CX_FRAME_PKT: {
				write_ivf_frame_header(fp, pkt);
				if(fwrite(pkt->data.frame.buf, 1, pkt->data.frame.sz, fp)) {
				}
				break;
			}
			default: {
				break;
			}
		}
		fflush(stdout);
	}
	
	frame_cnt++;
	
}

string ofxWebm::getNewMovieFilePath() {
	char new_file[512];
	sprintf(new_file, "openFrameworks-%04d", number_of_webm_files+1);
	string result = getBasePath() +"/" +string(new_file);
	return result;
}

string ofxWebm::getFFMPEG() {
	ofFile f(__FILE__);
	string fp = f.getEnclosingDirectory() +"../bin/ffmpeg ";
	return fp;
}

void ofxWebm::setBasePath() {
	base_path = Path::current();
}

string ofxWebm::getBasePath() {
	return base_path;
}

void ofxWebm::setNumberOfWebmFiles() {
	string dp = getBasePath() +"*.webm";
	std::set<std::string> files;
	Poco::Glob::glob(dp, files);
	std:set<std::string>::iterator it = files.begin();
	number_of_webm_files = files.size();
}

void ofxWebm::enableRecording() {
	enable_recording = true;
}
void ofxWebm::disableRecording() {
	enable_recording = false;
}
void ofxWebm::toggleRecording() {
	enable_recording = !enable_recording;
}
bool ofxWebm::isRecordingEnabled() {
	return enable_recording;
}