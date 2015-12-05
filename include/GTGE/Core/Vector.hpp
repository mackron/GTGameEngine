// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Vector
#define GT_Vector

#include <GTGE/Core/Math.hpp>
#include <GTGE/Core/stdlib.hpp>

#include <cstddef>
#include <cassert>
#include <new>
#include <functional>
#include <algorithm>

namespace GT
{
    template <typename T>
    class Vector
    {
    public:

        /// Constructor.
        Vector()
            : count(0), bufferSize(0), buffer(nullptr)
        {
        }

        /// Copy constructor.
        Vector(const Vector &other)
            : count(0), bufferSize(other.count), buffer((other.count > 0) ? static_cast<T*>(malloc(bufferSize * sizeof(T))) : nullptr)
        {
            for (size_t i = 0; i < other.count; ++i)
            {
                this->PushBack(other[i]);
            }
        }

        /// Constructor.
        Vector(size_t bufferSize)
            : count(0), bufferSize(bufferSize), buffer((bufferSize > 0) ? static_cast<T*>(malloc(bufferSize * sizeof(T))) : nullptr)
        {
        }

        /// Destructor.
        ~Vector()
        {
            for (size_t i = 0; i < this->count; ++i)
            {
                this->buffer[i].~T();
            }

            free(this->buffer);
        }

        /**
        *   \brief Appends an value to the end of the vector.
        */
        void PushBack(const T &value)
        {
            assert(this->bufferSize >= this->count);

            if (this->bufferSize == this->count)
            {
                this->Reserve(Max(static_cast<size_t>(1), this->count * 2));
            }

            new (this->buffer + this->count) T(value);

            ++this->count;
        }

        /**
        *   \brief  Removes the end value.
        *
        *   \remarks
        *       For efficiency, this does not downsize the buffer. If this is required, use Reserve() or OptimizeBuffer().
        */
        void PopBack()
        {
            // We can easily do a conditional branch here, but this situation almost certainly means the host application has
            // a bug somewhere. We'll do them a favour and crash :-)
            assert(this->count > 0);

            --this->count;
            this->buffer[this->count].~T();
        }


        /// Inserts an item at the given index.
        ///
        /// @param value [in] The value to insert.
        /// @param index [in] The index to insert the item at.
        ///
        /// @remarks
        ///     The index must be in the range of [0, this->count]
        void InsertAt(const T &value, size_t insertIndex)
        {
            assert(this->bufferSize >= this->count);
            assert(insertIndex      <= this->count);

            if (this->bufferSize == this->count)
            {
                // This is the annoying case where we need to do a resize. We double the size of the buffer here.
                size_t newBufferSize = Max(static_cast<size_t>(1), this->count * 2);

                T* oldBuffer = this->buffer;
                T* newBuffer = static_cast<T*>(malloc(newBufferSize * sizeof(T)));

                // 1
                for (size_t i = 0; i < insertIndex; ++i)
                {
                    new (newBuffer + i) T(oldBuffer[i]);
                }

                // 2
                new (newBuffer + insertIndex) T(value);

                // 3
                for (size_t i = insertIndex; i < this->count; ++i)
                {
                    new (newBuffer + i + 1) T(oldBuffer[i]);
                }


                this->buffer     = newBuffer;
                this->bufferSize = newBufferSize;


                // Finally, the old buffer needs to be deleted. We also need to call destructors here, unfortunately.
                for (size_t i = 0; i < this->count; ++i)
                {
                    oldBuffer[i].~T();
                }
                free(oldBuffer);
            }
            else
            {
                // There is no need to resize here. All we need to do is move everything behind the insertion index over by 1, remembering to call destructors.
                for (size_t i = this->count; i > insertIndex; --i)
                {
                    new (this->buffer + i) T(this->buffer[i - 1]);
                    this->buffer[i - 1].~T();
                }

                new (this->buffer + insertIndex) T(value);
            }

            ++this->count;
        }


