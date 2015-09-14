// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ImageEditor
#define GT_ImageEditor

#include "../EditorSubEditor.hpp"
#include "../../../Assets/ImageAsset.hpp"

namespace GT
{
    class ImageEditor : public EditorSubEditor
    {
    public:

        /// Constructor.
        ImageEditor(Editor &editor, const char* absolutePath, ImageAsset* pAsset);

        /// Destructor.
        virtual ~ImageEditor();


        /// @copydoc EditorSubEditor::SaveFile()
        bool SaveFile(const char* absolutePath);

        /// @copydoc EditorSubEditor::OnActivate()
        void OnActivate();

        /// @copydoc EditorSubEditor::OnDeactivate()
        void OnDeactivate();

        /// @copydoc EditorSubEditor::Cut()
        void Cut();

        /// @copydoc EditorSubEditor::Copy()
        void Copy();

        /// @copydoc EditorSubEditor::Paste()
        void Paste();

        /// @copydoc EditorSubEditor::Undo()
        void Undo();

        /// @copydoc EditorSubEditor::Redo()
        void Redo();

        /// @copydoc EditorSubEditor::SelectAll()
        void SelectAll();


    private:

        /// A pointer to the image asset.
        ImageAsset* m_pAsset;
    };
}

#endif
