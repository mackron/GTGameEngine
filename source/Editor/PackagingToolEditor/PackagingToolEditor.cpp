// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Editor/PackagingToolEditor/PackagingToolEditor.hpp>

namespace GTEngine
{
    PackagingToolEditor::PackagingToolEditor(Editor &ownerEditor, const char* path)
        : SubEditor(ownerEditor, path, path),
          mainElement(nullptr)
    {
        auto &gui    = this->GetGUI();
        auto &script = this->GetScript();

        this->mainElement = gui.CreateElement("<div parentid='Editor_SubEditorContainer' styleclass='packaging-tool-editor' />");
        assert(this->mainElement != nullptr);
        {
            // The element has been created, but we need to execute a script to have it turn into a proper sub-editor
            script.Execute(GTCore::String::CreateFormatted("GTGUI.Server.GetElementByID('%s'):PackagingToolEditor();", this->mainElement->id).c_str());
        }
    }

    PackagingToolEditor::~PackagingToolEditor()
    {
        this->GetGUI().DeleteElement(this->mainElement);
    }



    ///////////////////////////////////////////////////
    // Virtual Methods.

    void PackagingToolEditor::Show()
    {
        this->mainElement->Show();
    }

    void PackagingToolEditor::Hide()
    {
        this->mainElement->Hide();
    }
}