        /// Removes the item at the given index, moving everything after it downwards.
        ///
        /// @param index [in] The index of the item to remove.
        void Remove(size_t index)
        {
            // Just like PopBack(), we will prefer a crash to a run-time conditional branch since the host application probably has a bug somewhere in this case.
            assert(index < this->count);

            // The destructor of the old item must be called.
            this->buffer[index].~T();

            // We just move everything after it downwards.
            for (size_t i = index; i < this->count - 1; ++i)
            {
                new (this->buffer + i) T(this->buffer[i + 1]);
                this->buffer[i + 1].~T();
            }

            --this->count;
        }


        /// Removes the first occurance of the given item.
        ///
        /// @param value [in] A reference to the item to remove.
        void RemoveFirstOccuranceOf(const T &value)
        {
            auto index = this->FindFirst(value);
            if (index != static_cast<size_t>(-1))
            {
                this->Remove(index);
            }
        }


        /**
        *   \brief  Retrieves the number of items in the vector.
        */
        size_t GetCount() const
        {
            return this->count;
        }


        /// Determines whether of not the vector is empty.
        bool IsEmpty() const
        {
            return this->count == 0;
        }


        /**
        *   \brief               Resizes the internal buffer.
        *   \param  newSize [in] The new size of the buffer. If this is smaller than the current count, nothing will be done.
        *
        *   \remarks
        *       You can use this to reduce the size of the buffer.
        */
        void Reserve(size_t newBufferSize)
        {
            if (newBufferSize != this->bufferSize)        // <-- Don't bother doing anything if the new size is the same as the current size.
            {
                if (newBufferSize < this->count)
                {
                    newBufferSize = this->count;
                }

                // First we need to allocate a new buffer, making sure we keep a hold of the old buffer so we can
                // copy the existing events back over to the new buffer.
                T* oldBuffer = this->buffer;
                T* newBuffer = static_cast<T*>(malloc(newBufferSize * sizeof(T)));

                // If we have items, they need to be copied over.
                for (size_t i = 0; i < this->count; ++i)
                {
                    new (newBuffer + i) T(oldBuffer[i]);
                }

                this->buffer     = newBuffer;
                this->bufferSize = newBufferSize;

                // Finally, the old buffer needs to be deleted. We also need to call destructors here, unfortunately.
                for (size_t i = 0; i < this->count; ++i)
                {
                    oldBuffer[i].~T();
                }
                free(oldBuffer);
            }
        }

        /**
        *   \brief  Tightens the buffer so that it's only just large enough to fit the current number of items.
        */
        void OptimizeBuffer()
        {
            this->Reserve(Max(this->count, static_cast<size_t>(1U)));
        }


        /**
        *   \brief             Resizes the vector to contain <count> elements. Uses the default constructor for allocation.
        *   \param  count [in] The number of elements to include in the vector.
        *
        *   \remarks
        *       Note that this is different to Reserve() in that it actually adds the objects to the vector. Reserve() simply
        *       reserves a block of memory for the internal buffer.
        */
        void Resize(size_t newCount)
        {
            if (newCount < this->count)
            {
                size_t numTsToDelete = this->count - newCount;

                for (size_t i = 0; i < numTsToDelete; ++i)
                {
                    this->buffer[newCount + i].~T();
                }
            }
            else
            {
                // May need to resize...
                if (newCount > this->bufferSize)
                {
                    this->Reserve(newCount);
                }

                size_t numTsToAdd = newCount - this->count;

                for (size_t i = 0; i < numTsToAdd; ++i)
                {
                    new (this->buffer + this->count + i) T();
                }
            }

            this->count = newCount;
        }


        /**
        *   \brief  Empties the vector.
        *
        *   \remarks
        *       This runs in linear time because the destructors must be called. I'm assuming compilers will be smart enough to optimize out
        *       the destructor loop for basic types. Should do a disassemle and test that...
        */
        void Clear()
        {
            for (size_t i = 0U; i < this->count; ++i)
            {
                this->buffer[i].~T();
            }

            this->count = 0;
        }

