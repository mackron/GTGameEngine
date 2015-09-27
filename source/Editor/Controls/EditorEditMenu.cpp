// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorEditMenu.hpp>

namespace GT
{
    EditorEditMenu::EditorEditMenu(Editor &editor, HWindow hParentWindow)
        : EditorMenu(editor, hParentWindow)
    {
        this->AppendMenuItem("Preferences...", "");
    }

    EditorEditMenu::~EditorEditMenu()
    {
    }
}
