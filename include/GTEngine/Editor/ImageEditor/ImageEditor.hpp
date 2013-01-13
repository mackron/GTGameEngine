
#ifndef __GTEngine_ImageEditor_hpp_
#define __GTEngine_ImageEditor_hpp_

#include "../SubEditor.hpp"

namespace GTEngine
{
    /// Class representing an image editor.
    class ImageEditor : public SubEditor
    {
    public:

        /// Constructor.
        ImageEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath);

        /// Destructor.
        ~ImageEditor();



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