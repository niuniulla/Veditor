#include "AssetManager.hpp"
#include <iostream>
#include <algorithm>

namespace application
{
    AssetManager::AssetManager()
    {

    }

    AssetManager::~AssetManager()
    {
        std::for_each(m_texMap.begin(), m_texMap.end(), [](std::pair<std::string, SDL_Texture*> p){SDL_DestroyTexture(p.second);});
        std::for_each(m_fontMap.begin(), m_fontMap.end(), [](std::pair<std::string, TTF_Font*> p){TTF_CloseFont(p.second);});
    }

   AssetManager* AssetManager::get() 
    {
        static AssetManager *const obj = new AssetManager{};
        return obj;
    }

    void AssetManager::loadTextures(SDL_Renderer* renderer)
    {
        auto load = [&](const std::string name, const char* file){ 
            SDL_Surface* surf = IMG_Load(file);
            if (!surf)
                std::cout << "error surface" << std::endl;
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            if (!tex)
                std::cout << "error texture" << std::endl;
            m_texMap[name] = tex;
            SDL_FreeSurface(surf);
        };

        // https://icons8.com/icons/nolan
        load("button_wave", "../resources/icon/signal.png");
        load("button_content", "../resources/icon/lists.png");
        load("button_command", "../resources/icon/command.png");
        load("button_game", "../resources/icon/square.png");
        load("button_recording_on", "../resources/icon/circle-filled.png");
        load("button_recording_off", "../resources/icon/circle-empty.png");
        load("button_debug", "../resources/icon/debug.png");
    }

    void AssetManager::loadImGuiFonts(ImGuiIO& io)
    {
        auto load = [&](const std::string name, const char* file, const float size){ 
            m_IMFontMap[name] = io.Fonts->AddFontFromFileTTF(file, size);
        };

        io.Fonts->AddFontDefault();
        load("mono20", "../resources/font/monospace-medium.ttf", 20.0f);
    }

    void AssetManager::loadFonts()
    {

        auto load = [&](const std::string name, const char* file, const float fontsize){

            m_fontMap[name] = TTF_OpenFont(file, fontsize);
        }; 

        load("monospace-20", "../resources/font/monospace-medium.ttf", 20.0f);
 
    }

    SDL_Texture* AssetManager::getSDLTexture(const std::string name)
    {
        return m_texMap[name];
    }

    TTF_Font* AssetManager::getTTFFont(const std::string name)
    {
        return m_fontMap[name];
    }

    ImFont* AssetManager::getImGuiFont(const std::string name)
    {
        return m_IMFontMap[name];
    }

}