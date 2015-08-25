// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTLib/Deserializer.hpp>
#include <GTLib/Math.hpp>       // For GTLib::Min().
#include <cstring>              // For memcpy()

namespace GTLib
{
    /////////////////////////////////
    // Deserializer

    Deserializer::Deserializer()
    {
    }

    Deserializer::~Deserializer()
    {
    }

    size_t Deserializer::Read(void* outputBuffer, size_t bytesToRead)
    {
        if (this->HasRoomInChunk(bytesToRead))
        {
            return this->ReadImpl(outputBuffer, bytesToRead);
        }

        return 0;
    }

    size_t Deserializer::Peek(void* outputBuffer, size_t bytesToRead)
    {
        if (this->HasRoomInChunk(bytesToRead))
        {
            return this->PeekImpl(outputBuffer, bytesToRead);
        }

        return 0;
    }

    int64_t Deserializer::Seek(int64_t bytesToSkip)
    {
        if (this->HasRoomInChunk(bytesToSkip))
        {
            return this->SeekImpl(bytesToSkip);
        }

        return 0;
    }

    size_t Deserializer::Tell() const
    {
        return this->TellImpl();
    }


    size_t Deserializer::ReadString(GTLib::String &object)
    {
        size_t bytesRead = 0;

        uint32_t length;
        bytesRead += this->Read(length);

        if (length > 0)
        {
            // The next 'length' bytes will be the actual string content.
            char* buffer = reinterpret_cast<char*>(malloc(length));
            bytesRead += this->Read(buffer, length);

            object.Assign(buffer, length);

            free(buffer);
        }
        else
        {
            object.Assign("");
        }

        return bytesRead;
    }


    void Deserializer::StartChunk(size_t chunkSizeInBytes)
    {
        m_chunkStack.PushBack(_ChunkStackItem(this->Tell() + chunkSizeInBytes));
    }

    void Deserializer::EndChunk()
    {
        if (m_chunkStack.GetCount() > 0)
        {
            size_t currentPosition = this->Tell();

            auto &topItem = m_chunkStack[0];
            if (static_cast<int64_t>(currentPosition) < topItem.m_endPosition)
            {
                this->Seek(topItem.m_endPosition - currentPosition);
            }

            assert(static_cast<int64_t>(currentPosition) == topItem.m_endPosition);
            {
                m_chunkStack.PopBack();
            }
        }
    }

    bool Deserializer::HasRoomInChunk(int64_t bytesToRead) const
    {
        if (m_chunkStack.GetCount() > 0)
        {
            if (static_cast<int64_t>(this->Tell()) + bytesToRead > m_chunkStack.GetBack().m_endPosition)
            {
                return false;
            }
        }

        return true;
    }


    template <> inline size_t Deserializer::Read<GTLib::String>(GTLib::String &)
    {
        // Must use ReadString().
        assert(false);
        return 0;
    }




    ///////////////////////////////////
    // BasicDeserializer

    BasicDeserializer::BasicDeserializer(const void* bufferIn, size_t bufferSizeInBytesIn)
        : buffer(bufferIn), bufferSizeInBytes(bufferSizeInBytesIn), readPointer(0)
    {
    }

    /// Destructor.
    BasicDeserializer::~BasicDeserializer()
    {
    }


    size_t BasicDeserializer::ReadImpl(void* outputBuffer, size_t bytesToRead)
    {
        bytesToRead = this->Peek(outputBuffer, bytesToRead);
        this->readPointer += bytesToRead;

        return bytesToRead;
    }

    size_t BasicDeserializer::PeekImpl(void* outputBuffer, size_t bytesToRead)
    {
        // We can't read over the buffer.
        bytesToRead = GTLib::Min(bytesToRead, this->GetAvailableBufferSpaceInBytes());

        memcpy(outputBuffer, reinterpret_cast<const uint8_t*>(this->buffer) + this->readPointer, bytesToRead);

        return bytesToRead;
    }

    int64_t BasicDeserializer::SeekImpl(int64_t bytesToSkip)
    {
        // We can't skip past the buffer, nor can we go before it.
        int64_t targetPointer = static_cast<int64_t>(this->readPointer) + bytesToSkip;
        if (targetPointer >= static_cast<int64_t>(this->bufferSizeInBytes))
        {
            bytesToSkip = this->GetAvailableBufferSpaceInBytes();
        }
        else if (targetPointer < 0)
        {
            assert(bytesToSkip <= targetPointer);
            bytesToSkip = bytesToSkip - targetPointer;
        }

        this->readPointer += static_cast<ptrdiff_t>(bytesToSkip);

        return bytesToSkip;
    }

    size_t BasicDeserializer::TellImpl() const
    {
        return this->readPointer;
    }

    size_t BasicDeserializer::GetAvailableBufferSpaceInBytes() const
    {
        return this->bufferSizeInBytes - this->readPointer;
    }

    template <> inline size_t BasicDeserializer::Read<GTLib::String>(GTLib::String &)
    {
        // Must use ReadString().
        assert(false);
        return 0;
    }
}



