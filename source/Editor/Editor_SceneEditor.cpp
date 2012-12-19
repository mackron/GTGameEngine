
#include <GTEngine/Editor.hpp>

namespace GTEngine
{
    Editor_SceneEditor::Editor_SceneEditor(Editor &editorIn)
        : editor(editorIn)
    {
    }

    Editor_SceneEditor::~Editor_SceneEditor()
    {
    }


    void Editor_SceneEditor::Startup()
    {
    }


    bool Editor_SceneEditor::LoadScene(const char* absolutePath)
    {
        (void)absolutePath;
        return true;
    }

    bool Editor_SceneEditor::SaveScene(const char* absolutePath)
    {
        (void)absolutePath;
        return true;
    }
}