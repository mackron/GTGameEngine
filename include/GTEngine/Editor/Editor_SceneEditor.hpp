
#ifndef __GTEngine_Editor_SceneEditor_hpp_
#define __GTEngine_Editor_SceneEditor_hpp_

#include "../Scene.hpp"
#include "Editor3DViewportEventHandler.hpp"
#include <GTGUI/Element.hpp>

namespace GTEngine
{
    class Editor;

    class Editor_SceneEditor
    {
    public:

        /// Constructor.
        Editor_SceneEditor(Editor &editor);

        /// Destructor.
        ~Editor_SceneEditor();


        /// Starts up the scene editor.
        void Startup();


        /// Loads a new scene.
        ///
        /// @param absolutePath [in] The absolute path of the scene to load.
        ///
        /// @return True if the scene is loaded successfully; false otherwise.
        bool LoadScene(const char* absolutePath);

        /// Saves the given scene.
        ///
        /// @param absolutePath [in] The absolute path of the scene to save.
        ///
        /// @return True if the scene is saved successfully; false otherwise.
        bool SaveScene(const char* absolutePath);



    private:

        /// A reference to the Editor object that owns this sub-editor.
        Editor &editor;



    private:    // No copying.
        Editor_SceneEditor(const Editor_SceneEditor &);
        Editor_SceneEditor & operator=(const Editor_SceneEditor &);
    };
}

#endif