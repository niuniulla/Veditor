#ifndef DOCUMENTEDITOR_H
#define DOCUMENTEDITOR_H

#include <memory>
#include <string>
#include "Document.hpp"
#include "Cursor.hpp"

namespace veditor
{
    class Editor
    {
        public:
            Editor();
            ~Editor() {};

            bool openFile(const std::string filename);
            bool saveFile(const std::string filename);
            void newDocument();

            void moveCurUp();
            void moveCurDown();
            void moveCurLeft();
            void moveCurRight();
            void moveCurStart();
            void moveCurEnd();

            std::pair<size_t, size_t> getCurPos() const;

            void insertText(const char *c);
            void insertText(const std::string str);
            void newLine();

            void deleteChar(bool forward);

            void deleteText(bool forward);

            size_t getLineCount() const;
            std::string getLineContent(size_t l) const;
            int getMaxCharLength() const;
            inline size_t getCharCountInLine(size_t l) const {return m_vecDocument[m_activeID]->getCharCountInLine(l);};

            std::vector<std::unique_ptr<Document>>& getDocuments() {return m_vecDocument;};
            std::vector<std::unique_ptr<Cursor>>& getCursors() {return m_vecCursor;};
            int getDocumentCount() const {return m_vecDocument.size();};

            void setActive(int i) {m_activeID = i;};

        private:
            std::vector<std::unique_ptr<Document>> m_vecDocument;
            std::vector<std::unique_ptr<Cursor>> m_vecCursor;
            int m_activeID;
    };
}

#endif // DOCUMENTEDITOR_H