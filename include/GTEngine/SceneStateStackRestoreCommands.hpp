// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneStateStackRestoreCommands_hpp_
#define __GTEngine_SceneStateStackRestoreCommands_hpp_

#include <GTLib/Vector.hpp>
#include <GTLib/Map.hpp>
#include <GTLib/Serializer.hpp>
#include <GTLib/Deserializer.hpp>

namespace GTEngine
{
    class Scene;
    class SceneStateStackBranch;

    /// Simple structure containing the commands that need to be executed in order to restore a scene to a different state.
    struct SceneStateStackRestoreCommands
    {
        /// Constructor.
        SceneStateStackRestoreCommands(unsigned int deserializationFlags);

        /// Destructor.
        ~SceneStateStackRestoreCommands();


        /// Adds an insert command.
        void AddInsert(uint64_t sceneNodeID, uint64_t parentSceneNodeID, GTLib::BasicSerializer* sceneNodeSerializer);

        /// Adds a delete command.
        void AddDelete(uint64_t sceneNodeID, uint64_t parentSceneNodeID, GTLib::BasicSerializer* sceneNodeSerializer);

        /// Adds an update command.
        void AddUpdate(uint64_t sceneNodeID, uint64_t parentSceneNodeID, GTLib::BasicSerializer* sceneNodeSerializer);

        
        /// Adds a scene node to the hierarchy.
        void AddToHierarchy(uint64_t sceneNodeID, uint64_t parentSceneNodeID);

        /// Removes a scene node from the hierarchy.
        void RemoveFromHierarchy(uint64_t sceneNodeID);


        /// Goes through every command and updates the serialized data with the most recent data, starting from the current frame on the given branch.
        void UpdateToMostRecentSerializers(SceneStateStackBranch &branch, uint32_t startFrameIndex);

        /// Goes through every scene node in the hierarchy and updates it to the most recent values.
        void UpdateToMostRecentHierarchy(SceneStateStackBranch &branch, uint32_t startFrameIndex);


        /// Clears the commands.
        void Clear();
        
        /// Executes the commands on the given scene.
        void Execute(Scene &scene);



        /// The insert commands.
        GTLib::Map<uint64_t, GTLib::BasicSerializer*> inserts;

        /// The deletes commands.
        GTLib::Map<uint64_t, GTLib::BasicSerializer*> deletes;

        /// The update commands.
        GTLib::Map<uint64_t, GTLib::BasicSerializer*> updates;


        /// The hierarchy.
        GTLib::Map<uint64_t, uint64_t> hierarchy;


        /// The flags to use when deserializing scene nodes.
        unsigned int deserializationFlags;
    };
}

#endif
