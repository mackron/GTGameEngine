// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_SceneNodeEventHandler_hpp_
#define __GTEngine_SceneNodeEventHandler_hpp_

#include "Physics/Bullet.hpp"

namespace GT
{
    class Scene;
    class SceneNode;


    /// Base class for handling events on scene nodes.
    class SceneNodeEventHandler
    {
    public:

        /**
        *   \brief                 Called after a child has been attached to the node.
        *   \param  node      [in] A pointer to the node whose just had 'childNode' attached to it.
        *   \param  childNode [in] A pointer to the child node that was just attached as a child to 'node'.
        */
        virtual void OnAttach(SceneNode &node, SceneNode &childNode);

        /**
        *   \brief                 Called after a child has been detached from the node.
        *   \param  node      [in] A pointer to the node whose child is being detached.
        *   \param  childNode [in] A pointer to the child node that is being detached from 'node'.
        *
        *   \remarks
        *       Note that when this function is called, childNode->GetParent() will actually return null since technically
        *       it is not attached to anything. Remember, this function is called AFTER the child has been detached from
        *       the parent.
        */
        virtual void OnDetach(SceneNode &node, SceneNode &childNode);

        /**
        *   \brief  Called just before the node is destroyed.
        *
        *   \remarks
        *       This event is called during the nodes destructor. You should only do cleaning-up in this event and should
        *       not create new nodes.
        *       \par
        *       The destructor of a node will also detach that node from the parent. This function is called after OnDetach().
        */
        virtual void OnDestroy(SceneNode& node);


        /// Called after a node has been moved, rotated or both.
        ///
        /// @param node [in] A reference to the node that has been transformed.
        ///
        /// @remarks
        ///     This is not called for scaling. For scaling, use OnScale() instead.
        virtual void OnTransform(SceneNode &node);


        /**
        *   \brief  Called after a node has been scaled.
        *
        *   \remarks
        *       Since this function is called after the node has changed scale, calling node->GetScale() will yield
        *       the current scale of the node. The value returned by node->GetScale() will always be different than
        *       prevScale.
        */
        virtual void OnScale(SceneNode &node);

        /**
        *   \brief  Called after the layer the node is sitting on has changed.
        */
        virtual void OnLayerChanged(SceneNode &node, unsigned int prevLayer);

        /**
        *   \brief  Called after a scene node has changed scenes.
        */
        virtual void OnSceneChanged(SceneNode &node, Scene *prevScene);

        /**
        *   \brief  Called after a scene node has changed from static to dynamic, or vice-versa.
        */
        virtual void OnStaticChanged(SceneNode &node);

        /**
        *   \brief  Called after a scene node has changed visibility.
        */
        virtual void OnVisibleChanged(SceneNode &node);

        /**
        *   \brief                          Called when the scene node is being updated.
        *   \param  node               [in] A pointer to the node that is being updated.
        *   \param  deltaTimeInSeconds [in] The since the last update, in seconds.
        */
        virtual void OnUpdate(SceneNode &node, double deltaTimeInSeconds);


        /// Called when a physics object is touching another object.
        ///
        /// @param node  [in] A reference to the owner of the event.
        /// @param other [in] The other node the node is touching.
        /// @param pt    [in] Contact information.
        ///
        /// @remarks
        ///     This event is not only called when two objects go from a non-colliding to colliding state. Also, it will be called every frame
        ///     that the nodes are touching.
        ///     @par
        ///     This event will only be called for scene nodes with dynamics components whose collisions are detectable by the physics system. Kinematic/Kinematic,
        ///     Static/Static and Kinematic/Static collisions will not be handled. If these collisions are required, consider using a proximity component.
        virtual void OnContact(SceneNode &node, SceneNode &other, const btManifoldPoint &pt);



    public:

        // We need to define a default constructor.
        SceneNodeEventHandler() {}

        // This is just a warning silencer.
        virtual ~SceneNodeEventHandler() {}


    public:

        /**
        *   \brief  Retrieves the default scene node event handler.
        */
        static SceneNodeEventHandler & GetDefault();


    private:
        SceneNodeEventHandler(const SceneNodeEventHandler &);
        SceneNodeEventHandler & operator=(const SceneNodeEventHandler &);
    };
}

#endif