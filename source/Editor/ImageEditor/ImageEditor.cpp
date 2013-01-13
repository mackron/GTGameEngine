
#include <GTEngine/Editor/ImageEditor/ImageEditor.hpp>

namespace GTEngine
{
    ImageEditor::ImageEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath)
    {
    }

    ImageEditor::~ImageEditor()
    {
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void ImageEditor::Show()
    {
    }

    void ImageEditor::Hide()
    {
    }

    bool ImageEditor::Save()
    {
        return true;
    }
}