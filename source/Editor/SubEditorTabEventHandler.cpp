// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/Editor/SubEditorTabEventHandler.hpp>
#include <GTGE/Editor/SubEditor.hpp>

namespace GT
{
    SubEditorTabEventHandler::SubEditorTabEventHandler(SubEditor &ownerIn)
        : owner(ownerIn)
    {
    }

    SubEditorTabEventHandler::~SubEditorTabEventHandler()
    {
    }


    void SubEditorTabEventHandler::OnMouseButtonDown(GUIElement &, int button, int, int)
    {
        if (button == MouseButton_Middle)
        {
            this->owner.Close();
        }
    }
}
