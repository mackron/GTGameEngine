// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

// Clang has errors with C++0x and noexcept when using std::map. We will fix it by doing our own map... This is only very
// basic. Need to do some documentation. Hopefully Clang will be fixed for their 3.0 release.
#ifndef GT_Map
#define GT_Map

#include "stdlib.hpp"

#include <cstdint>
#include <cstddef>
#include <cassert>
#include <new>

namespace GTLib
{
    /// An item in a dictionary.
    template <typename T, typename U>
    class MapItem
    {
    public:

        MapItem(const T &keyIn, const U &valueIn)
            : key(keyIn), value(valueIn), index(0)
        {
        }

        ~MapItem()
        {
        }

        static MapItem<T, U>* Allocate(const T &key, const U &value)
        {
            return new MapItem(key, value);
        }
        static void Deallocate(MapItem* self)
        {
            delete self;
        }


    public:

        /// The key.
        const T key;

        /// The value;
        U value;

        /// The index of the item in the internal buffer. This will change when the content of the buffer changes. This is NOT used internally by Map, but rather is
        /// only used to allow an application to have fast access to the item by directly accessing the buffer. This is only set in Find().
        size_t index;


	private:	// Never copy by value.
		MapItem(const MapItem<T, U> &);
		MapItem<T, U> & operator=(const MapItem<T, U> &);
    };


    /// Class representing a map data structure.
    ///
    /// Internally, items are stored in a single buffer. This makes it easier to do random access and allows us to use the good old binary search.
    template <typename T, typename U>
    class Map
    {
    public:

        /// Structure containing the results of Find() operation.
        struct FindResult
        {
            FindResult()
                : index(0), exact(false)
            {
            }

            FindResult(const FindResult &other)
                : index(other.index), exact(other.exact)
            {
            }

            FindResult & operator=(const FindResult &other)
            {
                this->exact = other.exact;
                this->index = other.index;

                return *this;
            }


            /// The index in the buffer of where the item is located. This depends on \c exact. Note that the index may actually fall out-of-bounds
            /// if \c exact is false.
            size_t index;

            /// Specifies whether or not \c result is the exact match. If not, it points to the index that where it would otherwise be inserted.
            bool exact;
        };


    public:

        /// Constructs an empty dictionary.
        Map()
            : buffer(nullptr), bufferSize(0), count(0)
        {
        }

        /// Destructor. This will deallocate every item in the dictionary.
        ~Map()
        {
            this->Reset();
        }


        /// Adds an item to the dictionary.
        ///
        /// @param  key   [in] The key of the new item.
        /// @param  value [in] The value of the new item.
        MapItem<T, U>* Add(const T &key, const U &value)
        {
            // We first need to try and find the item. If it's exact, we replace; otherwise we insert after the result.
            FindResult result = this->BinarySearch(key);
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
                auto newitem = MapItem<T, U>::Allocate(key, value);
                if (newitem != nullptr)
                {
                    // If we don't have enough room to fit the item, we need a new buffer. Otherwise, we move everything
                    // after the insertion index down a position and just set the value directly.
                    if (this->count + 1 > this->bufferSize)
                    {
                        // Need a new buffer...
                        size_t newBufferSize = this->bufferSize * 2 + 1;
                        auto newbuffer = static_cast<MapItem<T, U>**>(malloc(sizeof(MapItem<T, U>*) * newBufferSize));
					    if (newbuffer == nullptr)
					    {
						    // We failed to allocate a buffer that is twice the size of the old one. We may have run out of memory, so we will instead attempt to create a buffer that is only
						    // 1 item larger.
						    newBufferSize = this->bufferSize + 1;
						    newbuffer = static_cast<MapItem<T, U>**>(malloc(sizeof(MapItem<T, U>*) * newBufferSize));
						    if (newbuffer == nullptr)
						    {
							    // We still failed to create the new buffer. Returning null in this case.
							    return nullptr;
						    }
					    }


					    assert(newbuffer != nullptr);
					    {
						    this->count += 1;
						    this->bufferSize = newBufferSize;

						    // Copying over is done in three parts...
						    // Part 1: The part before the insertion point.
						    size_t i = 0;
						    for (; i < result.index; ++i)
						    {
							    newbuffer[i] = this->buffer[i];
						    }

						    // Part 2: The new item.
						    newbuffer[i] = newitem;
						    ++i;

						    // Part 3: The part after the insertion point.
						    for (; i < this->count; ++i)
						    {
							    newbuffer[i] = this->buffer[i - 1];
						    }

						    free(this->buffer);
						    this->buffer = newbuffer;
					    }
                    }
                    else
                    {
                        // Don't need a new buffer...
					    this->count += 1;

                        // Inserting is done in two parts...
                        // Part 1: Move down a position starting at the insertion point.
                        for (size_t i = this->count - 1; i > result.index; --i)
                        {
                            this->buffer[i] = this->buffer[i - 1];
                        }

                        // Part 2: Apply the new item.
                        this->buffer[result.index] = newitem;
                    }
                }

                return newitem;
            }
        }

