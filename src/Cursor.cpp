#include "Cursor.hpp"


namespace veditor
{

    Cursor::Cursor():
        m_nLine{0},
        m_nChar{0},
        m_originalNChar{0},
        m_selectRect{0, 0, 0, 0}
    {

    }

    void Cursor::moveUp(size_t futureCharN)
    {
        m_nLine--;
        m_nChar = std::min(futureCharN, m_originalNChar);  
    }

    void Cursor::moveDown(size_t futureCharN)
    {
        m_nLine++;
        m_nChar = std::min(futureCharN, m_originalNChar);
    }

    void Cursor::moveLeft()
    {
        m_nChar--;
        m_originalNChar = m_nChar;
    }

    void Cursor::moveRight()
    {
        m_nChar++;
        m_originalNChar = m_nChar;
    }

    void Cursor::moveStart()
    {
        m_nChar = 0;
        m_originalNChar = m_nChar;
    }

    void Cursor::moveEnd(size_t maxNChar)
    {
        m_nChar = maxNChar;
        m_originalNChar = m_nChar;
    }

    void Cursor::moveTo(size_t nL, size_t nC)
    {
        m_nLine = nL;
        m_nChar = nC;
        m_originalNChar = nC;
    }

}