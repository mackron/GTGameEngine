// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetGlobalState.hpp"
#include <gtgl/gtgl.h>

#include "../CapabilityBits.hpp"

namespace GTEngine
{
    #define VIEWPORT_BIT        (1 << 1)
    #define SCISSOR_BIT         (1 << 2)
    #define CLEAR_COLOUR_BIT    (1 << 3)
    #define CLEAR_DEPTH_BIT     (1 << 4)
    #define CLEAR_STENCIL_BIT   (1 << 5)
    #define ENABLE_BIT          (1 << 6)
    #define DISABLE_BIT         (1 << 7)


    RCSetGlobalState::RCSetGlobalState()
        : operationBitfield(0),
          viewportParams(), scissorParams(),
          clearColorParams(), clearDepthParams(), clearStencilParams(),
          enableParams(), disableParams()
    {
    }

    void RCSetGlobalState::SetViewport(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        this->viewportParams.x      = x;
        this->viewportParams.y      = y;
        this->viewportParams.width  = width;
        this->viewportParams.height = height;

        this->operationBitfield |= VIEWPORT_BIT;
    }

    void RCSetGlobalState::SetScissor(GLint x, GLint y, GLsizei width, GLsizei height)
    {
        this->scissorParams.x      = x;
        this->scissorParams.y      = y;
        this->scissorParams.width  = width;
        this->scissorParams.height = height;

        this->operationBitfield |= SCISSOR_BIT;
    }

    
    void RCSetGlobalState::SetClearColour(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        this->clearColorParams.r = r;
        this->clearColorParams.g = g;
        this->clearColorParams.b = b;
        this->clearColorParams.a = a;

        this->operationBitfield |= CLEAR_COLOUR_BIT;
    }

    void RCSetGlobalState::SetClearDepth(GLfloat depth)
    {
        this->clearDepthParams.depth = depth;

        this->operationBitfield |= CLEAR_DEPTH_BIT;
    }

    void RCSetGlobalState::SetClearStencil(GLint stencil)
    {
        this->clearStencilParams.stencil = stencil;

        this->operationBitfield |= CLEAR_STENCIL_BIT;
    }


    void RCSetGlobalState::Enable(GLenum cap)
    {
        if (cap == GL_SCISSOR_TEST)
        {
            this->enableParams.caps  |=  SCISSOR_TEST_BIT;
            this->disableParams.caps &= ~SCISSOR_TEST_BIT;
        }
        else if (cap == GL_BLEND)
        {
            this->enableParams.caps  |=  BLENDING_BIT;
            this->disableParams.caps &= ~BLENDING_BIT;
        }
        else if (cap == GL_ALPHA_TEST)
        {
            this->enableParams.caps  |=  ALPHA_TEST_BIT;
            this->disableParams.caps &= ~ALPHA_TEST_BIT;
        }
        else if (cap == GL_DEPTH_TEST)
        {
            this->enableParams.caps  |=  DEPTH_TEST_BIT;
            this->disableParams.caps &= ~DEPTH_TEST_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_FILL)
        {
            this->enableParams.caps  |=  POLYGON_OFFSET_FILL_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_FILL_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_LINE)
        {
            this->enableParams.caps  |=  POLYGON_OFFSET_LINE_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_LINE_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_POINT)
        {
            this->enableParams.caps  |=  POLYGON_OFFSET_POINT_BIT;
            this->disableParams.caps &= ~POLYGON_OFFSET_POINT_BIT;
        }
        else if (cap == GL_TEXTURE_1D)
        {
            this->enableParams.caps  |=  TEXTURE1D_BIT;
            this->disableParams.caps &= ~TEXTURE1D_BIT;
        }
        else if (cap == GL_TEXTURE_2D)
        {
            this->enableParams.caps  |=  TEXTURE2D_BIT;
            this->disableParams.caps &= ~TEXTURE2D_BIT;
        }
        else if (cap == GL_TEXTURE_3D)
        {
            this->enableParams.caps  |=  TEXTURE3D_BIT;
            this->disableParams.caps &= ~TEXTURE3D_BIT;
        }
        else if (cap == GL_TEXTURE_CUBE_MAP)
        {
            this->enableParams.caps  |=  TEXTURECUBE_BIT;
            this->disableParams.caps &= ~TEXTURECUBE_BIT;
        }
        else if (cap == GL_CULL_FACE)
        {
            this->enableParams.caps  |=  FACE_CULLING_BIT;
            this->disableParams.caps &= ~FACE_CULLING_BIT;
        }

        this->operationBitfield |= ENABLE_BIT;
    }

