// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCCreateTexture_hpp_
#define __GTEngine_Rendering_OpenGL33_RCCreateTexture_hpp_

#include "../../RenderCommand.hpp"
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL33.hpp"


namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCCreateTexture : public RenderCommand
    {
    public:

        /// Constructor.
        RCCreateTexture();


        /// Renderer::CreateVertexArray()
        void CreateTexture(TextureState_OpenGL33* textureStateIn);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL texture object.
        TextureState_OpenGL33* textureState;



    private:    // No copying.
        RCCreateTexture(const RCCreateTexture &);
        RCCreateTexture & operator=(const RCCreateTexture &);
    };
}

#endif
