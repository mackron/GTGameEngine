// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_OpenGL21_RCDeleteTexture
#define GT_Rendering_OpenGL21_RCDeleteTexture

#include "../../RenderCommand.hpp"
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL21.hpp"


namespace GT
{
    /// Render command for clearing the buffers/
    class RCDeleteTexture : public RenderCommand
    {
    public:

        /// Constructor.
        RCDeleteTexture();


        /// Renderer::DeleteTexture()
        void DeleteTexture(TextureState_OpenGL21* textureStateIn);


        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A pointer to the OpenGL texture object.
        TextureState_OpenGL21* textureState;



    private:    // No copying.
        RCDeleteTexture(const RCDeleteTexture &);
        RCDeleteTexture & operator=(const RCDeleteTexture &);
    };
}

#endif
