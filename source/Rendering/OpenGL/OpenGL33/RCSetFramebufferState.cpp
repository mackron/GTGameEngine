// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetFramebufferState.hpp"
#include "../TypeConversion.hpp"
#include <assert.h>

namespace GTEngine
{
    RCSetFramebufferState::RCSetFramebufferState()
        : operationBitfield(0),
          framebufferObject(nullptr)
    {
    }


    GLuint* RCSetFramebufferState::GetFramebufferObject()
    {
        return this->framebufferObject;
    }


    void RCSetFramebufferState::Execute()
    {
        assert(this->framebufferObject != nullptr);
        {
        }
    }
}
