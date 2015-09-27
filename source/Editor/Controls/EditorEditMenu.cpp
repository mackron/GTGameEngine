// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorEditMenu.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorEditMenu::EditorEditMenu(Editor &editor, HWindow hParentWindow)
        : EditorMenu(editor, hParentWindow)
    {
        EditorMenuItem* pPreferences = this->AppendMenuItem("Preferences...", "");
        pPreferences->OnPressed([&]() {
            m_editor.OpenFile("[PREFERENCES]");
            this->Close();
        });
    }

    EditorEditMenu::~EditorEditMenu()
    {
    }
}
