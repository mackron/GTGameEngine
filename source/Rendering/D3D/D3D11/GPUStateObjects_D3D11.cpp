// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUStateObjects_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)

namespace GT
{
    GPURasterizerState_D3D11::GPURasterizerState_D3D11(const GPURasterizerStateDesc &desc, ID3D11RasterizerState* rasterizerStateD3D)
        : GPURasterizerState(desc),
            m_rasterizerStateD3D(rasterizerStateD3D)
    {
    }

    GPURasterizerState_D3D11::~GPURasterizerState_D3D11()
    {
    }


    ID3D11RasterizerState* GPURasterizerState_D3D11::GetD3D11RasterizerState()
    {
        return m_rasterizerStateD3D;
    }




    GPUDepthStencilState_D3D11::GPUDepthStencilState_D3D11(const GPUDepthStencilStateDesc &desc, ID3D11DepthStencilState* depthStencilStateD3D)
        : GPUDepthStencilState(desc),
            m_depthStencilStateD3D(depthStencilStateD3D)
    {
    }

    GPUDepthStencilState_D3D11::~GPUDepthStencilState_D3D11()
    {
    }


    ID3D11DepthStencilState* GPUDepthStencilState_D3D11::GetD3D11DepthStencilState()
    {
        return m_depthStencilStateD3D;
    }
}

#endif