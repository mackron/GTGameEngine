// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SceneEditorSceneEventHandler
#define GT_SceneEditorSceneEventHandler

#include "../../SceneEventHandler.hpp"

namespace GT
{
    class SceneEditor;

    class SceneEditorSceneEventHandler : public SceneEventHandler
    {
    public:

        /// Constructor.
        SceneEditorSceneEventHandler(SceneEditor &sceneEditor);

        /// Destructor.
        virtual ~SceneEditorSceneEventHandler();


        /// SceneEventHandler::OnSceneNodeAdded()
        void OnSceneNodeAdded(SceneNode &sceneNode);

        /// SceneEventHandler::OnObjectRemoved()
        void OnSceneNodeRemoved(SceneNode &sceneNode);


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
        void OnSceneNodeComponentChanged(SceneNode &node, Component &component, uint32_t whatChangedFlags);

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
