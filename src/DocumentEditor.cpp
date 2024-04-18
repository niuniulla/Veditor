#include "DocumentEditor.hpp"
#include <fstream>


namespace veditor
{ 
    Editor::Editor():
        m_activeID {0}
    {

    }

    bool Editor::openFile(std::string filename)
    {
        // todo
        // check if exist already


        std::ifstream inf(filename);

        if (inf.is_open())
        {
            m_vecDocument.push_back(std::make_unique<Document>(filename));
            std::string line;

            while (getline(inf, line))
            {
                m_vecDocument[m_vecDocument.size()-1]->addLine(line);
            }
            inf.close();
        } 
        else
        {
            std::cerr << "Can't open the file " << filename << std::endl;
            return false;
        }

        // set as active
        m_activeID = m_vecDocument.size() - 1;

        // todo 
        // set cursor
        m_vecCursor.push_back(std::make_unique<Cursor>());
        
        return true;
    }


    bool Editor::saveFile(const std::string filename)
    {
        m_vecDocument[m_activeID]->saveFile(filename);

        if (m_vecDocument[m_activeID]->getFile() != filename)
        {
            m_vecDocument[m_activeID]->setFile(filename) ;
        }

        return true;
    }

    void Editor::newDocument()
    {
        std::string filename = "~/new_document.txt";
        m_vecDocument.push_back(std::make_unique<Document>(filename));
        m_vecCursor.push_back(std::make_unique<Cursor>());
        m_vecDocument.back()->rename("new_document" + std::to_string((int)m_vecDocument.size())+".txt");
    }


    void Editor::moveCurUp()
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            if (m_vecCursor[m_activeID]->getNumLine() > 0)
            {
                size_t lc = m_vecDocument[m_activeID]->getCharCountInLine(m_vecCursor[m_activeID]->getNumLine() - 1);
                m_vecCursor[m_activeID]->moveUp(lc);
            } 
        }
             
    }

    void Editor::moveCurDown()
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            if ((m_vecCursor[m_activeID]->getNumLine()+1) < m_vecDocument[m_activeID]->getLineCount())
            {
                size_t lc = m_vecDocument[m_activeID]->getCharCountInLine(m_vecCursor[m_activeID]->getNumLine() + 1);
                m_vecCursor[m_activeID]->moveDown(lc);
            }  
            else if (m_vecDocument[m_activeID]->endWithBreak(m_vecCursor[m_activeID]->getNumLine()))
            {
                m_vecCursor[m_activeID]->moveDown(0);
            }
        }
    }

    void Editor::moveCurLeft()
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            if ((m_vecCursor[m_activeID]->getNumChar() > 0))
            {
                m_vecCursor[m_activeID]->moveLeft();
            }
        }
            
    }

    void Editor::moveCurRight()
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            if ((m_vecCursor[m_activeID]->getNumChar()) < m_vecDocument[m_activeID]->getCharCountInLine(m_vecCursor[m_activeID]->getNumLine())+1)
            {
                m_vecCursor[m_activeID]->moveRight();
            }
        }  
    }

    void Editor::moveCurStart()
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            m_vecCursor[m_activeID]->moveStart();     
        }
    }

    void Editor::moveCurEnd()
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            if (m_vecCursor[m_activeID]->getNumLine() < m_vecDocument[m_activeID]->getLineCount())
            {
                m_vecCursor[m_activeID]->moveEnd(m_vecDocument[m_activeID]->getCharCountInLine(m_vecCursor[m_activeID]->getNumLine()));
            }
            else
            {
                m_vecCursor[m_activeID]->moveEnd(0);
            }
        }
    }

    void Editor::insertText(const char* c)
    {
        std::cout << "active: " << m_activeID << std::endl; 
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            m_vecDocument[m_activeID]->addText(c, m_vecCursor[m_activeID]->getNumLine(), m_vecCursor[m_activeID]->getNumChar());
            moveCurRight();
        }
    }

    void Editor::insertText(const std::string str)
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            m_vecDocument[m_activeID]->addText(str, m_vecCursor[m_activeID]->getNumLine(), m_vecCursor[m_activeID]->getNumChar());
            for (int i=0; i<str.size(); i++)
            {
                moveCurRight();
            }
        }
    }

    void Editor::newLine()
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            m_vecDocument[m_activeID]->newLine(m_vecCursor[m_activeID]->getNumLine(), m_vecCursor[m_activeID]->getNumChar());
            moveCurDown();
            moveCurEnd(); 
        }
    }

    void Editor::deleteChar(bool forward)
    {
        m_vecDocument[m_activeID]->removeChar(m_vecCursor[m_activeID]->getNumLine(), m_vecCursor[m_activeID]->getNumChar(), forward);
    }

    std::pair<size_t, size_t> Editor::getCurPos() const
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            return std::pair<size_t, size_t> (m_vecCursor[m_activeID]->getNumLine(), m_vecCursor[m_activeID]->getNumChar());
        }
        return std::pair<size_t, size_t>(0, 0);
    }

    size_t Editor::getLineCount() const 
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            return m_vecDocument[m_activeID]->getLineCount();
        }
        return 0;
    }

    std::string Editor::getLineContent(size_t l) const
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            return m_vecDocument[m_activeID]->getLineContent(l);
        }
        return "";
    }

    int Editor::getMaxCharLength() const
    {
        if (m_activeID >= 0 && m_activeID < getDocumentCount())
        {
            return m_vecDocument[m_activeID]->getMaxCharLength();
        }
        return 0;
    }


}