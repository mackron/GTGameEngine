// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "ServerState_OpenGL21.hpp"

namespace GTEngine
{
    // Default objects.
    //GLuint ServerState_DefaultVAO = 0;
    //GLuint ServerState_DefaultVBO = 0;
    //GLuint ServerState_DefaultIBO = 0;


    // Current Objects.
    GLuint ServerState_GL_TEXTURE_BINDING_1D           = 0;
    GLuint ServerState_GL_TEXTURE_BINDING_2D           = 0;
    GLuint ServerState_GL_TEXTURE_BINDING_3D           = 0;
    GLuint ServerState_GL_TEXTURE_BINDING_CUBE         = 0;
    //GLuint ServerState_GL_VERTEX_ARRAY_BINDING = 0;
    GLuint ServerState_GL_ARRAY_BUFFER_BINDING         = 0;
    GLuint ServerState_GL_ELEMENT_ARRAY_BUFFER_BINDING = 0;
    GLuint ServerState_GL_CURRENT_PROGRAM              = 0;
    GLuint ServerState_GL_FRAMEBUFFER_BINDING          = 0;


    // Enabled state.
    uint32_t ServerState_EnabledVertexAttributes = 0;
}
