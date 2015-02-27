// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUVertexShader_D3D11_hpp_
#define __GT_GE_GPUVertexShader_D3D11_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_D3D11)
#include <GTGameEngine/Rendering/GPUVertexShader.hpp>
#include <d3d11.h>

namespace GT
{
    /// Class representing a D3D11 vertex shader.
    class GPUVertexShader_D3D11 : public GPUVertexShader
    {
    public:

        /// Constructor.
        GPUVertexShader_D3D11(ID3D11VertexShader* vertexShaderD3D, const void* shaderBinaryD3D, size_t shaderBinarySizeD3D);

        /// Destructor.
        virtual ~GPUVertexShader_D3D11();


        /// Retrieves a reference to the internal D3D11 shader object.
        ID3D11VertexShader* GetD3D11VertexShader();

        /// Retrieves a pointer ot the D3D shader binary.
        const void* GetD3D11ShaderBinary() const;

        /// Retrieves the size in bytes of the D3D shader binary.
        size_t GetD3D11ShaderBinarySize() const;


    private:

        /// A pointer to the D3D11 vertex shader object.
        ID3D11VertexShader* m_vertexShaderD3D;

        /// A pointer to the shader binary.
        void* m_shaderBinaryD3D;

        /// The size in bytes of the D3D shader binary.
        size_t m_shaderBinarySizeD3D;


    private:    // No copying.
        GPUVertexShader_D3D11(const GPUVertexShader_D3D11 &);
        GPUVertexShader_D3D11 & operator=(const GPUVertexShader_D3D11&);
    };
}

#endif
#endif