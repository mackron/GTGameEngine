// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCDeleteTexture_hpp_
#define __GTEngine_Rendering_OpenGL33_RCDeleteTexture_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCDeleteTexture : public RenderCommand
    {
    public:

        /// Constructor.
        RCDeleteTexture();


        /// Renderer::DeleteTexture()
        void DeleteTexture(GLuint* textureObject);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL texture object.
        GLuint* textureObject;



    private:    // No copying.
        RCDeleteTexture(const RCDeleteTexture &);
        RCDeleteTexture & operator=(const RCDeleteTexture &);
    };
}

#endif
