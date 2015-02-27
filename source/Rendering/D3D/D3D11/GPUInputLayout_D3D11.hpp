// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUVertexInputLayout_OpenGL21_hpp_
#define __GT_GE_GPUVertexInputLayout_OpenGL21_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTGameEngine/Rendering/GPUInputLayout.hpp>
#include <d3d11.h>

namespace GT
{
    class GPUInputLayout_D3D11 : public GPUInputLayout
    {
    public:

        /// Constructor.
        GPUInputLayout_D3D11(ID3D11InputLayout* inputLayoutD3D);

        /// Destructor.
        ~GPUInputLayout_D3D11();


        /// Retrieves a pointer to the D3D11 input layout object.
        ID3D11InputLayout* GetD3D11InputLayout();
            


    private:

        /// The internal D3D11 input layout object.
        ID3D11InputLayout* m_inputLayoutD3D;


    private:    // No copying.
        GPUInputLayout_D3D11(const GPUInputLayout_D3D11 &);
        GPUInputLayout_D3D11 & operator=(const GPUInputLayout_D3D11 &);
    };
}

#endif

#endif
