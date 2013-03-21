// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_Shader_OpenGL33_hpp_
#define __GTEngine_Rendering_Shader_OpenGL33_hpp_

#include <GTEngine/Rendering/Shader.hpp>
#include <gtgl/gtgl.h>

#include "ShaderState_OpenGL33.hpp"

namespace GTEngine
{
    class Shader_OpenGL33 : public Shader
    {
    public:

        /// Constructor.
        Shader_OpenGL33(ShaderState_OpenGL33* stateGLIn, const char* vertexShader, const char* fragmentShader, const char* geometryShader);

        /// Destructor.
        ~Shader_OpenGL33();


        /// Retrieves a pointer to the OpenGL33 shader program state.
        ShaderState_OpenGL33* GetOpenGLState() const;



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
        ShaderState_OpenGL33* stateGL;
        
        
        
    private:    // No copying.
        Shader_OpenGL33(const Shader_OpenGL33 &);
        Shader_OpenGL33 & operator=(const Shader_OpenGL33 &);
    };
}

#endif
