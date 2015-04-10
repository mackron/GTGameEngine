// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_SceneNodeList_hpp_
#define __GT_SceneNodeList_hpp_

#include "SceneNode.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
    /// Class representing a chunk within a scene node list.
    ///
    /// This also acts as an allocator.
    ///
    /// When an item is deleted, the index of other elements are not changed. Instead, the slot as marked as invalid and
    /// the slot will be reused at a later time.
    ///
    /// This class should be aligned to 16 bytes since that is required for scene nodes.
    ALIGNED_TYPE(class, 16) SceneNodeListChunk
    {
    public:

        /// Constructor.
        SceneNodeListChunk();


        /// Creates and inserts a new scene node.
        ///
        /// @param sceneNodeID [in]  The ID of the new scene node.
        /// @param indexOut    [out] A reference to the variable that will receive the location of the new scene node.
        ///
        /// @return A pointer to the new scene node, or null if there was an error.
        ///
        /// @remarks
        ///     This will return null if there is no room available in the chunk.
        SceneNode* CreateAndInsertSceneNode(uint64_t sceneNodeID, uint32_t &indexOut);

        /// Deletes a scene node by it's index.
        ///
        /// @param index [in] The index of the scene node to delete.
        void DeleteSceneNodeByIndex(uint32_t index);


        /// Retrieves a pointer to the scene node at the given index.
        ///
        /// @param index [in] The index of the scene node to retrieve.
        ///
        /// @remarks
        ///     The index must be between 0 and 63.
        SceneNode* GetSceneNodeByIndex(uint32_t index);


    private:

        /// The list of scene nodes.
        SceneNode m_sceneNodes[64];

        /// The bitfield specifying which elements in the scene node list are valid.
        uint64_t m_bitfield0;
    };


    /// Class representing a list of scene nodes.
    ///
    /// This also acts as an allocator.
    ///
    /// This class is made up of chunks of scene nodes. When a new scene node is required, but there's no room available, a new chunk will be
    /// created. The idea of this system is to have a list of actual scene node objects rather than a list of scene node pointers.
    ///
    /// This class should be aligned to 16 bytes since that is required for scene nodes.
    ALIGNED_TYPE(class, 16) SceneNodeList
    {
    public:

        /// Constructor.
        SceneNodeList();

        /// Destructor.
        ~SceneNodeList();
        

        /// Creates and inserts a new scene node.
        ///
        /// @param sceneNodeID [in]  The ID of the new scene node.
        /// @param indexOut    [out] A reference to the variable that will receive location of the scene node within the list.
        ///
        /// @return A pointer to the new scene node, or null if an error occurs.
        SceneNode* CreateAndInsertSceneNode(uint64_t sceneNodeID, uint64_t &indexOut);

        /// Deletes the scene node at the given index.
        ///
        /// @param index [in] The index of the scene node to delete.
        ///
        /// @remarks
        ///     The index should be the one that was returned with CreateAndInsertSceneNode(). This is not a purely flat list, so indices are not sequential.
        void DeleteSceneNodeByIndex(uint64_t index);

        
        /// Retrieves a pointer to the scene node at the given index.
        ///
        /// @param index [in] The index of the scene node to retrieve.
        ///
        /// @return A pointer to the scene node, or null if a scene node at the given index is not valid.
        SceneNode* GetSceneNodeByIndex(uint64_t index);



    private:

        /// Attemps to create and insert a new scene node into the chunk at the given index.
        SceneNode* CreateAndInsertSceneNodeInChunk(size_t chunkIndex, uint64_t sceneNodeID, uint64_t &indexOut);


    private:

        /// The list of chunks.
        GTLib::Vector<SceneNodeListChunk*> m_chunks;

        /// The number of active scene nodes in the list.
        uint64_t m_count;
    };
}

#endif
