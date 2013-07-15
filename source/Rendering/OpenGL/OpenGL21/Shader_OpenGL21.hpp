// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_Shader_OpenGL21_hpp_
#define __GTEngine_Rendering_Shader_OpenGL21_hpp_

#include <GTEngine/Rendering/Shader.hpp>
#include <gtgl/gtgl.h>

#include "ShaderState_OpenGL21.hpp"

namespace GTEngine
{
    class Shader_OpenGL21 : public Shader
    {
    public:

        /// Constructor.
        Shader_OpenGL21(ShaderState_OpenGL21* stateGLIn, const char* vertexShader, const char* fragmentShader, const char* geometryShader);

        /// Destructor.
        ~Shader_OpenGL21();


        /// Retrieves a pointer to the OpenGL21 shader program state.
        ShaderState_OpenGL21* GetOpenGLState() const;



        //////////////////////////////////////////////
        // Uniforms.

        void SetUniform(const char* name, float x);
        void SetUniform(const char* name, float x, float y);
        void SetUniform(const char* name, float x, float y, float z);
        void SetUniform(const char* name, float x, float y, float z, float w);
        void SetUniform(const char* name, const glm::mat2 &value);
        void SetUniform(const char* name, const glm::mat3 &value);
        void SetUniform(const char* name, const glm::mat4 &value);
        void SetUniform(const char* name, const Texture2D* value);
        void SetUniform(const char* name, const TextureCube* value);


    private:

        /// A pointer to the OpenGL object.
        ShaderState_OpenGL21* stateGL;
        
        
        
    private:    // No copying.
        Shader_OpenGL21(const Shader_OpenGL21 &);
        Shader_OpenGL21 & operator=(const Shader_OpenGL21 &);
    };
}

#endif
