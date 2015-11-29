// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_OpenGL21_RCDeleteShader_hpp_
#define __GTEngine_Rendering_OpenGL21_RCDeleteShader_hpp_

#include <gtgl/gtgl.h>

#include "../../RenderCommand.hpp"
#include "ShaderState_OpenGL21.hpp"

namespace GT
{
    /// Render command for clearing the buffers/
    class RCDeleteShader : public RenderCommand
    {
    public:

        /// Constructor.
        RCDeleteShader();


        /// Renderer::DeleteShader()
        void DeleteShader(ShaderState_OpenGL21* programState);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL shader program object.
        ShaderState_OpenGL21* programState;



    private:    // No copying.
        RCDeleteShader(const RCDeleteShader &);
        RCDeleteShader & operator=(const RCDeleteShader &);
    };
}

#endif
