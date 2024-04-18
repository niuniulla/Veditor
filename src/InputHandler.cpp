#include "InputHandler.hpp"

namespace veditor
{ 
    bool Input::init()
    {

        // create buffer
        int seconds = 30;
        int samplerate = 16000;
        m_recordingBuffer = std::make_shared<util::CircularBuffer>(seconds * samplerate);

        // setup sdl audio
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initiaize recording device.");
        m_audiorecording = std::make_shared<util::SDLAudioUtil>();
        m_audiorecording->init(0, seconds, samplerate, m_recordingBuffer.get(), true); // buffer of total of 30s

        // setup whisper
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Init WhisperCpp.");
        util::whisper_params params;
        params.model = "../resources/models/ggml-tiny.en.bin";
        std::cout << "DEBUG - whisper params - model: " << params.model << std::endl;
        m_whisper.init(params, m_audiorecording->getSampleRate());

        m_commands.inputmode = keyboard;

        std::cout << "INFO - Input mode: " << m_commands.inputmode << std::endl;

        transcripts = "";

        m_streamThreshold = -50.0f;

        return true;

    }


    void Input::setTextColor(Viewer& viewer)
    {
        viewer.setTextColor(m_commands.textcolorR, m_commands.textcolorG, m_commands.textcolorB);
    }


    void Input::handleProcess()
    {
        // if recording paused, not process
        if (!m_audiorecording->isRecordingOn())
        {
            return;
        }
        

        std::string transcript = "";

        //util::CircularBuffer &buffer = m_audiorecording->getBuffer();

        m_whisper.stream(m_recordingBuffer.get(), m_streamOptions, transcript);
        
        //editor.insertText(transcript);
        
        transcripts += transcript;
        std::cout << "text: " << transcripts << std::endl;
        //flagDocChanged = true;

    }


    void Input::handleEvent(SDL_Event* event, Editor &editor, Viewer& viewer, bool &flagDocChanged)
    {
        flagDocChanged = false;

        
        switch (event->type)
        {
            case SDL_MOUSEWHEEL:
            {
                int changeViewportX = event->wheel.x;
                int changeViewportY = event->wheel.y;

                m_cameraPos.first -= changeViewportX;
                m_cameraPos.second -= changeViewportY;

                viewer.setCameraPos(m_cameraPos.first, m_cameraPos.second);

                break;
            }
            case SDL_TEXTINPUT:
            {
                if (m_commands.inputmode == keyboard)
                {
                    char* charPressed = event->text.text;
                    editor.insertText(charPressed);
                    flagDocChanged = true;
                    //std::cout << "INFO - entered key: " << charPressed << std::endl;
                    //std::cout << "INFO - text: " << editor.getDocuments().size()<< std::endl;
                }
                break;
            }

            case SDL_KEYDOWN:
            {
                if (m_commands.inputmode == voicerecord)
                {
                    if (event->key.keysym.sym == SDLK_SPACE)
                    {
                        //std::cout << "DEBUG - spacebar pressed" << std::endl;
                        if (!m_commands.bRecording)
                        {
                            m_commands.bRecording = true;
                            m_commands.bRecorded = false;
                            //m_audiorecording->reset();
                            // start to record
                            std::cout << "INFO - recording..." << std::endl;
                            m_audiorecording->resume();
                        }
                    }   
                }
                break;
            }

            case SDL_KEYUP:
            {
                if (event->key.keysym.sym == SDLK_HOME)
                {
                    editor.moveCurStart();
                    flagDocChanged = true;
                }

                else if (event->key.keysym.sym == SDLK_END)
                {
                    editor.moveCurEnd();
                    flagDocChanged = true;
                }

                else if (event->key.keysym.sym == SDLK_UP)
                {
                    editor.moveCurUp();
                    flagDocChanged = true;
                }
                
                else if (event->key.keysym.sym == SDLK_DOWN)
                {
                    editor.moveCurDown();
                    flagDocChanged = true;
                }

                else if (event->key.keysym.sym == SDLK_LEFT)
                {
                    editor.moveCurLeft();
                    flagDocChanged = true;
                }

                else if (event->key.keysym.sym == SDLK_RIGHT)
                {
                    editor.moveCurRight();
                    flagDocChanged = true;
                }

                else if (event->key.keysym.sym == SDLK_RETURN)
                {
                    editor.newLine();
                    flagDocChanged = true;
                }

                else if (event->key.keysym.sym == SDLK_BACKSPACE)
                {
                    editor.deleteChar(false);
                    flagDocChanged = true;
                }

                else if (event->key.keysym.sym == SDLK_DELETE)
                {
                    editor.deleteChar(true);
                    flagDocChanged = true;
                }

                if (m_commands.inputmode == voicerecord)
                {
                    if (event->key.keysym.sym == SDLK_SPACE)
                    {
                        //std::cout << "DEBUG - spacebar released" << std::endl;
                        if (m_commands.bRecording)
                        {
                            m_commands.bRecording = false;
                            m_commands.bRecorded = true;
                            std::cout << "INFO - Finished recording." << std::endl;
                            // pause recording
                            m_audiorecording->pause();
                            // get recording buffer
                            //std::vector<float> buffer = m_audiorecording->getData();
                            // do inference
                            std::string transcript;
                            std::cout << "INFO - Transcripting..." << std::endl;
                            // call inference
                            //m_whisper.transcribe(buffer, transcript);
                            std::cout << "INFO - Finished Transcripting." << std::endl;
                            // add to txt
                            std::cout << "INFO - transcript: " << transcript << std::endl;
                            // insert the transcript to the text
                            editor.insertText(transcript);
                            flagDocChanged = true;
                        }
                    }   
                }
                break;
            }

            
            default:
                break;
        }
    }

}