// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/DefaultSubEditorAllocator.hpp>
#include <GTGameEngine/Editor/Controls/ImageEditor/ImageEditor.hpp>
#include <GTGameEngine/Editor/Controls/TextEditor/TextEditor.hpp>
#include <GTGameEngine/Assets/Asset.hpp>

namespace GT
{
    DefaultSubEditorAllocator::DefaultSubEditorAllocator()
        : SubEditorAllocator()
    {
    }

    DefaultSubEditorAllocator::~DefaultSubEditorAllocator()
    {
    }


    EditorSubEditor* DefaultSubEditorAllocator::CreateSubEditor(Editor &editor, const char* absolutePath, Asset* pAsset)
    {
        if (pAsset != nullptr)
        {
            switch (pAsset->GetClass())
            {
            case AssetClass_Image: return new ImageEditor(editor, absolutePath, reinterpret_cast<ImageAsset*>(pAsset));

            default:
                {
                    // The asset class is unknown to us.
                    break;
                }
            }
        }

        return nullptr;
    }

    void DefaultSubEditorAllocator::DeleteSubEditor(EditorSubEditor* pSubEditor)
    {
        delete pSubEditor;
    }


    EditorSubEditor* DefaultSubEditorAllocator::CreateTextFileSubEditor(Editor &editor, const char* absolutePath)
    {
        return new TextEditor(editor, absolutePath);
    }
}
