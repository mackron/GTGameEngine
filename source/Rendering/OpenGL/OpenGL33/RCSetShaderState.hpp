// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCSetShaderState_hpp_
#define __GTEngine_Rendering_OpenGL33_RCSetShaderState_hpp_

#include <GTEngine/ShaderParameterCache.hpp>
#include <GTCore/Dictionary.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

#include "../../RenderCommand.hpp"
#include "ShaderState_OpenGL33.hpp"

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCSetShaderState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetShaderState();


        /// Renderer::SetShaderParameter().
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, float x);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, float x, float y);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, float x, float y, float z);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, float x, float y, float z, float w);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, int x);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, int x, int y);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, int x, int y, int z);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, int x, int y, int z, int w);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::mat2x2 &value);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::mat3x3 &value);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::mat4x4 &value);
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, GLuint* textureObject, GLenum textureTarget);

        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::vec2 &value)  { this->SetShaderParameter(programStateIn, name, value.x, value.y); }
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::vec3 &value)  { this->SetShaderParameter(programStateIn, name, value.x, value.y, value.z); }
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::vec4 &value)  { this->SetShaderParameter(programStateIn, name, value.x, value.y, value.z, value.w); }
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::ivec2 &value) { this->SetShaderParameter(programStateIn, name, value.x, value.y); }
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::ivec3 &value) { this->SetShaderParameter(programStateIn, name, value.x, value.y, value.z); }
        void SetShaderParameter(ShaderState_OpenGL33* programStateIn, const char* name, const glm::ivec4 &value) { this->SetShaderParameter(programStateIn, name, value.x, value.y, value.z, value.w); }



        /// Retrieves the program object whose state is being set.
        ShaderState_OpenGL33* GetProgramState() const;



        /// RenderCommand::Execute()
        void Execute();



    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// The shader state being modified.
        ShaderState_OpenGL33* programState;

        /// The shader parameter cache for basic types. Textures are handled separately.
        ShaderParameterCache basicParametersToSet;

        /// The texture texture parameters to set, keyed by name.
        GTCore::Dictionary<ShaderState_OpenGL33::TextureParameter> textureParametersToSet;



    private:    // No copying.
        RCSetShaderState(const RCSetShaderState &);
        RCSetShaderState & operator=(const RCSetShaderState &);
    };
}

#endif
