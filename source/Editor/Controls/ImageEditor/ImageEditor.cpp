// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/Controls/ImageEditor/ImageEditor.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    ImageEditor::ImageEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePath, ImageAsset* pAsset)
        : EditorSubEditor(editor, allocator, absolutePath), m_pAsset(pAsset)
    {
        GUIContext &gui = editor.GetGUI();

        gui.SetElementSizeRatio(this->GetRootElement(), 1, 1);
        gui.SetElementBackgroundImage(this->GetRootElement(), absolutePath);
        gui.SetElementBackgroundImageColor(this->GetRootElement(), GTLib::Colour::White);
        gui.SetElementBackgroundImageRepeat(this->GetRootElement(), BackgroundRepeat_None, BackgroundRepeat_None);

        gui.OnElementMouseWheel(this->GetRootElement(), [&](int delta, int, int) {
            float scaleX;
            float scaleY;
            this->GetGUI().GetElementBackgroundImageScale(this->GetRootElement(), scaleX, scaleY);

            scaleX += scaleX * delta * 0.1f;
            scaleY += scaleY * delta * 0.1f;

            if (scaleX < 0.1f) {
                scaleX = 0.1f;
            }
            if (scaleY < 0.1f) {
                scaleY = 0.1f;
            }

            this->GetGUI().SetElementBackgroundImageScale(this->GetRootElement(), scaleX, scaleY);
        });
    }

    ImageEditor::~ImageEditor()
    {
    }


    bool ImageEditor::Save()
    {
        return false;
    }

    void ImageEditor::OnActivate()
    {
    }

    void ImageEditor::OnDeactivate()
    {
    }

    void ImageEditor::Cut()
    {
    }

    void ImageEditor::Copy()
    {
    }

    void ImageEditor::Paste()
    {
    }

    void ImageEditor::Undo()
    {
    }

    void ImageEditor::Redo()
    {
    }

    void ImageEditor::SelectAll()
    {
    }
}