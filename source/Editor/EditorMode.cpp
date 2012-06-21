
#include <GTEngine/Editor/EditorMode.hpp>
#include <GTEngine/Editor.hpp>

namespace GTEngine
{
    EditorMode::EditorMode(Editor &editor)
        : editor(editor)
    {
    }

    EditorMode::~EditorMode()
    {
    }

    Game & EditorMode::GetGame()
    {
        return this->editor.GetGame();
    }

    const Game & EditorMode::GetGame() const
    {
        return this->editor.GetGame();
    }


    void EditorMode::OnActivate()
    {
    }

    void EditorMode::OnDeactivate()
    {
    }

    void EditorMode::OnUpdate(double dtSeconds)
    {
        (void)dtSeconds;
    }

    void EditorMode::OnSwapRCQueues()
    {
    }
}