        /// Removes the item with the given key.
        void RemoveByKey(const T &key)
        {
            // We first need to find the item. If we don't find an exact match, we just ignore it.
            FindResult result = this->BinarySearch(key);
            if (result.exact)
            {
                this->RemoveByIndex(result.index);
            }
        }

        void RemoveByIndex(uint64_t index)
        {
            if (index < this->count)
            {
                // First delete the item...
                MapItem<T, U>::Deallocate(this->buffer[size_t(index)]);

                // Now move everything down to remove it from the array.
                for (uint64_t i = index; i < this->count - 1; ++i)
                {
                    this->buffer[i] = this->buffer[i + 1];
                }

                --this->count;
            }
        }

        /// Removes the item with the given key.
        void Remove(const T &key)
        {
            this->RemoveByKey(key);
        }

        /// Removes the item at the given index.
        void Remove(uint64_t index)
        {
            this->RemoveByIndex(index);
        }


        /// Finds the map item of the given key.
        ///
        /// @param key [in] The key of the item being retrieved.
        ///
        /// @return A pointer to the map item for the given key. Returns null if the item can not be found.
        MapItem<T, U>* Find(const T &key)
        {
            FindResult result = this->BinarySearch(key);
            if (result.exact)
            {
                auto item = this->buffer[result.index];
                item->index = result.index;

                return item;
            }

            return nullptr;
        }
        const MapItem<T, U>* Find(const T &key) const
        {
            return const_cast<Map<T, U>*>(this)->Find(key);
        }


        /// Determines whether or not an item of the given key exists.
        bool DoesItemExist(const T &key) const
        {
            return this->Find(key) != nullptr;
        }

        bool Exists(const T &key) const
        {
            return this->DoesItemExist(key);
        }


        /// Removes every item.
        ///
        /// @param  clearBuffer [in] Whether or not to also clear the internal buffer.
        void Clear(bool clearBuffer = false)
        {
            for (size_t i = 0; i < this->count; ++i)
            {
                MapItem<T, U>::Deallocate(this->buffer[i]);
            }

            this->count = 0;

            if (clearBuffer)
            {
                free(this->buffer);
                this->buffer     = nullptr;
                this->bufferSize = 0;
            }
        }

        /// Resets the map to it's default state.
        void Reset()
        {
            this->Clear(true);  // 'true' says to clear the internal buffer.
        }

        /// Retrieves the number of items in the map.
        size_t GetCount() const
        {
            return this->count;
        }

        /// Retrieves an item by it's index.
        ///
        /// @param index [in] The index of the item to retrieve.
        MapItem<T, U>* GetItemByIndex(size_t index)
        {
            if (index < this->count)
            {
                return this->buffer[index];
            }

            return nullptr;
        }


    private:

        /// Searches for the item of the given key.
        FindResult BinarySearch(const T &key) const
        {
            FindResult result;

            if (this->count > 0)
            {
                // It's just so much easier using signed values here...
                ptrdiff_t iMin  = 0;
                ptrdiff_t iMax = static_cast<ptrdiff_t>(this->count - 1);   // <-- Safe cast as per the the check above.
                ptrdiff_t iMid  = 0;

                ptrdiff_t indexOffset = 0; // This will either be 0 or 1 and is added to the index if we don't find an exact value.
                while (iMin <= iMax)
                {
                    iMid = (iMin + iMax) >> 1;    // This will be floored.

                    auto mid = this->buffer[iMid];
                    if (key < mid->key)
                    {
                        iMax        = iMid - 1;
                        indexOffset = 0;
                    }
                    else if (key > mid->key)
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

            #if 0
                size_t iMin = 0;
                size_t iMax = this->count - 1;

                while (iMin < iMax)
                {
                    size_t iMid = (iMin + iMax) >> 1;

                    assert(iMid < iMax);
                    {
                        auto mid = this->buffer[iMid];
                        if (mid->key < key)
                        {
                            iMin = iMid + 1;
                        }
                        else
                        {
                            iMax = iMid;
                        }
                    }
                }

                assert(iMin == iMax);
                {
                    result.index = iMin;
                    result.exact = this->buffer[iMin]->key == key;

                    if (!result.exact && this->buffer[iMin]->key < key)
                    {
                        result.index += 1;
                    }
                }
            #endif
            }

            return result;
        }


    public:

        /// The buffer containing the items in the dictionary. The buffer is just pointers to MapItem's.
        MapItem<T, U>** buffer;

        /// The number of items in the buffer. This is NOT in bytes, but in pointers.
        size_t bufferSize;

        /// The number of items in the map.
        size_t count;


    private:    // No copying for now, but really should be added soon.
        Map(const Map<T, U> &);
        Map<T, U> & operator=(const Map<T, U> &);
    };
}

#endif
