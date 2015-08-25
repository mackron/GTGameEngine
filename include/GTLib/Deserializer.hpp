// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_Deserializer
#define GT_Deserializer

#include "String.hpp"
#include "IO.hpp"
#include "Vector.hpp"

namespace GTLib
{
    // TODO: The new chunk stack system needs testing!

    /// A very basic class for doing deserialization.
    class Deserializer
    {
    public:

        /// Constructor.
        Deserializer();

        /// Destructor.
        virtual ~Deserializer();


        /// @copydoc GTLib::Deserializer::ReadImpl
        size_t Read(void* outputBuffer, size_t bytesToRead);

        /// @copydoc GTLib::Deserializer::PeekImpl
        size_t Peek(void* outputBuffer, size_t bytesToRead);

        /// @copydoc GTLib::Deserializer::SeekImpl
        int64_t Seek(int64_t bytesToSkip);

        /// @copydoc GTLib::Deserializer::TellImpl
        size_t Tell() const;



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
        size_t ReadString(GTLib::String &object);


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
        void StartChunk(size_t chunkSizeInBytes);

        /// Marks the end of the reading of a protected chunk of data.
        ///
        /// @remarks
        ///     If the current read position is not sitting at the end of the chunk, it will be seeked to it.
        void EndChunk();

        /// Determines whether or not there is room in the chunk for reading the given amount of bytes.
        ///
        /// @param bytesToRead [in] The number of bytes to check for.
        ///
        /// @return True if there is at least the given amount of bytes remaining in the top chunk; false otherwise.
        ///
        /// @remarks
        ///     This will always return true if there is no chunk on the stack.
        bool HasRoomInChunk(int64_t bytesToRead) const;


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

    template <> inline size_t Deserializer::Read<GTLib::String>(GTLib::String &);




    /// A basic deserializer that simply reads from a memory buffer.
    ///
    /// To use this class, simply provide a pointer to a buffer and the size of the buffer and then call Read().
    ///
    /// This class will NOT create a copy of the buffer.
    class BasicDeserializer : public Deserializer
    {
    public:

        /// Constructor.
        BasicDeserializer(const void* bufferIn, size_t bufferSizeInBytesIn);

        /// Destructor.
        virtual ~BasicDeserializer();


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
        size_t ReadImpl(void* outputBuffer, size_t bytesToRead);

        /// Deserializer::Peek().
        size_t PeekImpl(void* outputBuffer, size_t bytesToRead);

        /// Deserializer::Seek().
        int64_t SeekImpl(int64_t bytesToSkip);

        /// Deserializer::Tell().
        size_t TellImpl() const;

        /// Retrieves the amount of space reamining in the buffer.
        size_t GetAvailableBufferSpaceInBytes() const;



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

    template <> inline size_t BasicDeserializer::Read<GTLib::String>(GTLib::String &);






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
        FileDeserializer(FILE* fileIn);

        /// Destrucutor.
        FileDeserializer(GTLib::FileHandle fileIn);

        /// Destructor.
        virtual ~FileDeserializer();


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
        size_t ReadImpl(void* outputBuffer, size_t bytesToRead);

        /// Deserializer::Peek().
        size_t PeekImpl(void* outputBuffer, size_t bytesToRead);

        /// Deserializer::Seek().
        int64_t SeekImpl(int64_t bytesToSkip);

        /// Deserializer::Tell().
        size_t TellImpl() const;


    private:

        /// The standard C file to read from. If set to null, fileGT must be non-zero.
        FILE* fileSTD;

        /// The GT file handle object to read from. If this is zero, fileSTD must be non-null.
        FileHandle fileGT;


    private:    // No copying.
        FileDeserializer(const FileDeserializer &);
        FileDeserializer & operator=(const FileDeserializer &);
    };

    template <> inline size_t FileDeserializer::Read<GTLib::String>(GTLib::String &);
}

#endif
