// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDeleteShader_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDeleteShader::RCDeleteShader()
        : programState(nullptr)
    {
    }

    void RCDeleteShader::DeleteShader(ShaderState_OpenGL21* programStateIn)
    {
        this->programState = programStateIn;
    }

    void RCDeleteShader::Execute()
    {
        glDeleteProgram(this->programState->programObject);
    }
}
