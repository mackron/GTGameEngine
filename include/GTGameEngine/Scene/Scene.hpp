// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_Scene_hpp_
#define __GT_Scene_hpp_

#include "SceneNodeIDAllocator.hpp"
#include <GTLib/Map.hpp>
#include <GTLib/Vector.hpp>

namespace GT
{
    class SceneNode;
    class SceneCallback;
    class SceneNodeAllocator;
    class SceneSteppingPipeline;

    /// Class representing a scene.
    ///
    /// A scene is basically just a container for scene nodes.
    class Scene
    {
    public:

        /// Constructor.
        Scene(SceneCallback& callback);

        /// Destructor.
        ~Scene();


        /// Creates a new, empty scene node.
        ///
        /// @return A pointer to the new scene node.
        ///
        /// @remarks
        ///     This will NOT insert the new node into the scene. Call InsertSceneNode() to do this, but make sure all of the relevant components
        ///     and flags have been set beforehand.
        ///     @par
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


        /// Adds the given scene node to the scene.
        ///
        /// @param sceneNode [in] The scene node to add to the scene.
        ///
        /// @remarks
        ///     The scene node must have previously been created with CreateSceneNode().
        bool InsertSceneNode(SceneNode* pSceneNode);

        /// Removes the given scene node.
        ///
        /// @param sceneNode [in] The scene node to remove from the scene.
        void RemoveSceneNode(SceneNode* pSceneNode);


        /// Retrieves a pointer to the scene node with the given ID.
        ///
        /// @param sceneNodeID [in] The ID of the scene node to retrieve.
        SceneNode* GetSceneNodeByID(uint64_t sceneNodeID);


        /// Steps the scene.
        ///
        /// @param deltaTimeInSeconds [in] The time between frames, in seconds.
        void Step(double deltaTimeInSeconds);


    private:

        /// Caches a scene node insertion which will be done for real after the currently executing step.
        void CacheInsertSceneNode(SceneNode* pSceneNode);

        /// Caches a scene node removal which will be done for real after the currently executing step.
        void CacheRemoveSceneNode(SceneNode* pSceneNode);

        /// Caches a scene node deletion which will be done for real after the currently executing step.
        void CacheDeleteSceneNode(SceneNode* pSceneNode);



    private:

        /// The scene node ID allocator for generating unique scene node IDs. This is serialized with the scene so that IDs are persistent.
        SceneNodeIDAllocator m_idAllocator;

        /// The list of scene nodes currently inside the scene. This is keyed by the ID of the scene node so that scene nodes can be found
        /// relatively quickly by their ID.
        GTLib::Map<uint64_t, SceneNode*> m_sceneNodes;


        /// A reference to the scene's callback. The callback is used to allow the host application to customize how the scene works. The
        /// callback is where a lot of the real work happens when stepping the scene.
        SceneCallback &m_callback;

        /// A pointer to the scene node allocator. This will never be null.
        SceneNodeAllocator* m_pSceneNodeAllocator;

        /// A pointer to the object that will step the scene. This will never be null.
        SceneSteppingPipeline* m_pSteppingPipeline;


        /// Synchronization lock for access to cached scene nodes.
        GTLib::Mutex m_cacheLock;

        /// The list of scene nodes that need to be inserted at the end of the step.
        GTLib::Vector<SceneNode*> m_sceneNodesToInsert;

        /// The list of scene nodes that need to be removed at the end of the step.
        GTLib::Vector<SceneNode*> m_sceneNodesToRemove;

        /// The list of scene nodes that need to be deleted at the end of the step.
        GTLib::Vector<SceneNode*> m_sceneNodesToDelete;


        /// Flags.
        uint32_t m_flags;


        ///////////////////////////////////////////
        // Flags

        static const uint32_t Flag_OwnsAllocator        = (1 << 0);
        static const uint32_t Flag_OwnsSteppingPipeline = (1 << 1);
    };
}

#endif
