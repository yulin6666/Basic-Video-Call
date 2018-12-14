#include "AGFrameObserver.h"
#include <unistd.h>
#include <arpa/inet.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <fstream>
#include <unordered_set>
#include <utility>
#include <vector>
#include <iostream>
#include <memory.h>

using std::cout;
using std::endl;

AGAudioFrameObserver::AGAudioFrameObserver() {
}

AGAudioFrameObserver::~AGAudioFrameObserver() {
}

bool AGAudioFrameObserver::onMixedAudioFrame(IAudioFrameObserver::AudioFrame &audioFrame) {
    (void)audioFrame;
    return true;
}

bool AGAudioFrameObserver::onPlaybackAudioFrame(IAudioFrameObserver::AudioFrame &audioFrame) {
    (void)audioFrame;
    return true;
}

bool AGAudioFrameObserver::onRecordAudioFrame(IAudioFrameObserver::AudioFrame &frame) {
    size_t bytes = frame.samples * frame.bytesPerSample * frame.channels;
#define MAXPATH 256
    static FILE *pfile = NULL;
    char curdir[MAXPATH]={0};
    char* re = getcwd(curdir, MAXPATH);

    if(!pfile && !(pfile=fopen("./right_48khz.pcm","r")))
    {
        cout << "Can not find ./right_48khz.pcm" << endl;
        return false;
    }

    if(feof(pfile))
    {
        fclose(pfile);
        pfile=NULL;
        return false;
    }
    else
    {
        size_t n = fread(frame.buffer, 1, bytes, pfile);
    }

    return true;
}

bool AGAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, IAudioFrameObserver::AudioFrame& audioFrame) {
    (void)uid;
    (void)audioFrame;
    saveAudioFrame(0, audioFrame, "onPlaybackAudioFrame");
    return true;
}

void AGAudioFrameObserver::saveAudioFrame(int uid, const IAudioFrameObserver::AudioFrame& audioFrame, const char* tag) {
//    cout << "saveAudioFrame" << endl;
    std::string info_name = std::string(tag);
    info_name += "_";

    char uidbuf[65];
    snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
    info_name += std::string(uidbuf);
    info_name += "_";

    char channelsbuf[20];
    snprintf(channelsbuf, sizeof(channelsbuf),"%u", audioFrame.channels);
    info_name += std::string(channelsbuf);
    info_name += "_";

    char bitRatebuf[20];
    snprintf(bitRatebuf, sizeof(bitRatebuf),"%u", audioFrame.samplesPerSec);
    info_name += std::string(bitRatebuf);
    info_name += ".pcm";

    const uint8_t* data = static_cast<const uint8_t*>(audioFrame.buffer);
    uint32_t size = audioFrame.bytesPerSample * audioFrame.samples * audioFrame.channels;

    FILE *fp = fopen(info_name.c_str(), "a+b");
    ::fwrite(data, 1, size, fp);
    ::fclose(fp);
}


AGVideoFrameObserver::AGVideoFrameObserver() {
}

AGVideoFrameObserver::~AGVideoFrameObserver() {
}

bool AGVideoFrameObserver::onCaptureVideoFrame(IVideoFrameObserver::VideoFrame &frame) {
    cout << "onCaptureVideoFrame" << endl;
    unsigned int width = 640, height = 360, ystride = 640, ustride = 320, vstride = 320;
    unsigned int rotation = 0;
    unsigned int  totalBytes = width*height*3/2;
    std::string yuvBuf;
    yuvBuf.reserve(totalBytes);
    char * buf = const_cast<char *>(yuvBuf.data());
    char* ybuf = const_cast<char*>(&yuvBuf.data()[0]);
    int offset = height * ystride;
    char* ubuf = const_cast<char*>(&yuvBuf.data()[offset]);
    offset = height * ustride / 2;
    char* vbuf = const_cast<char*>(&yuvBuf.data()[offset]);

    frame.rotation = rotation;

#define MAXPATH 256
    static FILE *pfile = NULL;
    char curdir[MAXPATH]={0};
    char* re = getcwd(curdir, MAXPATH);
    if(!pfile && !(pfile=fopen("./video.yuv","r")))
    { 
        cout<<"fopen failed!"<<endl;
        return false;
    }

    if(feof(pfile))
    {
        cout << "feof?" << endl;
        fclose(pfile);
        pfile=NULL;
        return false;
    }
    
    size_t n = fread(buf, 1, totalBytes, pfile);

    memcpy(frame.yBuffer, ybuf, height*ystride);
    memcpy(frame.uBuffer, ubuf, height*ustride/2);
    memcpy(frame.vBuffer, vbuf, vstride*height/2);
    
    return true;
}

bool AGVideoFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame) {
    (void)uid;
    (void)videoFrame;
    saveVideoFrame(uid, videoFrame, "onRenderVideoFrame");
    return true;
}

void AGVideoFrameObserver::saveVideoFrame(int uid, const IVideoFrameObserver::VideoFrame& videoFrame, const char* tag) {
    cout << "saveVideoFrame" << endl;
    std::string info_name = std::string(tag);
    info_name += "_";

    char uidbuf[65];
    snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
    info_name += std::string(uidbuf);

    info_name += "_";
    char yStrideBuf[20];
    snprintf(yStrideBuf, sizeof(yStrideBuf),"%u", videoFrame.yStride);
    info_name += std::string(yStrideBuf);

    info_name += "_";
    char uStrideBuf[20];
    snprintf(uStrideBuf, sizeof(uStrideBuf),"%u", videoFrame.uStride);
    info_name += std::string(yStrideBuf);

    info_name += "_";
    char vStrideBuf[20];
    snprintf(vStrideBuf, sizeof(vStrideBuf),"%u", videoFrame.vStride);
    info_name += std::string(vStrideBuf);

    int y_size = videoFrame.height * videoFrame.yStride;
    int u_size = videoFrame.height * videoFrame.uStride / 2;
    int v_size = videoFrame.height * videoFrame.vStride / 2;

    FILE *fp = fopen(info_name.c_str(), "a+b");
    ::fwrite(videoFrame.yBuffer, 1, y_size, fp);
    ::fwrite(videoFrame.uBuffer, 1, u_size, fp);
    ::fwrite(videoFrame.vBuffer, 1, v_size, fp);

    ::fclose(fp);
}


