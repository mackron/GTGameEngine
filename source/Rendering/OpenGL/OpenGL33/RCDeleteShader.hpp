// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCDeleteShader_hpp_
#define __GTEngine_Rendering_OpenGL33_RCDeleteShader_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCDeleteShader : public RenderCommand
    {
    public:

        /// Constructor.
        RCDeleteShader();


        /// Renderer::DeleteShader()
        void DeleteShader(GLuint* programObject);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL shader program object.
        GLuint* programObject;



    private:    // No copying.
        RCDeleteShader(const RCDeleteShader &);
        RCDeleteShader & operator=(const RCDeleteShader &);
    };
}

#endif
