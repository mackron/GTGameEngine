// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorFooterControl.hpp>

namespace GT
{
    EditorFooterControl::EditorFooterControl(GUIContext &gui)
        : EditorControl(gui)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeight(hRootElement, 32U);
            gui.SetElementWidthRatio(hRootElement, 1.0f);

            gui.SetElementBorderTop(hRootElement, 1, GTLib::Colour(0.2f, 0.2f, 0.2f, 1.0f));
        }
    }

    EditorFooterControl::~EditorFooterControl()
    {
    }
}
