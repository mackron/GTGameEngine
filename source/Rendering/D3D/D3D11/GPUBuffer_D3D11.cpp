// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUBuffer_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)

namespace GT
{
    namespace GE
    {
        GPUBuffer_D3D11::GPUBuffer_D3D11(ID3D11Buffer* bufferD3D, GPUBufferType type, GPUBufferUsage usage, GPUBufferCPUAccessFlags cpuAccessFlags)
            : GPUBuffer(),
              m_bufferD3D(bufferD3D),
              m_type(type),
              m_usage(usage),
              m_cpuAccessFlags(cpuAccessFlags)
        {
        }

        GPUBuffer_D3D11::~GPUBuffer_D3D11()
        {
        }


        GPUBufferType GPUBuffer_D3D11::GetBufferType() const
        {
            return m_type;
        }

        GPUBufferUsage GPUBuffer_D3D11::GetBufferUsage() const
        {
            return m_usage;
        }

        GPUBufferCPUAccessFlags GPUBuffer_D3D11::GetBufferCPUAccessFlags() const
        {
            return m_cpuAccessFlags;
        }


        ID3D11Buffer* GPUBuffer_D3D11::GetD3D11Buffer()
        {
            return m_bufferD3D;
        }
    }
}

#endif
