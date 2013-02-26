// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "Debugging_OpenGL.hpp"
#include <GTEngine/Logging.hpp>

namespace GTEngine
{
    void OpenGLDebugMessageHandler(const char* message)
    {
        GTEngine::Log("%s", message);
    }
}