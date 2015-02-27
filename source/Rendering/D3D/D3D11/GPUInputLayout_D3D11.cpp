// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "GPUInputLayout_D3D11.hpp"

#if defined(GT_GE_BUILD_D3D11)
#include <cassert>
#include <cstdlib>

namespace GT
{
    GPUInputLayout_D3D11::GPUInputLayout_D3D11(ID3D11InputLayout* inputLayoutD3D)
        : m_inputLayoutD3D(inputLayoutD3D)
    {
    }

    GPUInputLayout_D3D11::~GPUInputLayout_D3D11()
    {
    }


    ID3D11InputLayout* GPUInputLayout_D3D11::GetD3D11InputLayout()
    {
        return m_inputLayoutD3D;
    }
}

#endif