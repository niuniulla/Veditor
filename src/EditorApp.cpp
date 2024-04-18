#include "EditorApp.hpp"

namespace application
{
    // constructor
    EditorApp::EditorApp()
    {

    }

    // destructor
    EditorApp::~EditorApp()
    {
        onExit();
    }

    // static member initialization
    std::atomic<bool> EditorApp::m_atomRunning{true}; 

    // native initialization
    bool EditorApp::init()
    {
        std::string name = "VEditor";
        m_aboutMsg = loadtxt(m_aboutFileName.c_str());
        int height = 1000;
        int width = 1400;
        m_enterMode = 0;

        // initialize SDL
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL cannot initialize. SDL_Error :%s", SDL_GetError());
            return false;
        }

            // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

        // create sdl window
        m_pSDLWindow = std::make_unique<SDLWindow>(name, width, height);

        if (!m_pSDLWindow)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL cannot initialize. SDL_Error : %s", SDL_GetError());
            return false;
        }

        // initialize SDL_TTF
        if (TTF_Init() < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error initializing SDL ttf: %s", TTF_GetError());
        }

        // // set bg color
        // SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
        // SDL_RenderClear(m_pRenderer);

        // // set renderer viewport
        // m_renderViewport = {0, 0, m_screenWidth, m_screenHeight};
        // SDL_RenderSetViewport(m_pRenderer, &m_renderViewport);

        // // create document editor
        // m_docEditor.newDocument();
        // SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Created New document.");

        // // set editor
        // //m_docViwer.setDocEditor(&m_docEditor);

        // std::cout << "DEBUG3 - set doc finsihed." << std::endl;

        // init inputhandler
        m_inputHandler.init();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Init inputhandler.");

        // // create texture
        // m_docViwer.updateTexture(m_pRenderer, m_docEditor);

        // // update renderer
        // m_docViwer.updateRenderer(m_pRenderer);

        // // update window drawing
        // //SDL_RenderPresent(m_pRenderer);

        // m_inputMode = veditor::voicerecord;

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL init successful.");


        return true;
    }

    // 
    void EditorApp::onEvent(SDL_Event *event)
    {
        if (event->type == SDL_QUIT)
        {
            SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "User close.");
            m_atomRunning = false;
        }
        
        // else if (event->type == SDL_WINDOWEVENT)
        // {
        //     if( event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        //     {
        //         m_bWindowResized = true;
        //         m_pSDLWindow->setWindowWidth(event->window.data1);
        //         m_pSDLWindow->setWindowHeight(event->window.data2);

        //         //SDL_RenderPresent(m_pRenderer);
        //     }
        // }
        else
        {
            m_inputHandler.handleEvent(event, m_docEditor, m_docViwer, m_flagDocChanged);
        }

    }

    void EditorApp::onProcess()
    {
        m_inputHandler.handleProcess();
    }

    // main loop 
    int EditorApp::onExecute()
    {
        SDL_Event event;

        if (!init())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize the engin.");
            return -1;
        }
        if (!onInit())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize user requirements.");
            return -1;
        }

        bool show_demo_window = true;

        while (m_atomRunning)
        {
            if (SDL_PollEvent(&event) != 0)
            {
                ImGui_ImplSDL2_ProcessEvent(&event); // process sdl event
                onEvent(&event);
            }

            onProcess();

            onLoop();
            onRender();

        }
        return 0;
    }

    // void EditorApp::onLoop()
    // {
    
    // }

    // void EditorApp::onRender()
    // {
    //     // if (m_flagDocChanged || m_bWindowResized)
    //     // {
    //     //     m_flagDocChanged = false;
    //     //     // clear the renderer  
    //     //     SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
    //     //     SDL_RenderClear(m_pRenderer);

    //     //     m_docViwer.updateTexture(m_pRenderer, m_docEditor);

    //     //     // update the renderer after event changes
    //     //     m_docViwer.updateRenderer(m_pRenderer);

    //     //     // draw renderer on window
    //     //     SDL_RenderPresent(m_pRenderer); 
    //     // }   

    //     // Rendering
    //     //ImGuiIO& io = ImGui::GetIO();
    //     //m_imguiIO.DisplaySize = ImVec2((float)m_screenWidth, (float)m_screenHeight);

        
    // }

    // clean-up
    void EditorApp::onExit()
    {
        // cleanup sdl
        TTF_Quit();
        SDL_Quit();
    
    }


    // helper function to load txt
    std::string EditorApp::loadtxt(const char* name)
    {
        std::stringstream ss;
        std::fstream f(name);
        if (f.is_open())
        {
            std::string l;
            while(getline(f, l))
            {
                ss << l << '\n';
            }
            f.close();
        }
        return ss.str();
    }

    SDL_Texture* EditorApp::loadImg2Texture(const char* name)
    {
        SDL_Surface* surf = IMG_Load(name);
        if (!surf)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load imgage: %s", SDL_GetError());
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(m_pSDLWindow->getNativeRenderer(), surf);
        if (!tex)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from image: %s", SDL_GetError());
        }
        SDL_FreeSurface(surf);
        
        surf = nullptr;
        return tex;
    }
}
