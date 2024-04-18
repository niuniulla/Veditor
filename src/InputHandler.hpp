#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <iostream>
#include <memory>

#include "EditorViewer.hpp"
#include "DocumentEditor.hpp"
#include "SDLAudioUtil.hpp"
#include "WhisperUtil.hpp"
#include "CircularBuffer.hpp"

namespace veditor
{
    enum InputMode
    {
        keyboard, voicerecord, stream, usefile,
    };

    struct Command
    {
        bool bRecording = false;
        bool bRecorded = false;

        // set font
        const char* fontfile = "../resources/font/monospace-medium.ttf";
        int fontsize = 20;

        // set text color
        int textcolorR = 255;
        int textcolorG = 255;
        int textcolorB = 255;

        InputMode inputmode = keyboard;

    };


    class Input
    {
        public:
            Input(){};
            ~Input() {};

            bool init();

            void setTextColor(Viewer& viewer);

            void setInputMode(InputMode mode);

            void handleEvent(SDL_Event *event, Editor &editor, Viewer& viewer, bool &flagDocChanged);

            void handleProcess();

            InputMode getMode() const {return m_commands.inputmode;};

            std::shared_ptr<util::SDLAudioUtil> getAudioRecording(){return m_audiorecording;};

            std::shared_ptr<util::CircularBuffer> getRecordingBuffer(){return m_recordingBuffer;};

            float* getStreamThreshold() {return &m_streamThreshold;};

            util::StreamOptions& getStreamOption() {return m_streamOptions;};

            std::string transcripts;

        private:
            std::pair<int, int> m_cameraPos;

            Command m_commands;

            std::shared_ptr<util::CircularBuffer> m_recordingBuffer;

            std::shared_ptr<util::SDLAudioUtil> m_audiorecording;

            util::whisper_params params;
            util::WhisperCTX m_whisper;

            float m_streamThreshold; 
            
            util::StreamOptions m_streamOptions;

    };
}


#endif // INPUTHANDLER_H