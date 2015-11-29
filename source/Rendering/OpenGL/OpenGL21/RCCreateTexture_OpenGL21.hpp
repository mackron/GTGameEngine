// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_OpenGL21_RCCreateTexture_hpp_
#define __GTEngine_Rendering_OpenGL21_RCCreateTexture_hpp_

#include "../../RenderCommand.hpp"
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL21.hpp"


namespace GT
{
    /// Render command for clearing the buffers/
    class RCCreateTexture : public RenderCommand
    {
    public:

        /// Constructor.
        RCCreateTexture();


        /// Renderer::CreateVertexArray()
        void CreateTexture(TextureState_OpenGL21* textureStateIn);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL texture object.
        TextureState_OpenGL21* textureState;



    private:    // No copying.
        RCCreateTexture(const RCCreateTexture &);
        RCCreateTexture & operator=(const RCCreateTexture &);
    };
}

#endif
