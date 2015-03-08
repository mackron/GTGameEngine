// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "RenderingTypes_OpenGL4.hpp"

#if defined(GT_GE_BUILD_OPENGL4)

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    //////////////////////////////////////////////////
    // InputLayout_OpenGL4

    InputLayout_OpenGL4::InputLayout_OpenGL4(AttributeDesc* const attributes, size_t attributeCount, unsigned int* slotAttributeCounts)
        : m_attributes(reinterpret_cast<AttributeDesc* const>(malloc(sizeof(AttributeDesc) * attributeCount))),
          m_attributeCount(attributeCount),
          m_slotAttributeRanges()
    {
        memcpy(m_attributes, attributes, sizeof(AttributeDesc) * attributeCount);


        uint32_t rangeStart = 0;
        uint32_t rangeEnd   = 0;
        for (int i = 0; i < GT_MAX_VERTEX_BUFFER_SLOTS; ++i)
        {
            unsigned int slotAttributeCount = slotAttributeCounts[i];

            rangeEnd += slotAttributeCount;
            {
                m_slotAttributeRanges[i] = (rangeStart << 16) | rangeEnd;
            }
            rangeStart = rangeEnd;
        }
    }

    InputLayout_OpenGL4::~InputLayout_OpenGL4()
    {
        free(m_attributes);
    }


    size_t InputLayout_OpenGL4::GetAttributeCount() const
    {
        return m_attributeCount;
    }

    const InputLayout_OpenGL4::AttributeDesc & InputLayout_OpenGL4::GetAttribute(size_t attributeIndex) const
    {
        assert(attributeIndex < m_attributeCount);

        return m_attributes[attributeIndex];
    }


    void InputLayout_OpenGL4::GetSlotAttributeRange(unsigned int slotIndex, size_t &rangeStartOut, size_t &rangeEndOut) const
    {
        assert(slotIndex < GT_MAX_VERTEX_BUFFER_SLOTS);

        rangeStartOut = static_cast<size_t>(m_slotAttributeRanges[slotIndex] >> 16);
        rangeEndOut   = static_cast<size_t>(m_slotAttributeRanges[slotIndex] & 0x0000FFFF);
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
