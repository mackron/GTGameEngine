// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_PrefabLinker_hpp_
#define __GTEngine_PrefabLinker_hpp_

#include "PrefabLibrary.hpp"

namespace GTEngine
{
    /// Base class for linking scene nodes to prefab, and vice versa.
    ///
    /// The purpose of this class is to implement the functionality required for keeping scene nodes synchronized with
    /// prefabs. This process is not in the scope of scenes or scene nodes, and so needs to be done at a slightly higher
    /// level (but not in editor scope).
    ///
    /// The prefab library does not work on it's own set of locally created scene nodes. Instead it references already-
    /// created nodes, or called virtual methods when a node needs to be created. This class will need to be inheritted
    /// to be useful.
    ///
    /// The prefab linker will create and delete Prefab components, but won't touch anything else.
    class PrefabLinker
    {
    public:

        /// Constructor.
        PrefabLinker();

        /// Destructor.
        virtual ~PrefabLinker();


        /// Links a scene node to the given prefab.
        ///
        /// @param baseSceneNode      [in] The base scene node that is being linked to a prefab.
        /// @param prefabRelativePath [in] The relative path of the prefab to link to.
        ///
        /// @remarks
        ///     This will call CreateSceneNode() and DeleteSceneNode() where needed.
        ///     @par
        ///     CreateSceneNode() will be called if a child node needs to be created.
        ///     @par
        ///     DeleteSceneNode() will be called if a child node that was previously part of the prefab is no longer part
        ///     of it. This will not be called on descendants that were not previously part of the prefab.
        ///     @par
        ///     It is OK to call this on a scene node that is already linked to the prefab. In this case, it will just be
        ///     synchronized.
        bool LinkSceneNodeToPrefab(SceneNode &baseSceneNode, const char* prefabRelativePath);

        /// Unlinks a scene node from the prefab it is currently linked to.
        ///
        /// @param sceneNode                        [in] A reference to the scene node whose getting unlinked from the prefab.
        /// @param deleteChildrenLinkedToSamePrefab [in] Whether or not to delete the children that are linked to the same prefab.
        ///
        /// @remarks
        ///     This will remove the Prefab component.
        ///     @par
        ///     This will be called recursively on children that are linked to the same prefab.
        void UnlinkSceneNodeFromPrefab(SceneNode &baseSceneNode, bool deleteChildrenLinkedToSamePrefab);


        /// Determines whether or not the given scene node is linked to the given prefab.
        ///
        /// @param sceneNode          [in] The scene node to check.
        /// @param prefabRelativePath [in] The relative path of the prefab.
        ///
        /// @return True if the scene node is linked to the given prefab; false otherwise.
        bool IsSceneNodeLinkedToPrefab(SceneNode &sceneNode, const char* prefabRelativePath);


        ///////////////////////////////////////////////////
        // Virtual Methods
        
        /// Called when a scene node needs to be created.
        ///
        /// @param deserializer [in] A reference to the deserializer to create the scene node from.
        ///
        /// @remarks
        ///     The return value must be a pointer to the newly created scene node.
        ///     @par
        ///     This will be called when a scene node is linked to a prefab, but that scene node needs to have children
        ///     created in order to satisfy the sycnrhonization.
        virtual SceneNode* CreateSceneNode() const = 0;

        /// Called when a scene node needs to be deleted.
        ///
        /// @param sceneNode [in] A pointer to the scene node to delete.
        ///
        /// @remarks
        ///     This will be called when a scene node is linked to a prefab, but that scene node needs to have children
        ///     deleted in order to satify the synchronization.
        virtual void DeleteSceneNode(SceneNode* sceneNode) const = 0;



    private:

        /// Performs a upwards recursive traversal and returns a pointer to the scene node that roots the prefab instantiation.
        ///
        /// @param sceneNode          [in] The scene node where the traversal will begin.
        /// @param prefabRelativePath [in] The relative path of the prefab.
        SceneNode* FindRootSceneNode(SceneNode &sceneNode, const char* prefabRelativePath) const;

        /// Deserializes the scene node based on prefab.
        ///
        /// @param sceneNode [in] The scene node being deserialized.
        /// @param localID   [in] The local hierarchy ID of the node in the prefab.
        /// @param prefab    [in] The prefab to deserialize from.
        ///
        /// @remarks
        ///     This is recursive.
        ///     @par
        ///     This may call CreateSceneNode().
        bool DeserializeSceneNode(SceneNode &sceneNode, uint64_t localID, const Prefab &prefab) const;
    };
}


#endif
