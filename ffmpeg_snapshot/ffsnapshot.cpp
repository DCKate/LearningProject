//
//  ffsnapshot.cpp
//  ffmpegsnapshot
//
//  Created by kateh on 2017/11/28.
//  Copyright © 2017年 FAKE. All rights reserved.
//

/**
 * @file
 * Demuxing and decoding example.
 *
 * Show how to use the libavformat and libavcodec API to demux and
 * decode audio and video data.
 * @example demuxing_decoding.c
 */


#include "ffsnapshot.hpp"

ffProcessor::ffProcessor(){
    godestroy=true;
    framequeue=init_kqueue();
    mediaqueue=init_kqueue();
}
ffProcessor::~ffProcessor(){
    if (godestroy) {
        avcodec_free_context(&video_dec_ctx);
        avcodec_free_context(&audio_dec_ctx);
        avformat_close_input(&fmt_ctx);
        av_frame_free(&frame);
        av_free(video_dst_data[0]);
    }

}

int ffProcessor::init(std::string src_filename){
    /* register all formats and codecs */
    av_register_all();
    
    /* open input file, and allocate format context */
    if (avformat_open_input(&fmt_ctx, src_filename.c_str(), NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", src_filename.c_str());
        godestroy=false;
        return -1;
    }
    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        godestroy=false;
        return -1;
    }
    
    if (open_codec_context(&video_stream_idx, &video_dec_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
        video_stream = fmt_ctx->streams[video_stream_idx];
        
        /* allocate image where the decoded image will be put */
        width = video_dec_ctx->width;
        height = video_dec_ctx->height;
        pix_fmt = video_dec_ctx->pix_fmt;
        int ret = av_image_alloc(video_dst_data, video_dst_linesize,
                             width, height, pix_fmt, 1);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate raw video buffer\n");
            return -1;
        }
    }
    
    if (open_codec_context(&audio_stream_idx, &audio_dec_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
        audio_stream = fmt_ctx->streams[audio_stream_idx];
    }
    
    /* dump input information to stderr */
    av_dump_format(fmt_ctx, 0, src_filename.c_str(), 0);
    return 0;
}
int ffProcessor::open_codec_context(int *stream_idx,
                              AVCodecContext **dec_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    AVCodec *dec = NULL;
    AVDictionary *opts = NULL;
    
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not find %s stream in input file \n",
                av_get_media_type_string(type));
        return ret;
    } else {
        stream_index = ret;
        st = fmt_ctx->streams[stream_index];
        
        /* find decoder for the stream */
        dec = avcodec_find_decoder(st->codecpar->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(type));
            return AVERROR(EINVAL);
        }
        
        /* Allocate a codec context for the decoder */
        *dec_ctx = avcodec_alloc_context3(dec);
        if (!*dec_ctx) {
            fprintf(stderr, "Failed to allocate the %s codec context\n",
                    av_get_media_type_string(type));
            return AVERROR(ENOMEM);
        }
        
        /* Copy codec parameters from input stream to output codec context */
        if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
            fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                    av_get_media_type_string(type));
            return ret;
        }
        
        bool refcount = true;
        /* Init the decoders, with or without reference counting */
        av_dict_set(&opts, "refcounted_frames", refcount ? "1" : "0", 0);
        if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(type));
            return ret;
        }
        *stream_idx = stream_index;
    }
    
    return 0;
}

int ffProcessor::decode_packet(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt)
{
    int decoded = pkt->size;
    int ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        return -1;
    }
    
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret < 0){
            return ret == AVERROR_EOF ? 0 : ret;
        }
        
        printf("saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);
        
        /* the picture is allocated by the decoder. no need to
         free it */
//        pgm_save(frame->data[0], frame->linesize[0],
//                 frame->width, frame->height, buf);
    }
    decoded=(pkt->stream_index==audio_stream_idx)?FFMIN(ret, pkt->size):decoded;
    return decoded;
}

int ffProcessor::get_format_from_sample_fmt(const char **fmt,
                               enum AVSampleFormat sample_fmt)
{
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;
    
    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }
    
    fprintf(stderr,
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;
}

