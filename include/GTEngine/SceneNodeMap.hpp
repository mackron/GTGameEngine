// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneNodeMap_hpp_
#define __GTEngine_SceneNodeMap_hpp_

#include "SceneNode.hpp"

namespace GTEngine
{
    /// Container class for storing pointers to scene nodes, indexed by their unique ID.
    ///
    /// Note that this is implemented as a type of map container. Keys are the scene node's ID. Values are
    /// pointers to the scene node.
    ///
    /// Iteration is O(n). Searching is O(log n).
    class SceneNodeMap
    {
    public:

        /// Constructor.
        SceneNodeMap();

        /// Copy constructor.
        SceneNodeMap(const SceneNodeMap &other);

        /// Destructor.
        ~SceneNodeMap();


        /// Inserts an scene node into the map.
        ///
        /// @param sceneNode [in] The scene node to add the map.
        ///
        /// @remarks
        ///     The scene node can not be null and so the parameter is a reference.
        ///     @par
        ///     If a scene node with the same index already exists, it'll be replaced.
        void Insert(SceneNode &sceneNode);

        /// Removes the given scene node.
        ///
        /// @param sceneNode [in] A reference to the scene node to remove.
        void Remove(SceneNode &sceneNode);

        /// Removes the scene node with the given ID.
        ///
        /// @param sceneNodeID [in] The ID of the scene node to remove.
        void Remove(uint64_t sceneNodeID);

        /// Removes an item by it's index.
        ///
        /// @param index [in] The index of the item to remove.
        void RemoveByIndex(size_t index);


        /// Finds a scene node by it's ID.
        ///
        /// @param sceneNodeID [in] The ID of the scene node to search for.
        /// @param index       [out] A reference to the variable that will receive the buffer index of the scene node, or the index to insert a new item at.
        ///
        /// @return A pointer to the scene node
        ///
        /// @remarks
        ///     If this returns null, 'index' will receive the index to insert a new scene node at.
        SceneNode* FindByID(uint64_t sceneNodeID, size_t &index) const;
        SceneNode* FindByID(uint64_t sceneNodeID) const;

        /// Finds the first scene node with the given name.
        ///
        /// @param sceneNodeName [in] The name of the scene node to search for.
        /// @param index         [out] A reference to the variable that will receive the buffer index of the scene node.
        ///
        /// @remarks
        ///     This runs in linear time.
        SceneNode* FindByName(const char* sceneNodeName, size_t &index) const;
        SceneNode* FindByName(const char* sceneNodeName) const;



        /// Determines whether or not a scene node of the given ID exists in the container.
        ///
        /// @param sceneNodeID [in] The id of the scene node to search for.
        ///
        /// @return True if the item exists; false otherwise.
        bool Exists(uint64_t sceneNodeID) const;


        /// Retrieves the number of items in the map.
        ///
        /// @return The number of scene nodes in the map.
        size_t GetCount() const;

        /// Retrieves the scene node at the given buffer index.
        ///
        /// @remarks
        ///     This should only be used for iteration. It's important to not add or remove items from the
        ///     map when iteration is happening since that will break the ordering.
              SceneNode* GetSceneNodeAtIndex(size_t index);
        const SceneNode* GetSceneNodeAtIndex(size_t index) const;


        /// Clears the map.
        void Clear();


        /// Assignment operator.
        SceneNodeMap & operator=(const SceneNodeMap &other);


    private:

        /// A pointer to the buffer containing the scene nodes.
        SceneNode** buffer;

        /// The size of the buffer in SceneNode*'s
        size_t bufferSize;

        /// The number of items in the list.
        size_t count;
    };
}


#endif
