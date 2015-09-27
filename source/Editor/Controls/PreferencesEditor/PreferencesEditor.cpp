// Copyright (C) 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/PreferencesEditor/PreferencesEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>

namespace GT
{
    PreferencesEditor::PreferencesEditor(Editor &editor, SubEditorAllocator &allocator, const char* identifier)
        : EditorSubEditor(editor, allocator, identifier)
    {
        m_gui.SetElementSizeRatio(this->GetRootElement(), 1, 1);
        m_gui.SetElementBorder(m_hRootElement, 1, m_editor.GetTheme().borderDim);


        // TEMP
        m_gui.SetElementVerticalAlign(m_hRootElement, VerticalAlign_Center);
        m_gui.SetElementHorizontalAlign(m_hRootElement, HorizontalAlign_Center);
        m_gui.SetElementFont(m_hRootElement, "Arial", FontWeight_Bold, FontSlant_None, 13);
        m_gui.SetElementTextColor(m_hRootElement, GT::Color(0.4f, 0.4f, 0.4f));
        m_gui.SetElementText(m_hRootElement, "Not yet implemented.");
    }

    PreferencesEditor::~PreferencesEditor()
    {
    }
}
