// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUStateObjects_OpenGL21_hpp_
#define __GT_GE_GPUStateObjects_OpenGL21_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL21)
#include <GTGameEngine/Rendering/RenderingTypes.hpp>
#include <GTLib/ReferenceCountedObject.hpp>

namespace GT
{
    /// OpenGL 2.1 implementation of the GPU rasterization state object.
    class GPURasterizerState_OpenGL21 : public ReferenceCountedObject, public GPURasterizerStateDesc
    {
    public:

        /// Constructor.
        GPURasterizerState_OpenGL21(const GPURasterizerStateDesc &desc);
    };



    /// OpenGL 2.1 implementation of the GPU depth/stencil state object.
    class GPUDepthStencilState_OpenGL21 : public ReferenceCountedObject, public GPUDepthStencilStateDesc
    {
    public:

        /// Constructor.
        GPUDepthStencilState_OpenGL21(const GPUDepthStencilStateDesc &desc);
    };
}

#endif

#endif


