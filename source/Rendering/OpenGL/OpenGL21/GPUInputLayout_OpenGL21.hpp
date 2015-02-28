// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_GPUVertexInputLayout_OpenGL21_hpp_
#define __GT_GE_GPUVertexInputLayout_OpenGL21_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_GE_BUILD_OPENGL21)
#include <GTLib/ReferenceCountedObject.hpp>
#include <GTGameEngine/Rendering/GPUInputLayoutDesc.hpp>
#include <GTGameEngine/Rendering/GPURenderingDeviceLimits.hpp>
#include <GTGameEngine/Rendering/OpenGL/OpenGL.hpp>
#include <cstdint>

namespace GT
{
    class GPUInputLayout_OpenGL21 : public ReferenceCountedObject
    {
    public:

        /// Structure describing an attribute.
        struct AttributeDesc
        {
            unsigned int           slotIndex;
            GLint                  attribLocation;
            GLint                  attribComponentCount;
            GLenum                 attribComponentType;
            GLsizei                attribOffset;
            GPUInputClassification attributeClass;            // Per-Vertex or Per-Instance
            unsigned int           instanceStepRate;
        };



    public:

        /// Constructor.
        GPUInputLayout_OpenGL21(AttributeDesc* const attributes, size_t attributeCount, unsigned int slotAttributeCounts[GT_GE_MAX_VERTEX_BUFFER_SLOTS]);

        /// Destructor.
        ~GPUInputLayout_OpenGL21();


        /// Retrieves the number of vertex attribute count.
        ///
        /// @return The number of attributes making up this vertex input layout.
        size_t GetAttributeCount() const;

        /// Retrieves a reference to the attribute descriptor for the attribute at the given index.
        ///
        /// @param attributeIndex [in] The index of the attribute descriptor to retrieve.
        ///
        /// @return A reference to the attribute descriptor at the given index.
        const AttributeDesc & GetAttribute(size_t attributeIndex) const;

        
        /// Retrieves the range of indices that make up attributes that are bound to the buffer on the given slot.
        ///
        /// @param slotIndex     [in]  The index of the slot whose attribute range is being retrieved.
        /// @param rangeStartOut [out] The index of the first element in the range.
        /// @param rangeEndOut   [out] The index of the last attribute in the range, plus 1.
        ///
        /// @remarks
        ///     If rangeStart and rangeEnd are the same value, it means there are no attributes bound to the given slot.
        void GetSlotAttributeRange(unsigned int slotIndex, size_t &rangeStartOut, size_t &rangeEndOut) const;


    private:

        /// The list of attribute descriptors. This is immutable.
        AttributeDesc* const m_attributes;

        /// The number of attributes containing within this layout object. This is immutable.
        size_t m_attributeCount;

        /// m_attributes will contain a list of vertex attributes, sorted by slot. Each item in this list corresponds to a slot, and contains the index of the
        /// first attribute for the respective slot, and the index + 1 of the last attribute for the slot.
        uint32_t m_slotAttributeRanges[GT_GE_MAX_VERTEX_BUFFER_SLOTS];


    private:    // No copying.
        GPUInputLayout_OpenGL21(const GPUInputLayout_OpenGL21 &);
        GPUInputLayout_OpenGL21 & operator=(const GPUInputLayout_OpenGL21 &);
    };
}

#endif

#endif
