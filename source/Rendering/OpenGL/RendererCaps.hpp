// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Rendering_OpenGL_RendererCaps
#define GT_Rendering_OpenGL_RendererCaps

#include <gtgl/gtgl.h>

namespace GT
{
    struct RendererCaps_OpenGL
    {
        GLint MaxColourAttachments;                         ///< The maximum number of framebuffer colour attachments.
        GLint MaxDrawBuffers;                               ///< The maximum number of draw buffers for MRT.
        GLint MaxTextureUnits;                              ///< The maximum number of active texture units.

        GLint MaxVertexShaderInstructions;                  ///< The maximum number of vertex shader instructions per shader.
        GLint MaxFragmentShaderInstructions;                ///< The maximum number of fragment shader instructions per shader.
        GLint MaxGeometryShaderInstructions;                ///< The maximum number of geometry shader instructions per shader.
        GLint MaxShaderUniforms;                            ///< The maximum number of uniforms per shader.

        GLboolean SupportsMixedSizedBufferAttachments;      ///< Whether or not buffers of different sizes can be attached to a framebuffer.
    };
}


#endif
