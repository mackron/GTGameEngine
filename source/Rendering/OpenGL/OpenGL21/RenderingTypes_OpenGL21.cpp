// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "RenderingTypes_OpenGL21.hpp"

namespace GT
{
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

    //////////////////////////////////////////////////
    // InputLayout_OpenGL21

    InputLayout_OpenGL21::InputLayout_OpenGL21(AttributeDesc* const attributes, size_t attributeCount, unsigned int* slotAttributeCounts)
        : m_attributes(reinterpret_cast<AttributeDesc* const>(malloc(sizeof(AttributeDesc) * attributeCount))),
          m_attributeCount(attributeCount),
          m_slotAttributeRanges()
    {
        memcpy(m_attributes, attributes, sizeof(AttributeDesc) * attributeCount);


        uint32_t rangeStart = 0;
        uint32_t rangeEnd   = 0;
        for (int i = 0; i < GT_GE_MAX_VERTEX_BUFFER_SLOTS; ++i)
        {
            unsigned int slotAttributeCount = slotAttributeCounts[i];

            rangeEnd += slotAttributeCount;
            {
                m_slotAttributeRanges[i] = (rangeStart << 16) | rangeEnd;
            }
            rangeStart = rangeEnd;
        }
    }

    InputLayout_OpenGL21::~InputLayout_OpenGL21()
    {
        free(m_attributes);
    }


    size_t InputLayout_OpenGL21::GetAttributeCount() const
    {
        return m_attributeCount;
    }

    const InputLayout_OpenGL21::AttributeDesc & InputLayout_OpenGL21::GetAttribute(size_t attributeIndex) const
    {
        assert(attributeIndex < m_attributeCount);

        return m_attributes[attributeIndex];
    }


    void InputLayout_OpenGL21::GetSlotAttributeRange(unsigned int slotIndex, size_t &rangeStartOut, size_t &rangeEndOut) const
    {
        assert(slotIndex < GT_GE_MAX_VERTEX_BUFFER_SLOTS);

        rangeStartOut = static_cast<size_t>(m_slotAttributeRanges[slotIndex] >> 16);
        rangeEndOut   = static_cast<size_t>(m_slotAttributeRanges[slotIndex] & 0x0000FFFF);
    }


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
}