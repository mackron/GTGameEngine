// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUStateObjects_D3D11_hpp_
#define __GT_GE_GPUStateObjects_D3D11_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTGameEngine/Rendering/GPUStateObjects.hpp>
#include <d3d11.h>

namespace GT
{
    namespace GE
    {
        /// D3D11 implementation of the GPU rasterization state object.
        class GPURasterizerState_D3D11 : public GPURasterizerState
        {
        public:

            /// Constructor.
            GPURasterizerState_D3D11(const GPURasterizerStateDesc &desc, ID3D11RasterizerState* rasterizerStateD3D);

            /// Destructor.
            virtual ~GPURasterizerState_D3D11();


            /// Retrieves a pointer to the internal D3D11 rasterizer state object.
            ID3D11RasterizerState* GetD3D11RasterizerState();


        private:

            /// A pointer to the D3D11 rasterizer state object.
            ID3D11RasterizerState* m_rasterizerStateD3D;
        };
    }
}

#endif

#endif


