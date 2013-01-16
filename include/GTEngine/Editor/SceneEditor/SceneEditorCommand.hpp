
#ifndef __GTEngine_SceneEditorCommand_hpp_
#define __GTEngine_SceneEditorCommand_hpp_

#include <GTCore/Vector.hpp>
#include <GTCore/Serializer.hpp>

namespace GTEngine
{
    enum SceneEditorCommandType
    {
        SceneEditorCommandType_Insert,
        SceneEditorCommandType_Delete,
        SceneEditorCommandType_Update
    };

    struct SceneEditorCommand
    {
        /// The type of this command (insert, delete or update).
        SceneEditorCommandType type;

        /// The unique ID's of the relevant scene nodes for this command. We need to use IDs here because sometimes the actual C++ objects
        /// won't actually exist and will need to be recreated.
        GTCore::Vector<size_t> sceneNodeIDs;

        /// The serialized data. This will be deserialized where appropriate.
        GTCore::BasicSerializer serializer;
    };
}

#endif
