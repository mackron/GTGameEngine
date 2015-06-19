// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorBodyControl.hpp>

namespace GT
{
    EditorBodyControl::EditorBodyControl(GUIContext &gui)
        : EditorControl(gui)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeightRatio(hRootElement, 1.0f);
            gui.SetElementWidthRatio(hRootElement, 1.0f);
        }
    }

    EditorBodyControl::~EditorBodyControl()
    {
    }
}