    void RCSetGlobalState::Disable(GLenum cap)
    {
        if (cap == GL_SCISSOR_TEST)
        {
            this->disableParams.caps |=  SCISSOR_TEST_BIT;
            this->enableParams.caps  &= ~SCISSOR_TEST_BIT;
        }
        else if (cap == GL_BLEND)
        {
            this->disableParams.caps |=  BLENDING_BIT;
            this->enableParams.caps  &= ~BLENDING_BIT;
        }
        else if (cap == GL_ALPHA_TEST)
        {
            this->disableParams.caps |=  ALPHA_TEST_BIT;
            this->enableParams.caps  &= ~ALPHA_TEST_BIT;
        }
        else if (cap == GL_DEPTH_TEST)
        {
            this->disableParams.caps |=  DEPTH_TEST_BIT;
            this->enableParams.caps  &= ~DEPTH_TEST_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_FILL)
        {
            this->disableParams.caps |=  POLYGON_OFFSET_FILL_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_FILL_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_LINE)
        {
            this->disableParams.caps |=  POLYGON_OFFSET_LINE_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_LINE_BIT;
        }
        else if (cap == GL_POLYGON_OFFSET_POINT)
        {
            this->disableParams.caps |=  POLYGON_OFFSET_POINT_BIT;
            this->enableParams.caps  &= ~POLYGON_OFFSET_POINT_BIT;
        }
        else if (cap == GL_TEXTURE_1D)
        {
            this->disableParams.caps |=  TEXTURE1D_BIT;
            this->enableParams.caps  &= ~TEXTURE1D_BIT;
        }
        else if (cap == GL_TEXTURE_2D)
        {
            this->disableParams.caps |=  TEXTURE2D_BIT;
            this->enableParams.caps  &= ~TEXTURE2D_BIT;
        }
        else if (cap == GL_TEXTURE_3D)
        {
            this->disableParams.caps |=  TEXTURE3D_BIT;
            this->enableParams.caps  &= ~TEXTURE3D_BIT;
        }
        else if (cap == GL_TEXTURE_CUBE_MAP)
        {
            this->disableParams.caps |=  TEXTURECUBE_BIT;
            this->enableParams.caps  &= ~TEXTURECUBE_BIT;
        }
        else if (cap == GL_CULL_FACE)
        {
            this->disableParams.caps |=  FACE_CULLING_BIT;
            this->enableParams.caps  &= ~FACE_CULLING_BIT;
        }

        this->operationBitfield |= DISABLE_BIT;
    }

    void RCSetGlobalState::EnableColourWrites()
    {
        this->enableParams.caps  |=  COLOUR_WRITES_BIT;
        this->disableParams.caps &= ~COLOUR_WRITES_BIT;
    }

    void RCSetGlobalState::DisableColourWrites()
    {
        this->disableParams.caps |=  COLOUR_WRITES_BIT;
        this->enableParams.caps  &= ~COLOUR_WRITES_BIT;
    }

    void RCSetGlobalState::EnableDepthWrites()
    {
        this->enableParams.caps  |=  DEPTH_WRITES_BIT;
        this->disableParams.caps &= ~DEPTH_WRITES_BIT;
    }

    void RCSetGlobalState::DisableDepthWrites()
    {
        this->disableParams.caps |=  DEPTH_WRITES_BIT;
        this->enableParams.caps  &= ~DEPTH_WRITES_BIT;
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

        if ((this->operationBitfield & ENABLE_BIT))
        {
            if ((this->enableParams.caps & GL_SCISSOR_TEST))
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
            if ((this->enableParams.caps & TEXTURE1D_BIT))
            {
                glEnable(GL_TEXTURE_1D);
            }
            if ((this->enableParams.caps & TEXTURE2D_BIT))
            {
                glEnable(GL_TEXTURE_2D);
            }
            if ((this->enableParams.caps & TEXTURE3D_BIT))
            {
                glEnable(GL_TEXTURE_3D);
            }
            if ((this->enableParams.caps & TEXTURECUBE_BIT))
            {
                glEnable(GL_TEXTURE_CUBE_MAP);
            }
            if ((this->enableParams.caps & FACE_CULLING_BIT))
            {
                glEnable(GL_CULL_FACE);
            }
        }

        if ((this->operationBitfield & DISABLE_BIT))
        {
            if ((this->enableParams.caps & GL_SCISSOR_TEST))
            {
                glDisable(GL_SCISSOR_TEST);
            }
            if ((this->enableParams.caps & BLENDING_BIT))
            {
                glDisable(GL_BLEND);
            }
            if ((this->enableParams.caps & ALPHA_TEST_BIT))
            {
                glDisable(GL_ALPHA_TEST);
            }
            if ((this->enableParams.caps & COLOUR_WRITES_BIT))
            {
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            }
            if ((this->enableParams.caps & DEPTH_WRITES_BIT))
            {
                glDepthMask(GL_FALSE);
            }
            if ((this->enableParams.caps & DEPTH_TEST_BIT))
            {
                glDisable(GL_DEPTH_TEST);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_FILL_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_LINE_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_LINE);
            }
            if ((this->enableParams.caps & POLYGON_OFFSET_POINT_BIT))
            {
                glDisable(GL_POLYGON_OFFSET_POINT);
            }
            if ((this->enableParams.caps & TEXTURE1D_BIT))
            {
                glDisable(GL_TEXTURE_1D);
            }
            if ((this->enableParams.caps & TEXTURE2D_BIT))
            {
                glDisable(GL_TEXTURE_2D);
            }
            if ((this->enableParams.caps & TEXTURE3D_BIT))
            {
                glDisable(GL_TEXTURE_3D);
            }
            if ((this->enableParams.caps & TEXTURECUBE_BIT))
            {
                glDisable(GL_TEXTURE_CUBE_MAP);
            }
            if ((this->enableParams.caps & FACE_CULLING_BIT))
            {
                glDisable(GL_CULL_FACE);
            }
        }
    }
}
