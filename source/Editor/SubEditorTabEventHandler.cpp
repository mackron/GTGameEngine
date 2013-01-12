
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


    void SubEditorTabEventHandler::OnMMBDown(GTGUI::Element &, int, int)
    {
        this->owner.Close();
    }
}