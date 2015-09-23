// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ModelEditor
#define GT_ModelEditor

#include "../EditorSubEditor.hpp"
#include "../EditorSceneViewport.hpp"
#include "../../../Assets/ModelAsset.hpp"

namespace GT
{
    class ModelEditor : public EditorSubEditor, public SIMDAlignedType
    {
    public:

        /// Constructor()
        ModelEditor(Editor &editor, SubEditorAllocator &allocator, const char* absolutePath, ModelAsset* pAsset);

        /// Destructor.
        ~ModelEditor();


        /// @copydoc EditorSubEditor::Save()
        bool Save();

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

        /// A pointer to the model asset. This is set by the constructor.
        ModelAsset* m_pAsset;


        /// The 3D viewport.
        EditorSceneViewport m_viewport;

        /// The panel.
        HGUIElement m_hPanel;
    };
}


#endif
