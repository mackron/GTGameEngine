// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUDataFormats_hpp_
#define __GT_GE_GPUDataFormats_hpp_

namespace GT
{
    namespace GE
    {
        enum GPUVertexAttribFormat
        {
            GPUVertexAttribFormat_Float       = 0,
            GPUVertexAttribFormat_Int         = 1,
            GPUVertexAttribFormat_UnsignedInt = 2,
        };

        enum GPUIndexFormat
        {
            GPUIndexFormat_UnsignedInt8  = 0,
            GPUIndexFormat_UnsignedInt16 = 1,
            GPUIndexFormat_UnsignedInt32 = 2,
        };
    }
}

#endif
