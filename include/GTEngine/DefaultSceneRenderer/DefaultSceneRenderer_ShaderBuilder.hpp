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
        Shader* CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID, const MaterialDefinition* material);
        Shader* CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID, const MaterialDefinition &material);
        Shader* CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID);


        /// Creates a gaussian blur shader.
        ///
        /// @param kernelSize   [in] The number of pixels in the blur kernal.
        /// @param sigmaSquared [in] The sigma factor, squared.
        ///
        /// @remarks
        ///     The kernel size should be an odd number so that there is a center pixel.
        Shader* CreateXGaussianBlurShader(unsigned int kernelSize, float sigmaSquared);
        Shader* CreateYGaussianBlurShader(unsigned int kernelSize, float sigmaSquared);


        /// Creates the shader to use when doing the depth pass.
        Shader* CreateDepthPassShader();

        /// Creates the shader to use when doing the highlight effect on objects.
        Shader* CreateHighlightShader();


    private:

        /// A generic function for creating a gaussian blur shader on either the X or Y axis.
        ///
        /// @param kernelSize   [in] The number of pixels in the blur kernal.
        /// @param sigmaSquared [in] The sigma factor, squared.
        /// @param xAxis        [in] Whether or not this is for the x axis. When set to false, it'll consider it the Y axis.
        Shader* CreateGaussianBlurShader(unsigned int kernelSize, float sigmaSquared, bool xAxis);


        /// Creates a vertex shader string.
        ///
        /// @param shaderID [in] The shader ID containing details about the material and lights.
        /// @param material [in] A pointer to the material in question.
        GTCore::String CreateVertexShaderString(const DefaultSceneRenderer_MaterialShaderID &shaderID, const MaterialDefinition* material) const;

        /// Creates a fragment shader string.
        ///
        /// @param shaderID [in] The shader ID containing details about the material and lights.
        /// @param material [in] A pointer to the material in question.
        GTCore::String CreateFragmentShaderString(const DefaultSceneRenderer_MaterialShaderID &shaderID, const MaterialDefinition* material) const;


        /// Retrieves a string segment containing the declaration of globals needs for a directional light.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_DeclareDirectionalLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a point light.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_DeclarePointLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a spot light.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_DeclareSpotLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a shadow-casting directional light.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_DeclareShadowDirectionalLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a shadow-casting point light.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_DeclareShadowPointLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a shadow-casting spot light.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_DeclareShadowSpotLight(int lightIndex) const;


        /// Retrieves a string segment containing the assignment operation string for a directional light in a vertex shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_AssignDirectionalLight(int lightIndex) const;

        /// Retrieves a string segment containing the assignment operation string for a point light in a vertex shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_AssignPointLight(int lightIndex) const;

        /// Retrieves a string segment containing the assignment operation string for a spot light in a vertex shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_AssignSpotLight(int lightIndex) const;

        /// Retrieves a string segment containing the assignment operation string for a shadow-casting directional light in a vertex shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_AssignShadowDirectionalLight(int lightIndex) const;

        /// Retrieves a string segment containing the assignment operation string for a shadow-casting point light in a vertex shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_AssignShadowPointLight(int lightIndex) const;

        /// Retrieves a string segment containing the assignment operation string for a shadow-casting spot light in a vertex shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String VertexShader_AssignShadowSpotLight(int lightIndex) const;




        /// Retrieves a string segment containing the declaration of globals needs for an ambient light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_DeclareAmbientLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a directional light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_DeclareDirectionalLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a point light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_DeclarePointLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a spot light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_DeclareSpotLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a shadow-casting directional light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_DeclareShadowDirectionalLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a shadow-casting point light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_DeclareShadowPointLight(int lightIndex) const;

        /// Retrieves a string segment containing the declaration of globals needs for a shadow-casting spot light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_DeclareShadowSpotLight(int lightIndex) const;


        /// Retrieves a string segment containing the light accumulation operation for an ambient light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_AccumulateAmbientLighting(int lightIndex) const;

        /// Retrieves a string segment containing the light accumulation operation for a directional light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_AccumulateDirectionalLighting(int lightIndex) const;

        /// Retrieves a string segment containing the light accumulation operation for a point light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_AccumulatePointLighting(int lightIndex) const;

        /// Retrieves a string segment containing the light accumulation operation for a spot light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_AccumulateSpotLighting(int lightIndex) const;

        /// Retrieves a string segment containing the light accumulation operation for a shadow-casting directional light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_AccumulateShadowDirectionalLighting(int lightIndex) const;

        /// Retrieves a string segment containing the light accumulation operation for a shadow-casting point light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_AccumulateShadowPointLighting(int lightIndex) const;

        /// Retrieves a string segment containing the light accumulation operation for a shadow-casting spot light in a fragment shader.
        ///
        /// @param lightIndex [in] The index of the light.
        GTCore::String FragmentShader_AccumulateShadowSpotLighting(int lightIndex) const;



        /// Retrieves the shader string of the given material's diffuse channel.
        ///
        /// @param material [in] A reference to the material whose shader string is being retrieved.
        const char* GetMaterialShaderString_Diffuse(const MaterialDefinition &material) const;

        /// Retrieves the shader string of the given material's emissive channel.
        ///
        /// @param material [in] A reference to the material whose shader string is being retrieved.
        const char* GetMaterialShaderString_Emissive(const MaterialDefinition &material) const;

        /// Retrieves the shader string of the given material's shininess channel.
        ///
        /// @param material [in] A reference to the material whose shader string is being retrieved.
        const char* GetMaterialShaderString_Shininess(const MaterialDefinition &material) const;

        /// Retrieves the shader string of the given material's specular channel.
        ///
        /// @param material [in] A reference to the material whose shader string is being retrieved.
        const char* GetMaterialShaderString_Specular(const MaterialDefinition &material) const;

        /// Retrieves the shader string of the given material's normal channel.
        ///
        /// @param material [in] A reference to the material whose shader string is being retrieved.
        const char* GetMaterialShaderString_Normal(const MaterialDefinition &material) const;

        /// Retrieves the shader string of the given material's refraction channel.
        ///
        /// @param material [in] A reference to the material whose shader string is being retrieved.
        const char* GetMaterialShaderString_Refraction(const MaterialDefinition &material) const;

        


    private:

        /// Structure representing the key for cached shaders.
        struct ShaderKey : public DefaultSceneRenderer_MaterialShaderID
        {
            /// A pointer to the owner material. Can be null.
            const MaterialDefinition* material;


            /// Constructor.
            ShaderKey(const DefaultSceneRenderer_MaterialShaderID &materialShaderID, const MaterialDefinition* materialIn)
                : DefaultSceneRenderer_MaterialShaderID(materialShaderID), material(materialIn)
            {
            }

            /// Constructor.
            ShaderKey(const DefaultSceneRenderer_MaterialShaderID &materialShaderID)
                : DefaultSceneRenderer_MaterialShaderID(materialShaderID), material(nullptr)
            {
            }
            
            /// Copy constructor.
            ShaderKey(const ShaderKey &other)
                : DefaultSceneRenderer_MaterialShaderID(other), material(other.material)
            {
            }
            
            
            /// Assignment operator.
            ShaderKey & operator=(const ShaderKey &other)
            {
                DefaultSceneRenderer_MaterialShaderID::operator=(other);
                this->material = other.material;
                
                return *this;
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
        GTCore::Map<ShaderKey, Shader*> cachedMaterialShaders;
    };
}

#endif
