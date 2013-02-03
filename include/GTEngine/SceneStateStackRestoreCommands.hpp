
#ifndef __GTEngine_SceneStateStackRestoreCommands_hpp_
#define __GTEngine_SceneStateStackRestoreCommands_hpp_

#include <GTCore/Vector.hpp>
#include <GTCore/Map.hpp>
#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>

namespace GTEngine
{
    class Scene;
    class SceneStateStackBranch;

    /// Simple structure containing the commands that need to be executed in order to restore a scene to a different state.
    struct SceneStateStackRestoreCommands
    {
        /// Constructor.
        SceneStateStackRestoreCommands();

        /// Destructor.
        ~SceneStateStackRestoreCommands();


        /// Adds an insert command.
        void AddInsert(uint64_t sceneNodeID, GTCore::BasicSerializer* sceneNodeSerializer);

        /// Adds a delete command.
        void AddDelete(uint64_t sceneNodeID, GTCore::BasicSerializer* sceneNodeSerializer);

        /// Adds an update command.
        void AddUpdate(uint64_t sceneNodeID, GTCore::BasicSerializer* sceneNodeSerializer);


        /// Goes through every command and updates the serialized data with the most recent data, starting from the current frame on the given branch.
        void UpdateToMostRecentSerializers(SceneStateStackBranch &branch, uint32_t startFrameIndex);

        /// Goes through every scene node in the hierarchy and updates it to the most recent values.
        void UpdateToMostRecentHierarchy(SceneStateStackBranch &branch, uint32_t startFrameIndex);


        /// Clears the commands.
        void Clear();
        
        /// Executes the commands on the given scene.
        void Execute(Scene &scene);



        /// The insert commands.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> inserts;

        /// The deletes commands.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> deletes;

        /// The update commands.
        GTCore::Map<uint64_t, GTCore::BasicSerializer*> updates;


        /// The hierarchy.
        GTCore::Map<uint64_t, uint64_t> hierarchy;
    };
}

#endif
