// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/ParticleList.hpp>
#include <new>
#include <cstring>  // For memcpy()

namespace GTEngine
{
    ParticleList::ParticleList(size_t initialBufferSize)
        : buffer(nullptr), bufferSize(0), count(0), startIndex(0)
    {
        this->Resize(initialBufferSize);
    }

    ParticleList::ParticleList(const ParticleList &other)
        : buffer(nullptr), bufferSize(0), count(0), startIndex(0)
    {
        if (other.buffer != nullptr && other.bufferSize > 0)
        {
            this->startIndex = other.startIndex;
            this->count      = other.count;
            this->bufferSize = other.bufferSize;
            this->buffer     = static_cast<Particle*>(malloc(other.bufferSize * sizeof(Particle)));

            memcpy(this->buffer, other.buffer, other.bufferSize * sizeof(Particle));
        }
    }

    ParticleList::~ParticleList()
    {
        _mm_free(this->buffer);
    }


    Particle & ParticleList::PushNewParticle()
    {
        // Make sure there is enough space.
        if (this->bufferSize == this->count)
        {
            this->Resize((this->bufferSize == 0) ? 1 : this->bufferSize * 2);
        }

        auto result = new (this->buffer + ((this->startIndex + this->count) % this->bufferSize)) Particle();      // TODO: See if we can avoid this constructor. Will need to document.
        this->count += 1;

        return *result;
    }

    void ParticleList::Remove(size_t index)
    {
        assert(this->count > 0);
        assert(this->count > index);
        {
            if (index == 0)
            {
                // Removing the front object.
                this->startIndex = (this->startIndex + 1) % this->bufferSize;
            }
            else if (index < this->count - 1)
            {
                if (index < this->count / 2)
                {
                    // Move the first half.

                    for (size_t i = 0; i < index; ++i)
                    {
                        auto dst = this->buffer + (this->startIndex + i    ) % this->bufferSize;
                        auto src = this->buffer + (this->startIndex + i + 1) % this->bufferSize;

                        memcpy(dst, src, sizeof(Particle));
                    }

                    this->startIndex = (this->startIndex + 1) % this->bufferSize;
                }
                else
                {
                    // Moving the back half.

                    for (size_t i = index; i < this->count - 1; ++i)
                    {
                        auto dst = this->buffer + (this->startIndex + i + 1) % this->bufferSize;
                        auto src = this->buffer + (this->startIndex + i    ) % this->bufferSize;

                        memcpy(dst, src, sizeof(Particle));
                    }
                }
            }


            this->count -= 1;
        }
    }


    size_t ParticleList::GetCount() const
    {
        return this->count;
    }

    Particle & ParticleList::GetParticle(size_t index)
    {
        return this->buffer[(this->startIndex + index) % this->bufferSize];
    }

    const Particle & ParticleList::GetParticle(size_t index) const
    {
        return this->buffer[(this->startIndex + index) % this->bufferSize];
    }


    void ParticleList::Clear()
    {
        this->count      = 0;
        this->startIndex = 0;
    }


    size_t ParticleList::GetBufferSize() const
    {
        return this->bufferSize;
    }

    void ParticleList::Resize(size_t newBufferSize)
    {
        if (newBufferSize > this->bufferSize)
        {
            auto oldBufferSize = this->bufferSize;
            auto oldBuffer     = this->buffer;
            auto newBuffer     = static_cast<Particle*>(_mm_malloc(newBufferSize * sizeof(Particle), 16));

            if (oldBuffer != nullptr)
            {
                // Copying over may need to be done in two parts due to the way the buffer if implemented.
                if (this->startIndex + this->count > oldBufferSize)
                {
                    // Needs to be done in two parts.
                    size_t part1Size = oldBufferSize - this->startIndex;
                    size_t part2Size = this->count - (part1Size);

                    memcpy(newBuffer,             oldBuffer + this->startIndex, part1Size * sizeof(Particle));
                    memcpy(newBuffer + part1Size, oldBuffer,                    part2Size * sizeof(Particle));
                }
                else
                {
                    // Can be done in one part.
                    memcpy(newBuffer, oldBuffer + this->startIndex, this->count * sizeof(Particle));
                }

                _mm_free(oldBuffer);
            }

            this->bufferSize   = newBufferSize;
            this->buffer       = newBuffer;
            this->startIndex   = 0;
        }
    }
}
