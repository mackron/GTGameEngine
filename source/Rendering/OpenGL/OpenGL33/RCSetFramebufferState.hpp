// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCSetFramebufferState_hpp_
#define __GTEngine_Rendering_OpenGL33_RCSetFramebufferState_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCSetFramebufferState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetFramebufferState();



        /// Retrieves the program object whose state is being set.
        GLuint* GetFramebufferObject();



        /// RenderCommand::Execute()
        void Execute();



    private:

        /// A bitfield containing information about the operations to perform.
        uint32_t operationBitfield;


        /// A pointer to the OpenGL framebuffer object.
        GLuint* framebufferObject;



    private:    // No copying.
        RCSetFramebufferState(const RCSetFramebufferState &);
        RCSetFramebufferState & operator=(const RCSetFramebufferState &);
    };
}

#endif
