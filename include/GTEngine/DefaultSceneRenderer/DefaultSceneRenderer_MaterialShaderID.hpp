// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_MaterialShaderID_hpp_
#define __GTEngine_DefaultSceneRenderer_MaterialShaderID_hpp_

#include "DefaultSceneRenderer_LightGroup.hpp"

namespace GTEngine
{
    /// Structure representing an ID of a material's shader.
    ///
    /// A material shader ID is created from a light group ID and material-specific flags.
    struct DefaultSceneRenderer_MaterialShaderID
    {
        /// Constructor.
        DefaultSceneRenderer_MaterialShaderID(const DefaultSceneRenderer_LightGroupID &lightGroupIDIn, uint32_t flagsIn)
            : lightGroupID(lightGroupIDIn), flags(flagsIn)
        {
        }

        /// Copy constructor.
        DefaultSceneRenderer_MaterialShaderID(const DefaultSceneRenderer_MaterialShaderID &other)
            : lightGroupID(other.lightGroupID), flags(other.flags)
        {
        }


        bool operator<(const DefaultSceneRenderer_MaterialShaderID &other) const
        {
            return this->lightGroupID < other.lightGroupID || (this->lightGroupID == other.lightGroupID && this->flags < other.flags);
        }

        bool operator>(const DefaultSceneRenderer_MaterialShaderID &other) const
        {
            return this->lightGroupID > other.lightGroupID || (this->lightGroupID == other.lightGroupID && this->flags > other.flags);
        }

        bool operator==(const DefaultSceneRenderer_MaterialShaderID &other) const
        {
            return this->lightGroupID == other.lightGroupID && this->flags == other.flags;
        }



        /// Enumerator containing the different flags that can be used in a material shader ID.
        enum Flags
        {
            IncludeMaterialPass = (1 << 1),
            NoNormalMapping     = (1 << 2)
        };


        /// The light group ID.
        DefaultSceneRenderer_LightGroupID lightGroupID;

        /// The material-specific flags. Defaults to zero.
        uint32_t flags;
    };
}

#endif