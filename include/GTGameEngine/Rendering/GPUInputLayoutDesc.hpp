// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUVertexInputLayoutDesc_hpp_
#define __GT_GE_GPUVertexInputLayoutDesc_hpp_

namespace GT
{
    namespace GE
    {
        enum GPUBasicType
        {
            GPUBasicType_Float = 0,
            GPUBasicType_SInt  = 1,
            GPUBasicType_UInt  = 2
        };

        enum GPUInputClassification
        {
            GPUInputClassification_PerVertex   = 0,
            GPUInputClassification_PerInstance = 1,
        };

        struct GPUInputLayoutAttribDesc
        {
            unsigned int           slotIndex;
            const char*            attributeName;
            GPUBasicType           attributeComponentType;      // Float, Signed Int, etc.
            unsigned int           attributeComponentCount;     // float = 1, float2 = 2, etc.
            unsigned int           attributePadding;            // The amount of padding between the end of this attribute, and the beginning of the next.
            GPUInputClassification attributeClass;              // Per-Vertex or Per-Instance
            unsigned int           instanceStepRate;
        };
    }
}

#endif
