// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUVertexInputLayoutDesc_hpp_
#define __GT_GE_GPUVertexInputLayoutDesc_hpp_

#include "GPUDataFormats.hpp"

namespace GT
{
    namespace GE
    {
        enum GPUInputClassification
        {
            GPUInputClassification_PerVertex   = 0,
            GPUInputClassification_PerInstance = 1,
        };

        struct GPUInputLayoutAttribDesc
        {
            unsigned int           slotIndex;
            const char*            attributeName;               // The name of the attribute variable inside the shader, or the semantic name in the case of Direct3D.
            GPUVertexAttribFormat  attributeComponentType;      // Float, Signed Int, etc.
            unsigned int           attributeComponentCount;     // float = 1, float2 = 2, etc.
            unsigned int           attributeOffset;             // The attribute's offset.
            GPUInputClassification attributeClass;              // Per-Vertex or Per-Instance
            unsigned int           instanceStepRate;
        };
    }
}

#endif
