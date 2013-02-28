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


    private:

        /// A pointer to the OpenGL object.
        ShaderState_OpenGL33* stateGL;
    };
}

#endif