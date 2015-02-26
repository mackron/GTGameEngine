// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUStateObjects_OpenGL21_hpp_
#define __GT_GE_GPUStateObjects_OpenGL21_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL21)
#include <GTGameEngine/Rendering/GPUStateObjects.hpp>

namespace GT
{
    namespace GE
    {
        /// OpenGL 2.1 implementation of the GPU rasterization state object.
        class GPURasterizerState_OpenGL21 : public GPURasterizerState
        {
        public:

            /// Constructor.
            GPURasterizerState_OpenGL21(const GPURasterizerStateDesc &desc);

            /// Destructor.
            virtual ~GPURasterizerState_OpenGL21();
        };



        /// OpenGL 2.1 implementation of the GPU depth/stencil state object.
        class GPUDepthStencilState_OpenGL21 : public GPUDepthStencilState
        {
        public:

            /// Constructor.
            GPUDepthStencilState_OpenGL21(const GPUDepthStencilStateDesc &desc);

            /// Destructor.
            virtual ~GPUDepthStencilState_OpenGL21();
        };
    }
}

#endif

#endif


