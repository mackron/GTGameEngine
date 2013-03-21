// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetGlobalState.hpp"
#include "ServerState_OpenGL33.hpp"
#include <gtgl/gtgl.h>

#include "../CapabilityBits.hpp"
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    #define VIEWPORT_BIT                    (1 << 1)
    #define SCISSOR_BIT                     (1 << 2)
    #define CLEAR_COLOUR_BIT                (1 << 3)
    #define CLEAR_DEPTH_BIT                 (1 << 4)
    #define CLEAR_STENCIL_BIT               (1 << 5)
    #define SET_CURRENT_SHADER_BIT          (1 << 6)
    #define SET_CURRENT_FRAMEBUFFER_BIT     (1 << 7)
    #define SET_BLEND_FUNCTION_BIT          (1 << 8)
    #define SET_BLEND_EQUATION_BIT          (1 << 9)
    #define SET_BLEND_COLOUR_BIT            (1 << 10)
    #define SET_ALPHA_FUNCTION_BIT          (1 << 11)
    #define SET_DEPTH_FUNCTION_BIT          (1 << 12)
    #define SET_STENCIL_MASK_BIT            (1 << 13)
    #define SET_STENCIL_FUNC_BIT            (1 << 14)
    #define SET_STENCIL_OP_BIT              (1 << 15)
    #define SET_FACE_CULLING_BIT            (1 << 16)
    #define SET_POLYGON_MODE_BIT            (1 << 17)
    #define SET_POLYGON_OFFSET_BIT          (1 << 18)
    #define SET_DRAW_BUFFERS_BIT            (1 << 19)
    #define ENABLE_BIT                      (1 << 20)
    #define DISABLE_BIT                     (1 << 21)


    /// Just a helper function so we can set a break point for when a redundancy message is received.
    void PostRedundancyMessage(const char* message)
    {
        GTEngine::Log(message);
    }

#ifdef NDEBUG
    #define CHECK_REDUNDANCY(settingBit, message)
    #define CHECK_ENABLE_REDUNDANCY(settingBit, message)
    #define CHECK_DISABLE_REDUNDANCY(settingBit, message)
#else
    #define CHECK_REDUNDANCY(settingBit, message) \
        if ((this->operationBitfield & settingBit)) \
        { \
            PostRedundancyMessage(message); \
        }

    #define CHECK_ENABLE_REDUNDANCY(settingBit, message) \
        if ((this->enableParams.caps & settingBit)) \
        { \
            PostRedundancyMessage(message); \
        }

    #define CHECK_DISABLE_REDUNDANCY(settingBit, message) \
        if ((this->disableParams.caps & settingBit)) \
        { \
            PostRedundancyMessage(message); \
        }
