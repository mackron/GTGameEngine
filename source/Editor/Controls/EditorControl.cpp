// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorControl.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorControl::EditorControl(Editor &editor)
        : GUIControl(editor.GetGUI(), 0),
          m_editor(editor)
    {
    }

    EditorControl::~EditorControl()
    {
    }


    Editor & EditorControl::GetEditor()
    {
        return m_editor;
    }

    const Editor & EditorControl::GetEditor() const
    {
        return m_editor;
    }
}
