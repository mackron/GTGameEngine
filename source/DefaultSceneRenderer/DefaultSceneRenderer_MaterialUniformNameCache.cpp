// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_MaterialUniformNameCache.hpp>

namespace GTEngine
{
    DefaultSceneRenderer_MaterialUniformNameCache::DefaultSceneRenderer_MaterialUniformNameCache()
        : lightUniforms()
    {
        // Pre-allocate a group of light uniforms.
        for (int i = 0; i < 8; ++i)
        {
            this->lightUniforms.PushBack(LightIndexUniformNames(i));
        }
    } 

    const char* DefaultSceneRenderer_MaterialUniformNameCache::GetLightUniformName(MaterialUniformNameID nameID, int index)
    {
        // Need to keep allocating until we have the index.
        if (index > static_cast<int>(lightUniforms.count))
        {
            for (int i = static_cast<int>(lightUniforms.count); i < index; ++i)
            {
                this->lightUniforms.PushBack(LightIndexUniformNames(i));
            }
        }

        return this->lightUniforms[index].names[nameID].c_str();
    }
}