// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/EditorHeaderControl.hpp>

namespace GT
{
    EditorHeaderControl::EditorHeaderControl(GUIContext &gui)
        : EditorControl(gui),
          m_menuBar(gui)
    {
        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != NULL)
        {
            gui.SetElementHeight(hRootElement, 26U);
            gui.SetElementWidthRatio(hRootElement, 1.0f);
            gui.SetElementVerticalAlign(hRootElement, VerticalAlign_Center);
            gui.SetElementBorderBottom(hRootElement, 1, GTLib::Colour(0.2f, 0.2f, 0.2f, 1.0f));

            gui.SetElementParent(m_menuBar.GetRootGUIElement(), this->GetRootGUIElement());
        }
    }

    EditorHeaderControl::~EditorHeaderControl()
    {
    }
}
