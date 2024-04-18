#include "SDLWindow.hpp"

namespace application
{
    SDLWindow::SDLWindow(std::string name, int w, int h) :
        m_appName(name),
        m_windowWidth(w),
        m_windowHeight(h)
    {
        SDL_WindowFlags wFlags = SDL_WindowFlags(SDL_WINDOW_RESIZABLE | 
                                                 SDL_WINDOW_SHOWN |
                                                 SDL_WINDOW_ALLOW_HIGHDPI);
         // create window
        m_pWindow = SDL_CreateWindow( "Editor", 
                                      SDL_WINDOWPOS_CENTERED, 
                                      SDL_WINDOWPOS_CENTERED,
                                      m_windowWidth, m_windowHeight, 
                                      wFlags);

        if (!m_pWindow)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create SDL window : %s", SDL_GetError());
        }

        SDL_RendererFlags rFlags = SDL_RendererFlags(SDL_RENDERER_ACCELERATED);

        m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, rFlags);
        
        if (!m_pRenderer)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create SDL renderer : %s", SDL_GetError());
        }

    }

    SDLWindow::~SDLWindow()
    {
        SDL_DestroyRenderer(m_pRenderer);
        SDL_DestroyWindow(m_pWindow);
    }

}