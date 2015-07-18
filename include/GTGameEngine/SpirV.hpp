// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

// This file contains customizations and extensions to SPIR-V.
#ifndef __GT_SpirV_hpp_
#define __GT_SpirV_hpp_

#include "external/spirv.h"

namespace GT
{
    enum class SpirVCommonTypeID
    {
        Float       = 1,
        FloatPtr    = 2,
        Float2      = 3,
        Float2Ptr   = 4,
        Float3      = 5,
        Float3Ptr   = 6,
        Float4      = 7,
        Float4Ptr   = 8,
        Integer     = 9,
        IntegerPtr  = 10,
        Integer2    = 11,
        Integer2Ptr = 12,
        Integer3    = 13,
        Integer3Ptr = 14,
        Integer4    = 15,
        Integer4Ptr = 16,
        Boolean     = 17,
        BooleanPtr  = 18,
        Texture1D   = 19,
        Texture2D   = 20,
        Texture3D   = 21,
        TextureCube = 22,
    };

    enum class SpirVOp : uint32_t
    {
        /// Result <id> = variable index, beginning at 0. This acts as the index into an array.
        /// Type = Variable type-id (use the pre-defined IDs).
        /// Value = A value for each component of the variable. Each component consumes a single word. A 4-component vector will consume 4 words for this part. For textures, this is empty.
        /// Name (string) = The name of the variable. This is null-terminated and on a 32-bit boundary.
        DeclareMaterialVariable = 1024,
    };
}

#endif
