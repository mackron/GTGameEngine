// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_HandleManager_hpp_
#define __GT_HandleManager_hpp_

#include "Vector.hpp"
#include <functional>

namespace GT
{
    /// Class for managing 32-bit handles.
    ///
    /// A handle is 32-bits, and is made up of two parts. The first part is 24 bits and is an index into an internal array. The second part is 8
    /// bits and represents a uniqueness identifier that is used to allow handles to be recycled.
    ///
    /// A handle represents an object, but is not a direct pointer to it. A pointer to the actual object is stored in an internal array. The first
    /// 24-bits of the handle will contain the index into the array where an actual pointer to the object is stored.
    ///
    /// The object itself will store a local copy of the handle which will be used for checking whether or not the handle is valid. If the handles
    /// match, the object is valid; otherwise the handle is considered invalid.
    ///
    /// Handles can be recycled, but in order to ensure the correct object is referenced, a uniqueness identifier is used. Each time the handle is
    /// recycled, the uniqueness ID is incremented. This ID is stored in the last 8 bits of the handle, which means the handle can be recycled a
    /// maximum of 255 times (the uniqueness ID starts at 1).
    ///
    /// Internally, pointers to the actual objects are stored in a flat array. Objects can be retrieved with both handles and raw indices into the
    /// array. Use GetHandleCount() and GetAssociatedObjectByIndex() to iterate over handles and their associated objects by indices.
    template <typename HandleType, typename ObjectType>
    class HandleManager
    {
    public:

        /// Constructor.
        HandleManager()
            : m_objectPointers(),
              m_recyclableHandles()
        {
            static_assert(sizeof(HandleType) == 4, "Handle types must be 32-bit.");
        }

        /// Destructor.
        ~HandleManager()
        {
        }



        /// Creates a new handle for the given object.
        ///
        /// @return The new handle or 0 if a handle could not be created.
        ///
        /// @remarks
        ///     The returned handle will not be associated with an object. Use AssociateObjectWithHandle() to do this.
        HandleType CreateHandle()
        {
            HandleType handle = 0;
            if (m_recyclableHandles.GetCount() > 0)
            {
                handle = m_recyclableHandles.GetBack();
                m_recyclableHandles.PopBack();

                handle = CreateHandle(GetHandleIndex(handle), GetHandleUniquenessID(handle) + 1);
            }
            else
            {
                if (m_objectPointers.GetCount() < 0xFFFFFF)
                {
                    size_t  index        = m_objectPointers.GetCount();
                    uint8_t uniquenessID = 1;       // <-- Never start this at zero, otherwise we'll have a handle which is equal to 0, which is not what we want. We want all valid handles to be non-zero.
                    handle = CreateHandle(index, uniquenessID);

                    m_objectPointers.PushBack(nullptr);
                }
                else
                {
                    // There are too many instantiated handles.
                    return 0;
                }
            }


            // Associate a null object by default.
            this->AssociateObjectWithHandle(handle, nullptr);

        
            return handle;
        }

        /// Deletes a handle and puts it up for recycling, if possible.
        ///
        /// @param handle [in] The handle to delete.
        void DeleteHandle(HandleType handle)
        {
            // Put the handle up for recycling.
            if (GetHandleUniquenessID(handle) < 0xFF)
            {
                m_recyclableHandles.PushBack(handle);
            }

            // Clear the element slot.
            size_t objectIndex = GetHandleIndex(handle);
            assert(objectIndex < m_objectPointers.GetCount());
            {
                m_objectPointers[objectIndex] = nullptr;
            }
        }


        /// Associates an object with the given handle.
        ///
        /// @param handle    [in] The handle.
        /// @param objectPtr [in] A pointer to the object to associate with the handle.
        void AssociateObjectWithHandle(HandleType handle, ObjectType* objectPtr)
        {
            if (handle != 0)
            {
                size_t handleIndex = GetHandleIndex(handle);
                assert(handleIndex < m_objectPointers.GetCount());
                {
                    m_objectPointers[handleIndex] = objectPtr;
                }
            }
        }

        /// Retrieves a pointer to the object that is associated with the given handle, if any.
        ///
        /// @param handle [in] The handle whose associated object is being retrieved.
        ///
        /// @return A pointer to the object that is associated with the given handle.
        ///
        /// @remarks
        ///     If the handle at the given index is deleted or up for recycling, the return value with be null.
        ObjectType* GetAssociatedObject(HandleType handle) const
        {
            if (handle != 0)
            {
                size_t index = GetHandleIndex(handle);
                if (index < m_objectPointers.GetCount())
                {
                    return m_objectPointers[index];
                }
                else
                {
                    // Invalid handle.
                    return nullptr;
                }
            }
            else
            {
                // Invalid handle (null)
                return nullptr;
            }
        }

        /// Iterates over the currently valid handles that have non-null objects associated with them.
        ///
        /// @param handler [in] The function that will be called for each handle.
        ///
        /// @remarks
        ///     When the return value of the handler function is false, the loop will stop iterating.
        void IterateAssociatedObjects(std::function<bool (ObjectType*)> handler) const
        {
            for (size_t iObject = 0; iObject < m_objectPointers.GetCount(); ++iObject)
            {
                auto objectPtr = m_objectPointers[iObject];
                if (objectPtr != nullptr)
                {
                    if (!handler(objectPtr))
                    {
                        break;
                    }
                }
            }
        }



    private:

        /// Helper function for retrieving the handle's index.
        ///
        /// @param handle [in] The handle whose index is being retrieved.
        static inline size_t GetHandleIndex(HandleType handle)
        {
            return (handle & 0xFFFFFF00) >> 8;
        }

        /// Helper function for retrieving the uniqueness ID of the given handle.
        ///
        /// @param handle [in] The handle whose uniqueness ID is being retrieved.
        static inline uint8_t GetHandleUniquenessID(HandleType handle)
        {
            return static_cast<uint8_t>(handle & 0x000000FF);
        }

        /// Helper function for created a handle from an index and uniqueness ID.
        ///
        /// @param index        [in] The index of the new handle.
        /// @param uniquenessID [in] The uniqueness ID of the new handle.
        static inline HandleType CreateHandle(size_t index, uint8_t uniquenessID)
        {
            const uint32_t index32 = static_cast<uint32_t>(index);
            assert(index32 <= 0xFFFFFF);
            {
                return static_cast<HandleType>((index32 << 8) | uniquenessID);
            }
        }



    private:

        /// The list of pointers to objects each handle maps to.
        GTLib::Vector<ObjectType*> m_objectPointers;

        /// The list of handles that can be recycled.
        GTLib::Vector<HandleType> m_recyclableHandles;
    };
}

#endif // !__GT_HandleManager_hpp_
