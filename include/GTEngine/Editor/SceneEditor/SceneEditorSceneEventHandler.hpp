// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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


        /// SceneEventHandler::OnSceneNodeNameChanged()
        void OnSceneNodeNameChanged(SceneNode &node);

        /// SceneEventHandler::OnSceneNodeParentChanged()
        void OnSceneNodeParentChanged(SceneNode &node, SceneNode* previousParent);


        /// SceneEventHandler::OnSceneNodeTransform()
        void OnSceneNodeTransform(SceneNode &node);

        /// SceneEventHandler::OnSceneNodeScale()
        void OnSceneNodeScale(SceneNode &node);

        /// SceneEventHandler::OnSceneNodeHide()
        void OnSceneNodeHide(SceneNode &node);

        /// SceneEventHandler::OnSceneNodeShow()
        void OnSceneNodeShow(SceneNode &node);


        /// SceneEventHandler::OnSceneNodeComponentAdded()
        void OnSceneNodeComponentAdded(SceneNode &node, Component &component);

        /// SceneEventHandler::OnSceneNodeComponentRemoved()
        void OnSceneNodeComponentRemoved(SceneNode &node, Component &component);

        /// SceneEventHandler::OnSceneNodeComponentChanged()
        void OnSceneNodeComponentChanged(SceneNode &node, Component &component);

        /// SceneEventHandler::OnStateStackFrameCommitted()
        void OnStateStackFrameCommitted();



    private:

        /// A reference to the main scene editor.
        SceneEditor &sceneEditor;


    private:    // No copying.
        SceneEditorSceneEventHandler(const SceneEditorSceneEventHandler &);
        SceneEditorSceneEventHandler & operator= (const SceneEditorSceneEventHandler &);
    };
}

#endif
