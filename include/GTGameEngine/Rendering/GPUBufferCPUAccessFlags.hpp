// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Rendering_GPUBufferCPUAccessFlags_hpp_
#define __GT_GE_Rendering_GPUBufferCPUAccessFlags_hpp_

namespace GT
{
    namespace GE
    {
        enum GPUBufferCPUAccessFlags
        {
            GPUBufferCPUAccess_None      = 0x00,
            GPUBufferCPUAccess_Read      = 0x01,
            GPUBufferCPUAccess_Write     = 0x02,
            GPUBufferCPUAccess_ReadWrite = GPUBufferCPUAccess_Read | GPUBufferCPUAccess_Write
        };
    }
}

#endif