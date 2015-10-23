// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Deserializer_hpp_
#define __GTLib_Deserializer_hpp_

#include <cstring>      // For memcpy()
#include "String.hpp"
#include "IO.hpp"
#include "Math.hpp"     // For GTLib::Min().
#include "Vector.hpp"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4482)   // 'this' used in initialise list.
#endif

namespace GTLib
{
    // TODO: The new chunk stack system needs testing!

    /// A very basic class for doing deserialization.
    class Deserializer
    {
    public:

        /// Constructor.
        Deserializer()
        {
        }

        /// Destructor.
        virtual ~Deserializer()
        {
        }


        /// @copydoc GTLib::Deserializer::ReadImpl
        size_t Read(void* outputBuffer, size_t bytesToRead)
        {
            if (this->HasRoomInChunk(bytesToRead))
            {
                return this->ReadImpl(outputBuffer, bytesToRead);
            }

            return 0;
        }

        /// @copydoc GTLib::Deserializer::PeekImpl
        size_t Peek(void* outputBuffer, size_t bytesToRead)
        {
            if (this->HasRoomInChunk(bytesToRead))
            {
                return this->PeekImpl(outputBuffer, bytesToRead);
            }

            return 0;
        }

        /// @copydoc GTLib::Deserializer::SeekImpl
        int64_t Seek(int64_t bytesToSkip)
        {
            if (this->HasRoomInChunk(bytesToSkip))
            {
                return this->SeekImpl(bytesToSkip);
            }

            return 0;
        }

        /// @copydoc GTLib::Deserializer::TellImpl
        size_t Tell() const
        {
            return this->TellImpl();
        }



        /// Reads a value from the buffer.
        ///
        /// @param object [in] A reference to the object that will receive the data.
        ///
        /// @remarks
        ///     This does a simple memcpy() into the buffer.
        template <typename T>
        size_t Read(T &object)
        {
            return this->Read(&object, sizeof(T));
        }

        /// Reads a string.
        ///
        /// @remarks
        ///     This performs the exact opposite of Serializer::WriteString().
        size_t ReadString(GTLib::String &object)
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


        /// Marks the beginning of the reading of a protected chunk of data.
        ///
        /// @param chunkSizeInBytes [in] The size of the chunk, in bytes.
        ///
        /// @remarks
        ///     Internally, this is a stack. This method simply pushes a new item onto the stack which will remain the current one
        ///     until it is popped with EndChunk(). Reading will never go beyond the size of the chunk.
        ///     @par
        ///     When the item is popped with EndChunk(), the current read position will be seeked to the end of the chunk if it is
        ///     not already there.
        void StartChunk(size_t chunkSizeInBytes)
        {
            m_chunkStack.PushBack(_ChunkStackItem(this->Tell() + chunkSizeInBytes));
        }

        /// Marks the end of the reading of a protected chunk of data.
        ///
        /// @remarks
        ///     If the current read position is not sitting at the end of the chunk, it will be seeked to it.
        void EndChunk()
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

        /// Determines whether or not there is room in the chunk for reading the given amount of bytes.
        ///
        /// @param bytesToRead [in] The number of bytes to check for.
        ///
        /// @return True if there is at least the given amount of bytes remaining in the top chunk; false otherwise.
        ///
        /// @remarks
        ///     This will always return true if there is no chunk on the stack.
        bool HasRoomInChunk(int64_t bytesToRead) const
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


    protected:

        /// Generic method for reading a number of bytes from the buffer.
        ///
        /// @param outputBuffer [out] A pointer to the buffer that will receive the data.
        /// @param bytesToRead  [in]  The number of bytes to read.
        ///
        /// @remarks
        ///     The return value should be the amount of bytes read. If 'bytesToRead' is larger than the amount of space remaining
        ///     in the source, it will read up until the end.
        virtual size_t ReadImpl(void* outputBuffer, size_t bytesToRead) = 0;

        /// Generic method for retrieving data, but keeping the read pointer in place.
        ///
        /// @param outputBuffer [out] A pointer to the buffer that will receive the data.
        /// @param bytesToRead  [in]  The number of bytes to read.
        ///
        /// @remarks
        ///     This method works exactly the same as Read(), except it does not move the read pointer.
        virtual size_t PeekImpl(void* outputBuffer, size_t bytesToRead) = 0;

