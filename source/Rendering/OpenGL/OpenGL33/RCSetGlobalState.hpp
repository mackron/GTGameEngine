// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL33_RCSetGlobalState_hpp_
#define __GTEngine_Rendering_OpenGL33_RCSetGlobalState_hpp_

#include <GTEngine/Rendering/RenderCommand.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

#include "ShaderState_OpenGL33.hpp"

namespace GTEngine
{
    /// Render command for clearing the buffers/
    class RCSetGlobalState : public RenderCommand
    {
    public:

        /// Constructor.
        RCSetGlobalState();


        /// Renderer::SetViewport()
        void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height);

        /// Renderer::SetScissor()
        void SetScissor(GLint x, GLint y, GLsizei width, GLsizei height);


        /// Renderer::SetClearColour()
        void SetClearColour(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

        /// Renderer::SetClearDepth()
        void SetClearDepth(GLfloat depth);

        /// Renderer::SetClearStencil()
        void SetClearStencil(GLint stencil);


        /// Renderer::SetCurrentShader()
        void SetCurrentShader(ShaderState_OpenGL33 *programState);


        /// Renderer::Enable*()
        void Enable(GLenum cap);

        /// Renderer::Disable*()
        void Disable(GLenum cap);

        /// Renderer::EnableColourWrites()
        void EnableColourWrites();

        /// Renderer::DisableColourWrites()
        void DisableColourWrites();

        /// Renderer::EnableDepthWrites()
        void EnableDepthWrites();

        /// Renderer::DisableDepthWrites()
        void DisableDepthWrites();



        /// RenderCommand::Execute()
        void Execute();


    private:

        /// A bitfield containing the state operations to call.
        uint32_t operationBitfield;

        /// glViewport parameters.
        struct
        {
            GLint x;
            GLint y;
            GLsizei width;
            GLsizei height;

        }viewportParams;

        /// glScissor parameters.
        struct
        {
            GLint x;
            GLint y;
            GLsizei width;
            GLsizei height;

        }scissorParams;


        /// glClearColor parameters.
        struct
        {
            GLfloat r;
            GLfloat g;
            GLfloat b;
            GLfloat a;

        }clearColorParams;

        /// glClearDepth parameters.
        struct
        {
            GLfloat depth;

        }clearDepthParams;

        /// glClearStencil parameters.
        struct
        {
            GLint stencil;

        }clearStencilParams;


        /// glUseProgram parameters
        struct
        {
            ShaderState_OpenGL33* programState;

        }currentShaderParams;


        /// glEnable caps.
        struct
        {
            uint32_t caps;

        }enableParams;

        /// glDisable caps.
        struct
        {
            uint32_t caps;

        }disableParams;





    private:    // No copying.
        RCSetGlobalState(const RCSetGlobalState &);
        RCSetGlobalState & operator=(const RCSetGlobalState &);
    };
}

#endif
