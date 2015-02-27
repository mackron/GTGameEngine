// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUStateObjects_hpp_
#define __GT_GE_GPUStateObjects_hpp_

#include <cstdint>

namespace GT
{
    enum GPUFillMode
    {
        GPUFillMode_Wireframe = 0,
        GPUFillMode_Solid     = 1
    };

    enum GPUCullMode
    {
        GPUCullMode_None  = 0,
        GPUCullMode_Front = 1,
        GPUCullMode_Back  = 2
    };

    enum GPUPolygonWinding
    {
        GPUPolygonWinding_CCW = 0,
        GPUPolygonWinding_CW  = 1
    };


    /// Structure describing a rasterization state.
    ///
    /// This is used when creating a rasterization state object.
    struct GPURasterizerStateDesc
    {
        GPUFillMode       fillMode;
        GPUCullMode       cullMode;
        GPUPolygonWinding polygonWinding;
        int               depthBias;
        float             depthBiasClamp;
        float             slopeScaledDepthBias;
        bool              enableDepthClip;
        bool              enableScissor;
        bool              enableMultisample;
        bool              enableAntialiasedLine;
    };


    /// Base class for the rasterizer state object.
    class GPURasterizerState
    {
    public:

        /// Constructor.
        GPURasterizerState(const GPURasterizerStateDesc &desc);

        /// Destructor.
        virtual ~GPURasterizerState();


        /// Retrieve a reference to the rasterizer state descriptor.
        const GPURasterizerStateDesc & GetDesc() const;


    private:

        /// The rasterizer state descriptor that was used to create the state object.
        GPURasterizerStateDesc m_desc;
    };





    enum GPUDepthWriteMask
    {
        GPUDepthWriteMask_Zero = 0,
        GPUDepthWriteMast_All  = 1
    };

    enum GPUComparisonFunc
    {
        GPUComparisonFunc_Never         = 0,
        GPUComparisonFunc_Less          = 1,
        GPUComparisonFunc_Equal         = 2,
        GPUComparisonFunc_Less_Equal    = 3,
        GPUComparisonFunc_Greater       = 4,
        GPUComparisonFunc_Not_Equal     = 5,
        GPUComparisonFunc_Greater_Eqaul = 6,
        GPUComparisonFunc_Always        = 7
    };


    enum GPUStencilOp
    {
        GPUStencilOp_Keep            = 0,
        GPUStencilOp_Zero            = 1,
        GPUStencilOp_Replace         = 2,
        GPUStencilOp_Increment_Clamp = 3,
        GPUStencilOp_Decrement_Clamp = 4,
        GPUStencilOp_Invert          = 5,
        GPUStencilOp_Increment       = 6,
        GPUStencilOp_Decrement       = 7
    };

    struct GPUStencilOpDesc
    {
        GPUStencilOp stencilFailOp;
        GPUStencilOp stencilDepthFailOp;
        GPUStencilOp stencilPassOp;
        GPUComparisonFunc stencilFunc;
    };


    /// Structure describing a depth/stencil state.
    ///
    /// This is used when creating a depth/stencil state object.
    struct GPUDepthStencilStateDesc
    {
        bool enableDepthTest;
        GPUDepthWriteMask depthWriteMask;
        GPUComparisonFunc depthFunc;
            
        bool enableStencilTest;
        uint8_t stencilReadMask;
        uint8_t stencilWriteMask;
        GPUStencilOpDesc stencilFrontFaceOp;
        GPUStencilOpDesc stencilBackFaceOp;
    };

    /// Base class for the depth/stencil state object.
    class GPUDepthStencilState
    {
    public:

        /// Cosntructor.
        GPUDepthStencilState(const GPUDepthStencilStateDesc &desc);

        /// Destructor.
        virtual ~GPUDepthStencilState();


        /// Retrieve a reference to the depth/stencil state descriptor.
        const GPUDepthStencilStateDesc & GetDesc() const;



    private:

        /// The depth/stencil state desciptor that was used to create the state object.
        GPUDepthStencilStateDesc m_desc;
    };
}

#endif // !__GT_GE_GPUStateObjects_hpp_