        /// Seeks past the given number of bytes.
        ///
        /// @param bytesToSkip [in] The number of bytes to skip.
        ///
        /// @return The number of bytes that were seeked past.
        ///
        /// @remarks
        ///     This can only seek forward.
        ///     @par
        ///     If there is not enough room to seek, this will completely fail (it will not do a partial seek).
        virtual int64_t SeekImpl(int64_t bytesToSkip) = 0;

        /// Retrieves the position of the read pointer.
        ///
        /// @remarks
        ///     This can be used in determining how many bytes have been read at a certain point.
        virtual size_t TellImpl() const = 0;


    private:

        /// Structure representing an item in the chunk barrier stack.
        struct _ChunkStackItem
        {
            _ChunkStackItem(intptr_t endPosition)
                : m_endPosition(endPosition)
            {
            }

            /// The target position. Reading will never go beyond this point when this item is at the top of the stack.
            intptr_t m_endPosition;
        };


        /// The stack of chunk sizes for protecting the reading of data.
        GTLib::Vector<_ChunkStackItem> m_chunkStack;
    };

    template <> inline size_t Deserializer::Read<GTLib::String>(GTLib::String &)
    {
        // Must use ReadString().
        assert(false);
        return 0;
    }




    /// A basic deserializer that simply reads from a memory buffer.
    ///
    /// To use this class, simply provide a pointer to a buffer and the size of the buffer and then call Read().
    ///
    /// This class will NOT create a copy of the buffer.
    class BasicDeserializer : public Deserializer
    {
    public:

        /// Constructor.
        BasicDeserializer(const void* bufferIn, size_t bufferSizeInBytesIn)
            : buffer(bufferIn), bufferSizeInBytes(bufferSizeInBytesIn), readPointer(0)
        {
        }

        /// Destructor.
        virtual ~BasicDeserializer()
        {
        }


        /// Reads a value from the buffer.
        ///
        /// @param object [in] A reference to the object that will receive the data.
        ///
        /// @remarks
        ///     This does a simple memcpy() into the buffer.
        template <typename T>
        size_t Read(T &object)
        {
            return this->Read(&object, sizeof(T));
        }



    private:

        /// Deserializer::Read().
        size_t ReadImpl(void* outputBuffer, size_t bytesToRead)
        {
            bytesToRead = this->Peek(outputBuffer, bytesToRead);
            this->readPointer += bytesToRead;

            return bytesToRead;
        }

        /// Deserializer::Peek().
        size_t PeekImpl(void* outputBuffer, size_t bytesToRead)
        {
            // We can't read over the buffer.
            bytesToRead = GTLib::Min(bytesToRead, this->GetAvailableBufferSpaceInBytes());

            memcpy(outputBuffer, reinterpret_cast<const uint8_t*>(this->buffer) + this->readPointer, bytesToRead);

            return bytesToRead;
        }

        /// Deserializer::Seek().
        int64_t SeekImpl(int64_t bytesToSkip)
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

        /// Deserializer::Tell().
        size_t TellImpl() const
        {
            return this->readPointer;
        }


        /// Retrieves the amount of space reamining in the buffer.
        size_t GetAvailableBufferSpaceInBytes() const
        {
            return this->bufferSizeInBytes - this->readPointer;
        }



    private:

        /// The buffer that we're reading from.
        const void* buffer;

        /// The size of the buffer in bytes.
        size_t bufferSizeInBytes;

        /// The current position of the read pointer.
        size_t readPointer;


    private:    // No copying.
        BasicDeserializer(const BasicDeserializer &);
        BasicDeserializer & operator=(const BasicDeserializer &);
    };

    template <> inline size_t BasicDeserializer::Read<GTLib::String>(GTLib::String &)
    {
        // Must use ReadString().
        assert(false);
        return 0;
    }






    /// Class for deserialization from a file.
    ///
    /// This class does not open or close the file - that should be done at a higher level. The reason for this is that it makes it makes things
    /// a lot more flexible.
    ///
    /// This will move the files write pointer as data is written.
    class FileDeserializer : public Deserializer
    {
    public:

