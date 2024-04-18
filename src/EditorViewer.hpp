#ifndef DOCUMENTVIEWER_H
#define DOCUMENTVIEWER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>

#include "Document.hpp"
#include "DocumentEditor.hpp"

namespace veditor
{
    class Viewer
    {
        public:
            Viewer();
            ~Viewer();

            void initialize(int sizeX, int sizeY);
            int getSizeX() const;
            int getSizeY() const;

            void updateRenderer(SDL_Renderer *pRenderer);
            bool setFont(const char *file, int fontSize);
            void setTextColor(const unsigned char r, const unsigned char g, const unsigned char b);
            void setCameraPos(int x, int y);
            
            bool updateTexture(SDL_Renderer *pRenderer, const Editor &editor);

        private:
            void setSize(int sizeX, int sizeY);
            void drawCursor(SDL_Surface *displaySurface, const Editor &editor);

            void drawLineNumber(SDL_Surface *displaySurface);


        private:
            int m_textureWidth, m_textureHeight; // size of the text area, 
            SDL_Texture *m_pTexture;

            // text font
            TTF_Font *m_pTextFont;
            SDL_Color m_textColor;
            int m_fontSize;
            int m_lineSkip;

            std::pair<int, int> m_cameraPos;

    };
}


#endif //DOCUMENTVIEWER_H