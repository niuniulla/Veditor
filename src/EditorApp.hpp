#ifndef EDITORAPP_H
#define EDITORAPP_H

#include <iostream>
#include <memory>
#include <atomic>
#include <fstream>
#include <sstream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "EditorViewer.hpp"
#include "InputHandler.hpp"
#include "SDLWindow.hpp"

#include "../extern/imgui/imgui.h"
#include "../extern/imgui/backends/imgui_impl_sdl2.h"
#include "../extern/imgui/backends/imgui_impl_sdlrenderer2.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

namespace application
{
    class EditorApp
    {
        public:
            EditorApp();
            ~EditorApp();
            
            bool init();
            void onEvent(SDL_Event *event);
            void onProcess();
            int onExecute();
            void onExit();

            virtual bool onInit() = 0;
            virtual void onLoop() = 0;
            virtual void onRender() = 0;
            

        protected:
            // viewer
            veditor::Viewer m_docViwer;

            // sdl stuff
            static std::atomic<bool> m_atomRunning;

            // sdl window
            std::unique_ptr<SDLWindow> m_pSDLWindow {nullptr};

            // event variables
            bool m_bWindowResized = false;
            int m_ChangeViewportX = 0;
            int m_ChangeViewportY = 0;
            SDL_Rect m_renderViewport;
            char *m_charPressed;
            bool m_bInputChar = false;
            bool m_bKeyPressed = false;
            SDL_Keycode m_keyDown;

            int m_enterMode;

            //editor
            veditor::Editor m_docEditor;

            // text imput mode
            veditor::InputMode m_inputMode;

            veditor::Input m_inputHandler;

            // text changed
            bool m_flagDocChanged = false;

            // about text
            std::string m_aboutFileName = "../resources/text/about.txt";
            std::string m_aboutMsg;

        protected:

            std::string loadtxt(const char* name);
            SDL_Texture* loadImg2Texture(const char* name);


    };

}



#endif // EDITORAPP_H