#include "Document.hpp"

#include <algorithm>
#include <fstream>


namespace veditor
{

    Document::Document(std::string path):
        m_bHasChanged {false},
        m_LineCount {0},
        m_file {path}
    {
        m_vecLineInfo.push_back(LineInfo{0, 0, false});
        m_text = "";

    }

    Document::~Document()
    {

    }

    std::map<std::string, std::string> Document::m_specialChars = {{"\t", "    "}};

    void Document::init()
    {
        m_LineCount = 1;
        m_vecLineInfo.push_back(LineInfo{0, 0, false});
    }

    void Document::rename(std::string name)
    {
        std::string p = getFilePath();
        m_file = p + "/" + name;
    }

    std::string Document::getFileName() const
    {
        int found = m_file.find_last_of('/');
        return m_file.substr(found+1);
    }

    std::string Document::getFilePath() const
    {
        int found = m_file.find_last_of('/');
        return m_file.substr(0, found);
    }

    void Document::createFromFile(const std::string fn)
    {

    }

    void Document::saveFile(const std::string filename)
    {
        std::ofstream outf(filename);
        outf << m_text;
        outf.close();
    }

    // insert char
    void Document::addChar(const char c, const size_t nLine, const size_t nChar)
    {
        size_t pos = getStringPos(nLine, nChar);
        m_text.insert(pos, 1, c);
        m_bHasChanged = true;
        updateCounts();
    }

    // insert string
    void Document::addText(const std::string str, const size_t nLine, const size_t nChar)
    {
        size_t pos = getStringPos(nLine, nChar);

        m_text.insert(pos, str);

        m_bHasChanged = true;
        updateCounts();  
    }

    // remove a char at a pos
    void Document::removeChar(const size_t nLine, const size_t nChar, const bool forward)
    {
        size_t pos = getStringPos(nLine, nChar);
        if (m_text.size() == 0)
        {
            return;
        }
        if (forward && (pos < m_text.size()))
        {
            m_text.erase(pos, 1);
        }
        else if (pos > 0)
        {
            m_text.erase(pos-1, 1);   
        }
        m_bHasChanged = true;
        updateCounts();  
    }

    void Document::removeText(const size_t nLine, const size_t nChar, const size_t len)
    {
        size_t start = getStringPos(nLine, nChar);

        if ((start + len) > m_text.size())
        {
            return;
        }
        m_text.erase(start, len);
        m_bHasChanged = true;
        updateCounts();
    }

    void Document::removeText(const size_t nLine1, const size_t nChar1, const size_t nLine2, const size_t nChar2)
    {
        size_t start = getStringPos(nLine1, nChar1);
        size_t end = getStringPos(nLine2, nChar2);
        size_t len = end - start + 1;
        removeText(nLine1, nChar1, len);
    }

    // get string of a line
    std::string Document::getLineContent(const size_t nLine) const
    {
        if (m_text == "")
            return "";
        size_t pos = m_vecLineInfo.at(nLine).start;
        size_t len = m_vecLineInfo.at(nLine).len;
        std::string lineStr = m_text.substr(pos, len);
        return lineStr;
    }

    // get the longest len of lines including \n
    size_t Document::getMaxCharLength() const
    {
        auto res = std::max_element(m_vecLineInfo.begin(), 
                                    m_vecLineInfo.end(),
                                    [](const auto& lhs, const auto& rhs) { 
                                        return lhs.len < rhs.len; 
                                    });
        return res->len;
    }

    const std::string& Document::getText() const
    {
        return m_text;
    }

    // to test, only add at the end
    void Document::addLine(std::string str)
    {
        // add only at the end
        m_text += "\n";
        m_text += str;
        m_LineCount++;
        m_vecLineInfo.push_back(LineInfo{m_text.size(), str.size(), false});
    }
    
    // add a new line at a pos
    void Document::newLine(const size_t nLine, const size_t nChar)
    {
        m_text.insert(getStringPos(nLine, nChar), "\n");
        m_bHasChanged = true;
        updateCounts();
    }

    // based on line and column numbern return the char position in string
    size_t Document::getStringPos(const size_t nLine, const size_t nChar) const
    {
        return m_vecLineInfo[nLine].start + nChar;
    }

    // update count based on text string
    void Document::updateCounts()
    {
        m_vecLineInfo.clear();

        size_t start = 0;
        size_t end = m_text.find('\n');

        while(end != std::string::npos)
        {
            m_vecLineInfo.push_back(LineInfo{start, end-start+1, true});
            start = end + 1;
            end = m_text.find('\n', start);
        }
        
        if (m_text.size() >= start)
        {
            m_vecLineInfo.push_back(LineInfo{start, m_text.size()-start, false});
        }


        m_LineCount = m_vecLineInfo.size();
    }

    bool Document::endWithBreak(size_t l)
    {
        return m_vecLineInfo.at(l).endBreak;
    }

}