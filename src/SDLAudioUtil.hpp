#ifndef SDLAUDIOUTIL_H
#define SDLAUDIOUTIL_H

#include <vector>

#include "SDL2/SDL.h"
#include "CircularBuffer.hpp"

namespace util
{
    
    class SDLAudioUtil
    {
        public:
            SDLAudioUtil();
            ~SDLAudioUtil();

            // initilize device - paused by default
            bool init(const int deviceNum, const int s, const int sr, CircularBuffer *buffer, const bool bCircular);

            bool resume(); // resume input
            bool pause(); // pause device
            bool reset(); // clear recording buffer

            CircularBuffer& getBuffer(); //get the poiter of the circular buffer
            std::vector<float>& getData(); // get the recording data

            std::vector<float> viewData(int& pr, int& pw); // get the recording data

            const SDL_AudioDeviceID getDeviceID() const {return m_deviceID;};

            void recordCircularCallback(Uint8* buffer, int len);

            void recordInfCallback(Uint8* buffer, int len);

            void getSeg(int ms, std::vector<float> &out);

            inline bool isRecordingOn() const {return m_audioRecordingOn;};
            inline int getSampleRate() const {return m_sampleRate;};

            bool m_audioRecordingOn;

        private:

            SDL_AudioDeviceID m_deviceID;

            int m_sampleRate;
            int m_lengthSecond;

            size_t m_recordingPos;
            size_t m_recordingLen;
            size_t m_outingPos;

            int m_sample;
            int m_channelNum;

    };
}

#endif // SDLAUDIOUTIL_H