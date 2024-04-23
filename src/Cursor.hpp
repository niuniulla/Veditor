#ifndef DOCUMENTCURSOR_H
#define DOCUMENTCURSOR_H

#include <iostream>
#include <vector>
#include <utility>

namespace veditor
{
    class Cursor
    {
        public:
            Cursor();
            ~Cursor() = default;
            size_t getNumLine() const {return m_nLine;};
            size_t getNumChar() const {return m_nChar;};
            std::vector<size_t> getSelectRect() const {return m_selectRect;};
            
            void moveUp(size_t futureCharN);
            void moveDown(size_t futureCharN);
            void moveLeft();
            void moveRight();
            void moveEnd(size_t maxNChar);
            void moveStart();

            void moveTo(size_t nLine, size_t nChar);

        public:
            int m_mode; // insert / replace

        private:
            size_t m_nLine;
            size_t m_nChar;
            std::vector<size_t> m_selectRect;
            size_t m_originalNChar;

    };
}

#endif //DOCUMENTCURSOR_H