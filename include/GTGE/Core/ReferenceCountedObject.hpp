// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ReferenceCountedObject
#define GT_ReferenceCountedObject

namespace GT
{
    /// Base class that can be inherited that implements a reference counter.
    class ReferenceCountedObject
    {
    public:

        /// Constructor.
        ///
        /// This will initialize the reference count to 1.
        ReferenceCountedObject()
            : m_referenceCount(1)
        {
        }


        /// Increments the reference count.
        ///
        /// @return The new reference count.
        ///
        /// @remarks
        ///     This is not thread-safe.
        unsigned int IncrementReferenceCount()
        {
            return m_referenceCount += 1;
        }

        /// Decrements the reference counter.
        ///
        /// @return The new reference count.
        ///
        /// @remarks
        ///     This is not thread-safe.
        unsigned int DecrementReferenceCount()
        {
            if (m_referenceCount > 0)
            {
                m_referenceCount -= 1;
            }

            return m_referenceCount;
        }

        /// Retrieves the reference counter.
        ///
        /// @return The current reference count.
        unsigned int GetReferenceCount()
        {
            return m_referenceCount;
        }



    private:

        /// The reference count.
        unsigned int m_referenceCount;
    };
}

#endif
