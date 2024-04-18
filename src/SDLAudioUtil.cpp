#include "SDLAudioUtil.hpp"
#include <iostream>
#include <cstring>


namespace util
{ 
    /* SDLAudio*/

    SDLAudioUtil::SDLAudioUtil()
    {

    }

    SDLAudioUtil::~SDLAudioUtil()
    {
        if(m_deviceID)
        {
            SDL_CloseAudioDevice(m_deviceID);
        }
    }

    bool SDLAudioUtil::init(const int deviceNum, const int s, const int sr, CircularBuffer *buffer, const bool bCircular)
    {
        m_sampleRate = sr;
        m_channelNum = 1;
        m_sample = 1024;
        m_deviceID = 0;
        m_audioRecordingOn = false;
        m_lengthSecond = s; // circular buffer size in second

        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            std::cout << "ERR - Failed to init SDL audio: " << SDL_GetError() << std::endl;
            return false;
        }

        //Get capture device count
        int deviceCount = SDL_GetNumAudioDevices(SDL_TRUE);

        if (deviceCount < 1)
        {
            std::cout << "ERR - There is no valid devices: " << SDL_GetError() << std::endl;
            return false;
        }
        
        std::cout << "INFO - There are: " << deviceCount << " recording devices available." << std::endl;
        
        // get device names
        for (int i=0; i<deviceCount; i++)
        {
            std::cout << "INFO - Device" << i << ": " << SDL_GetAudioDeviceName(i, SDL_TRUE) << std::endl;
        }

        //Default recording spec
        SDL_AudioSpec desiredRecordingSpec, receivedRecordingSpec;

        SDL_zero(desiredRecordingSpec);
        SDL_zero(receivedRecordingSpec);

        desiredRecordingSpec.freq = m_sampleRate;
        desiredRecordingSpec.format = AUDIO_F32; //float32 of little endian byte order
        desiredRecordingSpec.channels = m_channelNum;
        desiredRecordingSpec.samples = m_sample;

        desiredRecordingSpec.userdata = buffer;

        if (bCircular)
        {
            desiredRecordingSpec.callback = [](void *userdata, Uint8 *stream, int len) {
                CircularBuffer * buffer = (CircularBuffer *) userdata;
                buffer->fill(stream, len/sizeof(float));
            };
        }
        
        //Open recording device
        if ((deviceNum >= 0) && (deviceNum < deviceCount))
        {
            std::cout << "INFO - Attempt to open recording device: " << deviceNum << std::endl;
            m_deviceID = SDL_OpenAudioDevice( SDL_GetAudioDeviceName(deviceNum, SDL_TRUE), 
                                            SDL_TRUE, 
                                            &desiredRecordingSpec, 
                                            &receivedRecordingSpec, 
                                            0 
                                            );
        }
        else
        {
            std::cout << "INFO - Attempt to open the default recording device." << std::endl;
            m_deviceID = SDL_OpenAudioDevice( nullptr, 
                                            SDL_TRUE, 
                                            &desiredRecordingSpec, 
                                            &receivedRecordingSpec, 
                                            0 
                                            );
        }
        
        if(m_deviceID == 0)
        {
            std::cout << "ERR - Failed to open recording device :" << SDL_GetError() << std::endl;
            return false;
        }

        std::cout << "INFO - Opened the audio device: " << m_deviceID << std::endl;

        // update the parameters
        m_sampleRate = receivedRecordingSpec.freq;
        m_sample = receivedRecordingSpec.samples;
        m_channelNum = receivedRecordingSpec.channels;

        // set buffer size
        if (m_sampleRate != sr)
        {
            buffer->setLen(m_sampleRate * s);
        } 
        //m_recordingBuffer.resize(m_sampleRate * m_lengthSecond);
        //std::fill(m_recordingBuffer.begin(), m_recordingBuffer.end(), 0);
        // m_recordingBuffer.init(m_sampleRate, m_lengthSecond);
        std::cout << "INFO - audio recording buffer size: " << buffer->getResLen() << std::endl;

