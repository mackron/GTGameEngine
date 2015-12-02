// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_SceneEditorCommand
#define GT_SceneEditorCommand

#include <GTGE/Core/Vector.hpp>
#include <GTGE/Core/Serializer.hpp>

namespace GT
{
    enum SceneEditorCommandType
    {
        SceneEditorCommandType_Insert,
        SceneEditorCommandType_Delete,
        SceneEditorCommandType_Update,

        SceneEditorCommandType_InitialState,        // <-- A special command type where the serialized data contains the initial state of the scene.
    };

    struct SceneEditorCommand
    {
        /// The type of this command (insert, delete or update).
        SceneEditorCommandType type;

        /// The unique ID's of the relevant scene nodes for this command. We need to use IDs here because sometimes the actual C++ objects
        /// won't actually exist and will need to be recreated.
        Vector<size_t> sceneNodeIDs;

        /// The nodes that were selected at the time of this command.
        Vector<size_t> selectedNodes;

        /// The serialized data. This will be deserialized where appropriate.
        BasicSerializer serializer;
    };
}

#endif
