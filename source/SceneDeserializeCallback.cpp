// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneDeserializeCallback.hpp>

namespace GTEngine
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

    bool SceneDeserializeCallback::HandleChunk(const Serialization::ChunkHeader &, GTCore::Deserializer &)
    {
        return false;
    }
}