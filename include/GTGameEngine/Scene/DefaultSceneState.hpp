// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "SceneState.hpp"
#include "SceneNodeList.hpp"
#include <GTLib/Map.hpp>
#include <GTLib/Threading/Mutex.hpp>

namespace GT
{
    /// The default scene state.
    class DefaultSceneState : public SceneState
    {
    public:

        /// Constructor.
        DefaultSceneState();

        /// Destructor.
        ~DefaultSceneState();


        /// @copydoc DefaultSceneState::CrateSceneNode()
        SceneNode* CreateSceneNode(uint64_t sceneNodeID);

        /// @copydoc DefaultSceneState::DeleteSceneNode()
        void DeleteSceneNode(uint64_t sceneNodeID);


        /// @copydoc DefaultSceneState::InsertSceneNode()
        bool InsertSceneNode(SceneNode* pSceneNode);

        /// @copydoc DefaultSceneState::RemoveSceneNode()
        void RemoveSceneNode(SceneNode* pSceneNode);


        /// @copydoc DefaultSceneState::GetSceneNodeByID()
        SceneNode* GetSceneNodeByID(uint64_t sceneNodeID);


        /// @copydoc DefaultSceneState::SetEventHandler()
        void SetEventHandler(SceneEventHandler* pEventHandler);

        /// @copydoc DefaultSceneState::GetEventHandler()
        SceneEventHandler* GetEventHandler();


        /// @copydoc DefaultSceneState::Step()
        void Step(double deltaTimeInSeconds);


    private:

        /// Posts all of the cached events.
        void PostEvents();
        
        /// Cleans up all of the objects that have been marked for deletion.
        void Cleanup();

        /// Retrieves the index of the scene node within the primary list.
        bool GetSceneNodeListIndex(SceneNode* pSceneNode, uint64_t &sceneNodeIndexOut);


        /// Inserts the scene node for real.
        void InsertSceneNodeForReal_NoLock(SceneNode* pSceneNode);
        
        /// Removes a scene node for real.
        void RemoveSceneNodeForReal_NoLock(SceneNode* pSceneNode);

        /// Deletes a scene node for real by it's ID.
        ///
        /// This is not locked.
        void DeleteSceneNodeForReal_NoLock(SceneNode* pSceneNode, uint64_t sceneNodeListIndex);
        void DeleteSceneNodeForReal_NoLock(SceneNode* pSceneNode);


    private:

        /// The list of scene nodes. This list groups scene nodes into chunks. Each chunk will be processed on it's own thread
        /// during the step operation. When selecting an item in this list, two indices are specified - one is the index of the
        /// chunk, the other is the location within that chunk.
        SceneNodeList m_sceneNodes;

        /// A container for mapping scene node IDs to the index inside m_sceneNodes.
        GTLib::Map<uint64_t, uint64_t> m_sceneNodeMap;

        /// A pointer to the event handler.
        SceneEventHandler* m_pEventHandler;

        /// State flags.
        uint32_t m_stateFlags;


        /// The lock for synchronizing access to the scene nodes list. This locks access to insert, delete and searching operations.
        GTLib::Mutex m_containerLock;


        /// The list of scene nodes that were added during the stepping pipeline.
        GTLib::Vector<SceneNode*> m_sceneNodesInsertedWhileStepping;

        /// The list of scene nodes that were removed during the stepping pipeline.
        GTLib::Vector<SceneNode*> m_sceneNodesRemovedWhileStepping;

        /// The list of scene nodes that were deleted during the stepping pipeline.
        GTLib::Vector<SceneNode*> m_sceneNodesDeletedWhileStepping;


    private:

        static const uint32_t StateFlag_IsStepping = (1 << 0);
    };
}
