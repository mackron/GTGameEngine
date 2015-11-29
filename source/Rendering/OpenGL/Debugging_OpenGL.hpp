// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_OpenGL_Debugging_OpenGL_hpp_
#define __GTEngine_Rendering_OpenGL_Debugging_OpenGL_hpp_

namespace GT
{
    /// The function that will be called when an OpenGL debugging message is posted.
    ///
    /// @param message [in] The debug message.
    void OpenGLDebugMessageHandler(const char* message);
}

#endif