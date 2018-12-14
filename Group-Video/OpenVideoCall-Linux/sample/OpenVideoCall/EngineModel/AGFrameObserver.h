#pragma once
#include "IAgoraMediaEngine.h"

namespace agora{
    namespace media {
        class IAudioFrameObserver;
        class IVideoFrameObserver;
    }
}

using namespace agora::media;

class baseObserver {
    public:
        baseObserver() { m_filter = 0; };
        ~baseObserver() {};

        void setFilter(int filter) { m_filter = filter; };

        protected:
        int m_filter;
};

class AGAudioFrameObserver : public baseObserver, public IAudioFrameObserver{
    public:
        AGAudioFrameObserver();
        ~AGAudioFrameObserver();

        virtual bool onRecordAudioFrame(IAudioFrameObserver::AudioFrame& audioFrame); 
        virtual bool onPlaybackAudioFrame(IAudioFrameObserver::AudioFrame& audioFrame); 
        virtual bool onMixedAudioFrame(IAudioFrameObserver::AudioFrame& audioFrame);
        virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, IAudioFrameObserver::AudioFrame& audioFrame); 

    private:
        void saveAudioFrame(int uid, const IAudioFrameObserver::AudioFrame& audioFrame, const char* tag);
};

class AGVideoFrameObserver: public baseObserver, public IVideoFrameObserver {
    public:
        AGVideoFrameObserver();
        ~AGVideoFrameObserver();

        virtual bool onCaptureVideoFrame(IVideoFrameObserver::VideoFrame& videoFrame); 
        virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame); 

    private:
        void saveVideoFrame(int uid, const IVideoFrameObserver::VideoFrame& videoFrame, const char* tag);
};


