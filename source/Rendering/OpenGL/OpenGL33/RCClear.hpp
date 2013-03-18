// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCClear_hpp_
#define __GTEngine_Rendering_OpenGL33_RCClear_hpp_

#include "../../RenderCommand.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCClear : public RenderCommand
    {
    public:

        /// Constructor.
        RCClear();

        /// Renderer::Clear()
        void Clear(GLbitfield bufferMaskIn);



        /// RenderCommand::Execute()
        void Execute();


    private:

        /// The buffer mask to use when identifying which buffers to clear.
        GLbitfield bufferMask;


    private:    // No copying.
        RCClear(const RCClear &);
        RCClear & operator=(const RCClear &);
    };
}

#endif
