// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUStateObjects_hpp_
#define __GT_GE_GPUStateObjects_hpp_

namespace GT
{
    namespace GE
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
    }
}

#endif // !__GT_GE_GPUStateObjects_hpp_
