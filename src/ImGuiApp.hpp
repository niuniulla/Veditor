#ifndef IMGUIAPP_H
#define IMGUIAPP_H

#include <algorithm>

#include "EditorApp.hpp"
#include "AssetManager.hpp"
#include "SDLAudioUtil.hpp"
#include "../extern/implot/implot.h"

#include "ImGuiEditorViewer.hpp"
#include "AudioUtil.hpp"

namespace application
{
    
    class ImGuiApp : public EditorApp
    {
        public:
            ImGuiApp();
            ~ImGuiApp();

            virtual bool onInit() override;
            virtual void onLoop() override;
            virtual void onRender() override;

        private:
            // window show controls
            bool m_showAboutWindow = false;
            bool m_showWaveWindow = false;
            bool m_showCommandWindow = false;
            bool m_showContentWindow = false;
            bool m_showGameWindow = false;
            bool m_showDebugWindow = true;
            bool show_wave = false;
            bool show_msg = false;
            bool show_command = false;

            // subwindows - always shown
            void showSideMenu();
            void showTopMenu();

            // dependant windows
            void showAppLongText(bool* p_open, const char* name);
            void showAbout(bool* p_open);
            void showWave(bool* p_open);
            void showCommand(bool* p_open);
            void showContent(bool* p_open);
            void showGame(bool* p_open);
            void showDebug(bool *p_open);

            // Helper to display a little (?) mark which shows a tooltip when hovered.
            // In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
            static void HelpMarker(const char* desc);

            // document viewer
            veditor::IGViewer m_docViewer;

            //


            

    };
}

#endif // IMGUIAPP_H