int ffProcessor::decode(){
    if (!audio_stream && !video_stream) {
        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
        return -1;
    }
    
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        return -1;
    }
    
    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int ret = 0;
    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        AVPacket orig_pkt = pkt;
        do {
            if (pkt.stream_index == video_stream_idx) {
                ret = decode_packet(video_dec_ctx,frame,&pkt);
            }else if (pkt.stream_index == audio_stream_idx){
                ret = decode_packet(audio_dec_ctx,frame,&pkt);
            }else{
                ret=-1;
            }
            if (ret < 0)break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (pkt.size > 0);
        av_packet_unref(&orig_pkt);
    }
    
    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        if (pkt.stream_index == video_stream_idx) {
            ret = decode_packet(video_dec_ctx,frame,&pkt);
        }else if (pkt.stream_index == audio_stream_idx){
            ret = decode_packet(audio_dec_ctx,frame,&pkt);
        }
    } while (0);
    
    printf("Demuxing succeeded.\n");
    
    if (video_stream) {
        printf("Play the output video file with the command:\n"
               "ffplay -f rawvideo -pix_fmt %s -video_size %dx%d \n",
               av_get_pix_fmt_name(pix_fmt), width, height);
    }
    
    if (audio_stream) {
        enum AVSampleFormat sfmt = audio_dec_ctx->sample_fmt;
        int n_channels = audio_dec_ctx->channels;
        const char *fmt;
        
        if (av_sample_fmt_is_planar(sfmt)) {
            const char *packed = av_get_sample_fmt_name(sfmt);
            printf("Warning: the sample format the decoder produced is planar "
                   "(%s). This example will output the first channel only.\n",
                   packed ? packed : "?");
            sfmt = av_get_packed_sample_fmt(sfmt);
            n_channels = 1;
        }
        
        if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0)
            return -1;
        
        printf("Play the output audio file with the command:\n"
               "ffplay -f %s -ac %d -ar %d\n",
               fmt, n_channels, audio_dec_ctx->sample_rate);
    }
    
    return 0;
}

int ffProcessor::getJPG(std::string filename){
    if (!audio_stream && !video_stream) {
        fprintf(stderr, "Could not find audio or video stream in the input, aborting\n");
        return -1;
    }
    
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate frame\n");
        return -1;
    }
    
    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int ret = 0;
    bool getpicture=false;
    /* read frames from the file */
    while (av_read_frame(fmt_ctx, &pkt) >= 0 && !getpicture) {
        AVPacket orig_pkt = pkt;
        do {
            if (pkt.stream_index == video_stream_idx) {
                ret = avcodec_send_packet(video_dec_ctx, &pkt);
                if (ret < 0) {
                    fprintf(stderr, "Error sending a packet for decoding\n");
                    return -1;
                }
                
                while (ret >= 0) {
                    ret = avcodec_receive_frame(video_dec_ctx, frame);
                    if (ret < 0){
                        break;
                    }
                    getpicture = (dump_frame_to_jpeg(video_dec_ctx, frame,filename)>0);
                }

            }else if (pkt.stream_index == audio_stream_idx){
                ret = decode_packet(audio_dec_ctx,frame,&pkt);
            }

            if (ret < 0)break;
            pkt.data += ret;
            pkt.size -= ret;
        } while (!getpicture);
        av_packet_unref(&orig_pkt);
    }
    
    /* flush cached frames */
    pkt.data = NULL;
    pkt.size = 0;
    do {
        if (pkt.stream_index == video_stream_idx) {
            ret = decode_packet(video_dec_ctx,frame,&pkt);
        }else if (pkt.stream_index == audio_stream_idx){
            ret = decode_packet(audio_dec_ctx,frame,&pkt);
        }
    } while (0);
    
    return 0;
}


int ffProcessor::encode_packet(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt)
{
    int ret;
    
    /* send the frame to the encoder */
    if (frame){
        printf("Send frame %3" PRId64"\n", frame->pts);
    }
    
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0){
        fprintf(stderr, "Error encode video frame (%s)\n", av_err2str(ret));
        return ret;
    }
    
    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret < 0) {
            fprintf(stderr, "Error during encoding(%s)\n", av_err2str(ret));
            return ret;
        }
        
        printf("Write packet %3" PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        av_packet_unref(pkt);
    }
    return 1;
}

int ffProcessor::dump_frame_to_jpeg(AVCodecContext *video_dec_ctx, AVFrame *frame,std::string JPEGFName) {
    AVCodec *jpegCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!jpegCodec) {
        return -1;
    }
    AVCodecContext *jpegContext = avcodec_alloc_context3(jpegCodec);
    if (!jpegContext) {
        return -1;
    }

    jpegContext->pix_fmt = AV_PIX_FMT_YUVJ420P;
    jpegContext->height = frame->height;
    jpegContext->width = frame->width;
    jpegContext->time_base = video_dec_ctx->time_base;

    
    if (avcodec_open2(jpegContext, jpegCodec, NULL) < 0) {
        return -1;
    }
    int ret = -1;
    
    AVPacket packet = {.data = NULL, .size = 0};
    av_init_packet(&packet);

//    av_dump_format(fmt_ctx, 0, "", 0);

    ret = avcodec_send_frame(jpegContext, frame);
    if (ret < 0){
        fprintf(stderr, "Error encode video frame (%s)\n", av_err2str(ret));
        return ret;
    }
   
    ret = avcodec_receive_packet(jpegContext, &packet);
    if (ret < 0) {
        fprintf(stderr, "Error during encoding(%s)\n", av_err2str(ret));
        return ret;
    }
    
    printf("Write packet %3" PRId64" (size=%5d)\n", packet.pts, packet.size);
    
    FILE *JPEGFile = fopen(JPEGFName.c_str(), "wb");
    fwrite(packet.data, 1, packet.size, JPEGFile);
    fclose(JPEGFile);

    av_packet_unref(&packet);
    avcodec_close(jpegContext);
    
    return 0;
}
