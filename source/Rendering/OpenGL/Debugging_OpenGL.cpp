// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "Debugging_OpenGL.hpp"
#include <GTGE/GTEngine.hpp>

namespace GT
{
    void OpenGLDebugMessageHandler(const char* message)
    {
        g_Context->Logf("%s", message);
    }
}