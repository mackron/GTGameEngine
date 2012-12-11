
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
                : textBox(nullptr)
            {
            }


            /// The text area element.
            GTGUI::Element* textBox;
        };


        /// A pointer to the State object for the currently open model.
        State* currentState;

        /// A map of State objects, mapped to the path of the appropriate model.
        GTCore::Dictionary<State*> loadedStates;


    private:    // No copying.
        Editor_TextEditor(const Editor_TextEditor &);
        Editor_TextEditor & operator=(const Editor_TextEditor &);
    };
}

#endif