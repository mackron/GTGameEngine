// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_SortedVector_hpp_
#define __GTLib_SortedVector_hpp_

#include "stdlib.hpp"
#include <cstring>          // For memcpy().

namespace GTLib
{
    /// Class representing a sorted vector, in ascending order.
    ///
    /// This vector can contain multiple items of the same value.
    template <typename T>
    class SortedVector
    {
    public:

        /// Default constructor.
        SortedVector()
            : count(0), bufferSize(1), buffer(static_cast<T*>(malloc(bufferSize * sizeof(T))))
        {
        }

        /// Copy constructor.
        SortedVector(const SortedVector &other)
            : count(other.count), bufferSize(other.count), buffer(static_cast<T*>(malloc(bufferSize * sizeof(T))))
        {
            for (size_t i = 0; i < other.count; ++i)
            {
                new (this->buffer + i) T(other[i]);
            }
        }

        /// Constructor.
        SortedVector(size_t bufferSize)
            : count(0), bufferSize(bufferSize), buffer(static_cast<T*>(malloc(bufferSize * sizeof(T))))
        {
        }

        /// Destructor.
        ~SortedVector()
        {
            for (size_t i = 0; i < this->count; ++i)
            {
                this->buffer[i].~T();
            }

            free(this->buffer);
        }



        /// Inserts a value into the vector.
        ///
        /// @param value [in] The value to insert into the vector.
        void Insert(const T &value)
        {
            // We need to find the insertion point. We always insert after any existing items of the same value.
            size_t insertionPoint = this->FindInsertionPoint(value);

            // If there is not enough room for the item, we need to allocate a new buffer.
            if (this->count == this->bufferSize)
            {
                // We need to allocate a new buffer, leaving a space at the insertion point.
                auto newBufferSize = GTLib::Max(static_cast<size_t>(1), this->count * 2);

                auto oldBuffer = this->buffer;
                auto newBuffer = static_cast<T*>(malloc(newBufferSize * sizeof(T)));

                // Left of insertion point...
                for (size_t i = 0; i < insertionPoint; ++i)
                {
                    new (newBuffer + i) T(oldBuffer[i]);
                    oldBuffer[i].~T();
                }

                // Right of insertion point...
                for (size_t i = insertionPoint; i < this->count; ++i)
                {
                    new (newBuffer + i + 1) T(oldBuffer[i]);
                    oldBuffer[i].~T();
                }

                this->buffer     = newBuffer;
                this->bufferSize = newBufferSize;

                // Finally, the old buffer needs to be deleted. Destructors were already called in the loops above.
                free(oldBuffer);
            }
            else
            {
                // We just move everything after the insertion point upwards by 1.
                for (size_t i = this->count; i > insertionPoint; --i)
                {
                    new (this->buffer + i) T(this->buffer[i - 1]);
                    this->buffer[i - 1].~T();
                }
            }

            new (this->buffer + insertionPoint) T(value);
            ++this->count;
        }

        /// Removes the first occurance of the given value.
        ///
        /// @param value [in] The value of the item to remove.
        void RemoveFirstOccuranceOf(const T &value)
        {
            this->RemoveAt(this->FindFirst(value));
        }

        /// Removes every item of the given value.
        ///
        /// @param value [in] The value of the items to remove.
        void RemoveAll(const T &value)
        {
            this->RemoveRange(this->FindFirst(value), this->FindLast(value));
        }

        /// Removes the item at the given index.
        void RemoveAt(size_t index)
        {
            // The Find*() functions will return -1 if they don't exist, thus we will do an explicit check here.
            if (index != static_cast<size_t>(-1))
            {
                assert(index < this->count);

                // We first need to destruct the item being removed.
                this->buffer[index].~T();

                // Now we just move everything after it downwards.
                for (size_t i = index; i < this->count - 1; ++i)
                {
                    new (this->buffer + i) T(this->buffer[i + 1]);
                    this->buffer[i + 1].~T();
                }

                --this->count;
            }
        }

        /// Removes a range of items by index.
        ///
        /// @param startIndex [in] The first index in the range to remove.
        /// @param endIndex   [in] The last index in the range to remove.
        ///
        /// @remarks
        ///     Both <startIndex> and <endIndex> is inclusive.
        void RemoveRange(size_t startIndex, size_t endIndex)
        {
            if (startIndex != static_cast<size_t>(-1) && endIndex != static_cast<size_t>(-1))
            {
                assert(startIndex < this->count);
                assert(endIndex   < this->count);
                assert(startIndex < endIndex);

                size_t range = endIndex - startIndex + 1;

                // First we need to destruct the items being removed.
                for (size_t i = startIndex; i <= endIndex; ++i)
                {
                    this->buffer[i].~T();
                }

                // Now we move everything downwards.
                for (size_t i = startIndex; i < this->count - range; ++i)
                {
                    new (this->buffer + i) T(this->buffer[i + range]);
                    this->buffer[i + range].~T();
                }

                this->count -= range;
            }
        }


