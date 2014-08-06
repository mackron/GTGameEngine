// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_ServerState_OpenGL21_hpp_
#define __GTEngine_Rendering_ServerState_OpenGL21_hpp_

#include <gtgl/gtgl.h>
#include <cstdint>

namespace GTEngine
{
    // Default objects.
    //extern GLuint ServerState_DefaultVAO;
    //extern GLuint ServerState_DefaultVBO;
    //extern GLuint ServerState_DefaultIBO;


    // Current Objects.
    extern GLuint ServerState_GL_TEXTURE_BINDING_1D;
    extern GLuint ServerState_GL_TEXTURE_BINDING_2D;
    extern GLuint ServerState_GL_TEXTURE_BINDING_3D;
    extern GLuint ServerState_GL_TEXTURE_BINDING_CUBE;
    //extern GLuint ServerState_GL_VERTEX_ARRAY_BINDING;
    extern GLuint ServerState_GL_ARRAY_BUFFER_BINDING;
    extern GLuint ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING;      // <-- Indices.
    extern GLuint ServerState_GL_CURRENT_PROGRAM;
    extern GLuint ServerState_GL_FRAMEBUFFER_BINDING;


    // A bitfield containing the enabled vertex attributes when a 0 vertex array is bound. The bits that are set define the
    // vertex attributes that are enabled.
    extern uint32_t ServerState_EnabledVertexAttributes;
}


#endif