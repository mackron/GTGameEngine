
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
