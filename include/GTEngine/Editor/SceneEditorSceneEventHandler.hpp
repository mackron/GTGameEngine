
#ifndef __GTEngine_SceneEditorSceneEventHandler_hpp_
#define __GTEngine_SceneEditorSceneEventHandler_hpp_

#include "../SceneEventHandler.hpp"

namespace GTEngine
{
    class Editor_SceneEditor;

    class SceneEditorSceneEventHandler : public SceneEventHandler
    {
    public:

        /// Constructor.
        SceneEditorSceneEventHandler(Editor_SceneEditor &sceneEditor, Scene &scene);

        /// Destructor.
        ~SceneEditorSceneEventHandler();


        /// SceneEventHandler::OnObjectAdded()
        void OnObjectAdded(SceneObject &object);

        /// SceneEventHandler::OnObjectRemoved()
        void OnObjectRemoved(SceneObject &object);



    private:

        /// A reference to the main scene editor.
        Editor_SceneEditor &sceneEditor;

        /// A reference to the scene that this event handler is attached to.
        Scene &scene;


    private:    // No copying.
        SceneEditorSceneEventHandler(const SceneEditorSceneEventHandler &);
        SceneEditorSceneEventHandler & operator= (const SceneEditorSceneEventHandler &);
    };
}

#endif