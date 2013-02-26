// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_OpenGL_Debugging_OpenGL_hpp_
#define __GTEngine_Rendering_OpenGL_Debugging_OpenGL_hpp_

namespace GTEngine
{
    /// The function that will be called when an OpenGL debugging message is posted.
    ///
    /// @param message [in] The debug message.
    void OpenGLDebugMessageHandler(const char* message);
}

#endif