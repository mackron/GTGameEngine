// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/SubEditorTabEventHandler.hpp>
#include <GTEngine/Editor/SubEditor.hpp>

namespace GTEngine
{
    SubEditorTabEventHandler::SubEditorTabEventHandler(SubEditor &ownerIn)
        : owner(ownerIn)
    {
    }

    SubEditorTabEventHandler::~SubEditorTabEventHandler()
    {
    }


    void SubEditorTabEventHandler::OnMouseButtonDown(GTGUI::Element &, int button, int, int)
    {
        if (button == GTLib::MouseButton_Middle)
        {
            this->owner.Close();
        }
    }
}
