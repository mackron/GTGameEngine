// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCDeleteShader.hpp"
#include <gtgl/gtgl.h>

namespace GTEngine
{
    RCDeleteShader::RCDeleteShader()
        : programObject(nullptr)
    {
    }

    void RCDeleteShader::DeleteShader(GLuint* programObjectIn)
    {
        this->programObject = programObjectIn;
    }

    void RCDeleteShader::Execute()
    {
        glDeleteProgram(*this->programObject);
    }
}
