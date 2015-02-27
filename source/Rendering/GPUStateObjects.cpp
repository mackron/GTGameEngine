// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Rendering/GPUStateObjects.hpp>

namespace GT
{
    GPURasterizerState::GPURasterizerState(const GPURasterizerStateDesc &desc)
        : m_desc(desc)
    {
    }

    GPURasterizerState::~GPURasterizerState()
    {
    }

    const GPURasterizerStateDesc & GPURasterizerState::GetDesc() const
    {
        return m_desc;
    }





    GPUDepthStencilState::GPUDepthStencilState(const GPUDepthStencilStateDesc &desc)
        : m_desc(desc)
    {
    }

    GPUDepthStencilState::~GPUDepthStencilState()
    {
    }

    const GPUDepthStencilStateDesc & GPUDepthStencilState::GetDesc() const
    {
        return m_desc;
    }
}