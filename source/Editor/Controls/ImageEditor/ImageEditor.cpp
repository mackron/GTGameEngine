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
        gui.SetElementBackgroundImageRepeat(this->GetRootGUIElement(), BackgroundRepeat_None, BackgroundRepeat_None);

        gui.OnElementMouseWheel(this->GetRootGUIElement(), [&](int delta, int, int) {
            float scaleX;
            float scaleY;
            this->GetGUI().GetElementBackgroundImageScale(this->GetRootGUIElement(), scaleX, scaleY);

            scaleX += scaleX * delta * 0.1f;
            scaleY += scaleY * delta * 0.1f;

            if (scaleX < 0.1f) {
                scaleX = 0.1f;
            }
            if (scaleY < 0.1f) {
                scaleY = 0.1f;
            }

            this->GetGUI().SetElementBackgroundImageScale(this->GetRootGUIElement(), scaleX, scaleY);
        });
    }

    ImageEditor::~ImageEditor()
    {
    }
}