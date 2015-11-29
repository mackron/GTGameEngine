// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/DefaultSceneRenderer/DefaultSceneRenderer_ShaderBuilder.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/ShaderLibrary.hpp>

namespace GT
{
    DefaultSceneRenderer_ShaderBuilder::DefaultSceneRenderer_ShaderBuilder()
        : cachedMaterialShaders()
    {
    }

    DefaultSceneRenderer_ShaderBuilder::~DefaultSceneRenderer_ShaderBuilder()
    {
        for (size_t i = 0; i < this->cachedMaterialShaders.count; ++i)
        {
            auto shader = this->cachedMaterialShaders.buffer[i]->value;
            assert(shader != nullptr);
            {
                Renderer::DeleteShader(shader);
            }
        }
    }


    Shader* DefaultSceneRenderer_ShaderBuilder::CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID, const MaterialDefinition* material)
    {
        ShaderKey shaderKey(shaderID, material);

        auto iShader = this->cachedMaterialShaders.Find(shaderKey);
        if (iShader != nullptr)
        {
            return iShader->value;
        }


        // If we made it here it means that we need to create a new shader.
        String vertexSource   = this->CreateVertexShaderString(shaderID, material);
        String fragmentSource = this->CreateFragmentShaderString(shaderID, material);

        auto shader = Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
        if (shader != nullptr)
        {
            this->cachedMaterialShaders.Add(shaderKey, shader);
        }

        return shader;
    }

    Shader* DefaultSceneRenderer_ShaderBuilder::CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID, const MaterialDefinition &material)
    {
        return this->CreateShader(shaderID, &material);
    }

    Shader* DefaultSceneRenderer_ShaderBuilder::CreateShader(const DefaultSceneRenderer_MaterialShaderID &shaderID)
    {
        return this->CreateShader(shaderID, nullptr);
    }


    Shader* DefaultSceneRenderer_ShaderBuilder::CreateXGaussianBlurShader(unsigned int pixelCount, float sigmaSquared)
    {
        return this->CreateGaussianBlurShader(pixelCount, sigmaSquared, true);
    }

    Shader* DefaultSceneRenderer_ShaderBuilder::CreateYGaussianBlurShader(unsigned int pixelCount, float sigmaSquared)
    {
        return this->CreateGaussianBlurShader(pixelCount, sigmaSquared, false);
    }


    Shader* DefaultSceneRenderer_ShaderBuilder::CreateDepthPassShader()
    {
        // Vertex Shader.
        String vertexSource
        (
            "#version 120\n"

            "attribute vec3 VertexInput_Position;\n"
            "uniform   mat4 PVMMatrix;\n"
    
            "void main()\n"
            "{\n"
            "    gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);\n"
            "}\n"
        );


        // Fragment Shader.
        String fragmentSource
        (
            "#version 120\n"

            "void main()\n"
            "{\n"
            "}\n"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }


    Shader* DefaultSceneRenderer_ShaderBuilder::CreateHighlightShader()
    {
        // Vertex Shader.
        String vertexSource
        (
            "#version 120\n"

            "attribute vec3 VertexInput_Position;\n"
            "uniform   mat4 PVMMatrix;\n"
    
            "void main()\n"
            "{\n"
            "    gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);\n"
            "}"
        );


        // Fragment Shader.
        String fragmentSource
        (
            "#version 120\n"

            "void main()\n"
            "{\n"
            "    gl_FragData[0] = vec4(0.0, 0.0, 0.0, 0.0);\n"
            "}"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }


    Shader* DefaultSceneRenderer_ShaderBuilder::CreateShadowMapShader()
    {
        // Vertex Shader.
        String vertexSource
        (
            "#version 120\n"

            "attribute vec3 VertexInput_Position;\n"
            "varying   vec4 ShadowCoord;\n"
            "uniform   mat4 PVMMatrix;\n"
    
            "void main()\n"
            "{\n"
            "    gl_Position = PVMMatrix * vec4(VertexInput_Position, 1.0);\n"
            "    ShadowCoord = gl_Position;\n"
            "}"
        );


        // Fragment Shader.
        String fragmentSource
        (
            "#version 120\n"

            "varying vec4 ShadowCoord;\n"
    
            "void main()\n"
            "{\n"
            "    float depth = ShadowCoord.z / ShadowCoord.w;\n"
            "    gl_FragData[0] = vec4(depth, depth * depth, 0.0, 1.0);\n"
            "}"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }

    Shader* DefaultSceneRenderer_ShaderBuilder::CreatePointShadowMapShader()
    {
        // Vertex Shader.
        String vertexSource
        (
            "#version 120\n"

            "attribute vec3 VertexInput_Position;\n"
            "varying   vec4 ShadowCoord;\n"
    
            "uniform mat4 PVMMatrix;\n"
            "uniform mat4 ViewModelMatrix;\n"
    
            "void main()\n"
            "{\n"
            "    gl_Position = PVMMatrix       * vec4(VertexInput_Position, 1.0);\n"
            "    ShadowCoord = ViewModelMatrix * vec4(VertexInput_Position, 1.0);\n"
            "}"
        );


        // Fragment Shader.
        String fragmentSource
        (
            "#version 120\n"

            "varying vec4 ShadowCoord;\n"

            "void main()\n"
            "{\n"
            "    float depth = length(ShadowCoord.xyz);\n"
            "    gl_FragData[0] = vec4(depth, depth * depth, 0.0, 1.0);\n"
            "}"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }


    Shader* DefaultSceneRenderer_ShaderBuilder::CreateBloomShader()
    {
        // Vertex Shader.
        String vertexSource(this->GetFullscreenPrimitiveVertexShader());


        // Fragment Shader.
        String fragmentSource
        (
            "#version 120\n"

            "varying vec2 VertexOutput_TexCoord;\n"
    
            "uniform sampler2D ColourBuffer;\n"
    
            "void main()\n"
            "{\n"
            "    gl_FragData[0] = max(vec4(0.0), texture2D(ColourBuffer, VertexOutput_TexCoord) - vec4(1.0, 1.0, 1.0, 0.0));\n"
            "}"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }


    Shader* DefaultSceneRenderer_ShaderBuilder::CreateLDRFinalCompositionShader()
    {
        // Vertex Shader.
        String vertexSource(this->GetFullscreenPrimitiveVertexShader());


        // Fragment Shader.
        String fragmentSource
        (
            "#version 120\n"

            "varying vec2 VertexOutput_TexCoord;\n"
    
            "uniform sampler2D ColourBuffer;\n"
    
            "void main()\n"
            "{\n"
            "    gl_FragData[0] = texture2D(ColourBuffer, VertexOutput_TexCoord);\n"
            "}"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }

    Shader* DefaultSceneRenderer_ShaderBuilder::CreateHDRFinalCompositionShader()
    {
        // Vertex Shader.
        String vertexSource(this->GetFullscreenPrimitiveVertexShader());


        // Fragment Shader.
        String fragmentSource
        (
            "#version 120\n"

            "varying vec2 VertexOutput_TexCoord;\n"
    
            "uniform sampler2D ColourBuffer;\n"
            "uniform sampler2D LuminanceBuffer;\n"
            "uniform sampler2D BloomBuffer;\n"
            "uniform float     BloomFactor;\n"
    
            "void main()\n"
            "{\n"
            "    vec4  bloom     = texture2D(BloomBuffer, VertexOutput_TexCoord);\n"
            "    float luminance = texture2D(LuminanceBuffer, vec2(0.0, 0.0)).r;\n"
        
            "    gl_FragData[0] = (bloom * BloomFactor) + texture2D(ColourBuffer, VertexOutput_TexCoord);\n"
            "    gl_FragData[0] = gl_FragData[0] * luminance;\n"
            "}"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }

    Shader* DefaultSceneRenderer_ShaderBuilder::CreateHDRNoBloomFinalCompositionShader()
    {
        // Vertex Shader.
        String vertexSource(this->GetFullscreenPrimitiveVertexShader());


        // Fragment Shader.
        String fragmentSource
        (
            "#version 120\n"

            "varying vec2 VertexOutput_TexCoord;\n"
    
            "uniform sampler2D ColourBuffer;\n"
            "uniform sampler2D LuminanceBuffer;\n"
    
            "void main()\n"
            "{\n"
            "    gl_FragData[0] = texture2D(ColourBuffer, VertexOutput_TexCoord) * texture2D(LuminanceBuffer, vec2(0.0, 0.0)).r;\n"
            "}\n"
        );


        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }


    //////////////////////////////////////////////////////
    // Private

    Shader* DefaultSceneRenderer_ShaderBuilder::CreateGaussianBlurShader(unsigned int kernelSize, float sigmaSquared, bool xAxis)
    {
        // Some constants to make things easier later on.
        const double pi = glm::pi<double>();
        const double e  = glm::e<double>();
        const double s2 = static_cast<double>(sigmaSquared);


        // Vertex Shader.
        String vertexSource
        (
            "#version 120\n"
            
            "attribute vec3 VertexInput_Position;\n"
            "attribute vec4 VertexInput_TexCoord;\n"

            "varying vec4 VertexOutput_TexCoord;\n"

            "void main()\n"
            "{\n"
            "    VertexOutput_TexCoord = VertexInput_TexCoord;\n"
            "    gl_Position           = vec4(VertexInput_Position, 1.0);\n"
            "}"
        );

        String fragmentSource
        (
            "#version 120\n"
            
            "varying vec4 VertexOutput_TexCoord;\n"
        
            "uniform sampler2D Texture;\n"
            "uniform float     TextureSizeReciprocal;\n"

            "void main()\n"
            "{\n"
            "    vec2  uv       = VertexOutput_TexCoord.xy;\n"
            "    float uvOffset = TextureSizeReciprocal;\n"
            
            "    vec4  outputColour = vec4(0.0, 0.0, 0.0, 0.0);\n"
        );

        for (int iCoefficient = 0; iCoefficient < static_cast<int>(kernelSize); ++iCoefficient)
        {
            const double x = static_cast<double>(iCoefficient - static_cast<int>(kernelSize / 2));
            const double coefficient = (1.0 / (glm::sqrt(2.0 * pi * s2))) * (glm::pow(e, -((x*x) / (2.0 * s2))));

            if (xAxis)
            {
                // X Axis.
                fragmentSource.AppendFormatted
                (
                    "    outputColour = texture2D(Texture, vec2(uv.x + (%.10f * uvOffset), uv.y)) * %.10f + outputColour;\n", x, coefficient
                );
            }
            else
            {
                // Y Axis.
                fragmentSource.AppendFormatted
                (
                    "    outputColour = texture2D(Texture, vec2(uv.x, uv.y + (%.10f * uvOffset))) * %.10f + outputColour;\n", x, coefficient
                );
            }
        }

        fragmentSource.Append
        (
            "    gl_FragData[0] = outputColour;\n"
            "}\n"
        );

        return Renderer::CreateShader(vertexSource.c_str(), fragmentSource.c_str());
    }


    String DefaultSceneRenderer_ShaderBuilder::CreateVertexShaderString(const DefaultSceneRenderer_MaterialShaderID &shaderID, const MaterialDefinition* material) const
    {
        (void)material;

        uint16_t directionalLightCount       = shaderID.lightGroupID.GetDirectionalLightCount();
        uint16_t pointLightCount             = shaderID.lightGroupID.GetPointLightCount();
        uint16_t spotLightCount              = shaderID.lightGroupID.GetSpotLightCount();
        uint16_t shadowDirectionalLightCount = shaderID.lightGroupID.GetShadowDirectionalLightCount();
        uint16_t shadowPointLightCount       = shaderID.lightGroupID.GetShadowPointLightCount();
        uint16_t shadowSpotLightCount        = shaderID.lightGroupID.GetShadowSpotLightCount();
        
        
        String vertexSource
        (
            "#version 120\n"
            
            "attribute vec3 VertexInput_Position;\n"
            "attribute vec3 VertexInput_PositionVS;\n"
            "attribute vec4 VertexInput_TexCoord;\n"
            "attribute vec4 VertexInput_Normal;\n"
            ""
            "varying vec4 VertexOutput_Position;\n"
            "varying vec4 VertexOutput_PositionVS;\n"
            "varying vec4 VertexOutput_PositionWS;\n"
            "varying vec4 VertexOutput_TexCoord;\n"
            "varying vec4 VertexOutput_Normal;\n"
            ""
            "uniform mat4 PVMMatrix;\n"
            "uniform mat4 ViewModelMatrix;\n"
            "uniform mat4 ModelMatrix;\n"
            "uniform mat3 NormalMatrix;\n"
        );

        if (!(shaderID.flags & DefaultSceneRenderer_MaterialShaderID::NoNormalMapping) || material->HasNormalChannel())
        {
            vertexSource.Append
            (
                "attribute vec3 VertexInput_Tangent;\n"
                "attribute vec3 VertexInput_Bitangent;\n"
                ""
                "varying vec3 VertexOutput_Tangent;\n"
                "varying vec3 VertexOutput_Bitangent;\n"
            );
        }

        vertexSource.Append
        (
            "attribute vec4 VertexInput_Colour;\n"
            "varying vec4 VertexOutput_Colour;\n"
        );


        // Light structures.
        if (directionalLightCount > 0)
        {
            vertexSource.Append
            (
                "struct DirectionalLightVS\n"
                "{\n"
                "    mat4 ProjectionView;\n"
                "};\n"
            );

            for (uint16_t i = 0; i < directionalLightCount; ++i)
            {
                vertexSource.Append(this->VertexShader_DeclareDirectionalLight(i));
            }
        }

        if (pointLightCount > 0)
        {
            vertexSource.Append
            (
                "struct PointLightVS\n"
                "{\n"
                "    vec3 PositionVS;\n"
                "};\n"
            );

            for (uint16_t i = 0; i < pointLightCount; ++i)
            {
                vertexSource.Append(this->VertexShader_DeclarePointLight(i));
            }
        }

        if (spotLightCount > 0)
        {
            vertexSource.Append
            (
                "struct SpotLightVS\n"
                "{\n"
                "    mat4 ProjectionView;\n"
                "};\n"
            );

            for (uint16_t i = 0; i < spotLightCount; ++i)
            {
                vertexSource.Append(this->VertexShader_DeclareSpotLight(i));
            }
        }

        if (shadowDirectionalLightCount > 0)
        {
            vertexSource.Append
            (
                "struct ShadowDirectionalLightVS\n"
                "{\n"
                "    mat4 ProjectionView;\n"
                "};\n"
            );

            for (uint16_t i = 0; i < shadowDirectionalLightCount; ++i)
            {
                vertexSource.Append(this->VertexShader_DeclareShadowDirectionalLight(i));
            }
        }

        if (shadowPointLightCount > 0)
        {
            vertexSource.Append
            (
                "struct ShadowPointLightVS\n"
                "{\n"
                "    vec3 PositionVS;\n"
                "    vec3 PositionWS;\n"
                "};\n"
            );

            for (uint16_t i = 0; i < shadowPointLightCount; ++i)
            {
                vertexSource.Append(this->VertexShader_DeclareShadowPointLight(i));
            }
        }

        if (shadowSpotLightCount > 0)
        {
            vertexSource.Append
            (
                "struct ShadowSpotLightVS\n"
                "{\n"
                "    mat4 ProjectionView;\n"
                "};\n"
            );

            for (uint16_t i = 0; i < shadowSpotLightCount; ++i)
            {
                vertexSource.Append(this->VertexShader_DeclareShadowSpotLight(i));
            }
        }



        vertexSource.Append
        (
            "void main()\n"
            "{\n"
            "    VertexOutput_Position   = PVMMatrix       * vec4(VertexInput_Position, 1.0);\n"
            "    VertexOutput_PositionVS = ViewModelMatrix * vec4(VertexInput_Position, 1.0);\n"
            "    VertexOutput_PositionWS = ModelMatrix     * vec4(VertexInput_Position, 1.0);\n"
            "    VertexOutput_TexCoord   = VertexInput_TexCoord;\n"
            "    VertexOutput_Colour     = VertexInput_Colour;\n"
            "    VertexOutput_Normal.xyz = normalize(NormalMatrix * VertexInput_Normal.xyz); VertexOutput_Normal.w = VertexInput_Normal.w;\n"
            "\n"
        );

        if (!(shaderID.flags & DefaultSceneRenderer_MaterialShaderID::NoNormalMapping))
        {
            vertexSource.Append
            (
                "    VertexOutput_Tangent    = normalize(NormalMatrix * VertexInput_Tangent);\n"
                "    VertexOutput_Bitangent  = normalize(NormalMatrix * VertexInput_Bitangent);\n"
                "\n"
            );
        }

        for (uint16_t i = 0; i < directionalLightCount; ++i)
        {
            vertexSource.Append(this->VertexShader_AssignDirectionalLight(i));
        }

        for (uint16_t i = 0; i < pointLightCount; ++i)
        {
            vertexSource.Append(this->VertexShader_AssignPointLight(i));
        }

        for (uint16_t i = 0; i < spotLightCount; ++i)
        {
            vertexSource.Append(this->VertexShader_AssignSpotLight(i));
        }

        for (uint16_t i = 0; i < shadowDirectionalLightCount; ++i)
        {
            vertexSource.Append(this->VertexShader_AssignShadowDirectionalLight(i));
        }

        for (uint16_t i = 0; i < shadowPointLightCount; ++i)
        {
            vertexSource.Append(this->VertexShader_AssignShadowPointLight(i));
        }

        for (uint16_t i = 0; i < shadowSpotLightCount; ++i)
        {
            vertexSource.Append(this->VertexShader_AssignShadowSpotLight(i));
        }

        vertexSource.Append
        (
            "    gl_Position = VertexOutput_Position;\n"
            "}\n"
        );


        return vertexSource;
    }

    String DefaultSceneRenderer_ShaderBuilder::CreateFragmentShaderString(const DefaultSceneRenderer_MaterialShaderID &shaderID, const MaterialDefinition* material) const
    {
        uint16_t ambientLightCount           = shaderID.lightGroupID.GetAmbientLightCount();
        uint16_t directionalLightCount       = shaderID.lightGroupID.GetDirectionalLightCount();
        uint16_t pointLightCount             = shaderID.lightGroupID.GetPointLightCount();
        uint16_t spotLightCount              = shaderID.lightGroupID.GetSpotLightCount();
        uint16_t shadowDirectionalLightCount = shaderID.lightGroupID.GetShadowDirectionalLightCount();
        uint16_t shadowPointLightCount       = shaderID.lightGroupID.GetShadowPointLightCount();
        uint16_t shadowSpotLightCount        = shaderID.lightGroupID.GetShadowSpotLightCount();

        auto shadowLightCount = shadowDirectionalLightCount + shadowPointLightCount + shadowSpotLightCount;
        auto lightCount       = ambientLightCount + directionalLightCount + pointLightCount + spotLightCount + shadowLightCount;
        

        bool includeMaterialPass        =  (shaderID.flags & DefaultSceneRenderer_MaterialShaderID::IncludeMaterialPass)     && material != nullptr;
        bool doNormalMapping            = !(shaderID.flags & DefaultSceneRenderer_MaterialShaderID::NoNormalMapping)         && material != nullptr && material->IsChannelDefined("normal");
        bool sourceLightingFromTextures =  (shaderID.flags & DefaultSceneRenderer_MaterialShaderID::GetLightingFromTextures) && includeMaterialPass;

        // We won't be doing normal mapping if all we are using is an ambient light.
        if (lightCount - ambientLightCount == 0)
        {
            doNormalMapping = false;
        }


        // Input
        String fragmentSource
        (
            "#version 120\n"
            
            "varying vec4 VertexOutput_Position;\n"
            "varying vec4 VertexOutput_PositionVS;\n"
            "varying vec4 VertexOutput_PositionWS;\n"
            "varying vec4 VertexOutput_TexCoord;\n"
            "varying vec4 VertexOutput_Normal;\n"
        );

        if (doNormalMapping)
        {
            fragmentSource.Append
            (
                "varying vec3 VertexOutput_Tangent;\n"
                "varying vec3 VertexOutput_Bitangent;\n"
            );
        }

        fragmentSource.Append
        (
            "varying vec4 VertexOutput_Colour;\n"
        );

#if 0
        // Output
        if (includeMaterialPass)
        {
            fragmentSource.Append
            (
                "layout(location = 0) out vec4 ColourOut;\n"
            );
        }
        else
        {
            fragmentSource.Append
            (
                "layout(location = 0) out vec4 DiffuseOut;\n"
                "layout(location = 1) out vec4 SpecularOut;\n"
            );
        }
#endif

        // Uniforms
        if (sourceLightingFromTextures)
        {
            fragmentSource.Append
            (
                "uniform sampler2D DiffuseLighting;\n"
                "uniform sampler2D SpecularLighting;\n"
            );
        }


        // Material functions.
        if (material != nullptr)
        {
            if (includeMaterialPass)
            {
                if (material->IsChannelDefined("refraction"))
                {
                    fragmentSource.Append
                    (
                        "uniform sampler2D BackgroundTexture;\n"
                    );

                    fragmentSource.Append(this->GetMaterialShaderString_Refraction(*material));
                }

                fragmentSource.Append(this->GetMaterialShaderString_Diffuse(*material));
                fragmentSource.Append(this->GetMaterialShaderString_Emissive(*material));
                fragmentSource.Append(this->GetMaterialShaderString_Shininess(*material));
            }

            if (lightCount - ambientLightCount > 0)     // <-- Ambient lights don't use specular or normals, so don't want to include that.
            {
                fragmentSource.Append(this->GetMaterialShaderString_Specular(*material));

                if (doNormalMapping)
                {
                    fragmentSource.Append(this->GetMaterialShaderString_Normal(*material));
                }
            }
        }
        

        // Lighting functions and structures.
        if (lightCount - ambientLightCount > 0)
        {
            fragmentSource.Append
            (
                "float DiffuseFactor(vec3 N, vec3 L)\n"
                "{\n"
                "    return max(0.0, dot(N, L));\n"
                "}\n"

                "float SpecularFactor(vec3 N, vec3 H, float power)\n"
                "{\n"
                "    return max(0.0, pow(max(dot(N, H), 0.0), power));\n"
                "}\n"

                "float AttenuationFactor(float r, float f, float d)\n"
                "{\n"
                "    return pow(max(0.0, 1.0 - (d / r)), f + 1.0);\n"
                "}\n"
            );

            if (spotLightCount > 0 || shadowSpotLightCount > 0)
            {
                fragmentSource.Append
                (
                    "float SpotFactor(vec3 L, vec3 lightDirection, float cosAngleInner, float cosAngleOuter)\n"
                    "{\n"
                    "    return clamp((dot(-L, lightDirection) - cosAngleOuter) / (cosAngleInner - cosAngleOuter), 0.0, 1.0);\n"
                    "}\n"
                );
            }
        }

        if (shadowLightCount > 0)
        {
            fragmentSource.Append
            (
                "float linstep(float low, float high, float v)\n"
                "{\n"
                "    return clamp((v - low) / (high - low), 0.0, 1.0);\n"
                "}\n"
            );
        }


        // Ambient.
        if (ambientLightCount > 0)
        {
            fragmentSource.Append
            (
                "struct AmbientLightFS\n"
                "{\n"
                "    vec3 Colour;\n"
                "};\n"
                ""
                "void AccumulateAmbientLighting(in AmbientLightFS light, inout vec3 diffuseOut)\n"
                "{\n"
                "    diffuseOut += light.Colour;\n"
                "}\n"
            );
        }

        // Directional.
        if (directionalLightCount > 0 || shadowDirectionalLightCount > 0)
        {
            if (directionalLightCount > 0)
            {
                fragmentSource.Append
                (
                    "struct DirectionalLightFS\n"
                    "{\n"
                    "    vec3 Colour;\n"
                    "    vec3 Direction;\n"
                    "};\n"

                    "void AccumulateDirectionalLighting(in DirectionalLightFS light, in vec3 normal, inout vec3 diffuseOut, inout vec3 specularOut)\n"
                    "{\n"
                    "    vec3 N = normal;\n"
                    "    vec3 L = -light.Direction;\n"
                    "    vec3 H = normalize(L - normalize(VertexOutput_PositionVS.xyz));\n"
                    ""
                    "    float diffuse  = DiffuseFactor(N, L);\n"
                    "    float specular = SpecularFactor(N, H, Specular());\n"
                    ""
                    "    diffuseOut  += light.Colour * diffuse;\n"
                    "    specularOut += light.Colour * specular;\n"
                    "}\n"
                );
            }

            if (shadowDirectionalLightCount > 0)
            {
                fragmentSource.Append
                (
                    "struct ShadowDirectionalLightFS\n"
                    "{\n"
                    "    vec3 Colour;\n"
                    "    vec3 Direction;\n"
                    "};\n"

                    "float CalculateDirectionalShadow(in sampler2D shadowMap, in vec4 lightSpacePosition)\n"
                    "{\n"
                    "    vec3  shadowCoord   = lightSpacePosition.xyz / lightSpacePosition.w;\n"
                    "    vec2  shadowUV      = shadowCoord.xy * 0.5 + 0.5;\n"
                    "    float fragmentDepth = shadowCoord.z;\n"
                    ""
                    "    float bias     = 0.00075;\n"
                    "    vec2  moments  = texture2D(shadowMap, shadowUV).xy;\n"
                    "    float variance = moments.y - (moments.x * moments.x);\n"
                    "    float d        = fragmentDepth - moments.x;\n"
                    "    float p        = smoothstep(fragmentDepth - bias, fragmentDepth, moments.x);\n"
                    "    float pMax     = linstep(1.0, 1.0, variance / (variance + (d * d)));\n"
                    ""
                    "    return clamp(max(p, pMax), 0.0, 1.0);\n"
                    "}\n"

                    "void AccumulateShadowDirectionalLighting(in ShadowDirectionalLightFS light, in vec3 normal, in sampler2D shadowMap, in vec4 lightSpacePosition, inout vec3 diffuseOut, inout vec3 specularOut)\n"
                    "{\n"
                    "    vec3 N = normal;\n"
                    "    vec3 L = -light.Direction;\n"
                    "    vec3 H = normalize(L - normalize(VertexOutput_PositionVS.xyz));\n"
                    ""
                    "    float diffuse  = DiffuseFactor(N, L);\n"
                    "    float specular = SpecularFactor(N, H, Specular());\n"
                    "    float shadow   = CalculateDirectionalShadow(shadowMap, lightSpacePosition);\n"
                    ""
                    "    diffuseOut  += light.Colour * (diffuse  * shadow);\n"
                    "    specularOut += light.Colour * (specular * shadow);\n"
                    "}\n"
                );
            }
        }

        // Point
        if (pointLightCount > 0 || shadowPointLightCount > 0)
        {
            if (pointLightCount > 0)
            {
                fragmentSource.Append
                (
                    "struct PointLightFS\n"
                    "{\n"
                    "    vec3  Colour;\n"
                    "    float Radius;\n"
                    "    float Falloff;\n"
                    "};\n"

                    "void AccumulatePointLighting(in PointLightFS light, in vec3 normal, in vec3 lightVector, inout vec3 diffuseOut, inout vec3 specularOut)\n"
                    "{\n"
                    "    vec3 N = normal;\n"
                    "    vec3 L = normalize(lightVector);\n"
                    "    vec3 H = normalize(L - normalize(VertexOutput_PositionVS.xyz));\n"
                    ""
                    "    float diffuse     = DiffuseFactor(N, L);\n"
                    "    float specular    = SpecularFactor(N, H, Specular());\n"
                    "    float attenuation = AttenuationFactor(light.Radius, light.Falloff, length(lightVector));\n"
                    ""
                    "    diffuseOut  += light.Colour * (diffuse  * attenuation);\n"
                    "    specularOut += light.Colour * (specular * attenuation);\n"
                    "}\n"
                );
            }

            if (shadowPointLightCount > 0)
            {
                fragmentSource.Append
                (
                    "struct ShadowPointLightFS\n"
                    "{\n"
                    "    vec3  Colour;\n"
                    "    float Radius;\n"
                    "    float Falloff;\n"
                    "};\n"

                    "float CalculatePointShadow(in samplerCube shadowMap, in vec3 shadowCoord)\n"
                    "{\n"
                    "    float fragmentDepth = length(shadowCoord);\n"
                    ""
                    "    float bias     = 0.04 * fragmentDepth;\n"                      // This can affect seams. Lower value = more seams.
                    "    vec2  moments  = textureCube(shadowMap, shadowCoord).xy;\n"
                    "    float variance = moments.y - (moments.x * moments.x);\n"
                    "    float d        = fragmentDepth - moments.x;\n"
                    "    float p        = smoothstep(fragmentDepth - bias, fragmentDepth, moments.x);\n"
                    "    float pMax     = linstep(0.4, 1.0, variance / ((d * d) + variance));\n"
                    ""
                    "    return clamp(max(p, pMax), 0.0, 1.0);\n"
                    "}\n"

                    "void AccumulateShadowPointLighting(in ShadowPointLightFS light, in vec3 normal, in vec3 lightVector, in samplerCube shadowMap, in vec3 shadowCoord, inout vec3 diffuseOut, inout vec3 specularOut)\n"
                    "{\n"
                    "    vec3 N = normal;\n"
                    "    vec3 L = normalize(lightVector);\n"
                    "    vec3 H = normalize(L - normalize(VertexOutput_PositionVS.xyz));\n"
                    ""
                    "    float diffuse     = DiffuseFactor(N, L);\n"
                    "    float specular    = SpecularFactor(N, H, Specular());\n"
                    "    float attenuation = AttenuationFactor(light.Radius, light.Falloff, length(lightVector));\n"
                    "    float shadow      = CalculatePointShadow(shadowMap, shadowCoord);\n"
                    ""
                    "    diffuseOut  += light.Colour * (diffuse  * attenuation * shadow);\n"
                    "    specularOut += light.Colour * (specular * attenuation * shadow);\n"
                    "}\n"
                );
            }
        }


        // Spot.
        if (spotLightCount > 0 || shadowSpotLightCount > 0)
        {
            if (spotLightCount > 0)
            {
                fragmentSource.Append
                (
                    "struct SpotLightFS\n"
                    "{\n"
                    "    vec3  Colour;\n"
                    "    vec3  Position;\n"
                    "    vec3  Direction;\n"
                    "    float Length;\n"
                    "    float Falloff;\n"
                    "    float CosAngleInner;\n"
                    "    float CosAngleOuter;\n"
                    "};\n"    

                    "void AccumulateSpotLighting(in SpotLightFS light, in vec3 normal, inout vec3 diffuseOut, inout vec3 specularOut)\n"
                    "{\n"
                    "    vec3 N = normal;\n"
                    "    vec3 L = light.Position - VertexOutput_PositionVS.xyz;\n"
                    "    vec3 H = normalize(normalize(L) - normalize(VertexOutput_PositionVS.xyz));\n"
                    ""
                    "    float diffuse     = DiffuseFactor(N, normalize(L));\n"
                    "    float specular    = SpecularFactor(N, H, Specular());\n"
                    "    float attenuation = AttenuationFactor(light.Length, light.Falloff, length(L));\n"
                    "    float spot        = SpotFactor(normalize(L), light.Direction, light.CosAngleInner, light.CosAngleOuter);\n"
                    ""
                    "    diffuseOut  += light.Colour * (diffuse  * attenuation * spot);\n"
                    "    specularOut += light.Colour * (specular * attenuation * spot);\n"
                    "}\n"
                );
            }

            if (shadowSpotLightCount > 0)
            {
                fragmentSource.Append
                (
                    "struct ShadowSpotLightFS\n"
                    "{\n"
                    "    vec3  Colour;\n"
                    "    vec3  Position;\n"
                    "    vec3  Direction;\n"
                    "    float Length;\n"
                    "    float Falloff;\n"
                    "    float CosAngleInner;\n"
                    "    float CosAngleOuter;\n"
                    "};\n"   

                    "float CalculateSpotShadow(in sampler2D shadowMap, in vec4 lightSpacePosition)\n"
                    "{\n"
                    "    vec3  shadowCoord   = lightSpacePosition.xyz / lightSpacePosition.w;\n"
                    "    vec2  shadowUV      = shadowCoord.xy * 0.5 + 0.5;\n"
                    "    float fragmentDepth = shadowCoord.z;\n"
                    ""
                    "    float bias     = 0.001 * fragmentDepth;\n"
                    "    vec2  moments  = texture2D(shadowMap, shadowUV).xy;\n"
                    "    float variance = moments.y - (moments.x * moments.x);\n"
                    "    float d        = fragmentDepth - moments.x;\n"
                    "    float p        = smoothstep(fragmentDepth - bias, fragmentDepth, moments.x);\n"
                    "    float pMax     = linstep(0.4, 1.0, variance / (variance + (d * d)));\n"
                    ""
                    "    return clamp(max(p, pMax), 0.0, 1.0);\n"
                    "}\n"

                    "void AccumulateShadowSpotLighting(in ShadowSpotLightFS light, in vec3 normal, in sampler2D shadowMap, in vec4 lightSpacePosition, inout vec3 diffuseOut, inout vec3 specularOut)\n"
                    "{\n"
                    "    vec3 N = normal;\n"
                    "    vec3 L = light.Position - VertexOutput_PositionVS.xyz;\n"
                    "    vec3 H = normalize(normalize(L) - normalize(VertexOutput_PositionVS.xyz));\n"
                    ""
                    "    float diffuse     = DiffuseFactor(N, normalize(L));\n"
                    "    float specular    = SpecularFactor(N, H, Specular());\n"
                    "    float attenuation = AttenuationFactor(light.Length, light.Falloff, length(L));\n"
                    "    float spot        = SpotFactor(normalize(L), light.Direction, light.CosAngleInner, light.CosAngleOuter);\n"
                    "    float shadow      = CalculateSpotShadow(shadowMap, lightSpacePosition);\n"
                    ""
                    "    diffuseOut  += light.Colour * (diffuse  * attenuation * spot * shadow);\n"
                    "    specularOut += light.Colour * (specular * attenuation * spot * shadow);\n"
                    "}\n"
                );
            }
        }


        // Light declarations.

        for (uint16_t i = 0; i < ambientLightCount; ++i)
        {
            fragmentSource.Append(this->FragmentShader_DeclareAmbientLight(i));
        }

        for (uint16_t i = 0; i < directionalLightCount; ++i)
        {
            fragmentSource.Append(this->FragmentShader_DeclareDirectionalLight(i));
        }

        for (uint16_t i = 0; i < pointLightCount; ++i)
        {
            fragmentSource.Append(this->FragmentShader_DeclarePointLight(i));
        }

        for (uint16_t i = 0; i < spotLightCount; ++i)
        {
            fragmentSource.Append(this->FragmentShader_DeclareSpotLight(i));
        }

        for (uint16_t i = 0; i < shadowDirectionalLightCount; ++i)
        {
            fragmentSource.Append(this->FragmentShader_DeclareShadowDirectionalLight(i));
        }

        for (uint16_t i = 0; i < shadowPointLightCount; ++i)
        {
            fragmentSource.Append(this->FragmentShader_DeclareShadowPointLight(i));
        }

        for (uint16_t i = 0; i < shadowSpotLightCount; ++i)
        {
            fragmentSource.Append(this->FragmentShader_DeclareShadowSpotLight(i));
        }


        // main()
        fragmentSource.Append
        (
            "void main()\n"
            "{\n"
        );
        {
            // Screen UV.
            if (sourceLightingFromTextures || (material != nullptr && material->IsRefractive()))
            {
                fragmentSource.Append
                (
                    "    vec2 screenUV = (VertexOutput_Position.xy / VertexOutput_Position.w) * 0.5 + 0.5;\n"
                );
            }

            // Normal.
            if (lightCount - ambientLightCount > 0)
            {
                if (doNormalMapping)
                {
                    fragmentSource.Append
                    (
                        "    vec3 normal = normalize(mat3(VertexOutput_Tangent, VertexOutput_Bitangent, VertexOutput_Normal.xyz) * Normal());\n"
                    );
                }
                else
                {
                    fragmentSource.Append
                    (
                        "    vec3 normal = normalize(VertexOutput_Normal.xyz);\n"
                    );
                }
            }

            // Lighting.
            if (sourceLightingFromTextures)
            {
                fragmentSource.Append
                (
                    "    vec3 lightDiffuse  = texture2D(DiffuseLighting,  screenUV).xyz;\n"
                    "    vec3 lightSpecular = texture2D(SpecularLighting, screenUV).xyz;\n"
                );
            }

            if (lightCount > 0)
            {
                if (!sourceLightingFromTextures)
                {
                    fragmentSource.Append
                    (
                        "    vec3 lightDiffuse  = vec3(0.0, 0.0, 0.0);\n"
                        "    vec3 lightSpecular = vec3(0.0, 0.0, 0.0);\n"
                    );
                }


                for (uint16_t i = 0; i < ambientLightCount; ++i)
                {
                    fragmentSource.Append(this->FragmentShader_AccumulateAmbientLighting(i));
                }

                for (uint16_t i = 0; i < directionalLightCount; ++i)
                {
                    fragmentSource.Append(this->FragmentShader_AccumulateDirectionalLighting(i));
                }

                for (uint16_t i = 0; i < pointLightCount; ++i)
                {
                    fragmentSource.Append(this->FragmentShader_AccumulatePointLighting(i));
                }

                for (uint16_t i = 0; i < spotLightCount; ++i)
                {
                    fragmentSource.Append(this->FragmentShader_AccumulateSpotLighting(i));
                }

                for (uint16_t i = 0; i < shadowDirectionalLightCount; ++i)
                {
                    fragmentSource.Append(this->FragmentShader_AccumulateShadowDirectionalLighting(i));
                }

                for (uint16_t i = 0; i < shadowPointLightCount; ++i)
                {
                    fragmentSource.Append(this->FragmentShader_AccumulateShadowPointLighting(i));
                }

                for (uint16_t i = 0; i < shadowSpotLightCount; ++i)
                {
                    fragmentSource.Append(this->FragmentShader_AccumulateShadowSpotLighting(i));
                }
            }


            // Material.
            if (includeMaterialPass)
            {
                fragmentSource.Append
                (
                    "    vec4  materialDiffuse  = Diffuse();\n"
                    "    vec3  materialEmissive = Emissive();\n"
                );

                if (material->IsRefractive())
                {
                    fragmentSource.Append
                    (
                        "    vec3  materialRefraction = Refraction();\n"
                    );
                }

                if (lightCount > 0 || sourceLightingFromTextures)
                {
                    fragmentSource.Append
                    (
                        "    float materialShininess  = Shininess();\n"
                    );

                    if (material->IsRefractive())
                    {
                        fragmentSource.Append
                        (
                            "    lightDiffuse  *= materialDiffuse.a;\n"
                            "    lightSpecular *= materialDiffuse.a;\n"
                        );
                    }
                
                    fragmentSource.Append
                    (
                        "    gl_FragData[0].rgb = (materialDiffuse.rgb * lightDiffuse) + (materialShininess * lightSpecular) + materialEmissive;\n"
                    );
                }
                else
                {
                    fragmentSource.Append
                    (
                        "    gl_FragData[0].rgb = materialEmissive;\n"
                    );
                }

                if (material->IsRefractive())
                {
                    fragmentSource.Append
                    (
                        "    gl_FragData[0].rgb += texture2D(BackgroundTexture, screenUV + (normalize(materialRefraction).xy * 0.01)).rgb * (1.0 - materialDiffuse.a);\n"
                    );
                }
                else
                {
                    fragmentSource.Append
                    (
                        "    gl_FragData[0].a = materialDiffuse.a;\n"
                    );
                }
            }
            else
            {
                if (lightCount > 0)
                {
                    fragmentSource.Append
                    (
                        "    gl_FragData[0] = vec4(lightDiffuse,  1.0);\n"
                        "    gl_FragData[1] = vec4(lightSpecular, 1.0);\n"
                    );
                }
                else
                {
                    fragmentSource.Append
                    (
                        "    gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);\n"
                        "    gl_FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);\n"
                    );
                }
            }
        }
        fragmentSource.Append
        (
            "}\n"
        );



        return fragmentSource;
    }



    String DefaultSceneRenderer_ShaderBuilder::VertexShader_DeclareDirectionalLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec4 DirectionalLight%d_VertexPositionLS;\n"
            "uniform DirectionalLightVS DirectionalLightVS%d;\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_DeclarePointLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec3 PointLight%d_L;\n"
            "uniform PointLightVS PointLightVS%d;\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_DeclareSpotLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec4 SpotLight%d_VertexPositionLS;\n"
            "uniform SpotLightVS SpotLightVS%d;\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_DeclareShadowDirectionalLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec4 ShadowDirectionalLight%d_VertexPositionLS;\n"
            "uniform ShadowDirectionalLightVS ShadowDirectionalLightVS%d;\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_DeclareShadowPointLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec3 ShadowPointLight%d_L;\n"
            "varying vec3 ShadowPointLight%d_ShadowCoord;\n"
            "uniform ShadowPointLightVS ShadowPointLightVS%d;\n",

            lightIndex,
            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_DeclareShadowSpotLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec4 ShadowSpotLight%d_VertexPositionLS;\n"
            "uniform ShadowSpotLightVS ShadowSpotLightVS%d;\n",

            lightIndex,
            lightIndex
        );
    }


    String DefaultSceneRenderer_ShaderBuilder::VertexShader_AssignDirectionalLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    DirectionalLight%d_VertexPositionLS = DirectionalLightVS%d.ProjectionView * ModelMatrix * vec4(VertexInput_Position, 1.0);\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_AssignPointLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    PointLight%d_L = PointLightVS%d.PositionVS - VertexOutput_PositionVS.xyz;\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_AssignSpotLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    SpotLight%d_VertexPositionLS = SpotLightVS%d.ProjectionView * ModelMatrix * vec4(VertexInput_Position, 1.0);\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_AssignShadowDirectionalLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    ShadowDirectionalLight%d_VertexPositionLS = ShadowDirectionalLightVS%d.ProjectionView * ModelMatrix * vec4(VertexInput_Position, 1.0);\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_AssignShadowPointLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    ShadowPointLight%d_L           = ShadowPointLightVS%d.PositionVS - VertexOutput_PositionVS.xyz;\n"
            "    ShadowPointLight%d_ShadowCoord = VertexOutput_PositionWS.xyz     - ShadowPointLightVS%d.PositionWS;\n",

            lightIndex,
            lightIndex,
            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::VertexShader_AssignShadowSpotLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    ShadowSpotLight%d_VertexPositionLS = ShadowSpotLightVS%d.ProjectionView * ModelMatrix * vec4(VertexInput_Position, 1.0);\n",

            lightIndex,
            lightIndex
        );
    }





    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_DeclareAmbientLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "uniform AmbientLightFS AmbientLightFS%d;\n",

            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_DeclareDirectionalLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec4 DirectionalLight%d_VertexPositionLS;\n"
            "uniform DirectionalLightFS DirectionalLightFS%d;\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_DeclarePointLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec3 PointLight%d_L;\n"
            "uniform PointLightFS PointLightFS%d;\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_DeclareSpotLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec4 SpotLight%d_VertexPositionLS;\n"
            "uniform SpotLightFS SpotLightFS%d;\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_DeclareShadowDirectionalLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec4 ShadowDirectionalLight%d_VertexPositionLS;\n"
            "uniform ShadowDirectionalLightFS ShadowDirectionalLightFS%d;\n"
            "uniform sampler2D                ShadowDirectionalLightFS%d_ShadowMap;\n",

            lightIndex,
            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_DeclareShadowPointLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec3 ShadowPointLight%d_L;\n"
            "varying vec3 ShadowPointLight%d_ShadowCoord;\n"
            "uniform ShadowPointLightFS ShadowPointLightFS%d;\n"
            "uniform samplerCube        ShadowPointLightFS%d_ShadowMap;\n",

            lightIndex,
            lightIndex,
            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_DeclareShadowSpotLight(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "varying vec4 ShadowSpotLight%d_VertexPositionLS;\n"
            "uniform ShadowSpotLightFS ShadowSpotLightFS%d;\n"
            "uniform sampler2D         ShadowSpotLightFS%d_ShadowMap;\n",

            lightIndex,
            lightIndex,
            lightIndex
        );
    }



    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_AccumulateAmbientLighting(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    AccumulateAmbientLighting(AmbientLightFS%d, lightDiffuse);\n",

            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_AccumulateDirectionalLighting(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    AccumulateDirectionalLighting(DirectionalLightFS%d, normal, lightDiffuse, lightSpecular);\n",

            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_AccumulatePointLighting(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    AccumulatePointLighting(PointLightFS%d, normal, PointLight%d_L, lightDiffuse, lightSpecular);\n",

            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_AccumulateSpotLighting(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    AccumulateSpotLighting(SpotLightFS%d, normal, lightDiffuse, lightSpecular);\n",

            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_AccumulateShadowDirectionalLighting(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    AccumulateShadowDirectionalLighting(ShadowDirectionalLightFS%d, normal, ShadowDirectionalLightFS%d_ShadowMap, ShadowDirectionalLight%d_VertexPositionLS, lightDiffuse, lightSpecular);\n",

            lightIndex,
            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_AccumulateShadowPointLighting(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    AccumulateShadowPointLighting(ShadowPointLightFS%d, normal, ShadowPointLight%d_L, ShadowPointLightFS%d_ShadowMap, ShadowPointLight%d_ShadowCoord, lightDiffuse, lightSpecular);\n",

            lightIndex,
            lightIndex,
            lightIndex,
            lightIndex
        );
    }

    String DefaultSceneRenderer_ShaderBuilder::FragmentShader_AccumulateShadowSpotLighting(int lightIndex) const
    {
        return String::CreateFormatted
        (
            "    AccumulateShadowSpotLighting(ShadowSpotLightFS%d, normal, ShadowSpotLightFS%d_ShadowMap, ShadowSpotLight%d_VertexPositionLS, lightDiffuse, lightSpecular);\n",

            lightIndex,
            lightIndex,
            lightIndex
        );
    }




    const char* DefaultSceneRenderer_ShaderBuilder::GetMaterialShaderString_Diffuse(const MaterialDefinition &material) const
    {
        if (material.IsChannelDefined("diffuse"))
        {
            return ShaderLibrary::GetShaderString(material.GetChannelShaderID("diffuse").c_str());
        }

        return "vec4 Diffuse() { return vec4(0.0, 0.0, 0.0, 1.0); }";
    }

    const char* DefaultSceneRenderer_ShaderBuilder::GetMaterialShaderString_Emissive(const MaterialDefinition &material) const
    {
        if (material.IsChannelDefined("emissive"))
        {
            return ShaderLibrary::GetShaderString(material.GetChannelShaderID("emissive").c_str());
        }

        return "vec3 Emissive() { return vec3(0.0, 0.0, 0.0); }";
    }

    const char* DefaultSceneRenderer_ShaderBuilder::GetMaterialShaderString_Shininess(const MaterialDefinition &material) const
    {
        if (material.IsChannelDefined("shininess"))
        {
            return ShaderLibrary::GetShaderString(material.GetChannelShaderID("shininess").c_str());
        }

        return "float Shininess() { return 0.0; }";
    }

    const char* DefaultSceneRenderer_ShaderBuilder::GetMaterialShaderString_Specular(const MaterialDefinition &material) const
    {
        if (material.IsChannelDefined("specular"))
        {
            return ShaderLibrary::GetShaderString(material.GetChannelShaderID("specular").c_str());
        }

        return "float Specular() { return 64.0; }";
    }

    const char* DefaultSceneRenderer_ShaderBuilder::GetMaterialShaderString_Normal(const MaterialDefinition &material) const
    {
        if (material.IsChannelDefined("normal"))
        {
            return ShaderLibrary::GetShaderString(material.GetChannelShaderID("normal").c_str());
        }

        return "vec3 Normal() { return vec3(0.0, 0.0, 1.0); }";
    }

    const char* DefaultSceneRenderer_ShaderBuilder::GetMaterialShaderString_Refraction(const MaterialDefinition &material) const
    {
        if (material.IsChannelDefined("refraction"))
        {
            return ShaderLibrary::GetShaderString(material.GetChannelShaderID("refraction").c_str());
        }

        return "vec3 Refraction() { return vec3(0.0, 0.0, 1.0); }";
    }



    String DefaultSceneRenderer_ShaderBuilder::GetFullscreenPrimitiveVertexShader() const
    {
        return String
        (
            "#version 120\n"

            "attribute vec3 VertexInput_Position;\n"
            "attribute vec2 VertexInput_TexCoord;\n"

            "varying vec2 VertexOutput_TexCoord;\n"
    
            "void main()\n"
            "{\n"
            "    VertexOutput_TexCoord = VertexInput_TexCoord;\n"
            "    gl_Position           = vec4(VertexInput_Position, 1.0);\n"
            "}"
        );
    }
}