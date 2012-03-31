#ifndef __GTEngine_SceneNodeFactory_hpp_
#define __GTEngine_SceneNodeFactory_hpp_

#include <GTEngine/SceneNode.hpp>
#include <GTEngine/Model.hpp>

namespace GTEngine
{
    /**
    *   \brief  Static class for creating and deleting scene nodes.
    */
    class SceneNodeFactory
    {

    // Creating
    public:

        /**
        *   \brief  Creates an empty scene node.
        */
        static SceneNode* CreateEmptyNode();

        /**
        *   \brief  Creates a 2D camera node.
        */
        static SceneNode* Create2DCameraNode(float left, float right, float bottom, float yop, float znear, float zfar);

        /**
        *   \brief  Creates a 3D camera node.
        */
        static SceneNode* Create3DCameraNode(float fov, float aspect, float znear, float zfar);

        /**
        *   \brief             Creates a node with a model component attached to it.
        *   \param  model [in] The model to use with the model component.
        *
        *   \remarks
        *       If \c model is null, an empty node is created.
        */
        static SceneNode* CreateModelNode(Model* model, bool giveOwnership = false);


    // Deleting
    public:

        /**
        *   \brief  Deletes a node created with this factory.
        */
        static void DeleteNode(SceneNode* nodeToDelete);

    };
}

#endif