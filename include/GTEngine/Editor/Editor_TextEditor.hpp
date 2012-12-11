
#ifndef __GTEngine_Editor_TextEditor_hpp_
#define __GTEngine_Editor_TextEditor_hpp_

#include <GTGUI/Element.hpp>

namespace GTEngine
{
    class Editor;

    class Editor_TextEditor
    {
    public:

        /// Constructor.
        Editor_TextEditor(Editor &editor);

        /// Destructor.
        ~Editor_TextEditor();


        /// Starts up the image editor.
        void Startup();


        /// Loads the given text file.
        ///
        /// @param fileName [in] The name of the file to load.
        bool LoadFile(const char* fileName);

        /// Closes the given file.
        ///
        /// @param fileName [in] The name of the file to close.
        void CloseFile(const char* fileName);


        /// Retrieves a reference to the game.
              Game & GetGame();
        const Game & GetGame() const;



    private:

        /// The main editor that owns this sub-editor.
        Editor &editor;

        /// Structure containing the GUI elements.
        struct _GUI
        {
            _GUI()
            {
            }


        }GUI;


        
        /// Structure containing the editor state for each loaded image.
        struct State
        {
            State()
                : textBox(nullptr), textArea(nullptr)
            {
            }


            /// The main text box element.
            GTGUI::Element* textBox;

            /// The text area element. This is a child of textBox.
            GTGUI::Element* textArea;
        };


        /// A pointer to the State object for the currently open model.
        State* currentState;

        /// A map of State objects, mapped to the path of the appropriate model.
        GTCore::Dictionary<State*> loadedStates;


        /// The event handler that will be attached to the text area's. This is only used to notify the scripting environment of a change to the text.
        struct TextAreaEventHandler : public GTGUI::ElementEventHandler
        {
            /// GTGUI::ElementEventHandler::OnTextChanged().
            void OnTextChanged(GTGUI::Element &element);

        }textAreaEventHandler;


    private:    // No copying.
        Editor_TextEditor(const Editor_TextEditor &);
        Editor_TextEditor & operator=(const Editor_TextEditor &);
    };
}

#endif