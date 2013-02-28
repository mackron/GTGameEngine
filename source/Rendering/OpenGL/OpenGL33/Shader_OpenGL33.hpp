// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_Shader_OpenGL33_hpp_
#define __GTEngine_Rendering_Shader_OpenGL33_hpp_

#include <GTEngine/Rendering/Shader.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    class Shader_OpenGL33 : public Shader
    {
    public:

        /// Constructor.
        Shader_OpenGL33(GLuint* objectGLIn, const char* vertexShader, const char* fragmentShader, const char* geometryShader);

        /// Destructor.
        ~Shader_OpenGL33();


        /// Retrieves a pointer to the OpenGL program object.
        GLuint* GetOpenGLObjectPtr() const;


    private:

        /// A pointer to the OpenGL object.
        GLuint* objectGL;
    };
}

#endif