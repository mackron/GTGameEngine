// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/SceneNodeClass.hpp>

namespace GTEngine
{
    SceneNodeClass::SceneNodeClass()
        : sceneNodeSerializers(), hierarchy()
    {
    }

    SceneNodeClass::~SceneNodeClass()
    {
        this->Clear();
    }




    /////////////////////////////////////////////////////
    // Private

    void SceneNodeClass::Clear()
    {
        for (size_t i = 0; i < this->sceneNodeSerializers.count; ++i)
        {
            delete this->sceneNodeSerializers[i];
        }

        this->sceneNodeSerializers.Clear();
    }
}
