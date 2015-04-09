// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_Scene_hpp_
#define __GT_Scene_hpp_

#include "SceneNodeIDAllocator.hpp"

namespace GT
{
    class SceneNode;
    class SceneState;

    /// Class representing a scene.
    ///
    /// A scene is basically just a container for scene nodes.
    class Scene
    {
    public:

        /// Constructor.
        Scene(SceneState &sceneState);


        /// Creates a new, empty scene node.
        ///
        /// @return A pointer to the new scene node.
        ///
        /// @remarks
        ///     The new scene node will be created with a new unique ID.
        SceneNode* CreateSceneNode();

        /// Deletes the given scene node.
        ///
        /// @param pSceneNode [in] A pointer to the scene node to delete.
        ///
        /// @remarks
        ///     The object is only marked for deletion. The is deleted for real the next time Cleanup() is called. Cleanup() is called at the end
        ///     of the stepping pipeline.
        void DeleteSceneNode(SceneNode* pSceneNode);

        /// Retrieves a pointer to the scene node with the given ID.
        ///
        /// @param sceneNodeID [in] The ID of the scene node to retrieve.
        SceneNode* GetSceneNodeByID(uint64_t sceneNodeID);



    private:

        /// The scene node ID allocator for generating unique scene node IDs. This is serialized with the scene so that IDs are persistent.
        SceneNodeIDAllocator m_idAllocator;

        /// A pointer to the scene state object.
        SceneState* m_pState;
    };
}

#endif
