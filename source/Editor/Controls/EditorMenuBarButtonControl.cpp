// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorMenuBarButtonControl.hpp>

namespace GT
{
    EditorMenuBarButtonControl::EditorMenuBarButtonControl(GUIContext &gui, const char* text)
        : EditorControl(gui)
    {
        gui.SetElementPaddingLeft(this->GetRootGUIElement(), 8);
        gui.SetElementPaddingRight(this->GetRootGUIElement(), 8);
        gui.SetElementText(this->GetRootGUIElement(), text);
        gui.SetElementTextColor(this->GetRootGUIElement(), GTLib::Colour(0.75f, 0.75f, 0.75f, 1.0f));
    }

    EditorMenuBarButtonControl::~EditorMenuBarButtonControl()
    {
    }
}
