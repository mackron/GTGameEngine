
#ifndef __GTEngine_ModelEditor_hpp_
#define __GTEngine_ModelEditor_hpp_

#include "../SubEditor.hpp"

namespace GTEngine
{
    /// Class representing an image editor.
    class ModelEditor : public SubEditor
    {
    public:

        /// Constructor.
        ModelEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~ModelEditor();



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// SubEditor::Show()
        void Show();

        /// SubEditor::Hide()
        void Hide();

        /// SubEditor::Save()
        bool Save();


    private:


    };
}

#endif