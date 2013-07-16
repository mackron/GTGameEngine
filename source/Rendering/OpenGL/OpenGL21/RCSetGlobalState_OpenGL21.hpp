// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL21_RCSetGlobalState_hpp_
#define __GTEngine_Rendering_OpenGL21_RCSetGlobalState_hpp_

#include <GTCore/Vector.hpp>
#include <gtgl/gtgl.h>
#include <cstdint>

#include "../../RenderCommand.hpp"
#include "ShaderState_OpenGL21.hpp"
#include "FramebufferState_OpenGL21.hpp"

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
        void SetCurrentShader(ShaderState_OpenGL21* programState);

        /// Renderer::SetCurrentFramebuffer()
        void SetCurrentFramebufferEXT(FramebufferState_OpenGL21* framebufferObject);
        void SetCurrentFramebufferARB(FramebufferState_OpenGL21* framebufferObject);


        /// Renderer::SetBlendFunction()
        void SetBlendFunction(GLenum sfactor, GLenum dfactor);

        /// Renderer::SetBlendEquation()
        void SetBlendEquation(GLenum mode);

        /// Renderer::SetBlendColour()
        void SetBlendColour(GLfloat r, GLfloat g, GLfloat b, GLfloat a);


        /// Renderer::SetAlphaTestFunction()
        void SetAlphaTestFunction(GLenum func, GLclampf ref);


        /// Renderer::SetDepthFunction()
        void SetDepthFunction(GLenum func);


        /// Renderer::SetStencilMask()
        void SetStencilMask(GLenum face, GLuint mask);

        /// Renderer::SetStencilFunc()
        void SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask);

        /// Renderer::SetStencilOp()
        void SetStencilOp(GLenum face, GLenum stencilFail, GLenum depthFail, GLenum pass);


        /// Renderer::SetFaceCulling()
        void SetFaceCulling(GLenum face);


        /// Renderer::SetPolygonMode()
        void SetPolygonMode(GLenum face, GLenum mode);

        /// Renderer::SetPolygonOffset()
        void SetPolygonOffset(GLfloat factor, GLfloat units);


        /// Renderer::SetDrawBuffers()
        void SetDrawBuffers(size_t count, const int* buffers);


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

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
#endif

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
            ShaderState_OpenGL21* programState;

        }currentShaderParams;

        /// glBindFramebuffer parameter.
        struct
        {
            FramebufferState_OpenGL21* framebufferState;

        }currentFramebufferParams;


        /// glBlendFunc parameters.
        struct
        {
            GLenum sfactor;
            GLenum dfactor;

        }blendFuncParams;

        /// glBlendEquation parameters.
        struct
        {
            GLenum mode;

        }blendEquationParams;

        /// glBlendColor parameters.
        struct
        {
            GLfloat r;
            GLfloat g;
            GLfloat b;
            GLfloat a;

        }blendColor;


        /// glAlphaFunc parameters.
        struct
        {
            GLenum   func;
            GLclampf ref;

        }alphaFuncParams;


        /// glDepthFunc parameters.
        struct
        {
            GLenum func;

        }depthFuncParams;


        /// glStencilMaskSeparate params.
        struct
        {
            GLenum face;
            GLuint mask;

        }stencilMaskParams;

        /// glStencilFuncSeparate params.
        struct
        {
            GLenum face;
            GLenum func;
            GLint  ref;
            GLuint mask;

        }stencilFuncParams;

        /// glStencilOpSeparate params.
        struct
        {
            GLenum face;
            GLenum stencilFail;
            GLenum depthFail;
            GLenum pass;

        }stencilOpParams;


        /// glCullFace params.
        struct
        {
            GLenum face;

        }cullFaceParams;


        /// glPolygonMode params.
        struct
        {
            GLenum face;
            GLenum mode;

        }polygonModeParams;

        /// glPolygonOffset params.
        struct
        {
            GLfloat factor;
            GLfloat units;

        }polygonOffsetParams;


        /// glDrawBuffers params.
        struct
        {
            GTCore::Vector<GLenum> buffers;

        }drawBuffersParams;


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

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


    private:    // No copying.
        RCSetGlobalState(const RCSetGlobalState &);
        RCSetGlobalState & operator=(const RCSetGlobalState &);
    };
}

#endif
