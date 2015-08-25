// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_Serializer
#define GT_Serializer

#include <cstdlib>
#include <cstring>      // For memcpy()
#include "Math.hpp"     // For GTLib::Max().
#include "String.hpp"   // For GTLib::String specialization.
#include "IO.hpp"       // For FILE.

namespace GTLib
{
    /// Class for doing basic serialization.
    ///
    /// This works in the same kind of way as a file stream.
    class Serializer
    {
    public:

        /// Default constructor.
        Serializer();

        /// Destructor.
        virtual ~Serializer();


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
        void WriteString(const char* value, ptrdiff_t sizeInTs = -1);

        /// @copydoc GTLib::Serializer::WriteString(const char*, ptrdiff_t)
        void WriteString(const GTLib::String &value);
    };

    template <> inline void Serializer::Write<GTLib::String>(const GTLib::String &);




    /// Class for doing basic serialization.
    ///
    /// This works in the same kind of way as a file stream.
    class BasicSerializer : public Serializer
    {
    public:

        /// Default constructor.
        BasicSerializer();

        /// Copy constructor.
        BasicSerializer(const BasicSerializer &other);

        /// Constructor.
        BasicSerializer(size_t bufferSizeInBytesIn);

        /// Destructor.
        virtual ~BasicSerializer();


        /// Serializer::Write().
        void Write(const void* bufferIn, size_t bufferInSizeInBytes);


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
        void Clear();


        /// Assignment operator
        BasicSerializer & operator= (const BasicSerializer &other);


    private:

        /// Allocates additional space into the buffer.
        ///
        /// @param newBufferSizeInBytes [in] The new size of the buffer, in bytes.
        void Allocate(size_t newBufferSizeInBytes);

        /// Retrieves the amount of space available in the buffer.
        size_t GetAvailableBufferSpaceInBytes() const;

        /// Checks if there is enough space for the given number of bytes and allocates additional space if not.
        void CheckAndAllocate(size_t bytesToAdd);


    private:

        /// A pointer to the buffer containing the data.
        void* buffer;

        /// The size of the buffer in bytes.
        size_t bufferSizeInBytes;

        /// The position of the writing pointer.
        size_t writePointer;
    };

    template <> inline void BasicSerializer::Write<GTLib::String>(const GTLib::String &);





    /// Class for doing serialization from a file.
    ///
    /// This class does not open or close the file - that should be done at a higher level. The reason for this is that it makes it makes things
    /// a lot more flexible.
    ///
    /// This will move the files write pointer as data is written.
    class FileSerializer : public Serializer
    {
    public:

        /// Default constructor.
        FileSerializer(FILE* fileIn);

        /// Constructor.
        FileSerializer(GTLib::FileHandle fileIn);

        /// Destructor.
        virtual ~FileSerializer();


        /// Serializer::Write().
        void Write(const void* bufferIn, size_t bufferInSizeInBytes);


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

        /// The standard C file to write to. If set to null, fileGT must be non-zero.
        FILE* fileSTD;

        /// The GT file handle object to write to. If this is zero, fileSTD must be non-null.
        FileHandle fileGT;


    private:    // No copying
        FileSerializer(const FileSerializer &);
        FileSerializer & operator=(const FileSerializer &);
    };

    template <> inline void FileSerializer::Write<GTLib::String>(const GTLib::String &);
}

#endif
