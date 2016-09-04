// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_BufferReader
#define GT_BufferReader

namespace GT
{
    /// A helper class for safely reading a data buffer and ensuring it doesn't try reading past the buffer allocation.
    class BufferReader
    {
    public:

        /// Constructor.
        BufferReader(const void* buffer, size_t bufferSizeInBytes)
            : m_buffer(reinterpret_cast<const uint8_t*>(buffer)),
              m_bufferSizeInBytes(bufferSizeInBytes),
              m_currentPos(0)
        {
        }

        /// Destructor.
        ~BufferReader()
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


        /// Reads the given number of bytes.
        ///
        /// @param bufferOut   [out] A pointer to the buffer to copy the data into.
        /// @param bytesToRead [in]  The number of bytes to read.
        ///
        /// @return The number of bytes that were read.
        size_t Read(void* bufferOut, size_t bytesToRead)
        {
            if (bytesToRead > this->GetAvailableBufferSpaceInBytes())
            {
                bytesToRead = this->GetAvailableBufferSpaceInBytes();
            }

            memcpy(bufferOut, m_buffer + m_currentPos, bytesToRead);
            m_currentPos += bytesToRead;

            return bytesToRead;
        }


        /// Retrieves the position of the read pointer.
        ///
        /// @remarks
        ///     This can be used in determining how many bytes have been read at a certain point.
        size_t Tell() const
        {
            return m_currentPos;
        }

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
        int64_t Seek(int64_t bytesToSkip)
        {
            // We can't skip past the buffer, nor can we go before it.
            int64_t targetPointer = static_cast<int64_t>(m_currentPos) + bytesToSkip;
            if (targetPointer >= static_cast<int64_t>(m_bufferSizeInBytes))
            {
                bytesToSkip = this->GetAvailableBufferSpaceInBytes();
            }
            else if (targetPointer < 0)
            {
                assert(bytesToSkip <= targetPointer);
                bytesToSkip = bytesToSkip - targetPointer;
            }

            m_currentPos += static_cast<ptrdiff_t>(bytesToSkip);

            return bytesToSkip;
        }


        /// Finds the next occurance the given byte.
        ///
        /// @param value       [in]  The byte value to search for.
        /// @param positionOut [out] A reference to the variable that will receive the position in the buffer of the byte value.
        ///
        /// @remarks
        ///     This is mainly intended to find a null-terminator for strings.
        bool FindNextByte(uint8_t value, size_t &positionOut)
        {
            size_t endPos = m_currentPos;
            while (endPos < m_bufferSizeInBytes)
            {
                if (m_buffer[endPos] == value)
                {
                    // Found the null-terminator.
                    positionOut = endPos;
                    return true;
                }

                endPos += 1;
            }

            return false;
        }


        /// Retrieves the amount of space, in bytes, remaining in the buffer.
        size_t GetAvailableBufferSpaceInBytes() const
        {
            return m_bufferSizeInBytes - m_currentPos;
        }


    private:

        /// A pointer to the start of the buffer.
        const uint8_t* m_buffer;

        /// The size of the buffer.
        size_t m_bufferSizeInBytes;

        /// The current position inside the buffer.
        size_t m_currentPos;
    };
}

#endif
