// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneEventHandler_hpp_
#define __GTEngine_SceneEventHandler_hpp_

namespace GTEngine
{
    class Scene;
    class SceneObject;
    class SceneNode;
    class Component;

    /// Class for intercepting events from a scene.
    ///
    /// This is really useful for special cases where you might need to do things in response to such things as adding or removing scene nodes. A prime
    /// example of this is editors, where you might want to add metadata to an object as it is added to the editor. The engine's official scene editor
    /// does exactly this.
    class SceneEventHandler
    {
    public:

        /// Constructor.
        SceneEventHandler();

        /// Destructor.
        virtual ~SceneEventHandler();


        /// Called when a scene object is added to the scene.
        ///
        /// @param object [in] A reference to the object that was just added.
        virtual void OnObjectAdded(SceneObject &object);

        /// Called when a scene object is being removed from the scene.
        ///
        /// @param object [in] A reference to the object that is being removed.
        virtual void OnObjectRemoved(SceneObject &object);


        /// Called just after a scene node has had it's name changed.
        ///
        /// @param sceneNode [in] A reference to the scene node that has just had it's name changed.
        virtual void OnSceneNodeNameChanged(SceneNode &sceneNode);

        /// Called just after a scene node has had it's parent changed.
        ///
        /// @param sceneNode [in] A reference to the scene node whose parent has just been changed.
        virtual void OnSceneNodeParentChanged(SceneNode &sceneNode, SceneNode* previousParent);

        /// Called just after a scene node has been translated and or rotated.
        ///
        /// @param sceneNode [in] A reference to the scene node that has just had it's tranformation changed.
        virtual void OnSceneNodeTransform(SceneNode &sceneNode);

        /// Called just after a scene node has been scaled.
        ///
        /// @param sceneNode [in] A reference to the scene node that has just had it's scale changed.
        virtual void OnSceneNodeScale(SceneNode &sceneNode);

        /// Called when a scene node is made invisible.
        ///
        /// @param sceneNode [in] A reference to the scene node that has just been made invisible.
        virtual void OnSceneNodeHide(SceneNode &sceneNode);

        /// Called when a scene node is made visible.
        ///
        /// @param sceneNode [in] A reference to the scene node that has just been made visible.
        virtual void OnSceneNodeShow(SceneNode &sceneNode);

        /// Called after a component has been added to a scene node.
        ///
        /// @param node      [in] A reference to the node that has just had a new component attached.
        /// @param component [in] A reference to the newly added component.
        virtual void OnSceneNodeComponentAdded(SceneNode &node, Component &component);

        /// Called just before a component is removed from the scene node, but after the scene has handled the changes.
        ///
        /// @param node      [in] A reference to the node that is having a component removed.
        /// @param component [in] A reference to the component being removed
        virtual void OnSceneNodeComponentRemoved(SceneNode &node, Component &component);

        /// Called when a component of a scene node has changed.
        ///
        /// @param node      [in] A reference to the scene node whose component has just been modified.
        /// @param component [in] A reference to the component that was changed.
        virtual void OnSceneNodeComponentChanged(SceneNode &node, Component &component);


        /// Called when a frame is committed to the state stack.
        virtual void OnStateStackFrameCommitted();
    };
}

#endif
