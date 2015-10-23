// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_List_hpp_
#define __GTLib_List_hpp_

#include <cstddef>

namespace GTLib
{
    /**
    *   \brief  Structure for an individual item in a list.
    */
    template <typename T>
    struct ListItem
    {
        /**
        *   \brief  Constructor.
        */
        ListItem(const T &value)
            : value(value), prev(nullptr), next(nullptr)
        {
        }

        /**
        *   \brief  Destructor.
        */
        ~ListItem()
        {
        }


        /// The value of the item.
        T value;

        /// A pointer to the previous item in the list.
        ListItem<T> *prev;

        /// A pointer to the next item in the list.
        ListItem<T> *next;


    public: // Allocators.
        static ListItem<T> * Allocate(const T &value)
        {
            return new ListItem<T>(value);
        }

        static void Deallocate(ListItem<T> * itemToDeallocate)
        {
            delete itemToDeallocate;
        }


    private:    // No copying.
        ListItem(const ListItem<T> &);
        ListItem & operator=(const ListItem<T> &);
    };

    /**
    *   \brief  Generic list container class.
    */
    template <typename T>
    class List
    {
    public:

        /**
        *   \brief  Default constructor.
        */
        List()
            : root(nullptr), last(nullptr)
        {
        }

        /**
        *   \brief  Destructor.
        */
        virtual ~List()
        {
            this->Clear();
        }

        /**
        *   \brief  Appends an item to the end of the list.
        */
        ListItem<T>* Append(const T &value)
        {
            auto newItem = ListItem<T>::Allocate(value);
            this->AttachToEnd(newItem);

            return newItem;
        }

        /**
        *   \brief  Inserts an item at the start of the list.
        */
        ListItem<T>* Prepend(const T &value)
        {
            auto newItem = ListItem<T>::Allocate(value);

            if (this->root)
            {
                auto oldroot = this->root;

                this->root->prev = newItem;
                this->root = newItem;
                this->root->next = oldroot;
            }
            else
            {
                this->root = newItem;
                this->last = newItem;
            }

            return newItem;
        }

        /**
        *   \brief  Remomves an item from the list. Assumes the item is part of the given list.
        */
        void Remove(ListItem<T> *itemToRemove)
        {
            if (itemToRemove)
            {
                this->Detach(itemToRemove);
                ListItem<T>::Deallocate(itemToRemove);
            }
        }

        void RemoveRoot() { this->Remove(this->root); }
        void RemoveLast() { this->Remove(this->last); }

        /// The same as RemoveRoot().
        void RemoveFirst() { this->RemoveRoot(); }


        /// Inserts a new item after the given item.
        ListItem<T>* InsertAfter(const T &value, ListItem<T>* itemBefore)
        {
            auto newItem = ListItem<T>::Allocate(value);

            newItem->prev = itemBefore;

            if (itemBefore != nullptr)
            {
                newItem->next    = itemBefore->next;
                itemBefore->next = newItem;

                if (newItem->next != nullptr)
                {
                    newItem->next->prev = newItem;
                }
            }
            else
            {
                newItem->next = nullptr;
            }



            if (this->root == nullptr || newItem->prev == nullptr)
            {
                this->root = newItem;
            }

            if (this->last == nullptr || newItem->next == nullptr)
            {
                this->last = newItem;
            }
            


            return newItem;
        }


        /**
        *   \brief            Attaches an already allocated item to the end of the list.
        *   \param  item [in] The item to attach.
        */
        void AttachToEnd(ListItem<T> *item)
        {
            if (item != nullptr)
            {
                // The previous item is always the previously last item.
                item->prev = this->last;
                item->next = nullptr;

                if (this->last != nullptr)
                {
                    this->last->next = item;
                }

                if (this->root == nullptr)
                {
                    this->root = item;
                }

                // The new item is always the last one.
                this->last = item;
            }
        }

        /**
        *   \brief  Detaches an item from the list, but does not deallocate it. This is really good for moving the item to another list.
        *   \param  item [in] The item to detach.
        */
        void Detach(ListItem<T> *item)
        {
            if (item)
            {
                if (item->prev != nullptr)
                {
                    item->prev->next = item->next;
                }
                else
                {
                    // The item is the root.
                    this->root = item->next;
                }

                if (item->next != nullptr)
                {
                    item->next->prev = item->prev;
                }
                else
                {
                    // The item is the end.
                    this->last = item->prev;
                }

			    item->prev = nullptr;
			    item->next = nullptr;
            }
        }


        /**
        *   \brief  Finds the first occurance of an item based on it's value.
        */
        ListItem<T> * Find(const T &value) const
        {
            auto i = this->root;
            while (i != nullptr)
            {
                if (i->value == value)
                {
                    return i;
                }

                i = i->next;
            }

            return nullptr;
        }

        /**
        *   \brief  Determines whether or not the given item exists.
        */
        bool Exists(const T &value) const
        {
            return this->Find(value) != nullptr;
        }

        /**
        *   \brief  Empties the list.
        *
        *   \remarks
        *       All items will be made invalid after calling this method.
        */
        void Clear()
        {
            while (this->root)
            {
                auto next = this->root->next;
                ListItem<T>::Deallocate(this->root);
                this->root = next;
            }

            this->root = nullptr;
            this->last = nullptr;
        }


        /**
        *   \brief  Retrieves the number of items in the list. This runs in linear time.
        */
        size_t Count() const
        {
            size_t result = 0;
            for (auto i = this->root; i != nullptr; i = i->next)
            {
                ++result;
            }

            return result;
        }


        /**
        *   \brief  Determines whether or not eh list is empty.
        */
        bool IsEmpty() const
        {
            return this->root != nullptr;
        }


    public:

        /// A pointer to the first list item.
        ListItem<T> *root;

        /// A pointer to the last list item.
        ListItem<T> *last;


    private:    // No copying for now.
        List(const List<T> &);
        List & operator=(const List<T> &);
    };
}


#endif
