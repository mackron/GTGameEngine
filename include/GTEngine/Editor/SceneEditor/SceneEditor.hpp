
#ifndef __GTEngine_SceneEditor_hpp_
#define __GTEngine_SceneEditor_hpp_

#include "../SubEditor.hpp"

namespace GTEngine
{
    /// Class representing a scene editor.
    class SceneEditor : public SubEditor
    {
    public:

        /// Constructor.
        SceneEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~SceneEditor();



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();

        /// SubEditor::Save()
        void Save();



    private:


    };
}

#endif