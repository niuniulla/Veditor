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
        m_vecLines.push_back(LineInfo{0, 0, false});
        m_text = "";

    }

    Document::~Document()
    {

    }


    void Document::init()
    {
        m_LineCount = 1;
        m_vecLines.push_back(LineInfo{0, 0, false});
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

    void Document::addChar(const char c, const size_t nLine, const size_t nChar)
    {
        if (nLine == m_LineCount)
        {
            m_text.push_back(c);
        }
        size_t pos = getStringPos(nLine, nChar);
        m_text.append(pos, c);
        updateCounts();
    }

    void Document::addText(const std::string str, const size_t nLine, const size_t nChar)
    {
        if (nLine == m_LineCount)
        {
            m_text.append(str);
        }
        else
        {
            size_t pos = getStringPos(nLine, nChar);

            m_text.insert(pos, str);
        }
        
        updateCounts();  
    }

    void Document::removeChar(const size_t nLine, const size_t nChar, const bool forward)
    {
        if (forward)
        {
            size_t pos = getStringPos(nLine, nChar);
            m_text.erase(pos, 1);
        }
        else
        {
            size_t pos = getStringPos(nLine, nChar-1);
            m_text.erase(pos, 1);   
        }
    }

    std::string Document::getLineContent(const size_t nLine) const
    {
        if (m_text == "")
            return "";
        size_t pos = m_vecLines.at(nLine).start;
        size_t len = m_vecLines.at(nLine).len;
        std::string lineStr = m_text.substr(pos, len);
        return lineStr;
    }

    size_t Document::getMaxCharLength() const
    {
        auto res = std::max_element(m_vecLines.begin(), 
                                    m_vecLines.end(),
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
        m_vecLines.push_back(LineInfo{m_text.size(), str.size(), false});
    }

    void Document::newLine(const size_t nLine, const size_t nChar)
    {
        m_text.insert(getStringPos(nLine, nChar), "\n");
        updateCounts();
    }

    size_t Document::getStringPos(const size_t nLine, const size_t nChar) const
    {
        if (nLine == m_LineCount)
        {
            return m_text.size();
        }
        return m_vecLines[nLine].start + nChar;
    }


    void Document::updateCounts()
    {
        m_vecLines.clear();
        size_t start = 0;
        size_t end = m_text.find('\n');
        while(end != std::string::npos)
        {
            m_vecLines.push_back(LineInfo{start, end-start, true});
            start = end + 1;
            end = m_text.find('\n', start);
        }
        
        if (m_text.size() > start)
        {
            m_vecLines.push_back(LineInfo{start, m_text.size()-start, false});
        }


        m_LineCount = m_vecLines.size();
    }

    bool Document::endWithBreak(size_t l)
    {
        return m_vecLines.at(l).endBreak;
    }

}