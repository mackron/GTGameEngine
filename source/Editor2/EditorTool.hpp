// Copyright (C) 2015 David Reid. See included LICENCE file.

#ifndef GT_EditorTool
#define GT_EditorTool

typedef struct easygui_element easygui_element;
typedef struct drvfs_context drvfs_context;

namespace GT
{
    class Editor2;

    /// Base class for easy_appkit tools that are used by the editor.
    ///
    /// @remarks
    ///     This creates the internal easy_appkit tool but does not attach any event handlers to it - that is the responsibility
    ///     of higher level classes since they may want to do things their own way.
    class EditorTool
    {
    public:

        /// Constructor.
        EditorTool(Editor2 &editor);

        /// Destructor.
        ~EditorTool();


        /// Initializes the tool.
        ///
        /// @remarks
        ///     This is where the GUI elements are actually created.
        ///     @par
        ///     Sub-classes will need to implement this and call it's parent's implementation before doing anything themselves.
        virtual bool Init(const char* attributes);



        /// Retrieves a reference to the editor that owns this tool.
        Editor2 & GetEditor() { return m_editor; }

        /// Retrieves a pointer to the virtual file system.
        drvfs_context* GetVFS();

        /// Retrieves a pointer to the easy_appkit tool.
        easygui_element* GetAKTool() { return m_pTool; }



    protected:

        /// A reference to the main editor that owns this tool.
        Editor2 &m_editor;

        /// A pointer to the easy_appkit tool GUI element.
        easygui_element* m_pTool;
    };


    /// Helper function for retrieving a pointer to the EditorTool object that is associated with the given easy_appkit tool.
    EditorTool* GetEditorTool(easygui_element* pTool);
}

#endif
