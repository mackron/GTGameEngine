// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/SceneDeserializeCallback.hpp>

namespace GT
{
    SceneDeserializeCallback::SceneDeserializeCallback()
    {
    }

    SceneDeserializeCallback::~SceneDeserializeCallback()
    {
    }


    bool SceneDeserializeCallback::IsChunkHandled(const Serialization::ChunkHeader &) const
    {
        return false;
    }

    bool SceneDeserializeCallback::HandleChunk(const Serialization::ChunkHeader &, Deserializer &)
    {
        return false;
    }
}