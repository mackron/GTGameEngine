// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUStateObjects_OpenGL21.hpp"

#if defined(GT_GE_BUILD_OPENGL21)

namespace GT
{
    namespace GE
    {
        GPURasterizerState_OpenGL21::GPURasterizerState_OpenGL21(const GPURasterizerStateDesc &desc)
            : GPURasterizerState(desc)
        {
        }

        GPURasterizerState_OpenGL21::~GPURasterizerState_OpenGL21()
        {
        }
    }
}

#endif
