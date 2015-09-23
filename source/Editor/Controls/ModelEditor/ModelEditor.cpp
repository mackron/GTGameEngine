// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/ModelEditor/ModelEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    ModelEditor::ModelEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePath, ModelAsset* pAsset)
        : EditorSubEditor(editor, allocator, absolutePath),
          m_pAsset(pAsset), m_viewport(editor), m_hPanel(0)
    {
        GUIContext &gui = this->GetGUI();
        //const EditorTheme &theme = this->GetEditor().GetTheme();

        HGUIElement hRootElement = this->GetRootGUIElement();
        if (hRootElement != 0)
        {
            gui.SetElementSizeRatio(hRootElement, 1, 1);
            gui.SetElementChildAxis(hRootElement, ChildAxis_Horizontal);
            gui.EnableElementChildWidthFlexing(hRootElement);


            // Viewport
            gui.SetElementParent(m_viewport.GetRootGUIElement(), hRootElement);
            gui.SetElementSizeRatio(m_viewport.GetRootGUIElement(), 1, 1);
            gui.SetElementBorder(m_viewport.GetRootGUIElement(), 1, GTLib::Colour(0.2f, 0.2f, 0.2f));
                

            // Panel
            m_hPanel = gui.CreateElement();
            gui.SetElementParent(m_hPanel, hRootElement);
            gui.SetElementWidth(m_hPanel, 256U);
            gui.SetElementHeightRatio(m_hPanel, 1);
        }
    }

    ModelEditor::~ModelEditor()
    {
    }

    bool ModelEditor::Save()
    {
        return false;
    }

    void ModelEditor::OnActivate()
    {
    }

    void ModelEditor::OnDeactivate()
    {
    }

    void ModelEditor::Cut()
    {
    }

    void ModelEditor::Copy()
    {
    }

    void ModelEditor::Paste()
    {
    }

    void ModelEditor::Undo()
    {
    }

    void ModelEditor::Redo()
    {
    }

    void ModelEditor::SelectAll()
    {
    }
}
