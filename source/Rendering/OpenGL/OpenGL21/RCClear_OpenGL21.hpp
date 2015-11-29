// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_OpenGL21_RCClear
#define GT_Rendering_OpenGL21_RCClear

#include "../../RenderCommand.hpp"
#include <gtgl/gtgl.h>

namespace GT
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
