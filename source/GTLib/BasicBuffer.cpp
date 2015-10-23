// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/BasicBuffer.hpp>
#include <GTLib/stdlib.hpp>
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
        if (this->m_size > 0)
        {
            this->m_ptr = malloc(this->m_size);
            std::memcpy(this->m_ptr, other.m_ptr, this->m_size);
        }
    }

    BasicBuffer::BasicBuffer(const void *m_ptr, size_t m_size)
        : m_ptr(nullptr), m_size(m_size)
    {
        if (this->m_size > 0)
        {
            this->m_ptr = malloc(this->m_size);
            std::memcpy(this->m_ptr, m_ptr, this->m_size);
        }
    }

    BasicBuffer::~BasicBuffer()
    {
        free(this->m_ptr);
    }

    void * BasicBuffer::Allocate(size_t m_size, bool tightlyPacked)
    {
        if ((m_size < this->m_size && tightlyPacked) || m_size > this->m_size)
        {
            free(this->m_ptr);

            if (m_size > 0)
            {
                this->m_ptr = malloc(m_size);
            }
            else
            {
                this->m_ptr = nullptr;
            }

            this->m_size = m_size;
        }

        return this->m_ptr;
    }

    BasicBuffer & BasicBuffer::operator=(const BasicBuffer &other)
    {
        if (this != &other)
        {
            // Start with a tightly packed buffer no matter what the m_size.
            this->Allocate(other.m_size, true);
            std::memcpy(this->m_ptr, other.m_ptr, other.m_size);
        }

        return *this;
    }
}
