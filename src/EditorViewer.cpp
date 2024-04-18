#include "EditorViewer.hpp"

namespace veditor
{ 

    Viewer::Viewer():
        m_textureWidth {0},
        m_textureHeight {0},
        m_pTexture {nullptr},
        m_textColor {0, 0, 0},
        m_fontSize {0},
        m_lineSkip {0}
    {
        // init camera
        m_cameraPos.first = 0;
        m_cameraPos.second = 0;

        // intitialize texture
        if (m_pTexture != nullptr)
        {
            SDL_DestroyTexture(m_pTexture);
        }

    }

    Viewer::~Viewer()
    {
        if (m_pTexture != nullptr)
        {
            SDL_DestroyTexture(m_pTexture);
            m_pTexture = nullptr;
        }
        if (!m_pTextFont)
        {
            TTF_CloseFont(m_pTextFont);
        }
        
    }

    int Viewer::getSizeX() const
    {
        return m_textureWidth;
    }

    int Viewer::getSizeY() const
    {
        return m_textureHeight;
    }

    void Viewer::setSize(int sizeX, int sizeY)
    {
        m_textureWidth = sizeX;
        m_textureHeight = sizeY;
    }

    void Viewer::setCameraPos(int x, int y)
    {
        m_cameraPos.first = x;
        m_cameraPos.second = y;
    }

    bool Viewer::updateTexture(SDL_Renderer *pRenderer, const Editor &editor)
    {
        // test font
        if(!m_pTextFont || (m_fontSize == 0) || (m_lineSkip == 0))
        {
            std::cout << "Should set font first." << std::endl;
            return false;
        }

        setSize(editor.getMaxCharLength() * m_fontSize,
                editor.getLineCount() * m_lineSkip);

        SDL_Surface *TextSurface = nullptr;

        // temprary surface to blit the window surface
        SDL_Surface *stemp = nullptr; 

        // initialize a temporary texture surface
        TextSurface = SDL_CreateRGBSurface(0, m_textureWidth, m_textureHeight, 32, 0, 0, 0, 0);
        
        for (int i=0; i<editor.getLineCount(); i++)
        {
            
            if ((editor.getCharCountInLine(i) == 0) ||
                (editor.getLineContent(i) == ""))
            {
                break;
            }
            // update the position of the temp surface vs. window surface
            SDL_Rect rec = {0, i*m_lineSkip , 0, 0};
            
            stemp = TTF_RenderText_Solid(m_pTextFont, editor.getLineContent(i).c_str(), m_textColor);

            if (!stemp)
            {
                std::cout << "text size: " << stemp->h << ",  " << stemp->w <<std::endl;
            }

            if((SDL_BlitSurface(stemp, NULL, TextSurface, &rec)) < 0)
            {
                std::cout << "Error BlitSurface : " << SDL_GetError() << std::endl;
            }

            SDL_FreeSurface(stemp);
        }

        // draw cursor
        drawCursor(TextSurface, editor);

        // test surface was correctly created
        if (!TextSurface)
        {
            std::cout << "faild to create surface" << std::endl;
            return false;
        }

        // create texture
        m_pTexture = SDL_CreateTextureFromSurface(pRenderer, TextSurface);

        // free window surface
        SDL_FreeSurface(TextSurface);

        return true;
    }

    // function to update the drawing of the document
    // obsolete
    void Viewer::updateRenderer(SDL_Renderer *pRenderer)
    {    
        // init render rect
        SDL_Rect dest;
        dest = {0, 0, m_textureWidth, m_textureHeight}; 
        // check if the border of the text horizontally
        if (m_cameraPos.first < 0)
            dest.x = m_cameraPos.first;
        if (m_cameraPos.second > 0)
            dest.y = m_cameraPos.second;   

        // set the rect to render on renderer, 
        // the texture will be squeezed in to this area
        SDL_RenderCopy(pRenderer, m_pTexture, NULL, &dest); 

    }

    // obsolete
    bool Viewer::setFont(const char *file, int fontSize)
    {
        m_pTextFont = TTF_OpenFont(file, fontSize);
        if (!m_pTextFont)
        {
            std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
            return false;
        }
        // font size
        m_fontSize = fontSize;

        // line height
        m_lineSkip = TTF_FontLineSkip(m_pTextFont);

        return true;
    }

    // obsolete
    void Viewer::setTextColor(const unsigned char r, const unsigned char  g, const unsigned char  b)
    {
        m_textColor = {r, g, b};
    }

    // obsolete
    void Viewer::drawCursor(SDL_Surface *displaySurface, const Editor &editor)
    {
        size_t curL = editor.getCurPos().first;
        size_t curC = editor.getCurPos().second;
        int posy = curL  * m_lineSkip;
        int posx = 0;

        // get position of the cursor in the line
        if (curC > 0)
        {
            SDL_Surface *temp = TTF_RenderText_Solid(m_pTextFont, 
                                                    editor.getLineContent(curL).substr(0, curC).c_str(),
                                                    m_textColor);
            posx = temp->w;
            SDL_FreeSurface(temp);
        }

        int adjust = 6; // TODO, make it proportional to the font width

        SDL_Rect cursorArea = {posx-adjust, posy, 0, 0};
        const char *sym = "|";
        SDL_Surface *curSurface = TTF_RenderText_Blended(m_pTextFont, sym, m_textColor);
        
        if((SDL_BlitSurface(curSurface, NULL, displaySurface, &cursorArea)) < 0)
        {
            std::cout << "Error BlitSurface for cursor: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(curSurface);

    }

    void Viewer::drawLineNumber(SDL_Surface *displaySurface)
    {

    }

}