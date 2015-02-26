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




        /// D3D11 implementation of the GPU depth/stencil state object.
        class GPUDepthStencilState_D3D11 : public GPUDepthStencilState
        {
        public:

            /// Constructor.
            GPUDepthStencilState_D3D11(const GPUDepthStencilStateDesc &desc, ID3D11DepthStencilState* depthStencilStateD3D);

            /// Destructor.
            virtual ~GPUDepthStencilState_D3D11();


            /// Retrieves a pointer to the internal D3D11 rasterizer state object.
            ID3D11DepthStencilState* GetD3D11DepthStencilState();


        private:

            /// A pointer to the D3D11 rasterizer state object.
            ID3D11DepthStencilState* m_depthStencilStateD3D;
        };
    }
}

#endif

#endif


