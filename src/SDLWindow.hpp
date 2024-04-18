#ifndef SDLWINDOW_H
#define SDLWINDOW_H

#include <string>
#include "SDL2/SDL.h"

namespace application 
{
    class SDLWindow
    {
        public:
            SDLWindow(std::string name, int w, int h);
            ~SDLWindow();

            [[nodiscard("SDL Window")]] inline SDL_Window* getNativeWindow() const {return m_pWindow;};
            [[nodiscard("SDL Renderer")]] inline SDL_Renderer* getNativeRenderer() const {return m_pRenderer;};

            inline int getWindowWidth() const {return m_windowWidth;};
            inline int getWindowHeight() const {return m_windowHeight;};

            inline void setWindowWidth(int w) {m_windowWidth = w;};
            inline void setWindowHeight(int h) {m_windowHeight = h;};


        private:
            int m_windowWidth;
            int m_windowHeight;
            std::string m_appName;

            SDL_Window *m_pWindow {nullptr};
            SDL_Renderer *m_pRenderer {nullptr};
    };
}

#endif // SDLWINDOW_H