
#include <GTEngine/ShaderParameter.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

#define GTENGINE_SHADERPARAMETER_IMPL(typeName) \
    void typeName::SetOnCurrentShader(const char* name) const \
    { \
        Renderer::SetShaderParameter(name, this->value); \
    } \


namespace GTEngine
{
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Float);
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Float2);
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Float3);
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Float4);
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Float2x2);
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Float3x3);
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Float4x4);
    //GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Texture1D);
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Texture2D);
    //GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_Texture3D);
    GTENGINE_SHADERPARAMETER_IMPL(ShaderParameter_TextureCube);
}
