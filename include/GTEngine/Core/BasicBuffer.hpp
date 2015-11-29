// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_BasicBuffer_hpp_
#define __GT_BasicBuffer_hpp_

#include <cstddef>

namespace GT
{
    /// Simple structure representing a buffer.
    struct BasicBuffer
    {
        /// @Default constructor.
        ///
        /// @remarks
        ///     This will create an empty buffer where \c this->ptr will be nullptr and \c this->size will be 0.
        BasicBuffer();

        /// Copy constructor.
        ///
        /// @remarks
        ///     This will make a copy of the other buffer, including the internal data. Each buffer can be used independently of each other. 
        BasicBuffer(const BasicBuffer &other);
            
        /// Copy constructor.
        ///
        /// @remarks
        ///     This will make a copy of \c ptr.
        BasicBuffer(const void *ptr, size_t size);

        /// Destructor.
        ///
        /// @remarks
        ///     This will deallocate the internal buffer.
        ~BasicBuffer();
            
        /// Resizes the buffer for the given size.
        ///
        /// @param  size          [in] The new size of the buffer.
        /// @param  tightlyPacked [in] Specifies if the buffer should be shrunk if \c size is smaller than the current size. False by default.
        ///
        /// @return                    A pointer to the internal buffer.
        ///
        /// @remarks
        ///     If \c size if smaller than \c this->size and \c tightlyPacked is false (the default), the buffer will be left unmodified.
        ///     @par
        ///     A buffer size of 0 is invalid and will cause Allocate() to return nullptr and \c this->ptr to be set to nullptr.
        void * Allocate(size_t size, bool tightlyPacked = false);

        /// A helper method for freeing the entire buffer.
        void Free() { this->Allocate(0, true); }
            

        /// Retrieves a pointer to the internal data buffer.
              void* GetDataPointer()       { return m_ptr; }
        const void* GetDataPointer() const { return m_ptr; }

        /// Retrieves the size in bytes of the internal data buffer.
        size_t GetDataSizeInBytes() const { return m_size; }
        

            
        /// Assigns the content of this buffer to that of another.
        ///
        /// @param  other [in] A reference to the source buffer.
        BasicBuffer & operator=(const BasicBuffer &other);
            
            
    private:
            
        /// A pointer to the start of the buffer.
        void *m_ptr;
            
        /// The size of the buffer.
        size_t m_size;
    };
}


#endif
