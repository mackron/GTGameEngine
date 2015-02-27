// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_GPUBufferMapType_hpp_
#define __GT_GE_Rendering_GPUBufferMapType_hpp_

namespace GT
{
    enum GPUBufferMapType
    {
        GPUBufferMapType_Read              = 1,
        GPUBufferMapType_Write             = 2,
        GPUBufferMapType_ReadWrite         = 3,
        GPUBufferMapType_Write_Discard     = 4,
        GPUBufferMapType_Write_NoOverwrite = 5      //< Cannot be used with constant buffer.
    };
}

#endif
