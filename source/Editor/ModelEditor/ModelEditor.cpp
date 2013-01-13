
#include <GTEngine/Editor/ModelEditor/ModelEditor.hpp>

namespace GTEngine
{
    ModelEditor::ModelEditor(Editor &ownerEditor, const char* absolutePath, const char* relativePath)
        : SubEditor(ownerEditor, absolutePath, relativePath)
    {
    }

    ModelEditor::~ModelEditor()
    {
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void ModelEditor::Show()
    {
    }

    void ModelEditor::Hide()
    {
    }

    bool ModelEditor::Save()
    {
        return true;
    }
}