// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUStateObjects_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)

namespace GT
{
    namespace GE
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
    }
}

#endif