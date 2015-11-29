// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Dictionary_hpp_
#define __GTLib_Dictionary_hpp_

#include "Strings/Compare.hpp"
#include "Strings/Create.hpp"
#include "stdlib.hpp"
#include <cstring>
#include <cstdint>
#include <new>

namespace GT
{
    /// An item in a dictionary.
    template <typename T, typename U>
    class DictionaryItemUTF
    {
    public:

        DictionaryItemUTF(const T* key, ptrdiff_t keySizeInBytes, const U &value)
            : key(key), keySizeInBytes(keySizeInBytes), value(value), index(0)
        {
        }

        ~DictionaryItemUTF()
        {
        }

        static DictionaryItemUTF<T, U>* Allocate(const T* key, ptrdiff_t keySizeInBytes, const U &value)
        {
            return new DictionaryItemUTF(key, keySizeInBytes, value);
        }
        static void Deallocate(DictionaryItemUTF<T, U>* self)
        {
            Strings::Delete(const_cast<T*>(self->key));
            delete self;
        }


    public:

        /// The key.
        const T* key;

        /// The size in bytes of the key.
        ptrdiff_t keySizeInBytes;

        /// The value;
        U value;

        /// The index of the item in the internal buffer. This will change when the content of the buffer changes. This is NOT used internally by Dictionary, but rather is
        /// only used to allow an application to have fast access to the item by directly accessing the buffer.  This is only set in Find().
        size_t index;


    private:    // No copying.
        DictionaryItemUTF(const DictionaryItemUTF &);
        DictionaryItemUTF & operator=(const DictionaryItemUTF &);
    };


    /// A basic map-style class whose key is a string and value is a pointer.
    ///
    /// The implementation is a template so that different string encodings can be used easily.
    template <typename T, typename U>
    class DictionaryUTF
    {
    public:

        /// Structure containing the results of Find() operation.
        struct FindResult
        {
            FindResult()
                : exact(false), index(0)
            {
            }

            FindResult(const FindResult &other)
                : exact(other.exact), index(other.index)
            {
            }

            FindResult & operator=(const FindResult &other)
            {
                this->exact = other.exact;
                this->index = other.index;

                return *this;
            }

            /// Specifies whether or not \c result is the exact match. If not, it points to the index that where it would otherwise be inserted.
            bool exact;

            /// The index in the buffer of where the item is located. This depends on \c exact. Note that the index may actually fall out-of-bounds
            /// if \c exact is false.
            size_t index;
        };


    public:

        /// Constructs an empty dictionary.
        DictionaryUTF()
            : buffer(nullptr), bufferSize(0), count(0)
        {
        }

        /// Copy constructor.
        DictionaryUTF(const DictionaryUTF &other)
            : buffer(nullptr), bufferSize(other.bufferSize), count(other.count)
        {
            this->buffer = static_cast<DictionaryItemUTF<T, U>**>(malloc(sizeof(DictionaryItemUTF<T, U>*) * this->bufferSize));

            for (size_t i = 0; i < this->count; ++i)
            {
                this->buffer[i] = DictionaryItemUTF<T, U>::Allocate(Strings::Create(other.buffer[i]->key, other.buffer[i]->keySizeInBytes), other.buffer[i]->keySizeInBytes, other.buffer[i]->value);
            }
        }


        /// Destructor.
        virtual ~DictionaryUTF()
        {
            this->Reset();
        }


