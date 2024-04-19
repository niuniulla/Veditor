#include "ImGuiApp.hpp"


namespace application
{

    ImGuiApp::ImGuiApp()
    {
        
    }

    ImGuiApp::~ImGuiApp()
    {
        // Cleanup imgui
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }

    bool ImGuiApp::onInit()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& m_imguiIO = ImGui::GetIO(); (void)m_imguiIO;
        m_imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        m_imguiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        m_imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking - should be on docking branch
        m_imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForSDLRenderer(m_pSDLWindow->getNativeWindow(), 
                                          m_pSDLWindow->getNativeRenderer());

        ImGui_ImplSDLRenderer2_Init(m_pSDLWindow->getNativeRenderer());

        // load resources
        AssetManager::get()->loadTextures(m_pSDLWindow->getNativeRenderer());
        AssetManager::get()->loadImGuiFonts(m_imguiIO);

        // set palette for text
        m_docViewer.setPalette(veditor::IGViewer::GetDarkPalette());
        return true;
        
    }

    void ImGuiApp::onLoop()
    {

    }

    void ImGuiApp::onRender()
    {
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGuiIO& io = ImGui::GetIO();

        // show top menu bar
        showTopMenu();

        // show left side menu bar
        showSideMenu();
        
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        viewport->WorkPos = ImVec2(80, 20);
        
        // set docking view
        ImGui::DockSpaceOverViewport();

        // show text area
        m_docViewer.render(m_docEditor);

        // dependent windows
        showWave(&m_showWaveWindow);
        showCommand(&m_showCommandWindow);
        showContent(&m_showContentWindow);
        showGame(&m_showGameWindow);
        showDebug(&m_showDebugWindow);
        showAbout(&m_showAboutWindow);


        ImGui::Render();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        SDL_RenderSetScale( m_pSDLWindow.get()->getNativeRenderer(), 
                            io.DisplayFramebufferScale.x, 
                            io.DisplayFramebufferScale.y);

        SDL_SetRenderDrawColor( m_pSDLWindow.get()->getNativeRenderer(), 
                                (Uint8)(clear_color.x * 255), 
                                (Uint8)(clear_color.y * 255), 
                                (Uint8)(clear_color.z * 255), 
                                (Uint8)(clear_color.w * 255));

        SDL_RenderClear(m_pSDLWindow.get()->getNativeRenderer());
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(m_pSDLWindow.get()->getNativeRenderer());
    }


    // construct side bar menu
    void ImGuiApp::showSideMenu()
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Set position to the bottom of the viewport
        ImGui::SetNextWindowPos(ImVec2(0, 20));

        // Extend width to viewport width
        ImGui::SetNextWindowSize(ImVec2(80, viewport->Size.y-20));

        // Add menu bar flag and disable everything else
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse ;
        
        
        ImGui::Begin(" ", NULL, flags); 
   
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

        if (ImGui::BeginTabBar("TabBar", tab_bar_flags))
        {
            float s = 64.0f;
            ImVec2 size = ImVec2(s, s);  // Size of the image we want to make visible
            
            // show wave button
            SDL_Texture* tex;
            bool bRecording = false;
            if (m_inputHandler.getAudioRecording()->isRecordingOn())
            {
                tex = AssetManager::get()->getSDLTexture("button_recording_on");
                bRecording = true;
            }
            else
            {
                tex = AssetManager::get()->getSDLTexture("button_recording_off");
                bRecording = false;
            }
            if (ImGui::ImageButton("button_recording_off", (void*)tex, size))
            {
                std::cout << "image button0" << std::endl;
                if (bRecording)
                {
                    m_inputHandler.getAudioRecording()->pause();
                }
                else
                {
                    m_inputHandler.getAudioRecording()->resume();
                }

            }
            // button wave tooltip   
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                ImGui::SetTooltip("Turn on/off audio input device", ImGui::GetStyle().HoverDelayShort);


            // show wave button
            tex = AssetManager::get()->getSDLTexture("button_wave");
            if (ImGui::ImageButton("button_wave", (void*)tex, size))
            {
                std::cout << "image button1" << std::endl;
                m_showWaveWindow = !m_showWaveWindow;
            }
            // button wave tooltip   
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                ImGui::SetTooltip("Show voice signal", ImGui::GetStyle().HoverDelayShort);

            // show content button
            tex = AssetManager::get()->getSDLTexture("button_content");
            if (ImGui::ImageButton("button_content", (void*)tex, size))
            {
                std::cout << "image button2" << std::endl;
                m_showContentWindow = !m_showContentWindow;
            }
            // button content tooltip 
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                ImGui::SetTooltip("Show transcription", ImGui::GetStyle().HoverDelayShort);

            // show command button
            tex = AssetManager::get()->getSDLTexture("button_command");
            if (ImGui::ImageButton("button_command", (void*)tex, size))
            {
                std::cout << "image button3" << std::endl;
                m_showCommandWindow = !m_showCommandWindow;
            }
            // button command tooltip
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                ImGui::SetTooltip("Show commands", ImGui::GetStyle().HoverDelayShort);

            // show button game
            tex = AssetManager::get()->getSDLTexture("button_game");
            if (ImGui::ImageButton("button_game", (void*)tex, size))
            {
                std::cout << "image button4" << std::endl;
                m_showGameWindow = !m_showGameWindow;
            }
                
            // button game tooltip
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                ImGui::SetTooltip("Show games", ImGui::GetStyle().HoverDelayShort);

            // show button game
            tex = AssetManager::get()->getSDLTexture("button_debug");
            if (ImGui::ImageButton("button_debug", (void*)tex, size))
            {
                std::cout << "image button5" << std::endl;
                m_showDebugWindow = !m_showDebugWindow;
            }
                
            // button game tooltip
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
                ImGui::SetTooltip("Show debug options", ImGui::GetStyle().HoverDelayShort);


            //ImGui::Image((void*)tex, ImVec2(128, 128));
            //ImGui::TreePop();
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    // show top menu
    void ImGuiApp::showTopMenu()
    {
        // menubar
        if (ImGui::BeginMainMenuBar()) 
        {

            if (ImGui::BeginMenu("File")) 
            {
                if (ImGui::MenuItem("New")) 
                {
                    m_docEditor.newDocument();
                }
                if (ImGui::MenuItem("Open")) 
                {
                    m_docEditor.newDocument();
                }
                if (ImGui::MenuItem("Exit")) 
                {
                    m_atomRunning = false;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Assist")) 
            {
                if (ImGui::MenuItem("Record"))
                {
                    // record
                }
                if (ImGui::MenuItem("Audio"))
                {
                    // load file
                }
                if (ImGui::MenuItem(".WAV"))
                {
                    // load file
                    //util::FileManager::open_wav();
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("View")) 
            {
                if (ImGui::MenuItem("Wave"))
                {
                    m_showWaveWindow = !m_showWaveWindow;
                }
                if (ImGui::MenuItem("Content"))
                {
                    m_showContentWindow = !m_showContentWindow;
                }
                if (ImGui::MenuItem("Options"))
                {
                    m_showDebugWindow = !m_showDebugWindow;
                }
                if (ImGui::MenuItem("Command"))
                {
                    m_showCommandWindow = !m_showCommandWindow;
                }
                if (ImGui::MenuItem("Game"))
                {
                    m_showGameWindow = !m_showGameWindow;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("About")) 
            {
                if(ImGui::MenuItem("About"))
                {
                    m_showAboutWindow = true;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    // show window to dsplay about
    void ImGuiApp::showAbout(bool* p_open)
    {
        if (*p_open)
        {
            ImGui::Begin("About", p_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text(m_aboutMsg.c_str());
            if (ImGui::Button("Close"))
                m_showAboutWindow = false;

            ImGui::End();
        }
    }

    // show window of recording values
    void ImGuiApp::showWave(bool* p_open)
    {
        if (*p_open)
        {
            ImGui::Begin("Voice", p_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

            // flag for plotting axis - loack range, no label, no ticklabel
            const ImPlotAxisFlags axisflags = ImPlotAxisFlags_LockMax | 
                                              ImPlotAxisFlags_LockMin |
                                              ImPlotAxisFlags_NoLabel |
                                              ImPlotAxisFlags_NoTickLabels;
            int pr, pw, ps; // position in buffer
            std::vector<float> data;
            data.resize(m_inputHandler.getRecordingBuffer()->getLen());
            m_inputHandler.getRecordingBuffer()->peep(data, pr, pw, ps);
            std::vector<float> data_display (data.size(), 0.0f);
            std::vector<int> active_display(data.size(), 0);
            int nth = 2000; // display a point for every nth data in buffer
            int count = 0;
            pr /= nth; pw /= nth; ps /= nth;

            // get data for display
            for (int i = 0; i < data.size(); i += nth)
            {
                data_display[count++] = data[i];
                std::vector<float> tmpdata {data[i], data[i+nth]};
                active_display[count] = util::vad_naive(tmpdata, -50, 1.0f, true, 200, 50, m_inputHandler.getAudioRecording()->getSampleRate());
                //std::cout << "active: " << tmpdata[100] << " ";
            }
            // std::vector<int32_t> intdata(data.size(), 0);
            // util::SDL_Convert_F32_to_S32_Scalar(intdata.data(), data.data(), data.size());
            // auto min = std::min_element(intdata.begin(), intdata.end());
            // auto max = std::max_element(intdata.begin(), intdata.end());
            // std::cout << "active: " << *min << " " << *max << std::endl;

            if (ImPlot::BeginPlot("##Line Plots")) 
            {
                ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                ImPlot::SetupAxes("X", "Y", axisflags, axisflags); // set axis with flag
                ImPlot::SetupAxesLimits(0, count, -0.5, 0.5); // set axis limits
                ImPlot::PlotLine("##f(x)", data_display.data(), count);

                ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.3f);
                ImPlot::PlotShaded("##v(x)", active_display.data(), count);

                ImPlot::PopStyleVar();
                ImGui::PushItemWidth(ImGui::GetWindowWidth()-17); // magic number to align slider with plot area
                ImGui::Text("Position of writing in buffer by recording device");
                ImGui::SliderInt("##Reading position", &pw, 0, count, "");

                ImGui::Text("Position of reading in recording buffer by whisper");
                ImGui::SliderInt("##writing position", &pr, 0, count, "");

                ImGui::Text("reslen recording buffer by whisper");
                ImGui::SliderInt("##writing position", &ps, 0, count, "");


                ImPlot::EndPlot();
            }

            if (ImGui::Button("Close"))
                m_showWaveWindow = false;
            ImGui::End();
        }
    }

    void ImGuiApp::showCommand(bool* p_open)
    {
        if (*p_open)
        {
            ImGui::Begin("command", p_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

            ImGui::Text("Place to show commands");
            ImGui::End();
        }
    }

    void ImGuiApp::showContent(bool* p_open)
    {
        if (*p_open)
        {
            static bool b = false;
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly |
                                        ImGuiInputTextFlags_NoHorizontalScroll;
            static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };
            float fontSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;
            float windowSize = ImGui::GetWindowWidth();
            static int charNo = (int)(windowSize / fontSize);
            std::string str = m_inputHandler.transcripts;
            // std::string str = "";
            // str += " The Pentium F00F bug, shorthand for F0 0F C7 C8, ";
            // str += "the hexadecimal encoding of one offending instruction ";
            // str += "more formally, the invalid operand with locked CMPXCHG8B ";
            // str += " instruction bug, is a design flaw in the majority of ";
            // str += " Intel Pentium, Pentium MMX, and Pentium OverDrive ";
            // str += " processors (all in the P5 microarchitecture). ";
            // str += "label: ";
            // str += "\tlock cmpxchg8b eax ";

            std::string strBreak = "";
            bool addbreak = false;
            for (int i=0; i<str.size(); i++)
            {
                strBreak += str[i];
                
                if ((i % charNo) == 0)
                {
                    addbreak = true;
                }
                if (addbreak && (str[i] == ' '))
                {
                    strBreak += "\n";
                    addbreak = false;
                } 
                
            }

            ImGui::Begin("Transcript", p_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

            //ImGui::Text("Place to show transcripts");
            ImGui::InputTextMultiline("##ITEM: InputTextMultiline", &strBreak[0], IM_ARRAYSIZE(strBreak.c_str()), ImVec2(-1.0f,-1.0f), flags);

            ImGui::End();
        }
    }

    void ImGuiApp::showGame(bool* p_open)
    {
        if (*p_open)
        {
            ImGui::Begin("game", p_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

            ImGui::Text("Place to show games");
            ImGui::End();
        }
    }

    void ImGuiApp::showDebug(bool* p_open)
    {
        if (*p_open)
        {
            ImGui::Begin("debug", p_open);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

            ImGui::Text("Place to show parameters for debug purposes");
            bool ret = false;
            static bool b = false;
            static float col4f[4] = { 1.0f, 0.5, 0.0f, 0.5f };
            
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;

            ImGui::Checkbox("Use VAD", &(m_inputHandler.getStreamOption().useVad));
            if (m_inputHandler.getStreamOption().useVad == true)
            {
                ImGui::SliderFloat("VAD threshold", &(m_inputHandler.getStreamOption().vadThreshold), -100.0f, -10.0f);  // Testing basic item
            }
            
            ImGui::Checkbox("Use filter", &(m_inputHandler.getStreamOption().useFilter));
            if (m_inputHandler.getStreamOption().useFilter == true)
            {
                ImGui::SliderFloat("low limit 0-100", &(m_inputHandler.getStreamOption().filterLow), 0.0f, 100.0f);
                ImGui::SliderFloat("high limit 100-200", &(m_inputHandler.getStreamOption().filterHigh), 100.0f, 200.0f);
            }

            ImGui::Checkbox("Change buffer", &(m_inputHandler.getStreamOption().changeLen));
            if (m_inputHandler.getStreamOption().changeLen == true)
            {
                ImGui::SliderInt("seconds 1-10", &(m_inputHandler.getStreamOption().lengthSecond), 1, 10);
            }

            ImGui::End();
        }
    }

    // Helper to display a little (?) mark which shows a tooltip when hovered.
    // In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
    void ImGuiApp::HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

}