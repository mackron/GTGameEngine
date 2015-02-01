// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPURenderingDeviceInfo_hpp_
#define __GT_GE_GPURenderingDeviceInfo_hpp_

namespace GT
{
    namespace GE
    {
        // Rendering API codes.
        enum RenderingAPI
        {
            RenderingAPI_Any      = 0,
            RenderingAPI_Any_Gen1 = 1,
            RenderingAPI_Any_Gen2 = 2,
            RenderingAPI_OpenGL21 = (1 << 8) | RenderingAPI_Any_Gen1,
            RenderingAPI_OpenGL45 = (1 << 8) | RenderingAPI_Any_Gen2,
            RenderingAPI_D3D9     = (2 << 8) | RenderingAPI_Any_Gen1,
            RenderingAPI_D3D11    = (2 << 8) | RenderingAPI_Any_Gen2
        };


        struct GPURenderingDeviceInfo
        {
            /// The description of the device.
            char description[256];

            /// The supported APIs of the given device. This will be terminated with a 0.
            RenderingAPI supportedAPIs[16];


            /// The identifier that is used for identifying the device for the OpenGL API. With OpenGL, we can only use the primary display, so for
            /// this one, we set it to 1 for the rendering device and 0 for the others.
            int identifier_OpenGL;

            /// The identifier that is used for identifying the device for the D3D11 API. This is a pointer to the IDXGIAdapter object.
            void* identifier_D3D11;
        };
    }
}

#endif
