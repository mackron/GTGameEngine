// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Scene/SceneNodeIDAllocator.hpp>

namespace GT
{
    SceneNodeIDAllocator::SceneNodeIDAllocator(uint64_t minID)
        : m_minID(minID),
          m_nextID(minID),
          m_lock()
    {
    }


    uint64_t SceneNodeIDAllocator::AllocateID()
    {
        m_lock.Lock();

        uint64_t newID = m_nextID;
        m_nextID += 1;

        m_lock.Unlock();
        return newID;
    }
}