        /**
        *   \brief             Removes every item except for the item at the given index.
        *   \param  index [in] The index of the item to keep.
        *
        *   \remarks
        *       The kept item will be moved to the front of the vector.
        */
        void RemoveAllExcept(size_t index)
        {
            assert(index < this->count);

            // Before...
            for (size_t i = 0U; i < index; ++i)
            {
                this->buffer[i].~T();
            }

            // After...
            for (size_t i = index + 1; i < this->count; ++i)
            {
                this->buffer[i].~T();
            }

            // Move to front...
            if (index > 0)
            {
                new (this->buffer) T(this->buffer[index]);
                this->buffer[index].~T();
            }

            this->count = 1;
        }


        /**
        *   \brief  Retrieves the last item in the vector.
        */
              T & GetBack()       { assert(this->count > 0); return this->buffer[this->count - 1]; }
        const T & GetBack() const { assert(this->count > 0); return this->buffer[this->count - 1]; }

        /**
        *   \brief  Retrieves the first item in the vector.
        */
              T & GetFront()       { assert(this->count > 0); return this->buffer[0]; }
        const T & GetFront() const { assert(this->count > 0); return this->buffer[0]; }


        /**
        *   \brief             Retrieves an item by its index.
        *   \param  index [in] The index of the item to retrieve.
        */
        template <typename U>
        T & Get(U index)
        {
            assert(static_cast<size_t>(index) < this->count);
            return this->buffer[index];
        }

        template <typename U>
        const T & Get(U index) const
        {
            assert(static_cast<size_t>(index) < this->count);
            return this->buffer[index];
        }


        /**
        *   \brief  Determines if an item in the vector is equal to the given value.
        */
        bool Exists(const T &value) const
        {
            return this->FindFirst(value) != static_cast<size_t>(-1);
        }

        /// Determines if an item in the vector is equal to the given value, starting from the end and working towards the start.
        bool ExistsReverse(const T &value) const
        {
            for (size_t i = this->count; i > 0; --i)
            {
                if (this->buffer[i - 1] == value)
                {
                    return true;
                }
            }

            return false;
        }

        /// Retrieves the index of the first occurance of the given item.
        ///
        /// @param value [in] The item to find.
        ///
        /// @return The index of the given item, or -1 if the item does not exist.
        size_t FindFirst(const T &value) const
        {
            for (size_t i = 0; i < this->count; ++i)
            {
                if (this->buffer[i] == value)
                {
                    return i;
                }
            }

            return static_cast<size_t>(-1);
        }

        /// Retrieves the index of the first occurance of the given item.
        ///
        /// @param value    [in]  The item to find.
        /// @param indexOut [out] A reference to the variable that will receive the index of the item.
        ///
        /// @return True if the item was found; false otherwise.
        ///
        /// @remarks
        ///     If 'false' is returned, 'indexOut' will be left unmodified.
        bool FindFirstIndexOf(const T &value, size_t &indexOut) const
        {
            for (size_t i = 0; i < this->count; ++i)
            {
                if (this->buffer[i] == value)
                {
                    indexOut = i;
                    return true;
                }
            }

            return false;
        }


        /// Sorts the vector based on the given comparison function.
        ///
        /// @param compareFunc [in] The function to use for comparisons.
        ///
        /// @remarks
        ///     If compareFunc returns -1, a is assumed to be "lower" than b. If it return 0, it is assumed to be the same.
        void Sort(std::function<bool (const T &a, const T &b)> compareFunc)
        {
            std::sort(this->buffer, this->buffer + this->count, compareFunc);
        }


        /**
        *   \brief  Retrieves the value of an item based on it's index.
        */
        template <typename U>
        T & operator[](U index) { return this->Get(index); }

        template <typename U>
        const T & operator[](U index) const { return this->Get(index); }


        /// Assignment operator.
        Vector & operator=(const Vector &other)
        {
            if (&other != this)
            {
                this->Clear();

                for (size_t i = 0; i < other.count; ++i)
                {
                    this->PushBack(other[i]);
                }
            }

            return *this;
        }


    public:

        /// The number of items in the vector.
        size_t count;

        /// The size of the allocated array for the vector. This is not the number of items and will always
        /// be >= 'count'.
        size_t bufferSize;

        /// The buffer containing the the vector contents.
        T* buffer;
    };
}


#endif
