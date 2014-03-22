// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_RCCache_hpp_
#define __GTEngine_Rendering_RCCache_hpp_

#include <GTLib/Vector.hpp>
#include <new>

namespace GTEngine
{
    /// Container class designed specifically for caching rendering commands. Note that this is not a generic cache of
    /// RenderCommand pointers, but instead a cache of <T> render commands.
    ///
    /// A cached render command is acquired with Acquire(). There is no unacquire. The cache is reset with Reset(). Reset()
    /// runs in O(1) time, so it's quick. Resetting does not deallocate internal buffers.
    ///
    /// The workflow with these caches is to keep acquiring objects, and then reset it when they are no longer needed. You
    /// do not "deallocate" acquired objects.
    ///
    /// When a new render command is acquired, it will be a default constructed object. When acquiring, the old object will
    /// have its destructor called, and then the default constructor will be applied.
    ///
    /// The cache is built up of blocks. When the cache runs out of room internally, it allocates an extra block to account.
    /// This system is useful since it means a copy can be avoided when resizing. The size of each block is constant, but
    /// can be defined on a per-instantiation basis using the <BlockSize> template argument. This is 128 by default. The
    /// block size is in <T>'s.
    template <typename T, size_t BlockSize = 128>
    class RCCache
    {
    public:

        /// Constructor. This allocates a single block.
        RCCache()
            : blocks(), acquiredCount(0)
        {
            this->PushNewBlock();
        }

        /// Destructor.
        ~RCCache()
        {
            for (size_t i = 0; i < this->blocks.count; ++i)
            {
                auto block = this->blocks[i];

                // Since we manipulate the block buffers directly, we need to iterate over the whole buffer
                // and call the destructors. The vectors themselves won't do this because they aren't aware
                // that they are filled with valid objects.
                for (size_t j = 0; j < BlockSize; ++j)
                {
                    block->buffer[j].~T();
                }

                delete block;
            }
        }

        /// Acquires a new T object. This returns a reference to the object.
        T & Acquire()
        {
            // First we need to make sure there's room for the new object.
            if (this->acquiredCount == this->GetBufferSize())
            {
                this->PushNewBlock();
            }

            // The index of the new object is going to be equal to this->acquiredCount, before incrementing. With this number
            // we can quickly grab the block index and block sub index.
            size_t blockIndex    = this->acquiredCount / BlockSize;
            size_t blockSubIndex = this->acquiredCount - (blockIndex * BlockSize);

            auto blockPtr = this->blocks[blockIndex]->buffer + blockSubIndex;

            // Here is where we destruct the old, and default construct the new.
            blockPtr->~T();
            new (blockPtr) T();

            ++this->acquiredCount;

            return *blockPtr;
        }

        /// Resets the cache so that acquiring restarts from the start of the internal buffers again.
        void Reset()
        {
            this->acquiredCount = 0;
        }


    private:

        /// Retrieves the number of acquired Ts.
        size_t GetAcquiredCount() const
        {
            return this->acquiredCount;
        }

        /// Retrieves the total size of the cache's internal buffers.
        size_t GetBufferSize() const
        {
            return this->blocks.count * BlockSize;
        }


        /// Pushes a new block to the end of the block array.
        void PushNewBlock()
        {
            auto newBlock = new GTLib::Vector<T>(BlockSize);   // <-- <BlockSize> here specifies the size of the internal buffer.

            // It would be tempting to use newBlock->Resize(BlockSize) to allocate default Ts, however that will require copy
            // constructors on the render commands. To resolve, we're going to manipulate the buffer directly. The vectors will
            // think there are no items in it, but we know better.
            for (size_t i = 0; i < BlockSize; ++i)
            {
                new (newBlock->buffer + i) T();
            }

            blocks.PushBack(newBlock);
        }


    private:

        /// The array of blocks. We need random access here, so a vector is appropriate.
        GTLib::Vector<GTLib::Vector<T>*> blocks;

        /// The number of Ts that are acquired.
        size_t acquiredCount;
    };
}

#endif