        return true;
    }

    bool SDLAudioUtil::resume()
    {
        if(m_deviceID == 0)
        {
            std::cout << "ERR - No active recording device t resume." << std::endl;
            return false;
        }
        if (m_audioRecordingOn)
        {
            std::cout << "ERR - Recording device already on." << std::endl;
            return false;
        }

        SDL_PauseAudioDevice(m_deviceID, SDL_FALSE);
        m_audioRecordingOn = true;

        return true;

    }

    bool SDLAudioUtil::pause()
    {
        if(m_deviceID == 0)
        {
            std::cout << "ERR - No active recording device to pause." << std::endl;
            return false;
        }
        if (!m_audioRecordingOn)
        {
            std::cout << "ERR - Recording device already paused." << std::endl;
            return false;
        }

        SDL_PauseAudioDevice(m_deviceID, SDL_TRUE);
        m_audioRecordingOn = false;

        return true;
    }

    // bool SDLAudioUtil::reset()
    // {
    //     if (m_recordingBuffer.size() == 0)
    //     {
    //         std::cout << "WARNING - Nothing to reset." << std::endl;
    //         return false;
    //     }

    //     SDL_LockAudioDevice(m_deviceID);
    //     m_recordingBuffer.clear();
    //     SDL_UnlockAudioDevice(m_deviceID);

    //     return true;
    // }

    // // not safe
    // CircularBuffer& SDLAudioUtil::getBuffer()
    // {
    //     return m_recordingBuffer;
    // }

    // // not safe
    // std::vector<float>& SDLAudioUtil::getData()
    // {
    //     return m_recordingBuffer.data;
    // }

    // // safe access to buffer
    // std::vector<float> SDLAudioUtil::viewData(int& pr, int& pw)
    // {
    //     std::vector<float> data(m_recordingBuffer.len, 0.0f);
    //     m_recordingBuffer.peep(data, pr, pw);

    //     return data;
    // }

    // void util::SDLAudioUtil::recordCircularCallback(Uint8 *stream, int byteLen)
    // {
    //     size_t newFloatLen = byteLen / sizeof(_Float32);
    //     m_recordingBuffer.fill(stream, newFloatLen);
    // }


    // void util::SDLAudioUtil::recordInfCallback(Uint8 *stream, int byteLen)
    // {
    //     int newFloatLen = byteLen / sizeof(_Float32);
    //     m_recordingBuffer.fill(stream, newFloatLen); 

    // }

    // void util::SDLAudioUtil::getSeg(int ms, std::vector<float> &out)
    // {
    //     if (!m_audioRecordingOn)
    //     {
    //         std::cout << "ERR - Not recording." << std::endl;
    //         return;
    //     }

    //     if (ms <= 0)
    //     {
    //         std::cout << "ERR - Not valid length." << std::endl;
    //         return;
    //     }

    //     out.clear();

    //     // compute float length
    //     size_t len = m_sampleRate * ms / 1000;

    //     //Lock callback
    //     SDL_LockAudioDevice(m_deviceID);
    //     bool bEnoughLen = (len < m_recordingLen);
    //     SDL_UnlockAudioDevice(m_deviceID);
    //     if (!bEnoughLen)
    //     {
    //         std::cout << "ERR - Not valid length." << std::endl;
    //         return;
    //     }

    //     out.resize(len);

    //     SDL_LockAudioDevice(m_deviceID);
    //     int s0 = m_recordingPos - len;
    //     if (s0 < 0) {
    //         s0 += m_recordingBuffer.size();
    //     }

    //     if (s0 + len > m_recordingBuffer.size()) {
    //         const size_t n0 = m_recordingBuffer.size() - s0;

    //         memcpy(out.data(), &m_recordingBuffer[s0], n0 * sizeof(float));
    //         memcpy(&out[n0], &m_recordingBuffer[0], (len - n0) * sizeof(float));
    //     } else {
    //         memcpy(out.data(), &m_recordingBuffer[s0], len * sizeof(float));
    //     }

    //     SDL_UnlockAudioDevice(m_deviceID);
    // }

}