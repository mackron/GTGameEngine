// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "Debugging_OpenGL.hpp"
#include <GTEngine/Logging.hpp>

namespace GT
{
    void OpenGLDebugMessageHandler(const char* message)
    {
        Log("%s", message);
    }
}