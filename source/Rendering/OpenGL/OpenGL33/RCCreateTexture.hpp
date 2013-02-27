// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCCreateTexture_hpp_
#define __GTEngine_Rendering_OpenGL33_RCCreateTexture_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCCreateTexture : public RenderCommand
    {
    public:

        /// Constructor.
        RCCreateTexture();


        /// Renderer::CreateVertexArray()
        void CreateTexture(GLuint* textureObjectIn);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL texture object.
        GLuint* textureObject;



    private:    // No copying.
        RCCreateTexture(const RCCreateTexture &);
        RCCreateTexture & operator=(const RCCreateTexture &);
    };
}

#endif
