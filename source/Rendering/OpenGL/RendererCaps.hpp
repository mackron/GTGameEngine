// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL_RendererCaps_hpp_
#define __GTEngine_Rendering_OpenGL_RendererCaps_hpp_

#include <gtgl/gtgl.h>

namespace GTEngine
{
    struct RendererCaps_OpenGL
    {
        GLint MaxColourAttachments;         ///< The maximum number of framebuffer colour attachments.
        GLint MaxDrawBuffers;               ///< The maximum number of draw buffers for MRT.
        GLint MaxTextureUnits;              ///< The maximum number of active texture units.
    };
}


#endif