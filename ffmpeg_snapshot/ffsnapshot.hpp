//
//  ffsnapshot.hpp
//  ffmpegsnapshot
//
//  Created by kateh on 2017/11/28.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#ifndef ffsnapshot_hpp
#define ffsnapshot_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include "kqueue.hpp"
extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}
class ffProcessor{
public:
    ffProcessor();
    ~ffProcessor();
    int init(std::string src_filename);
    int decode();
    int getJPG(std::string filename);
private:
    bool godestroy;
    AVFormatContext *fmt_ctx = NULL;
    AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
    int width, height;
    enum AVPixelFormat pix_fmt;
    AVStream *video_stream = NULL, *audio_stream = NULL;
        
    uint8_t *video_dst_data[4] = {NULL};
    int video_dst_linesize[4];
    
    int video_stream_idx = -1, audio_stream_idx = -1;
    AVFrame *frame = NULL;
    AVPacket pkt;
    kqueue* framequeue;
    kqueue* mediaqueue;
    int video_frame_count = 0;
    int audio_frame_count = 0;
    
    int open_codec_context(int *stream_idx,AVCodecContext **dec_ctx, enum AVMediaType type);
    int decode_packet(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt);
    int get_format_from_sample_fmt(const char **fmt,enum AVSampleFormat sample_fmt);
    int encode_packet(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt);
    int dump_frame_to_jpeg(AVCodecContext *video_dec_ctx, AVFrame *frame,std::string JPEGFName);
};

#endif /* ffsnapshot_hpp */
