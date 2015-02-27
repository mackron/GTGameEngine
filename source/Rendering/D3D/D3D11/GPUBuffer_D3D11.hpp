// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_GPUBuffer_D3D11_hpp_
#define __GT_GE_Rendering_GPUBuffer_D3D11_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTGameEngine/Rendering/GPUBuffer.hpp>
#include <d3d11.h>

namespace GT
{
    /// Class representing a buffer on the GPU.
    class GPUBuffer_D3D11 : public GPUBuffer
    {
    public:

        /// Constructor.
        GPUBuffer_D3D11(ID3D11Buffer* bufferD3D, GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags);

        /// Destructor.
        ~GPUBuffer_D3D11();


        /// GPUBuffer::GetBufferType().
        GPUBufferType GetBufferType() const;

        /// GPUBuffer::GetBufferUsage().
        GPUBufferUsage GetBufferUsage() const;

        /// GPUBuffer::GetBufferCPUAccessFlags().
        GPUBufferCPUAccessFlags GetBufferCPUAccessFlags() const;



        /// Retrieves a pointer to the internal D3D11 buffer object.
        ID3D11Buffer* GetD3D11Buffer();



    private:

        /// The internal D3D11 buffer object.
        ID3D11Buffer* m_bufferD3D;

        /// The buffer type.
        GPUBufferType m_type;

        /// The buffer usage.
        GPUBufferUsage m_usage;

        /// The buffer CPU access flags.
        GPUBufferCPUAccessFlags m_cpuAccessFlags;
    };
}
#endif

#endif