#endif
        
    


    RCSetGlobalState::RCSetGlobalState()
        : operationBitfield(0),
          viewportParams(), scissorParams(),
          clearColorParams(), clearDepthParams(), clearStencilParams(),
          currentShaderParams(), currentFramebufferParams(),
          blendFuncParams(), blendEquationParams(),
          alphaFuncParams(),
          depthFuncParams(),
          stencilMaskParams(), stencilFuncParams(), stencilOpParams(),
          cullFaceParams(),
          polygonModeParams(), polygonOffsetParams(),
          drawBuffersParams(),
          enableParams(), disableParams()
    {
        this->enableParams.caps  = 0;
        this->disableParams.caps = 0;
    }

    void RCSetGlobalState::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        CHECK_REDUNDANCY(VIEWPORT_BIT, "Warning: Renderer: Redundant call to SetViewport().");


        this->viewportParams.x      = x;
        this->viewportParams.y      = y;
        this->viewportParams.width  = width;
        this->viewportParams.height = height;

        this->operationBitfield |= VIEWPORT_BIT;
    }

    void RCSetGlobalState::SetScissor(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        CHECK_REDUNDANCY(SCISSOR_BIT, "Warning: Renderer: Redundant call to SetScissor().");


        this->scissorParams.x      = x;
        this->scissorParams.y      = y;
        this->scissorParams.width  = width;
        this->scissorParams.height = height;

        this->operationBitfield |= SCISSOR_BIT;
    }

    
    void RCSetGlobalState::SetClearColour(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        CHECK_REDUNDANCY(CLEAR_COLOUR_BIT, "Warning: Renderer: Redundant call to SetClearColour().");


        this->clearColorParams.r = r;
        this->clearColorParams.g = g;
        this->clearColorParams.b = b;
        this->clearColorParams.a = a;

        this->operationBitfield |= CLEAR_COLOUR_BIT;
    }

    void RCSetGlobalState::SetClearDepth(GLfloat depth)
    {
        CHECK_REDUNDANCY(CLEAR_DEPTH_BIT, "Warning: Renderer: Redundant call to SetClearDepth().");


        this->clearDepthParams.depth = depth;

        this->operationBitfield |= CLEAR_DEPTH_BIT;
    }

    void RCSetGlobalState::SetClearStencil(GLint stencil)
    {
        CHECK_REDUNDANCY(CLEAR_STENCIL_BIT, "Warning: Renderer: Redundant call to SetClearStencil().");


        this->clearStencilParams.stencil = stencil;

        this->operationBitfield |= CLEAR_STENCIL_BIT;
    }


    void RCSetGlobalState::SetCurrentShader(ShaderState_OpenGL33* programState)
    {
        CHECK_REDUNDANCY(SET_CURRENT_SHADER_BIT, "Warning: Renderer: Redundant call to SetCurrentShader().");


        this->currentShaderParams.programState = programState;

        this->operationBitfield |= SET_CURRENT_SHADER_BIT;
    }

    void RCSetGlobalState::SetCurrentFramebuffer(FramebufferState_OpenGL33* framebufferState)
    {
        CHECK_REDUNDANCY(SET_CURRENT_FRAMEBUFFER_BIT, "Warning: Renderer: Redundant call to SetCurrentFramebuffer().");


        this->currentFramebufferParams.framebufferState = framebufferState;

        this->operationBitfield |= SET_CURRENT_FRAMEBUFFER_BIT;
    }


    void RCSetGlobalState::SetBlendFunction(GLenum sfactor, GLenum dfactor)
    {
        CHECK_REDUNDANCY(SET_BLEND_FUNCTION_BIT, "Warning: Renderer: Redundant call to SetBlendFunction().");


        this->blendFuncParams.sfactor = sfactor;
        this->blendFuncParams.dfactor = dfactor;

        this->operationBitfield |= SET_BLEND_FUNCTION_BIT;
    }

    void RCSetGlobalState::SetBlendEquation(GLenum mode)
    {
        CHECK_REDUNDANCY(SET_BLEND_EQUATION_BIT, "Warning: Renderer: Redundant call to SetBlendEquation().");


        this->blendEquationParams.mode = mode;

        this->operationBitfield |= SET_BLEND_EQUATION_BIT;
    }

    void RCSetGlobalState::SetBlendColour(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        CHECK_REDUNDANCY(SET_BLEND_COLOUR_BIT, "Warning: Renderer: Redundant call to SetBlendColour().");


        this->blendColor.r = r;
        this->blendColor.g = g;
        this->blendColor.b = b;
        this->blendColor.a = a;

        this->operationBitfield |= SET_BLEND_COLOUR_BIT;
    }

    
    void RCSetGlobalState::SetAlphaTestFunction(GLenum func, GLclampf ref)
    {
        CHECK_REDUNDANCY(SET_ALPHA_FUNCTION_BIT, "Warning: Renderer: Redundant call to SetAlphaTestFunction().");


        this->alphaFuncParams.func = func;
        this->alphaFuncParams.ref  = ref;

        this->operationBitfield |= SET_ALPHA_FUNCTION_BIT;
    }


    void RCSetGlobalState::SetDepthFunction(GLenum func)
    {
        CHECK_REDUNDANCY(SET_DEPTH_FUNCTION_BIT, "Warning: Renderer: Redundant call to SetDepthFunction().");


        this->depthFuncParams.func = func;

        this->operationBitfield |= SET_DEPTH_FUNCTION_BIT;
    }


    void RCSetGlobalState::SetStencilMask(GLenum face, GLuint mask)
    {
        CHECK_REDUNDANCY(SET_STENCIL_MASK_BIT, "Warning: Renderer: Redundant call to SetStencilMask().");


        this->stencilMaskParams.face = face;
        this->stencilMaskParams.mask = mask;

        this->operationBitfield |= SET_STENCIL_MASK_BIT;
    }

    void RCSetGlobalState::SetStencilFunc(GLenum face, GLenum func, GLint ref, GLuint mask)
    {
        CHECK_REDUNDANCY(SET_STENCIL_FUNC_BIT, "Warning: Renderer: Redundant call to SetStencilFunc().");


        this->stencilFuncParams.face = face;
        this->stencilFuncParams.func = func;
        this->stencilFuncParams.ref  = ref;
        this->stencilFuncParams.mask = mask;

        this->operationBitfield |= SET_STENCIL_FUNC_BIT;
    }

    void RCSetGlobalState::SetStencilOp(GLenum face, GLenum stencilFail, GLenum depthFail, GLenum pass)
    {
        CHECK_REDUNDANCY(SET_STENCIL_OP_BIT, "Warning: Renderer: Redundant call to SetStencilOp().");


        this->stencilOpParams.face        = face;
        this->stencilOpParams.stencilFail = stencilFail;
        this->stencilOpParams.depthFail   = depthFail;
        this->stencilOpParams.pass        = pass;

        this->operationBitfield |= SET_STENCIL_OP_BIT;
    }



    void RCSetGlobalState::SetFaceCulling(GLenum face)
    {
        CHECK_REDUNDANCY(SET_FACE_CULLING_BIT, "Warning: Renderer: Redundant call to SetFaceCullingBit().");


        this->cullFaceParams.face = face;

        this->operationBitfield |= SET_FACE_CULLING_BIT;
    }


    void RCSetGlobalState::SetPolygonMode(GLenum face, GLenum mode)
    {
        CHECK_REDUNDANCY(SET_POLYGON_MODE_BIT, "Warning: Renderer: Redundant call to SetPolygonMode().");


        this->polygonModeParams.face = face;
        this->polygonModeParams.mode = mode;

        this->operationBitfield |= SET_POLYGON_MODE_BIT;
    }

    void RCSetGlobalState::SetPolygonOffset(GLfloat factor, GLfloat units)
    {
        CHECK_REDUNDANCY(SET_POLYGON_OFFSET_BIT, "Warning: Renderer: Redundant call to SetPolygonOffset().");


        this->polygonOffsetParams.factor = factor;
        this->polygonOffsetParams.units  = units;

        this->operationBitfield |= SET_POLYGON_OFFSET_BIT;
    }


    void RCSetGlobalState::SetDrawBuffers(size_t count, int* buffers)
    {
        CHECK_REDUNDANCY(SCISSOR_BIT, "Warning: Renderer: Redundant call to SetDrawBuffers().");


        this->drawBuffersParams.buffers.Clear();

        for (size_t i = 0; i < count; ++i)
        {
            this->drawBuffersParams.buffers.PushBack(GL_COLOR_ATTACHMENT0_EXT + buffers[i]);
        }

        this->operationBitfield |= SET_DRAW_BUFFERS_BIT;
    }


    void RCSetGlobalState::Enable(GLenum cap)
    {
        if (cap == GL_SCISSOR_TEST)
        {
            CHECK_ENABLE_REDUNDANCY(SCISSOR_TEST_BIT, "Warning: Renderer: Redundant call to Enable(GL_SCISSOR_TEST).");
            this->enableParams.caps  |=  SCISSOR_TEST_BIT;
            this->disableParams.caps &= ~SCISSOR_TEST_BIT;
        }
        else if (cap == GL_BLEND)
        {
            CHECK_ENABLE_REDUNDANCY(BLENDING_BIT, "Warning: Renderer: Redundant call to Enable(GL_BLEND).");
            this->enableParams.caps  |=  BLENDING_BIT;
            this->disableParams.caps &= ~BLENDING_BIT;
        }
        else if (cap == GL_ALPHA_TEST)
        {
            CHECK_ENABLE_REDUNDANCY(ALPHA_TEST_BIT, "Warning: Renderer: Redundant call to Enable(GL_ALPHA_TEST).");
            this->enableParams.caps  |=  ALPHA_TEST_BIT;
            this->disableParams.caps &= ~ALPHA_TEST_BIT;
        }
        else if (cap == GL_DEPTH_TEST)
        {
            CHECK_ENABLE_REDUNDANCY(DEPTH_TEST_BIT, "Warning: Renderer: Redundant call to Enable(GL_DEPTH_TEST).");
            this->enableParams.caps  |=  DEPTH_TEST_BIT;
            this->disableParams.caps &= ~DEPTH_TEST_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_FILL)
        {
            CHECK_ENABLE_REDUNDANCY(POLYGON_OFFSET_FILL_BIT, "Warning: Renderer: Redundant call to Enable(GL_POLYGON_OFFSET_FILL).");
            this->enableParams.caps  |=  POLYGON_OFFSET_FILL_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_FILL_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_LINE)
        {
            CHECK_ENABLE_REDUNDANCY(POLYGON_OFFSET_LINE_BIT, "Warning: Renderer: Redundant call to Enable(GL_POLYGON_OFFSET_LINE).");
            this->enableParams.caps  |=  POLYGON_OFFSET_LINE_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_LINE_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_POINT)
        {
            CHECK_ENABLE_REDUNDANCY(POLYGON_OFFSET_POINT_BIT, "Warning: Renderer: Redundant call to Enable(GL_POLYGON_OFFSET_POINT).");
            this->enableParams.caps  |=  POLYGON_OFFSET_POINT_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_POINT_BIT;
        }
        else if (cap == GL_CULL_FACE)
        {
            CHECK_ENABLE_REDUNDANCY(FACE_CULLING_BIT, "Warning: Renderer: Redundant call to Enable(GL_CULL_FACE).");
            this->enableParams.caps  |=  FACE_CULLING_BIT;
            this->disableParams.caps &= ~FACE_CULLING_BIT;
        }

        this->operationBitfield |= ENABLE_BIT;
    }

    void RCSetGlobalState::Disable(GLenum cap)
    {
        if (cap == GL_SCISSOR_TEST)
        {
            CHECK_DISABLE_REDUNDANCY(SCISSOR_TEST_BIT, "Warning: Renderer: Redundant call to Disable(GL_SCISSOR_TEST).");
            this->disableParams.caps |=  SCISSOR_TEST_BIT;
            this->enableParams.caps  &= ~SCISSOR_TEST_BIT;
        }
        else if (cap == GL_BLEND)
        {
            CHECK_DISABLE_REDUNDANCY(BLENDING_BIT, "Warning: Renderer: Redundant call to Disable(GL_BLEND).");
            this->disableParams.caps |=  BLENDING_BIT;
            this->enableParams.caps  &= ~BLENDING_BIT;
        }
        else if (cap == GL_ALPHA_TEST)
        {
            CHECK_DISABLE_REDUNDANCY(ALPHA_TEST_BIT, "Warning: Renderer: Redundant call to Disable(GL_ALPHA_TEST).");
            this->disableParams.caps |=  ALPHA_TEST_BIT;
            this->enableParams.caps  &= ~ALPHA_TEST_BIT;
        }
        else if (cap == GL_DEPTH_TEST)
        {
            CHECK_DISABLE_REDUNDANCY(DEPTH_TEST_BIT, "Warning: Renderer: Redundant call to Disable(GL_DEPTH_TEST).");
            this->disableParams.caps |=  DEPTH_TEST_BIT;
            this->enableParams.caps  &= ~DEPTH_TEST_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_FILL)
        {
            CHECK_DISABLE_REDUNDANCY(POLYGON_OFFSET_FILL_BIT, "Warning: Renderer: Redundant call to Disable(GL_POLYGON_OFFSET_FILL).");
            this->disableParams.caps |=  POLYGON_OFFSET_FILL_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_FILL_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_LINE)
        {
            CHECK_DISABLE_REDUNDANCY(POLYGON_OFFSET_LINE_BIT, "Warning: Renderer: Redundant call to Disable(GL_POLYGON_OFFSET_LINE).");
            this->disableParams.caps |=  POLYGON_OFFSET_LINE_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_LINE_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_POINT)
        {
            CHECK_DISABLE_REDUNDANCY(POLYGON_OFFSET_POINT_BIT, "Warning: Renderer: Redundant call to Disable(GL_POLYGON_OFFSET_POINT).");
            this->disableParams.caps |=  POLYGON_OFFSET_POINT_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_POINT_BIT;
        }
        else if (cap == GL_CULL_FACE)
        {
            CHECK_DISABLE_REDUNDANCY(FACE_CULLING_BIT, "Warning: Renderer: Redundant call to Disable(GL_CULL_FACE).");
            this->disableParams.caps |=  FACE_CULLING_BIT;
            this->enableParams.caps  &= ~FACE_CULLING_BIT;
        }

        this->operationBitfield |= DISABLE_BIT;
    }

    void RCSetGlobalState::EnableColourWrites()
    {
        CHECK_ENABLE_REDUNDANCY(COLOUR_WRITES_BIT, "Warning: Renderer: Redundant call to EnableColourWrites().");
        this->enableParams.caps  |=  COLOUR_WRITES_BIT;
        this->disableParams.caps &= ~COLOUR_WRITES_BIT;

        this->operationBitfield |= ENABLE_BIT;
    }

    void RCSetGlobalState::DisableColourWrites()
    {
        CHECK_DISABLE_REDUNDANCY(COLOUR_WRITES_BIT, "Warning: Renderer: Redundant call to DisableColourWrites().");
        this->disableParams.caps |=  COLOUR_WRITES_BIT;
        this->enableParams.caps  &= ~COLOUR_WRITES_BIT;

        this->operationBitfield |= DISABLE_BIT;
    }

    void RCSetGlobalState::EnableDepthWrites()
    {
        CHECK_ENABLE_REDUNDANCY(DEPTH_WRITES_BIT, "Warning: Renderer: Redundant call to EnableDepthWrites().");
        this->enableParams.caps  |=  DEPTH_WRITES_BIT;
        this->disableParams.caps &= ~DEPTH_WRITES_BIT;

        this->operationBitfield |= ENABLE_BIT;
    }

    void RCSetGlobalState::DisableDepthWrites()
    {
        CHECK_DISABLE_REDUNDANCY(DEPTH_WRITES_BIT, "Warning: Renderer: Redundant call to DisableDepthWrites().");
        this->disableParams.caps |=  DEPTH_WRITES_BIT;
        this->enableParams.caps  &= ~DEPTH_WRITES_BIT;

        this->operationBitfield |= DISABLE_BIT;
    }

    
    void RCSetGlobalState::Execute()
    {
        if ((this->operationBitfield & VIEWPORT_BIT))
        {
            glViewport(this->viewportParams.x, this->viewportParams.y, this->viewportParams.width, this->viewportParams.height);
        }

        if ((this->operationBitfield & SCISSOR_BIT))
        {
            glScissor(this->scissorParams.x, this->scissorParams.y, this->scissorParams.width, this->scissorParams.height);
        }

        if ((this->operationBitfield & CLEAR_COLOUR_BIT))
        {
            glClearColor(this->clearColorParams.r, this->clearColorParams.g, this->clearColorParams.b, this->clearColorParams.a);
        }

        if ((this->operationBitfield & CLEAR_DEPTH_BIT))
        {
            glClearDepth(this->clearDepthParams.depth);
        }

        if ((this->operationBitfield & CLEAR_STENCIL_BIT))
        {
            glClearStencil(this->clearStencilParams.stencil);
        }


        if ((this->operationBitfield & SET_CURRENT_SHADER_BIT))
        {
            // 1) Bind the program.
            glUseProgram(this->currentShaderParams.programState->programObject);

            // 2) Bind textures.
            auto &textures = this->currentShaderParams.programState->textures;
            for (size_t i = 0; i < textures.count; ++i)
            {
                auto &texture = textures.buffer[i]->value;

                glActiveTexture(GL_TEXTURE0 + texture.textureUnit);
                glBindTexture(texture.textureTarget, *texture.textureObject);

                // State needs to be set.
                if (texture.textureTarget == GL_TEXTURE_1D)
                {
                    ServerState_GL_TEXTURE_BINDING_1D = *texture.textureObject;
                }
                else if (texture.textureTarget == GL_TEXTURE_2D)
                {
                    ServerState_GL_TEXTURE_BINDING_2D = *texture.textureObject;
                }
                else if (texture.textureTarget == GL_TEXTURE_3D)
                {
                    ServerState_GL_TEXTURE_BINDING_3D = *texture.textureObject;
                }
                else if (texture.textureTarget == GL_TEXTURE_CUBE_MAP)
                {
                    ServerState_GL_TEXTURE_BINDING_CUBE = *texture.textureObject;
                }
            }


            // 3) Set state.
            ServerState_GL_CURRENT_PROGRAM = this->currentShaderParams.programState->programObject;
        }

        if ((this->operationBitfield & SET_CURRENT_FRAMEBUFFER_BIT))
        {
            if (this->currentFramebufferParams.framebufferState != nullptr)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, this->currentFramebufferParams.framebufferState->framebufferObject);
                ServerState_GL_FRAMEBUFFER_BINDING = this->currentFramebufferParams.framebufferState->framebufferObject;
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                GLenum backBuffer = GL_BACK;
                glDrawBuffers(1, &backBuffer);

                ServerState_GL_FRAMEBUFFER_BINDING = 0;
            }
        }


        if ((this->operationBitfield & SET_BLEND_FUNCTION_BIT))
        {
            glBlendFunc(this->blendFuncParams.sfactor, this->blendFuncParams.dfactor);
        }

        if ((this->operationBitfield & SET_BLEND_EQUATION_BIT))
        {
            glBlendEquation(this->blendEquationParams.mode);
        }

        if ((this->operationBitfield & SET_BLEND_COLOUR_BIT))
        {
            glBlendColor(this->blendColor.r, this->blendColor.g, this->blendColor.b, this->blendColor.a);
        }


        if ((this->operationBitfield & SET_ALPHA_FUNCTION_BIT))
        {
            glAlphaFunc(this->alphaFuncParams.func, this->alphaFuncParams.ref);
        }


        if ((this->operationBitfield & SET_DEPTH_FUNCTION_BIT))
        {
            glDepthFunc(this->depthFuncParams.func);
        }


        if ((this->operationBitfield & SET_STENCIL_MASK_BIT))
        {
            glStencilMaskSeparate(this->stencilMaskParams.face, this->stencilMaskParams.mask);
        }

        if ((this->operationBitfield & SET_STENCIL_FUNC_BIT))
        {
            glStencilFuncSeparate(this->stencilFuncParams.face, this->stencilFuncParams.func, this->stencilFuncParams.ref, this->stencilFuncParams.mask);
        }

        if ((this->operationBitfield & SET_STENCIL_OP_BIT))
        {
            glStencilOpSeparate(this->stencilOpParams.face, this->stencilOpParams.stencilFail, this->stencilOpParams.depthFail, this->stencilOpParams.pass);
        }


        if ((this->operationBitfield & SET_FACE_CULLING_BIT))
        {
            if (this->cullFaceParams.face == GL_NONE)
            {
                glDisable(GL_CULL_FACE);
            }
            else
            {
                glEnable(GL_CULL_FACE);
                glCullFace(this->cullFaceParams.face);
            }
        }


        if ((this->operationBitfield & SET_POLYGON_MODE_BIT))
        {
            glPolygonMode(this->polygonModeParams.face, this->polygonModeParams.mode);
        }

        if ((this->operationBitfield & SET_POLYGON_OFFSET_BIT))
        {
            glPolygonOffset(this->polygonOffsetParams.factor, this->polygonOffsetParams.units);
        }


        if ((this->operationBitfield & SET_DRAW_BUFFERS_BIT))
        {
            glDrawBuffers(static_cast<GLsizei>(this->drawBuffersParams.buffers.count), this->drawBuffersParams.buffers.buffer);
        }


        if ((this->operationBitfield & ENABLE_BIT))
        {
            if ((this->enableParams.caps & SCISSOR_TEST_BIT))
            {
                glEnable(GL_SCISSOR_TEST);
            }
            if ((this->enableParams.caps & BLENDING_BIT))
            {
                glEnable(GL_BLEND);
            }
            if ((this->enableParams.caps & ALPHA_TEST_BIT))
            {
                glEnable(GL_ALPHA_TEST);
            }
            if ((this->enableParams.caps & COLOUR_WRITES_BIT))
            {
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            }
            if ((this->enableParams.caps & DEPTH_WRITES_BIT))
            {
                glDepthMask(GL_TRUE);
            }
            if ((this->enableParams.caps & DEPTH_TEST_BIT))
            {
                glEnable(GL_DEPTH_TEST);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_FILL_BIT))
            {
                glEnable(GL_POLYGON_OFFSET_FILL);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_LINE_BIT))
            {
                glEnable(GL_POLYGON_OFFSET_LINE);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_POINT_BIT))
            {
                glEnable(GL_POLYGON_OFFSET_POINT);
            }
            if ((this->enableParams.caps & FACE_CULLING_BIT))
            {
                glEnable(GL_CULL_FACE);
            }
        }

        if ((this->operationBitfield & DISABLE_BIT))
        {
            if ((this->disableParams.caps & SCISSOR_TEST_BIT))
            {
                glDisable(GL_SCISSOR_TEST);
            }
            if ((this->disableParams.caps & BLENDING_BIT))
            {
                glDisable(GL_BLEND);
            }
            if ((this->disableParams.caps & ALPHA_TEST_BIT))
            {
                glDisable(GL_ALPHA_TEST);
            }
            if ((this->disableParams.caps & COLOUR_WRITES_BIT))
            {
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            }
            if ((this->disableParams.caps & DEPTH_WRITES_BIT))
            {
                glDepthMask(GL_FALSE);
            }
            if ((this->disableParams.caps & DEPTH_TEST_BIT))
            {
                glDisable(GL_DEPTH_TEST);
            }
            if ((this->disableParams.caps & POLYGON_OFFSET_FILL_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
            if ((this->disableParams.caps & POLYGON_OFFSET_LINE_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_LINE);
            }
            if ((this->disableParams.caps & POLYGON_OFFSET_POINT_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_POINT);
            }
            if ((this->disableParams.caps & FACE_CULLING_BIT))
            {
                glDisable(GL_CULL_FACE);
            }
        }
    }
}
