// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMainMenuBarControl.hpp>

namespace GT
{
    EditorMainMenuBarControl::EditorMainMenuBarControl(GUIContext &gui)
        : EditorMenuBarControl(gui)
    {
        this->CreateAndInsertButton("FILE");
        this->CreateAndInsertButton("EDIT");
        this->CreateAndInsertButton("VIEW");
        this->CreateAndInsertButton("HELP");
    }

    EditorMainMenuBarControl::~EditorMainMenuBarControl()
    {
    }
}
