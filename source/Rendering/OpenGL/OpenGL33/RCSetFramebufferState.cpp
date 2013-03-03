// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCSetFramebufferState.hpp"
#include "../TypeConversion.hpp"
#include <assert.h>

namespace GTEngine
{
    RCSetFramebufferState::RCSetFramebufferState()
        : operationBitfield(0),
          framebufferState(nullptr)
    {
    }


    FramebufferState_OpenGL33* RCSetFramebufferState::GetFramebufferState()
    {
        return this->framebufferState;
    }


    void RCSetFramebufferState::Execute()
    {
        assert(this->framebufferState != nullptr);
        {
        }
    }
}
