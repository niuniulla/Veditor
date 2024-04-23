#ifndef DOCUMENTTEXT_H
#define DOCUMENTTEXT_H

#include <string>
#include <vector>
#include <list>
#include <string_view>
#include <iostream>
#include <map>

namespace veditor
{
    struct LineInfo
    {
        size_t start;
        size_t len;
        bool endBreak;
    };

    class Document
    {
        public:
            Document(){};
            Document(std::string name);
            ~Document();

            void init();

            void createFromFile(const std::string fn);
            void saveFile(const std::string filename);

            void setFile(const std::string s) {m_file = s;};
            void rename(std::string name);
            std::string getFile() const {return m_file;};
            std::string getFileName() const;
            std::string getFilePath() const;

            void addChar(const char c, const size_t nLine, const size_t nChar);
            void addText(const std::string str, const size_t nLine, const size_t nChar);

            void removeChar(const size_t nLine, const size_t nChar, const bool forward=true);

            inline size_t getLineCount() const {return m_LineCount;};
            inline size_t getCharCountInLine(size_t l) const {return m_vecLineInfo[l].len;};
            size_t getMaxCharLength() const;

            std::string getLineContent(size_t line) const;

            void addLine(std::string str);

            void newLine(const size_t nLine, const size_t nChar);

            const std::string& getText() const;

            void removeText(const size_t nLine, const size_t nChar, const size_t len);
            void removeText(const size_t nLine1, const size_t nChar1, const size_t nLine2, const size_t nChar2);

            bool endWithBreak(size_t l);

        private:
            size_t getStringPos(const size_t nLine, const size_t nChar) const;
            void updateCounts();

        private:
            bool m_bHasChanged;
            size_t m_LineCount;
            std::vector<LineInfo> m_vecLineInfo; // (ind_start, len_linestr)
            std::string m_text;
            std::string m_file;

            static std::map<std::string, std::string> m_specialChars;

    };

}

#endif //DOCUMENTTEXT_H