        /// Finds the index of an item of the given value.
        ///
        /// @param value [in] The value of the item to search for.
        ///
        /// @return The index of the item, or -1 if the item is not found.
        ///
        /// @remarks
        ///     There can be multiple items of the same value in the list. The specific item returned from this method is undefined. Use FindFirst()
        //      or FindLast() to retrieve the first and last of the items respectively.
        size_t Find(const T &value)
        {
            size_t result;
            if (this->Find(value, result))
            {
                return result;
            }
            else
            {
                return static_cast<size_t>(-1);
            }
        }

        /// Retrieves the index of the first item with the given value.
        ///
        /// @param value [in] The value to search for.
        ///
        /// @return The index of the first item of the given value, or -1 if the item is not found.
        size_t FindFirst(const T &value)
        {
            size_t i = this->Find(value);
            if (i != static_cast<size_t>(-1) && i > 0)
            {
                // We iterate backwards until we find the first item that is not equal to value.
                for ( ; i > 0; --i)
                {
                    if (!(this->buffer[i - 1] == value))
                    {
                        return i;
                    }
                }
            }

            return i;
        }

        /// Retrieves the index of the last item with the given value.
        ///
        /// @param value [in] The value to search for.
        ///
        /// @return The index of the last item of the given value, or -1 if the item is not found.
        size_t FindLast(const T &value)
        {
            size_t i = this->Find(value);
            if (i != static_cast<size_t>(-1) && i < this->count - 1)
            {
                for (i = i + 1; i < this->count; ++i)
                {
                    if (!(this->buffer[i] == value))
                    {
                        return i - 1;
                    }
                }
            }

            return i;
        }

        /// Finds the insertion point of the given value.
        ///
        /// @param value [in] The value whose insertion point is being found.
        ///
        /// @return The index where the given value can be placed.
        size_t FindInsertionPoint(const T &value)
        {
            size_t result;
            if (this->Find(value, result))
            {
                // We want to insert at the end of any items with the same value. To do this, we just keep scanning forward until
                // we find the first item without.
                for (result = result + 1; result < this->count; ++result)
                {
                    if (this->buffer[result] != value)
                    {
                        return result;
                    }
                }
            }

            return result;
        }


        /// Generic function for finding a value.
        ///
        /// @param value [in]  The value to search for.
        /// @param index [out] A reference to the variable that will receive the result.
        ///
        /// @return True if the value at the returned index is equal to <value>
        bool Find(const T &value, size_t &index)
        {
            if (this->count > 0)
            {
                // It's just so much easier using signed values here...
                ptrdiff_t low  = 0;
                ptrdiff_t high = static_cast<ptrdiff_t>(this->count - 1);   // <-- Safe cast as per the the check above.
                ptrdiff_t mid  = 0;

                ptrdiff_t offset = 0; // This will either be 0 or 1 and is added to the index if we don't find an exact value.
                while (low <= high)
                {
                    mid = (low + high) / 2;    // This will be floored.

                    if (value < this->buffer[mid])
                    {
                        high = mid - 1;
                        offset = 0;
                    }
                    else if (value > this->buffer[mid])
                    {
                        low = mid + 1;
                        offset = 1;
                    }
                    else
                    {
                        index = static_cast<size_t>(mid);
                        return true;
                    }
                }

                // If we have made it here, we weren't exact.
                index = static_cast<size_t>(mid + offset);
            }
            else
            {
                index = 0;
            }

            return false;
        }


        /// Determines whether or not the given item exists.
        ///
        /// @param value [in] The value of the item to check for.
        bool Exists(const T &value)
        {
            size_t devnull;
            return this->Find(value, devnull);
        }


        /// Empties the vector.
        ///
        /// @remarks
        ///     This runs in linear time because of destructors. I'm assuming compilers will optimize it out for basic types.
        void Clear()
        {
            for (size_t i = 0U; i < this->count; ++i)
            {
                this->buffer[i].~T();
            }

            this->count = 0;
        }


        /// Retrieves a reference to the item at the given index.
        ///
        /// @param index [in] The index of the item to retrieve.
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



        ///////////////////////////////////////////////////////////////
        // Operators.

        /// Assignment operator.
        SortedVector & operator=(const SortedVector &other)
        {
            if (this != &other)
            {
                this->Clear();

                if (this->bufferSize < other.bufferSize)
                {
                    this->bufferSize = other.bufferSize;

                    free(this->buffer);
                    this->buffer = static_cast<T*>(malloc(this->bufferSize * sizeof(T)));
                }

                for (size_t i = 0; i < other.count; ++i)
                {
                    new (this->buffer + i) T(other.buffer[i]);
                }

                this->count = other.count;
            }
        }

        /// Accessor operator.
        template <typename U>       T & operator[](U index)       { return this->Get(index); }
        template <typename U> const T & operator[](U index) const { return this->Get(index); }



        ///////////////////////////////////////////////////////////////
        // Attributes.

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