        /// Adds an item to the dictionary.
        ///
        /// @param  key            [in] The key of the new item.
        /// @param  keySizeInBytes [in] The size in bytes of the key, not including the null terminator, if any.
        /// @param  value          [in] The value of the new item.
        ///
        /// @remarks
        ///     Existing items are replaced.
        DictionaryItemUTF<T, U>* Add(const T* key, ptrdiff_t keySizeInBytes, const U &value)
        {
            // We first need to try and find the item. If it's exact, we replace; otherwise we insert after the result.
            FindResult result = this->BinarySearch(key, keySizeInBytes);
            if (result.exact)
            {
                auto item = this->buffer[result.index];

                item->value.~U();
                new (&item->value) U(value);

                return item;
            }
            else
            {
                // We need a new item.
                auto newitem = DictionaryItemUTF<T, U>::Allocate(Strings::Create(key, keySizeInBytes), keySizeInBytes, value);

                // If we don't have enough room to fit the item, we need a new buffer. Otherwise, we move everything
                // after the insertion index down a position and just set the value directly.
                ++this->count;
                if (this->count > this->bufferSize)
                {
                    // Need a new buffer...
                    this->bufferSize = this->bufferSize * 2 + 1;
                    auto newbuffer = static_cast<DictionaryItemUTF<T, U>**>(malloc(sizeof(DictionaryItemUTF<T, U>*) * this->bufferSize));

                    // Copying over is done in three parts...
                    // Part 1: The part before the insertion point.
                    size_t i = 0;
                    for ( ; i < result.index; ++i)
                    {
                        newbuffer[i] = this->buffer[i];
                    }

                    // Part 2: The new item.
                    newbuffer[i] = newitem;
                    ++i;

                    // Part 3: The part after the insertion point.
                    for ( ; i < this->count; ++i)
                    {
                        newbuffer[i] = this->buffer[i - 1];
                    }

                    free(this->buffer);
                    this->buffer = newbuffer;
                }
                else
                {
                    // Don't need a new buffer...

                    // Inserting is done in two parts...
                    // Part 1: Move down a position starting at the insertion point.
                    for (size_t i = this->count - 1; i > result.index; --i)
                    {
                        this->buffer[i] = this->buffer[i - 1];
                    }

                    // Part 2: Apply the new item.
                    this->buffer[result.index] = newitem;
                }

                return newitem;
            }
        }

        DictionaryItemUTF<T, U>* Add(const T* key, const U &value)
        {
            return this->Add(key, -1, value);
        }

        /// Removes the item with the given key.
        void Remove(const T* key, ptrdiff_t keySizeInBytes = -1)
        {
            this->RemoveByKey(key, keySizeInBytes);
        }

        /// Removes the item at the given index.
        void Remove(size_t index)
        {
            this->RemoveByIndex(index);
        }

        /// Removes every item, but leaves the buffer allocated. This is essentially a fast version of Clear().
        void RemoveAll()
        {
            this->Clear();
        }

        /// Removes the item with the given key.
        void RemoveByKey(const T* key, ptrdiff_t keySizeInBytes = -1)
        {
            // We first need to find the item. If we don't find an exact match, we just ignore it.
            FindResult result = this->BinarySearch(key, keySizeInBytes);
            if (result.exact)
            {
                this->Remove(result.index);
            }
        }


        /// Removes the item at the given index.
        void RemoveByIndex(size_t index)
        {
            if (index < this->count)
            {
                // First delete the item...
                DictionaryItemUTF<T, U>::Deallocate(this->buffer[index]);

                // Now move everything down to remove it from the array.
                for (size_t i = index; i < this->count - 1; ++i)
                {
                    this->buffer[i] = this->buffer[i + 1];
                }

                --this->count;
            }
        }

        /// Finds the dictionary item of the given key.
        ///
        /// @param  key            [in] The key of the item being retrieved.
        /// @param  keySizeInBytes [in] The size in bytes of the key string. Can be (size_t)-1 if the key is null terminated.
        ///
        /// @return The dictionary item for the given key, or null if the item is not found.
        DictionaryItemUTF<T, U>* Find(const T *key, ptrdiff_t keySizeInBytes = -1)
        {
            FindResult result = this->BinarySearch(key, keySizeInBytes);
            if (result.exact)
            {
                auto item = this->buffer[result.index];
                item->index = result.index;

                return item;
            }

            return nullptr;
        }

        const DictionaryItemUTF<T, U>* Find(const T *key, ptrdiff_t keySizeInBytes = -1) const
        {
            return const_cast<DictionaryUTF<T, U>*>(this)->Find(key, keySizeInBytes);
        }


