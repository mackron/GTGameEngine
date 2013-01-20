
#ifndef __GTEngine_TextEditor_hpp_
#define __GTEngine_TextEditor_hpp_

#include "../SubEditor.hpp"

namespace GTEngine
{
    /// Class representing a text editor.
    class TextEditor : public SubEditor
    {
    public:

        /// Constructor.
        TextEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~TextEditor();




        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();

        /// SubEditor::Save()
        bool Save();


    private:

        /// The main text box element.
        GTGUI::Element* textBox;

        /// The text area element. This is a child of textBox.
        GTGUI::Element* textArea;

        /// The event handler attached to the text area.
        struct TextAreaEventHandler : public GTGUI::ElementEventHandler
        {
            /// Constructor.
            TextAreaEventHandler(TextEditor* ownerTextEditor);

            /// GTGUI::ElementEventHandler::OnTextChanged().
            void OnTextChanged(GTGUI::Element &element);


            /// A pointer to the text editor that owns the text area.
            TextEditor* ownerTextEditor;


        private:    // No copying.
            TextAreaEventHandler(const TextAreaEventHandler &);
            TextAreaEventHandler & operator=(const TextAreaEventHandler &);

        }*textAreaEventHandler;


    private:    // No copying.
        TextEditor(const TextEditor &);
        TextEditor & operator=(const TextEditor &);
    };
}

#endif
