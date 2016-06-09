// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Serializer
#define GT_Serializer

#include <cstdlib>
#include <cstring>      // For memcpy()
#include "Math.hpp"     // For Max().
#include "String.hpp"   // For String specialization.
#include <dr_libs/dr_fs.h>

namespace GT
{
    /// Class for doing basic serialization.
    ///
    /// This works in the same kind of way as a file stream.
    class Serializer
    {
    public:

        /// Default constructor.
        Serializer()
        {
        }

        /// Destructor.
        virtual ~Serializer()
        {
        }


        /// Writes a buffer of the given size.
        ///
        /// @param bufferIn            [in] A pointer to the buffer containing the data to write.
        /// @param bufferInSizeInBytes [in] The size of the buffer pointed to by bufferIn, in bytes.
        virtual void Write(const void* bufferIn, size_t bufferInSizeInBytes) = 0;


        /// Generic template method for adding an object to the buffer.
        ///
        /// @param object [in] A reference to the object to write.
        ///
        /// @remarks
        ///     This will just do a simple memcpy() operation.
        template <typename T>
        void Write(const T &object)
        {
            this->Write(&object, sizeof(T));
        }


        /// Writes a string.
        ///
        /// @remarks
        ///     This is a special case. Two properties will be written for strings. The first 32-bytes is the size of the string in char's. The next
        ///     bytes is the actual content of the string itself.
        void WriteString(const char* value, ptrdiff_t sizeInTs = -1)
        {
            uint32_t length = (sizeInTs == -1) ? static_cast<uint32_t>(strlen(value)) : static_cast<uint32_t>(sizeInTs);

            // We write the length first.
            this->Write(length);

            // Now the actual content.
            this->Write(reinterpret_cast<const void*>(value), length);
        }

        /// @copydoc Serializer::WriteString(const char*, ptrdiff_t)
        void WriteString(const String &value)
        {
            this->WriteString(value.c_str(), value.GetLengthInTs());
        }
    };

    template <> inline void Serializer::Write<String>(const String &)
    {
        // Must use WriteString().
        assert(false);
    }




    /// Class for doing basic serialization.
    ///
    /// This works in the same kind of way as a file stream.
    class BasicSerializer : public Serializer
    {
    public:

        /// Default constructor.
        BasicSerializer()
            : buffer(nullptr), bufferSizeInBytes(0), writePointer(0)
        {
        }

        /// Copy constructor.
        BasicSerializer(const BasicSerializer &other)
            : buffer(malloc(other.GetBufferSizeInBytes())), bufferSizeInBytes(other.GetBufferSizeInBytes()), writePointer(other.GetBufferSizeInBytes())
        {
            memcpy(this->buffer, other.GetBuffer(), other.GetBufferSizeInBytes());
        }

        /// Constructor.
        BasicSerializer(size_t bufferSizeInBytesIn)
            : buffer(malloc(bufferSizeInBytesIn)), bufferSizeInBytes(bufferSizeInBytesIn), writePointer(0)
        {
        }

        /// Destructor.
        virtual ~BasicSerializer()
        {
            free(this->buffer);
        }


        /// Serializer::Write().
        void Write(const void* bufferIn, size_t bufferInSizeInBytes)
        {
            this->CheckAndAllocate(bufferInSizeInBytes);

            memcpy(reinterpret_cast<uint8_t*>(this->buffer) + this->writePointer, bufferIn, bufferInSizeInBytes);
            this->writePointer += bufferInSizeInBytes;
        }


        /// Generic template method for adding an object to the buffer.
        ///
        /// @param object [in] A reference to the object to write.
        ///
        /// @remarks
        ///     This will just do a simple memcpy() operation.
        template <typename T>
        void Write(const T &object)
        {
            this->Write(&object, sizeof(T));
        }



        /// Retrieves a pointer to the buffer.
              void* GetBuffer()       { return this->buffer; }
        const void* GetBuffer() const { return this->buffer; }

        /// Retrieves the number of bytes of valid data in the buffer.
        size_t GetBufferSizeInBytes() const { return this->writePointer; }


        /// Resets the serializer to it's default state.
        void Clear()
        {
            free(this->buffer);
            this->buffer = nullptr;

            this->bufferSizeInBytes = 0U;
            this->writePointer      = 0U;
        }



        /// Assignment operator
        BasicSerializer & operator= (const BasicSerializer &other)
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


    private:

        /// Allocates additional space into the buffer.
        ///
        /// @param newBufferSizeInBytes [in] The new size of the buffer, in bytes.
        void Allocate(size_t newBufferSizeInBytes)
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

        /// Retrieves the amount of space available in the buffer.
        size_t GetAvailableBufferSpaceInBytes() const
        {
            return this->bufferSizeInBytes - this->writePointer;
        }

        /// Checks if there is enough space for the given number of bytes and allocates additional space if not.
        void CheckAndAllocate(size_t bytesToAdd)
        {
            if (this->GetAvailableBufferSpaceInBytes() < bytesToAdd)
            {
                this->Allocate(this->bufferSizeInBytes + Max(bytesToAdd, this->bufferSizeInBytes * 2U));
            }
        }


    private:

        /// A pointer to the buffer containing the data.
        void* buffer;

        /// The size of the buffer in bytes.
        size_t bufferSizeInBytes;

        /// The position of the writing pointer.
        size_t writePointer;
    };

    template <> inline void BasicSerializer::Write<String>(const String &)
    {
        // Must use WriteString().
        assert(false);
    }





    /// Class for doing serialization from a file.
    ///
    /// This class does not open or close the file - that should be done at a higher level. The reason for this is that it makes it makes things
    /// a lot more flexible.
    ///
    /// This will move the files write pointer as data is written.
    class FileSerializer : public Serializer
    {
    public:

        /// Constructor.
        FileSerializer(drfs_file* pFileIn)
            : m_pFile(pFileIn)
        {
        }

        /// Destructor.
        virtual ~FileSerializer()
        {
        }


        /// Serializer::Write().
        void Write(const void* bufferIn, size_t bufferInSizeInBytes)
        {
            assert(m_pFile != nullptr);

            drfs_write(m_pFile, bufferIn, static_cast<unsigned int>(bufferInSizeInBytes), nullptr);
        }


        /// Generic template method for adding an object to the buffer.
        ///
        /// @param object [in] A reference to the object to write.
        ///
        /// @remarks
        ///     This will just do a simple memcpy() operation.
        template <typename T>
        void Write(const T &object)
        {
            this->Write(&object, sizeof(T));
        }



    private:

        /// The virtual file system file object to write to.
        drfs_file* m_pFile;


    private:    // No copying
        FileSerializer(const FileSerializer &);
        FileSerializer & operator=(const FileSerializer &);
    };

    template <> inline void FileSerializer::Write<String>(const String &)
    {
        // Must use WriteString().
        assert(false);
    }
}

#endif
