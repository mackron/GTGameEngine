// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Strings_List
#define GT_Strings_List

#include "../stdlib.hpp"
#include "Size.hpp"
#include "Equal.hpp"
#include <cstring>

namespace GT
{
    namespace Strings
    {
        /**
        *   \brief  A class representing an item in a string list.
        */
        template <typename T>
        struct ListItem
        {
            ListItem(const T* start, const T* end, ListItem* prev, ListItem* next)
                : start(start), end(end), prev(prev), next(next)
            {
            }


            /// Determines whether or not this item is equal to another.
            bool IsEqual(const ListItem &other)
            {
                return Equal(this->start, this->end - this->start, other.start, other.end - other.start);
            }
            bool IsEqual(const char* other, ptrdiff_t otherSizeInTs = -1)
            {
                return Equal(this->start, this->end - this->start, other, otherSizeInTs);
            }


            /// A pointer to the first character in the string.
            const T* start;

            /// A pointer to 1 character past the last character in the string. This will usually point to a null terminator, but not always.
            /// Keep in mind that it's possible for \c end to point to an invalid memory location (since it's 1 past the last character). Be
            /// careful when dereferencing.
            const T* end;

            /// A pointer to the previous item in the list. This will be nullptr if the item is the root item.
            ListItem<T>* prev;

            /// A pointer to the next list item in the list. This will be nullptr if the item is the last item.
            ListItem<T>* next;


        public:

            // TODO: See how we can improve the efficiency of these allocations. Going to need to build a benchmarking app.
            static inline ListItem<T>* Allocate(const T* start, const T* end, ListItem<T>* prev, ListItem<T>* next)
            {
                return new ListItem<T>(start, end, prev, next);
            }

            static inline void Deallocate(ListItem<T>* item)
            {
                delete item;
            }
        };


        /**
        *   \brief  A special container class for storing a list of strings.
        *
        *   This class works a little different to other container classes like std::list. It is tailored very specificly
        *   to strings.
        *
        *   The first important difference is that a copy is never made of input strings. Instead, the application must ensure
        *   the strings remain valid throughout the life of the List. It can be sort of thought of as a std::list of const char*s.
        *
        *   The second important difference is that it's very lightweight. You need to understand the concept of a doubly linked
        *   list in order to do things like iterations. There are only 3 methods - Append(), Remove() and BuildString().
        *
        *   Append() and Remove() are self explanatory. Append() will return a ListItem object which can be passed to Remove() if
        *   if needs to be removed. BuildString() will take all of the list items and construct a concatentated string. You can
        *   use BuildStringSize() to get the required size for the destination buffer.
        *
        *   For flexibility and simplicity in the implementation of List, the \c root and \c last elements are public. Make sure
        *   you know what you're doing if you use them. The List class does not do any hand-holding.
        *
        *   To remove the root item, call Remove() for the root item directly. The same rule applies for \c last.
        *
        *   You can use a string list to build a string from multiple parts. Use BuildString() for this.
        */
        template <typename T>
        class List
        {
        public:

            /**
            *   \brief  Default constructor.
            */
            List()
                : root(nullptr), last(nullptr), lengthInTs(0), str(nullptr)
            {
            }

            /**
            *   \brief  Copy constructor.
            *
            *   \remarks
            *       This will create new instantiations of each item, but each of those items will still hold only a reference
            *       to the original strings (they will not make their own local copy).
            */
            List(const List<T> &other)
                : root(nullptr), last(nullptr), lengthInTs(0), str(nullptr)
            {
                for (auto i = other.root; i != nullptr; i = i->next)
                {
                    this->Append(i->start, i->end - i->start);
                }
            }

            /**
            *   \brief  Destructor.
            *
            *   The destructor will deallocate all ListItems that were created with Append().
            */
            ~List()
            {
                this->Clear();
            }

            /**
            *   \brief                Appends a string to the list.
            *   \param  str      [in] A pointer to the start of the string to append.
            *   \param  sizeInTs [in] The size in Ts of the string, not including the null terminator. See remarks.
            *   \return               A pointer to the new ListItem object.
            *
            *   \remarks
            *       \c sizeInTs can be set to -1, in which case it will be assumed that \c str is null terminated. However, if
            *       the size in bytes is pre-determined, always set \c sizeInTs to that value - it will save a call to Size().
            *       \par
            *       The return value can be passed to Remove().
            */
            ListItem<T>* Append(const T* strIn, ptrdiff_t sizeInTs = -1)
            {
                if (strIn != nullptr)
                {
                    if (sizeInTs == -1)
                    {
                        sizeInTs = SizeInTs(strIn);
                    }

                    auto newItem = ListItem<T>::Allocate(strIn, strIn + sizeInTs, this->last, nullptr);

                    if (this->last)
                    {
                        this->last->next = newItem;
                    }

                    if (!this->root)
                    {
                        this->root = newItem;
                    }

                    // The new item is always the last one.
                    this->last = newItem;

                    this->lengthInTs += static_cast<size_t>(sizeInTs);   // <-- Safe cast. Set to Size(str) if it's -1 on entry.
                    return newItem;
                }

                return nullptr;
            }