        /// Determines if an item with the given key exists.
        bool DoesItemExist(const T* key, ptrdiff_t keySizeInBytes = -1) const
        {
            return this->Find(key, keySizeInBytes) != nullptr;
        }
        bool Exists(const T* key, ptrdiff_t keySizeInBytes = -1) const
        {
            return this->DoesItemExist(key, keySizeInBytes);
        }


        /// Removes every item from the dictionary.
        void Clear(bool clearBuffer = false)
        {
            for (size_t i = 0; i < this->count; ++i)
            {
                DictionaryItemUTF<T, U>::Deallocate(this->buffer[i]);
            }

            this->count = 0;

            if (clearBuffer)
            {
                free(this->buffer);
                this->buffer     = nullptr;
                this->bufferSize = 0;
            }
        }


        /// Resets the dictionary to it's default state.
        void Reset()
        {
            this->Clear(true);  // 'true' says to clear the internal buffer.
        }


        /// Assignment operator.
        DictionaryUTF & operator=(const DictionaryUTF &other)
        {
            if (this != &other)
            {
                this->Reset();

                this->count      = other.count;
                this->bufferSize = this->count;
                this->buffer     = static_cast<DictionaryItemUTF<T, U>**>(malloc(sizeof(DictionaryItemUTF<T, U>*) * this->bufferSize));

                for (size_t i = 0; i < other.count; ++i)
                {
                    this->buffer[i] = DictionaryItemUTF<T, U>::Allocate(Strings::Create(other.buffer[i]->key, other.buffer[i]->keySizeInBytes), other.buffer[i]->keySizeInBytes, other.buffer[i]->value);
                }
            }

            return *this;
        }


    private:

        /// Searches for the item of the given key.
        FindResult BinarySearch(const T* key, ptrdiff_t keySizeInBytes) const
        {
            FindResult result;

            if (this->count > 0)
            {
                // It's just so much easier using signed values here...
                ptrdiff_t iMin  = 0;
                ptrdiff_t iMax = static_cast<ptrdiff_t>(this->count - 1);	// <-- the subtraction here is safe because this->count is checked that it's larger than 0 just above.
                ptrdiff_t iMid  = 0;

                ptrdiff_t indexOffset = 0; // This will either be 0 or 1 and is added to the index if we don't find an exact value.
                while (iMin <= iMax)
                {
                    iMid = (iMin + iMax) >> 1;

                    auto mid           = this->buffer[iMid];
                    int  compareResult = Strings::FastCompare(key, keySizeInBytes, mid->key, mid->keySizeInBytes);

                    if (compareResult < 0)
                    {
                        iMax        = iMid - 1;
                        indexOffset = 0;
                    }
                    else if (compareResult > 0)
                    {
                        iMin        = iMid + 1;
                        indexOffset = 1;
                    }
                    else
                    {
                        result.exact = true;
                        result.index = static_cast<size_t>(iMid);
                        
                        return result;
                    }
                }

                // If we have made it here, we weren't exact.
                result.index = static_cast<size_t>(iMid + indexOffset);
            }

            return result;
        }


    public:

        /// The buffer containing the items in the dictionary. The buffer is just pointers to DictionaryItemUTF's.
        DictionaryItemUTF<T, U>** buffer;

        /// The number of items in the buffer. This is NOT in bytes, but in pointers.
        size_t bufferSize;

        /// The number of items in the map.
        size_t count;
    };



    template <typename T>
    class Dictionary : public DictionaryUTF<char, T>
    {
        public: ~Dictionary() {}
    };

    template <typename T>
    class Dictionary16 : public DictionaryUTF<char16_t, T>
    {
        public: ~Dictionary16() {}
    };

    template <typename T>
    class Dictionary32 : public DictionaryUTF<char32_t, T>
    {
        public: ~Dictionary32() {}
    };
}

#endif
