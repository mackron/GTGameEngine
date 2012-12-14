
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
        /// @param fileName [in] The absolute path of the file to load.
        bool LoadFile(const char* absolutePath);

        /// Saves the given text file.
        ///
        /// @param fileName [in] The absolute path of the file to save.
        bool SaveFile(const char* absolutePath);

        /// Closes the given file.
        ///
        /// @param fileName [in] The absolute path of the file to close.
        void CloseFile(const char* absolutePath);


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
        struct TextAreaEventHandler;
        struct State
        {
            /// Constructor.
            State(GTGUI::Element* textBox, GTGUI::Element* textArea, const char* absolutePath);

            /// Destructor.
            ~State();


            /// The main text box element.
            GTGUI::Element* textBox;

            /// The text area element. This is a child of textBox.
            GTGUI::Element* textArea;

            /// The event handler attached to the text area.
            TextAreaEventHandler* textAreaEventHandler;

            /// The absolute path of the text file.
            GTCore::String absolutePath;

            /// Keeps track of whether or not the file has been marked as modified.
            bool isMarkedAsModified;


        private:    // No copying.
            State(const State &);
            State & operator=(const State &);
        };


        /// A pointer to the State object for the currently open model.
        State* currentState;

        /// A map of State objects, mapped to the path of the appropriate model.
        GTCore::Dictionary<State*> loadedStates;


        /// The event handler that will be attached to the text area's. This is only used to notify the scripting environment of a change to the text.
        struct TextAreaEventHandler : public GTGUI::ElementEventHandler
        {
            /// Constructor.
            TextAreaEventHandler(State* state);

            /// GTGUI::ElementEventHandler::OnTextChanged().
            void OnTextChanged(GTGUI::Element &element);


            /// A pointer to the state object containing information for use by the event handler.
            State* state;


        private:    // No copying.
            TextAreaEventHandler(const TextAreaEventHandler &);
            TextAreaEventHandler & operator=(const TextAreaEventHandler &);
        };


    private:    // No copying.
        Editor_TextEditor(const Editor_TextEditor &);
        Editor_TextEditor & operator=(const Editor_TextEditor &);
    };
}

#endif
