// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_FramebufferState_OpenGL33_hpp_
#define __GTEngine_Rendering_FramebufferState_OpenGL33_hpp_

#include <GTCore/Map.hpp>
#include <gtgl/gtgl.h>

namespace GTEngine
{
    /// Structure containing some state information about framebuffers.
    ///
    /// The main purpose for this is to keep track of the textures units that have textures attached so that the
    /// renderer can efficiently attach/detach buffers.
    ///
    /// Only textures are supported as framebuffer attachments in the OpenGL 3.3 renderer.
    struct FramebufferState_OpenGL33
    {
        /// The OpenGL object of the framebuffers.
        GLuint framebufferObject;


        /// Structure containing information about a colour buffer attached to the framebuffer.
        struct Attachment
        {
            /// The index this colour buffer is attached to.
            GLenum attachmentPoint;

            /// The texture target.
            GLenum textureTarget;

            /// A pointer to the OpenGL texture object.
            GLuint* textureObject;
        };

        /// The map containing the attached colour buffers. This is keyed by the attachment point.
        GTCore::Map<GLenum, Attachment> attachments;


        /// Constructor.
        FramebufferState_OpenGL33()
            : framebufferObject(0), attachments()
        {
        }
    };
}

#endif