// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCDeleteShader_OpenGL21.hpp"
#include <gtgl/gtgl.h>

namespace GT
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
