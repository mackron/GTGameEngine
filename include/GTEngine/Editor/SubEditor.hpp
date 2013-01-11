
#ifndef __GTEngine_SubEditor_hpp_
#define __GTEngine_SubEditor_hpp_

#include <GTCore/String.hpp>

namespace GTEngine
{
    /// Base class for sub-editors.
    ///
    /// A sub-editor is the scene editor, model editor, text editor, etc. There is one sub-editor associated with every open file in the editor.
    ///
    /// If an open file does not have an associated sub-editor, then it will use an instantiation of this class which will just be an empty editor.
    class SubEditor
    {
    public:

        /// Constructor.
        SubEditor(const char* path, const char* relativeTo);

        /// Destructor.
        virtual ~SubEditor();


        /// Retrieves the absolute path of this editor's file.
        const char* GetAbsolutePath() const;

        /// Retrieves the relative path of this editor's file.
        const char* GetRelativePath() const;



        ///////////////////////////////////////////////////
        // Virtual Methods.

        /// Called when the editor needs to be shown.
        virtual void Show();

        /// Called when the editor needs to be hidden.
        virtual void Hide();

        /// Called when the file needs to be saved.
        virtual void Save();



    private:

        /// The absolute path of the file this editor is editting.
        GTCore::String absolutePath;

        /// The relative path of the file this editor is editting.
        GTCore::String relativePath;
    };
}

#endif