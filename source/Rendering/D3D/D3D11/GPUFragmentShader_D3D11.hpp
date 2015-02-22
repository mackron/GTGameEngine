// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUFragmentShader_D3D11_hpp_
#define __GT_GE_GPUFragmentShader_D3D11_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTGameEngine/Rendering/GPUFragmentShader.hpp>
#include <d3d11.h>

namespace GT
{
    namespace GE
    {
        /// Class representing a D3D11 fragment shader.
        class GPUFragmentShader_D3D11 : public GPUFragmentShader
        {
        public:

            /// Constructor.
            GPUFragmentShader_D3D11(ID3D11PixelShader* pixelShaderD3D, const void* shaderBinaryD3D, size_t shaderBinarySizeD3D);

            /// Destructor.
            virtual ~GPUFragmentShader_D3D11();


            /// Retrieves a reference to the internal D3D11 shader object.
            ID3D11PixelShader* GetD3D11PixelShader();

            /// Retrieves a pointer ot the D3D shader binary.
            const void* GetD3D11ShaderBinary() const;

            /// Retrieves the size in bytes of the D3D shader binary.
            size_t GetD3D11ShaderBinarySize() const;


        private:

            /// A pointer to the D3D11 vertex shader object.
            ID3D11PixelShader* m_pixelShaderD3D;

            /// A pointer to the shader binary.
            void* m_shaderBinaryD3D;

            /// The size in bytes of the D3D shader binary.
            size_t m_shaderBinarySizeD3D;


        private:    // No copying.
            GPUFragmentShader_D3D11(const GPUFragmentShader_D3D11 &);
            GPUFragmentShader_D3D11 & operator=(const GPUFragmentShader_D3D11&);
        };
    }
}

#endif
#endif
