// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_DefaultSceneRenderer_MaterialShaderID
#define GT_DefaultSceneRenderer_MaterialShaderID

#include "DefaultSceneRenderer_LightGroup.hpp"

namespace GT
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
        
        /// Destructor (GCC warning silencer).
        virtual ~DefaultSceneRenderer_MaterialShaderID()
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
            IncludeMaterialPass     = (1 << 1),
            NoNormalMapping         = (1 << 2),
            GetLightingFromTextures = (1 << 3)
        };


        /// The light group ID.
        DefaultSceneRenderer_LightGroupID lightGroupID;

        /// The material-specific flags. Defaults to zero.
        uint32_t flags;
    };
}

#endif
