// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Editor/DefaultSubEditorAllocator.hpp>
#include <GTGameEngine/Editor/Controls/ImageEditor/ImageEditor.hpp>
#include <GTGameEngine/Editor/Controls/ModelEditor/ModelEditor.hpp>
#include <GTGameEngine/Editor/Controls/TextEditor/TextEditor.hpp>
#include <GTGameEngine/Editor/Controls/PreferencesEditor/PreferencesEditor.hpp>
#include <GTGameEngine/Assets/Asset.hpp>
#include <GTGameEngine/Editor/Editor.hpp>
#include <GTGameEngine/EngineContext.hpp>

namespace GT
{
    DefaultSubEditorAllocator::DefaultSubEditorAllocator()
        : SubEditorAllocator()
    {
    }

    DefaultSubEditorAllocator::~DefaultSubEditorAllocator()
    {
    }


    EditorSubEditor* DefaultSubEditorAllocator::CreateSubEditor(Editor &editor, const char* absolutePath)
    {
        // Check non-asset editors first. These are editors like the Preferences editor.
        if (strcmp(absolutePath, "[PREFERENCES]") == 0)
        {
            return new PreferencesEditor(editor, *this, absolutePath);
        }

        Asset* pAsset = editor.GetEngineContext().GetAssetLibrary().Load(absolutePath);
        if (pAsset != nullptr)
        {
            switch (pAsset->GetClass())
            {
            case AssetClass_Image: return new ImageEditor(editor, *this, absolutePath, reinterpret_cast<ImageAsset*>(pAsset));
            case AssetClass_Model: return new ModelEditor(editor, *this, absolutePath, reinterpret_cast<ModelAsset*>(pAsset));

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
        return new TextEditor(editor, *this, absolutePath);
    }
}
