#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include<map>
#include<string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"

#include "../extern/imgui/imgui.h"
#include "../extern/imgui/backends/imgui_impl_sdl2.h"
#include "../extern/imgui/backends/imgui_impl_sdlrenderer2.h"

#include "AssetManager.hpp"

namespace application
{
    class AssetManager 
    {
        public:

            ~AssetManager();

            // copy constructor
            AssetManager(AssetManager const&) = delete; // for c++17 and up
            //copy assignment
            void operator=(AssetManager const&) = delete;

            void loadTextures(SDL_Renderer* renderer);
            void loadImGuiFonts(ImGuiIO& io);
            void loadFonts();

            SDL_Texture* getSDLTexture(const std::string name);
            TTF_Font* getTTFFont(const std::string name);
            ImFont* getImGuiFont(const std::string name);

            static AssetManager* get();


        private:

            AssetManager();
            static AssetManager _assetmanager;

            std::map<std::string, SDL_Texture*> m_texMap;
            std::map<std::string, TTF_Font*> m_fontMap;
            std::map<std::string, ImFont*> m_IMFontMap;
    };
}

#endif // ASSETMANAGER_H