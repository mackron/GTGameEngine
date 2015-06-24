// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMainMenuBarControl.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    EditorMainMenuBar::EditorMainMenuBar(Editor &editor)
        : EditorMenuBar(editor)
    {
        this->CreateAndInsertButton("FILE");
        this->CreateAndInsertButton("EDIT");
        this->CreateAndInsertButton("VIEW");
        this->CreateAndInsertButton("HELP");
    }

    EditorMainMenuBar::~EditorMainMenuBar()
    {
    }
}
