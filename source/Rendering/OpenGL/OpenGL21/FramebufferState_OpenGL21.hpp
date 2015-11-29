// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_FramebufferState_OpenGL21_hpp_
#define __GTEngine_Rendering_FramebufferState_OpenGL21_hpp_

#include <GTEngine/Core/Map.hpp>
#include <gtgl/gtgl.h>

#include "TextureState_OpenGL21.hpp"


namespace GTEngine
{
    /// Structure containing some state information about framebuffers.
    ///
    /// The main purpose for this is to keep track of the textures units that have textures attached so that the
    /// renderer can efficiently attach/detach buffers.
    ///
    /// Only textures are supported as framebuffer attachments in the OpenGL 3.3 renderer.
    struct FramebufferState_OpenGL21
    {
        /// The OpenGL object of the framebuffers.
        GLuint framebufferObject;

        /// The OpenGL object of the depth/stencil renderbuffer. If a texture is being used, this will be 0.
        GLuint depthStencilRenderbuffer;

        /// The width of the write-only depth/stencil renderbuffer.
        GLsizei depthStencilRenderbufferWidth;

        /// The height of the write-only depth/stencil renderbuffer.
        GLsizei depthStencilRenderbufferHeight;


        /// Structure containing information about a colour buffer attached to the framebuffer.
        struct Attachment
        {
            /// The index this colour buffer is attached to.
            GLenum attachmentPoint;

            /// The texture target.
            GLenum textureTarget;

            /// A pointer to the OpenGL texture state.
            TextureState_OpenGL21* textureState;
        };

        /// The map containing the attached colour buffers. This is keyed by the attachment point.
        GTLib::Map<GLenum, Attachment> attachments;


        /// Constructor.
        FramebufferState_OpenGL21()
            : framebufferObject(0), depthStencilRenderbuffer(0), depthStencilRenderbufferWidth(0), depthStencilRenderbufferHeight(0), attachments()
        {
        }
    };
}

#endif