// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Editor/PackagingToolEditor/PackagingToolEditor.hpp>

namespace GT
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
            script.Get(String::CreateFormatted("GTGUI.Server.GetElementByID('%s')", this->mainElement->id).c_str());
            assert(script.IsTable(-1));
            {
                script.Push("PackagingToolEditor");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);   // <-- 'self'.
                    script.Push(this);      // <-- '_internalPtr'
                    script.Call(2, 0);
                }
            }
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