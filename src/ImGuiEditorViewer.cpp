#include "ImGuiEditorViewer.hpp"

namespace veditor
{

    IGViewer::IGViewer() :
        m_lineSpacing{1.0f},
        m_isActive{false},
        m_startTime{0.0f},
        m_tabSize{4},
        m_renderDocNo {-1}
    {
        m_vecSettings.clear();
    }

    void IGViewer::setPalette(const Palette & aValue)
    {
        m_paletteBase = aValue;
    }

    void IGViewer::render(Document* doc, Cursor* cur)
    {
        ImGui::PushFont(application::AssetManager::get()->getImGuiFont("mono20"));
        // font size from loaded font
        const float fontSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;
        // get char space in x and y direction
        m_charSkip = ImVec2(fontSize, ImGui::GetTextLineHeightWithSpacing() * m_lineSpacing);
        // get the size of the content
        auto contentSize = ImGui::GetWindowContentRegionMax(); // TODO
        // container of drawing items, we will put all text related drawing elements in this list
        ImDrawList *drawList = ImGui::GetWindowDrawList(); // refer to the global coordinates not the widget

        // test if on top of the content
        if (mScrollToTop)
        {
            mScrollToTop = false;
            ImGui::SetScrollY(0.f);
        }

        // get the window position to start the drawing
        ImVec2 windowScreenPos = ImGui::GetCursorScreenPos(); 

        // get scrolling pos
        float scrollX = ImGui::GetScrollX();
        float scrollY = ImGui::GetScrollY();

        int scrollLines = (int)floor(scrollY / m_charSkip.y);

        int globalLineMax = (int)doc->getLineCount();
        //globalLineMax = 100;
	    auto lineMax = std::max(0, std::min((int)globalLineMax - 1, scrollLines + (int)floor((scrollY + contentSize.y) / m_charSkip.y)));
        int leftMargin = 10;

        // draw line number
        char buf[16];
	    snprintf(buf, 16, " %d ", globalLineMax);

        float textStart = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr).x + leftMargin;

        
        //for (int i=0; i<100; i++)
        while(scrollLines < globalLineMax)
        {
            std::string mLineBuffer = doc->getLineContent(scrollLines);
            auto prevColor = m_paletteBase[(int)PaletteIndex::Default];

            // get the start position of each line
            // x = screen pos x + number width + margin
            // y = screen pos y + char skip * line number
            ImVec2 textScreenPos = ImVec2(windowScreenPos.x  + textStart, windowScreenPos.y + scrollLines * m_charSkip.y);

            // draw selection

            // draw the text cursor
            bool focused = ImGui::IsWindowFocused();
            if (focused && (scrollLines==cur->getNumLine()))
            {
                renderCursor(doc, cur, textScreenPos, drawList);
            }
            
            // draw line number
            if (m_vecSettings[m_renderDocNo].m_showLineNo)
            {
                snprintf(buf, 16, "%i  ", scrollLines + 1);
                auto numColor = m_paletteBase[(int)PaletteIndex::LineNumber];
                ImVec2 lineStartScreenPos = ImVec2(windowScreenPos.x, windowScreenPos.y + scrollLines * m_charSkip.y);
                drawList->AddText(ImVec2(lineStartScreenPos.x, lineStartScreenPos.y), numColor, buf);

            }

            // display text
            ImVec2 bufferOffset; // no use for now - 0
            auto textColor = m_paletteBase[(int)PaletteIndex::Default];
            const ImVec2 newOffset(textScreenPos.x + bufferOffset.x, textScreenPos.y + bufferOffset.y);
            drawList->AddText(newOffset, textColor, mLineBuffer.c_str());
            bufferOffset.x += m_charSkip.x;
            scrollLines++;
        }
        ImGui::SetCursorPos({1000*m_charSkip.x, globalLineMax*m_charSkip.y}); //set content region

