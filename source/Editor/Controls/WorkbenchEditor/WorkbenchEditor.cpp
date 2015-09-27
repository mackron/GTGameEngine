// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/WorkbenchEditor/WorkbenchEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    WorkbenchEditor::WorkbenchEditor(Editor &editor, SubEditorAllocator &allocator, const char* identifier)
        : EditorSubEditor(editor, allocator, identifier)
    {
        m_gui.SetElementSizeRatio(this->GetRootElement(), 1, 1);
        m_gui.SetElementBorder(m_hRootElement, 1, m_editor.GetTheme().borderDim);


        m_pTV = new TreeView(editor.GetGUI(), m_hRootElement);

        HGUIElement m_hTV = m_pTV->GetRootElement();
        m_gui.SetElementMargin(m_hTV, 4);
        m_gui.SetElementSize(m_hTV, 300U, 500U);
        m_gui.SetElementBorder(m_hTV, 1, m_editor.GetTheme().borderDefault);

        m_gui.SetElementFont(m_hTV, m_editor.GetTheme().defaultFontFamily, FontWeight_Normal, FontSlant_None, 13);



        //m_pItem0 = new TreeViewItem(editor.GetGUI(), nullptr, "Item 0");
        //m_gui.SetElementParent(m_pItem0->GetRootElement(), m_hTV);
        m_pItem0 = m_pTV->CreateAndAppendItem(nullptr, "Item 0");

        //m_pItem1 = new TreeViewItem(editor.GetGUI(), nullptr, "Item 1");
        //m_gui.SetElementParent(m_pItem1->GetRootElement(), m_hTV);
        m_pItem1 = m_pTV->CreateAndAppendItem(nullptr, "Item 1");


        m_pItem0_0 = m_pTV->CreateAndInsertItem(m_pItem0, "Item 0 0", m_pItem0->GetChildCount());
        m_pItem1_0 = m_pTV->CreateAndInsertItem(m_pItem1, "Item 1 0", m_pItem1->GetChildCount());
    }

    WorkbenchEditor::~WorkbenchEditor()
    {
        delete m_pItem1;
        delete m_pItem0;
    }
}
