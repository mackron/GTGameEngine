
#ifndef __GTEngine_SceneEditorSceneEventHandler_hpp_
#define __GTEngine_SceneEditorSceneEventHandler_hpp_

#include "../../SceneEventHandler.hpp"

namespace GTEngine
{
    class SceneEditor;

    class SceneEditorSceneEventHandler : public SceneEventHandler
    {
    public:

        /// Constructor.
        SceneEditorSceneEventHandler(SceneEditor &sceneEditor);

        /// Destructor.
        virtual ~SceneEditorSceneEventHandler();


        /// SceneEventHandler::OnObjectAdded()
        void OnObjectAdded(SceneObject &object);

        /// SceneEventHandler::OnObjectRemoved()
        void OnObjectRemoved(SceneObject &object);

        /// SceneEventHandler::OnObjectRefreshed()
        void OnObjectRefreshed(SceneObject &object);

        /// SceneEventHandler::OnSceneNodeTransform()
        void OnSceneNodeTransform(SceneNode &node);

        /// SceneEventHandler::OnSceneNodeScale()
        void OnSceneNodeScale(SceneNode &node);

        /// SceneEventHandler::OnSceneNodeHide()
        void OnSceneNodeHide(SceneNode &node);

        /// SceneEventHandler::OnSceneNodeShow()
        void OnSceneNodeShow(SceneNode &node);



    private:

        /// A reference to the main scene editor.
        SceneEditor &sceneEditor;


    private:    // No copying.
        SceneEditorSceneEventHandler(const SceneEditorSceneEventHandler &);
        SceneEditorSceneEventHandler & operator= (const SceneEditorSceneEventHandler &);
    };
}

#endif
