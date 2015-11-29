// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCClear_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GT
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
