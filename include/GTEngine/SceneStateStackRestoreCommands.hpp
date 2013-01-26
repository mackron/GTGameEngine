
#ifndef __GTEngine_SceneStateStackRestoreCommands_hpp_
#define __GTEngine_SceneStateStackRestoreCommands_hpp_

#include <GTCore/Vector.hpp>
#include <GTCore/Map.hpp>
#include <GTCore/Serializer.hpp>
#include <GTCore/Deserializer.hpp>

namespace GTEngine
{
    class Scene;

    /// Simple structure containing the commands that need to be executed in order to restore a scene to a different state.
    struct SceneStateStackRestoreCommands
    {
        /// Constructor.
        SceneStateStackRestoreCommands();

        /// Destructor.
        ~SceneStateStackRestoreCommands();


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
    };
}

#endif
