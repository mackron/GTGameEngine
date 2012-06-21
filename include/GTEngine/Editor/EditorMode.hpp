
#ifndef __GTEngine_Editor_EditorMode_hpp_
#define __GTEngine_Editor_EditorMode_hpp_

namespace GTEngine
{
    class Game;
    class Editor;

    /// Base class for the different editor modes.
    class EditorMode
    {
    public:

        /// Constructor.
        EditorMode(Editor &editor);

        /// Destructor.
        virtual ~EditorMode();


        /// Retrieves a reference to the editor that owns this editor mode.
              Editor & GetEditor()       { return this->editor; }
        const Editor & GetEditor() const { return this->editor; }

        /// Retrieves a reference to the game that owns the editor and by extension this editor mode.
              Game & GetGame();
        const Game & GetGame() const;


        ///////////////////////////////////////////////////////////////
        // Events.

        /// Called when the mode is activated.
        virtual void OnActivate();

        /// Called when the mode is deactivated.
        virtual void OnDeactivate();

        /// Called when the editor needs to be updated.
        virtual void OnUpdate(double dtSeconds);

        /// Swaps the RC buffers.
        virtual void OnSwapRCQueues();


    protected:

        /// A reference to the editor that owns this editor mode.
        Editor &editor;



    private:    // No copying.
        EditorMode(const EditorMode &);
        EditorMode & operator=(const EditorMode &);
    };
}

#endif