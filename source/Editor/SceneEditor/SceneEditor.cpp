
#include <GTEngine/Editor/SceneEditor/SceneEditor.hpp>

namespace GTEngine
{
    SceneEditor::SceneEditor(Editor &ownerEditor, const char* path, const char* relativeTo)
        : SubEditor(ownerEditor, path, relativeTo)
    {
    }

    SceneEditor::~SceneEditor()
    {
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void SceneEditor::Show()
    {
    }

    void SceneEditor::Hide()
    {
    }

    void SceneEditor::Save()
    {
    }
}