        /// Constructor.
        FileDeserializer(FILE* fileIn)
            : fileSTD(fileIn), fileGT(0)
        {
        }
        FileDeserializer(GTLib::FileHandle fileIn)
            : fileSTD(nullptr), fileGT(fileIn)
        {
        }

        /// Destructor.
        virtual ~FileDeserializer()
        {
        }


        /// Reads a value from the buffer.
        ///
        /// @param object [in] A reference to the object that will receive the data.
        template <typename T>
        size_t Read(T &object)
        {
            return this->Read(&object, sizeof(T));
        }


    private:

        /// Deserializer::Read().
        size_t ReadImpl(void* outputBuffer, size_t bytesToRead)
        {
            assert(this->fileSTD != nullptr || this->fileGT != 0);
            assert(this->fileSTD == nullptr || this->fileGT == 0);


            if (this->fileSTD != nullptr)
            {
                return IO::Read(this->fileSTD, outputBuffer, bytesToRead);
            }
            else
            {
                return GTLib::ReadFile(this->fileGT, outputBuffer, bytesToRead);
            }
        }

        /// Deserializer::Peek().
        size_t PeekImpl(void* outputBuffer, size_t bytesToRead)
        {
            assert(this->fileSTD != nullptr || this->fileGT != 0);
            assert(this->fileSTD == nullptr || this->fileGT == 0);


            if (this->fileSTD != nullptr)
            {
                auto readPointer = IO::Tell(this->fileSTD);

                size_t bytesRead = IO::Read(this->fileSTD, outputBuffer, bytesToRead);
                IO::Seek(this->fileSTD, readPointer, SeekOrigin::Start);

                return bytesRead;
            }
            else
            {
                auto readPointer = GTLib::GetFilePointer(this->fileGT);

                intptr_t bytesRead = GTLib::ReadFile(this->fileGT, outputBuffer, bytesToRead);
                if (bytesRead != -1)
                {
                    SeekFile(this->fileGT, readPointer, SeekOrigin::Start);

                    return bytesRead;
                }
                else
                {
                    return 0;
                }
            }
        }

        /// Deserializer::Seek().
        int64_t SeekImpl(int64_t bytesToSkip)
        {
            assert(this->fileSTD != nullptr || this->fileGT != 0);
            assert(this->fileSTD == nullptr || this->fileGT == 0);


            if (this->fileSTD != nullptr)
            {
                if (IO::Seek(this->fileSTD, bytesToSkip, SeekOrigin::Current))
                {
                    return bytesToSkip;
                }
            }
            else
            {
                int64_t prevFilePointer = GTLib::GetFilePointer(this->fileGT);
                int64_t newFilePointer  = GTLib::SeekFile(this->fileGT, bytesToSkip, SeekOrigin::Current);
                if (newFilePointer != -1)
                {
                    // For consistency with the STD method, we want to skip all of bytesToSkip, or none. Nothing in between.
                    if ((newFilePointer - prevFilePointer) == bytesToSkip)
                    {
                        return bytesToSkip;
                    }
                    else
                    {
                        GTLib::SeekFile(this->fileGT, prevFilePointer, SeekOrigin::Start);
                    }
                }
            }


            return 0;
        }

        /// Deserializer::Tell().
        size_t TellImpl() const
        {
            assert(this->fileSTD != nullptr || this->fileGT != 0);
            assert(this->fileSTD == nullptr || this->fileGT == 0);


            if (this->fileSTD != nullptr)
            {
                return static_cast<size_t>(IO::Tell(this->fileSTD));
            }
            else
            {
                return static_cast<size_t>(GTLib::GetFilePointer(this->fileGT));
            }
        }


    private:

        /// The standard C file to read from. If set to null, fileGT must be non-zero.
        FILE* fileSTD;

        /// The GT file handle object to read from. If this is zero, fileSTD must be non-null.
        FileHandle fileGT;


    private:    // No copying.
        FileDeserializer(const FileDeserializer &);
        FileDeserializer & operator=(const FileDeserializer &);
    };

    template <> inline size_t FileDeserializer::Read<GTLib::String>(GTLib::String &)
    {
        // Must use ReadString().
        assert(false);
        return 0;
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
