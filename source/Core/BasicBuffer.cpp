// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/BasicBuffer.hpp>
#include <GTGE/Core/stdlib.hpp>
#include <cstring>  // For memory stuff.

namespace GT
{
    BasicBuffer::BasicBuffer()
        : m_ptr(nullptr), m_size(0)
    {
    }

    BasicBuffer::BasicBuffer(const BasicBuffer &other)
        : m_ptr(nullptr), m_size(other.m_size)
    {
        if (m_size > 0)
        {
            m_ptr = malloc(m_size);
            std::memcpy(m_ptr, other.m_ptr, m_size);
        }
    }

    BasicBuffer::BasicBuffer(const void* ptr, size_t size)
        : m_ptr(nullptr), m_size(size)
    {
        if (m_size > 0)
        {
            m_ptr = malloc(m_size);
            std::memcpy(m_ptr, ptr, m_size);
        }
    }

    BasicBuffer::~BasicBuffer()
    {
        free(m_ptr);
    }

    void* BasicBuffer::Allocate(size_t size, bool tightlyPacked)
    {
        if ((size < m_size && tightlyPacked) || size > m_size)
        {
            free(m_ptr);

            if (size > 0)
            {
                m_ptr = malloc(size);
            }
            else
            {
                m_ptr = nullptr;
            }

            m_size = size;
        }

        return m_ptr;
    }

    BasicBuffer & BasicBuffer::operator=(const BasicBuffer &other)
    {
        if (this != &other)
        {
            // Start with a tightly packed buffer no matter what the m_size.
            this->Allocate(other.m_size, true);
            std::memcpy(m_ptr, other.m_ptr, other.m_size);
        }

        return *this;
    }
}
