#ifndef IMGUIEDITORVIEWER_H
#define IMGUIEDITORVIEWER_H

#include <array>
#include <stdio.h>
#include <string>
#include <math.h>
#include <chrono>
#include "../extern/imgui/imgui.h"

#include "DocumentEditor.hpp"
#include "AssetManager.hpp"


namespace veditor
{
    enum class PaletteIndex
	{
		Default,
		Keyword,
		Number,
		String,
		CharLiteral,
		Punctuation,
		Preprocessor,
		Identifier,
		KnownIdentifier,
		PreprocIdentifier,
		Comment,
		MultiLineComment,
		Background,
		Cursor,
		Selection,
		ErrorMarker,
		Breakpoint,
		LineNumber,
		CurrentLineFill,
		CurrentLineFillInactive,
		CurrentLineEdge,
		Max
	};

    typedef std::array<ImU32, (unsigned)PaletteIndex::Max> Palette;
    typedef u_int8_t Char;

    struct Glyph
	{
		Char _char;
		PaletteIndex _colorIndex = PaletteIndex::Default;
		bool _comment : 1;
		bool _multiLineComment : 1;
		bool _preprocessor : 1;

		Glyph(Char aChar, PaletteIndex aColorIndex) : 
            _char(aChar), 
            _colorIndex(aColorIndex),
			_comment(false), 
            _multiLineComment(false), 
            _preprocessor(false) 
        {

        }
	};

	

	struct Settings
    {
        bool m_showLineNo = true;
		Settings(){};
    };


    class IGViewer 
    {
        public:
            IGViewer();
            ~IGViewer() {};


            void render(Document* doc, Cursor* cur);

			void render(Editor &editor);

            void setPalette(const Palette & aValue);

            inline bool isActive() {return m_isActive;};

			void renderCursor(Document* doc, Cursor* cur, ImVec2 refPos, ImDrawList *drawList);


        protected:
			float distanceToLineStart(const Document* doc, const Cursor* cur) const;
            

        private:
            Palette m_paletteBase;

            // display parameters
            float m_lineSpacing; // line spacing scale
            ImVec2 m_charSkip; // font size on x and y
            bool mScrollToTop = true;
            float m_startTime;

            bool m_isActive;

			int m_renderDocNo;

			// text related parameters
			int m_tabSize;
			std::vector<Settings> m_vecSettings;
		
		public:
			static int UTF8CharLength(Char c);

			const static Palette & GetDarkPalette();

    };

}


#endif // IMGUIEDITORVIWER_H