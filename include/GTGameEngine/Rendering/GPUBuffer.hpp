// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_GPUBuffer_hpp_
#define __GT_GE_Rendering_GPUBuffer_hpp_

#include "GPUBufferType.hpp"
#include "GPUBufferUsage.hpp"
#include "GPUBufferCPUAccessFlags.hpp"

namespace GT
{
    namespace GE
    {
        /// Class representing a buffer on the GPU.
        class GPUBuffer
        {
        public:

            /// Constructor.
            GPUBuffer();

            /// Destructor.
            virtual ~GPUBuffer();

            
            /// Retrieves the buffer type.
            virtual GPUBufferType GetBufferType() const = 0;

            /// Retrieves the buffer usage.
            virtual GPUBufferUsage GetBufferUsage() const = 0;

            /// Retrieves the buffer CPU access flags.
            virtual GPUBufferCPUAccessFlags GetBufferCPUAccessFlags() const = 0;
        };
    }
}

#endif
