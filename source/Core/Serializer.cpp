// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTLib/Serializer.hpp>

namespace GTLib
{
    //////////////////////////////
    // Serializer

    Serializer::Serializer()
    {
    }

    Serializer::~Serializer()
    {
    }


    void Serializer::WriteString(const char* value, ptrdiff_t sizeInTs)
    {
        uint32_t length = (sizeInTs == -1) ? static_cast<uint32_t>(strlen(value)) : static_cast<uint32_t>(sizeInTs);

        // We write the length first.
        this->Write(length);

        // Now the actual content.
        this->Write(reinterpret_cast<const void*>(value), length);
    }

    void Serializer::WriteString(const GTLib::String &value)
    {
        this->WriteString(value.c_str(), value.GetLengthInTs());
    }


    template <> inline void Serializer::Write<GTLib::String>(const GTLib::String &)
    {
        // Must use WriteString().
        assert(false);
    }



    //////////////////////////////
    // BasicSerializer

    BasicSerializer::BasicSerializer()
        : buffer(nullptr), bufferSizeInBytes(0), writePointer(0)
    {
    }

    BasicSerializer::BasicSerializer(const BasicSerializer &other)
        : buffer(malloc(other.GetBufferSizeInBytes())), bufferSizeInBytes(other.GetBufferSizeInBytes()), writePointer(other.GetBufferSizeInBytes())
    {
        memcpy(this->buffer, other.GetBuffer(), other.GetBufferSizeInBytes());
    }

    BasicSerializer::BasicSerializer(size_t bufferSizeInBytesIn)
        : buffer(malloc(bufferSizeInBytesIn)), bufferSizeInBytes(bufferSizeInBytesIn), writePointer(0)
    {
    }

    BasicSerializer::~BasicSerializer()
    {
        free(this->buffer);
    }


    void BasicSerializer::Write(const void* bufferIn, size_t bufferInSizeInBytes)
    {
        this->CheckAndAllocate(bufferInSizeInBytes);

        memcpy(reinterpret_cast<uint8_t*>(this->buffer) + this->writePointer, bufferIn, bufferInSizeInBytes);
        this->writePointer += bufferInSizeInBytes;
    }

    void BasicSerializer::Clear()
    {
        free(this->buffer);
        this->buffer = nullptr;

        this->bufferSizeInBytes = 0U;
        this->writePointer      = 0U;
    }



    BasicSerializer & BasicSerializer::operator=(const BasicSerializer &other)
    {
        if (this != &other)
        {
            free(this->buffer);

            this->bufferSizeInBytes = other.GetBufferSizeInBytes();
            this->writePointer      = other.GetBufferSizeInBytes();

            this->buffer = malloc(this->bufferSizeInBytes);
            memcpy(this->buffer, other.GetBuffer(), this->bufferSizeInBytes);
        }

        return *this;
    }


    void BasicSerializer::Allocate(size_t newBufferSizeInBytes)
    {
        if (newBufferSizeInBytes != this->bufferSizeInBytes)
        {
            if (newBufferSizeInBytes < this->bufferSizeInBytes)
            {
                newBufferSizeInBytes = this->bufferSizeInBytes;
            }

            auto oldBuffer = this->buffer;

            this->bufferSizeInBytes = newBufferSizeInBytes;
            this->buffer            = malloc(newBufferSizeInBytes);
            memcpy(this->buffer, oldBuffer, this->writePointer);


            free(oldBuffer);
        }
    }

    size_t BasicSerializer::GetAvailableBufferSpaceInBytes() const
    {
        return this->bufferSizeInBytes - this->writePointer;
    }

    void BasicSerializer::CheckAndAllocate(size_t bytesToAdd)
    {
        if (this->GetAvailableBufferSpaceInBytes() < bytesToAdd)
        {
            this->Allocate(this->bufferSizeInBytes + GTLib::Max(bytesToAdd, this->bufferSizeInBytes * 2U));
        }
    }


    template <> inline void BasicSerializer::Write<GTLib::String>(const GTLib::String &)
    {
        // Must use WriteString().
        assert(false);
    }




    //////////////////////////////
    // FileSerializer

    FileSerializer::FileSerializer(FILE* fileIn)
        : fileSTD(fileIn), fileGT(0)
    {
    }
    FileSerializer::FileSerializer(GTLib::FileHandle fileIn)
        : fileSTD(nullptr), fileGT(fileIn)
    {
    }

    FileSerializer::~FileSerializer()
    {
    }


    void FileSerializer::Write(const void* bufferIn, size_t bufferInSizeInBytes)
    {
        assert(this->fileSTD != nullptr || this->fileGT != 0);
        assert(this->fileSTD == nullptr || this->fileGT == 0);

        if (this->fileSTD != nullptr)
        {
            GTLib::IO::Write(this->fileSTD, bufferIn, bufferInSizeInBytes);
        }
        else
        {
            GTLib::WriteFile(this->fileGT, bufferIn, bufferInSizeInBytes);
        }
    }


    template <> inline void FileSerializer::Write<GTLib::String>(const GTLib::String &)
    {
        // Must use WriteString().
        assert(false);
    }
}