        ImGui::PopFont();

    }


    void IGViewer::render(Editor &editor)
    {
        if (editor.getDocumentCount() == 0)
        {
            return;
        }
        editor.setActive(-1);
        for (int i=0; i<editor.getDocumentCount(); i++)
        {
            m_renderDocNo = i;
            if (m_vecSettings.size() < i+1)
            {
                m_vecSettings.push_back(Settings());
            }

            ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar | 
                                     ImGuiWindowFlags_AlwaysHorizontalScrollbar | 
                                     ImGuiWindowFlags_AlwaysVerticalScrollbar;

            // default window size
            ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

            std::string docname = editor.getDocuments().at(i)->getFileName();
            if (!ImGui::Begin(docname.c_str()))
            {
                ImGui::End();
                return;
            }

            ImGui::BeginChild(docname.c_str(), ImVec2(0, 0), false, flags);

            bool focused = ImGui::IsWindowFocused();
            
            if (focused)
            {
                editor.setActive(i);
            }

            render(editor.getDocuments().at(i).get(), editor.getCursors().at(i).get());

            ImGui::EndChild();

            ImGui::End();
        }
    }

    void IGViewer::renderCursor(Document* doc, Cursor* cur, ImVec2 refPos, ImDrawList *drawList)
    {
        double timeEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        auto elapsed = timeEnd - m_startTime;
                
        if (elapsed > 400)
        {
            float width = 0.2f; // width of the cursor drawing

            int cindex = cur->getNumLine();
            //float cx = cur->getNumChar() * m_charSkip.x;
            float cx = distanceToLineStart(doc, cur);

            char buf2[2];
            buf2[0] = 'j';
            buf2[1] = '\0';
            width = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf2).x;
        
            ImVec2 cstart(refPos.x + cx, refPos.y);
            ImVec2 cend(refPos.x + cx + width/4.0f, refPos.y + m_charSkip.y);

            drawList->AddRectFilled(cstart, cend, m_paletteBase[(int)PaletteIndex::Default]);
            if (elapsed > 800)
                m_startTime = timeEnd;
        }
    }

    float IGViewer::distanceToLineStart(const Document* doc, const Cursor* cur) const
    {
        const auto& line = doc->getLineContent(cur->getNumLine());
        float distance = 0.0f;
        float spaceSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr).x;
        int colIndex = cur->getNumChar();

        for (size_t it = 0u; it < line.size() && it < colIndex; )
        {
            if (line[it] == '\t')
            {
                distance = (1.0f + std::floor((1.0f + distance) / (float(m_tabSize) * spaceSize))) * (float(m_tabSize) * spaceSize);
                ++it;
            }
            else
            {
                auto d = UTF8CharLength(line[it]);
                char tempCString[7];
                int i = 0;
                for (; i < 6 && d-- > 0 && it < (int)line.size(); i++, it++)
                    tempCString[i] = line[it];

                tempCString[i] = '\0';
                distance += ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, tempCString, nullptr, nullptr).x;
            }
        }

	    return distance;
    }

    int IGViewer::UTF8CharLength(Char c)
    {
        if ((c & 0xFE) == 0xFC)
		    return 6;
        if ((c & 0xFC) == 0xF8)
            return 5;
        if ((c & 0xF8) == 0xF0)
            return 4;
        else if ((c & 0xF0) == 0xE0)
            return 3;
        else if ((c & 0xE0) == 0xC0)
            return 2;
        return 1;
    }

    const Palette& IGViewer::GetDarkPalette()
    {
        const static Palette p = { {
                0xff7f7f7f,	// Default
                0xffd69c56,	// Keyword	
                0xff00ff00,	// Number
                0xff7070e0,	// String
                0xff70a0e0, // Char literal
                0xffffffff, // Punctuation
                0xff408080,	// Preprocessor
                0xffaaaaaa, // Identifier
                0xff9bc64d, // Known identifier
                0xffc040a0, // Preproc identifier
                0xff206020, // Comment (single line)
                0xff406020, // Comment (multi line)
                0xff101010, // Background
                0xffe0e0e0, // Cursor
                0x80a06020, // Selection
                0x800020ff, // ErrorMarker
                0x40f08000, // Breakpoint
                0xff707000, // Line number
                0x40000000, // Current line fill
                0x40808080, // Current line fill (inactive)
                0x40a0a0a0, // Current line edge
            } };
        return p;
    }


    
}