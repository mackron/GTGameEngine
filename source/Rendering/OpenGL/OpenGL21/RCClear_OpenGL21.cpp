// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCClear_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCClear::RCClear()
        : bufferMask(0)
    {
    }

    void RCClear::Clear(GLbitfield bufferMaskIn)
    {
        this->bufferMask = bufferMaskIn;
    }

    void RCClear::Execute()
    {
        glClear(this->bufferMask);
    }
}
