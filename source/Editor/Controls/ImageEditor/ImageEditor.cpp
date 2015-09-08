// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/ImageEditor/ImageEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    ImageEditor::ImageEditor(Editor &editor, const char* absolutePath, ImageAsset* pAsset)
        : EditorSubEditor(editor), m_pAsset(pAsset)
    {
        GUIContext &gui = editor.GetGUI();

        gui.SetElementSizeRatio(this->GetRootGUIElement(), 1, 1);
        gui.SetElementBackgroundImage(this->GetRootGUIElement(), absolutePath);
        gui.SetElementBackgroundImageColor(this->GetRootGUIElement(), GTLib::Colour::White);

        gui.SetElementBorder(this->GetRootGUIElement(), 2, GTLib::Colour::Black);
    }

    ImageEditor::~ImageEditor()
    {
    }
}