            /// Prepends a string to the list.
            ///
            /// @param str      [in] A pointer to the start of the string to append.
            /// @param sizeInTs [in] The size in Ts of the string, not including the null terminator.
            ///
            /// @return A pointer to the new ListItem object.
            ListItem<T>* Prepend(const T* strIn, ptrdiff_t sizeInTs = -1)
            {
                if (strIn != nullptr)
                {
                    if (sizeInTs == -1)
                    {
                        sizeInTs = SizeInTs(strIn);
                    }

                    auto newItem = ListItem<T>::Allocate(strIn, strIn + sizeInTs, nullptr, this->root);

                    if (this->root != nullptr)
                    {
                        this->root->prev = newItem;
                    }

                    if (this->last == nullptr)
                    {
                        this->last = newItem;
                    }

                    // The new item is always the root.
                    this->root = newItem;

                    this->lengthInTs += static_cast<size_t>(sizeInTs);
                    return newItem;
                }

                return nullptr;
            }


            /**
            *   \brief  Takes the items from 'other' and attaches them to the end of the list.
            *
            *   \remarks
            *       This will clear the input list.
            */
            void AbsorbToEnd(List &other)
            {
                if (this->last)
                {
                    if (other.root)
                    {
                        other.root->prev = this->last;
                        this->last->next = other.root;
                        this->last       = other.last;
                    }
                }
                else
                {
                    this->root = other.root;
                    this->last = other.last;
                }

                other.root = nullptr;
                other.last = nullptr;

                this->lengthInTs += other.BuildStringSize() - 1;    // BuildStringSize() includes the null terminator.
            }

            /**
            *   \brief  Takes the items from 'other' and attaches them to the start of the list.
            *
            *   \remarks
            *       This will clear the input list.
            */
            void AbsorbToStart(List &other)
            {
                if (this->root)
                {
                    if (other.root != nullptr && other.last != nullptr)
                    {
                        other.last->next = this->root;
                        this->root->prev = other.last;
                        this->root       = other.root;
                    }
                }
                else
                {
                    this->root = other.root;
                    this->last = other.last;
                }

                other.root = nullptr;
                other.last = nullptr;

                this->lengthInTs += other.BuildStringSize() - 1;    // BuildStringSize() includes the null terminator.
            }


            /**
            *   \brief            Removes an item from the list.
            *   \param  item [in] A pointer to the item to remove.
            */
            void Remove(ListItem<T> *item)
            {
                if (item)
                {
                    if (item->prev)
                    {
                        item->prev->next = item->next;
                    }
                    else
                    {
                        // The item is the root.
                        this->root = item->next;
                    }

                    if (item->next)
                    {
                        item->next->prev = item->prev;
                    }
                    else
                    {
                        // The item is the end.
                        this->last = item->prev;
                    }

                    this->lengthInTs -= static_cast<size_t>(item->end - item->start);
                    ListItem<T>::Deallocate(item);
                }
            }

            /**
            *   \brief  Retrieves the number of items in the list. This is O(n).
            */
            size_t Count() const
            {
                size_t result = 0;
                for (ListItem<T> *i = this->root; i != nullptr; i = i->next)
                {
                    ++result;
                }

                return result;
            }

            /**
            *   \brief  Removes every item from the list. This will invalidate every ListItem created with this list.
            */
            void Clear()
            {
                while (this->root)
                {
                    ListItem<T> *next = this->root->next;
                    ListItem<T>::Deallocate(this->root);
                    this->root = next;
                }

                this->root = nullptr;
                this->last = nullptr;

				free(this->str);
            }

            /**
            *   \brief            Builds a concatenated string from every list item.
            *   \param  dest [in] The buffer that will receive the concatenated string.
            *   \return           The number of bytes copied into \c dest.
            *
            *   \remarks
            *       Use BuildStringSize() to get the required size of the buffer.
            */
            size_t BuildString(T* dest) const
            {
                if (dest)
                {
                    ListItem<T>* next = this->root;
                    while (next)
                    {
                        size_t sizeInTs = static_cast<size_t>(next->end - next->start);

                        std::memcpy(dest, next->start, sizeInTs * sizeof(T));
                        dest = dest + sizeInTs;

                        next = next->next;
                    }

                    *dest = '\0';
                }

                return this->lengthInTs + 1;
            }

            /**
            *   \brief  Retrieves the required buffer size (in Ts) for BuildString().
            */
            inline size_t BuildStringSize() const { return this->lengthInTs + 1; }


			/**
			*	\brief	Converts the path to a c-style string and returns a pointer to that string. See remarks.
			*
			*	\remarks
			*		The returned string will be made invalid the next time c_str() is called or when the path is destructred.
			*		\par
			*		The string will be re-constructed for each call. Don't call it more frequently than needed.
			*/
			const T* c_str()
			{
				free(this->str);	// <-- Always ensure we delete the old string.

                this->str = static_cast<T*>(malloc((this->lengthInTs + 1) * sizeof(T)));
				this->BuildString(this->str);

				return this->str;
			}


            /**
            *   \brief  Assignment operator. All existing items will be deleted.
            */
            List<T> & operator= (const List<T> &other)
            {
                if (this != &other)
                {
                    this->Clear();

                    for (auto i = other.root; i != nullptr; i = i->next)
                    {
                        this->Append(i->start, i->end - i->start);
                    }
                }

                return *this;
            }


        public:

            /// The first item in the list.
            ListItem<T>* root;

            /// The last item in the list.
            ListItem<T>* last;


        private:

            /// The size of the buffer required for BuildString(). Does not include the null terminator.
            size_t lengthInTs;

			/// The temporary string we keep a hold of for the c_str() method.
			T* str;
        };
    }
}

#endif
