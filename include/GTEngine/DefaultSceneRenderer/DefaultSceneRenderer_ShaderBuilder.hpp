// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_DefaultSceneRenderer_ShaderBuilder_hpp_
#define __GTEngine_DefaultSceneRenderer_ShaderBuilder_hpp_

#include "DefaultSceneRenderer_MaterialShaderID.hpp"
#include "../Material.hpp"
#include <GTCore/Map.hpp>

namespace GTEngine
{
    /// Class for building shaders for materials.
    class DefaultSceneRenderer_ShaderBuilder
    {
    public:

        /// Constructor.
        DefaultSceneRenderer_ShaderBuilder();

        /// Destructor.
        ~DefaultSceneRenderer_ShaderBuilder();


        /// Creates a shader from the given material shader ID.
        ///
        /// @param shaderID [in] A reference to the shader ID to base the shader off.
        Shader* CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID, const Material* material);
        Shader* CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID, const Material &material);
        Shader* CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID);




    private:

        /// Structure representing the key for cached shaders.
        struct ShaderKey : public DefaultSceneRenderer_MaterialShaderID
        {
            /// A pointer to the owner material. Can be null.
            const Material* material;


            /// Constructor.
            ShaderKey(const DefaultSceneRenderer_MaterialShaderID &materialShaderID, const Material* materialIn)
                : DefaultSceneRenderer_MaterialShaderID(materialShaderID), material(materialIn)
            {
            }

            /// Constructor.
            ShaderKey(const DefaultSceneRenderer_MaterialShaderID &materialShaderID)
                : DefaultSceneRenderer_MaterialShaderID(materialShaderID), material(nullptr)
            {
            }


            bool operator<(const ShaderKey &other) const
            {
                return DefaultSceneRenderer_MaterialShaderID::operator<(other) || (DefaultSceneRenderer_MaterialShaderID::operator==(other) && this->material < other.material);
            }

            bool operator>(const ShaderKey &other) const
            {
                return DefaultSceneRenderer_MaterialShaderID::operator>(other) || (DefaultSceneRenderer_MaterialShaderID::operator==(other) && this->material > other.material);
            }
        };


        /// The map of shaders that have been built.
        GTCore::Map<ShaderKey, Shader*> cachedShaders;
    };
}

#endif