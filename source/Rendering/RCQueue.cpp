// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include "RCQueue.hpp"
#include "RenderCommand.hpp"
#include <cstring>      // For memcpy().

namespace GT
{
    RCQueue::RCQueue()
        : commands()
    {
    }

    RCQueue::~RCQueue()
    {
    }

    
    void RCQueue::Append(RenderCommand &cmd)
    {
        this->commands.PushBack(&cmd);
    }

    void RCQueue::Append(const RCQueue &other)
    {
        if (this->commands.bufferSize <= this->commands.count + other.commands.count)
        {
            this->commands.Reserve(this->commands.count + other.commands.count);
        }

        memcpy(this->commands.buffer + this->commands.count, other.commands.buffer, other.commands.count * sizeof(RenderCommand*));
        this->commands.count += other.commands.count;
    }

    void RCQueue::Execute()
    {
        for (size_t i = 0; i < this->commands.count; ++i)
        {
            this->commands.buffer[i]->Execute();
        }
    }

    void RCQueue::Clear()
    {
        // Just to make sure we don't cycle over and attempt to call destructors, we'll simply set the count to 0
        // which is enough to clear the buffer. We would otherwise use this->commands.Clear();
        this->commands.count = 0;
    }






    ////////////////////////////////////////////
    // SynchronizedRCQueue

    SynchronizedRCQueue::SynchronizedRCQueue()
        : RCQueue(), lock(NULL)
    {
        this->lock = easyutil_create_mutex();
    }

    SynchronizedRCQueue::~SynchronizedRCQueue()
    {
        easyutil_delete_mutex(this->lock);
    }

    void SynchronizedRCQueue::Append(RenderCommand &cmd)
    {
        easyutil_lock_mutex(this->lock);
        {
            RCQueue::Append(cmd);
        }
        easyutil_unlock_mutex(this->lock);
    }

    void SynchronizedRCQueue::Append(const RCQueue &other)
    {
        easyutil_lock_mutex(this->lock);
        {
            RCQueue::Append(other);
        }
        easyutil_unlock_mutex(this->lock);
    }

    void SynchronizedRCQueue::Execute()
    {
        easyutil_lock_mutex(this->lock);
        {
            RCQueue::Execute();
        }
        easyutil_unlock_mutex(this->lock);
    }

    void SynchronizedRCQueue::Clear()
    {
        easyutil_lock_mutex(this->lock);
        {
            RCQueue::Clear();
        }
        easyutil_unlock_mutex(this->lock);
    }

    bool SynchronizedRCQueue::IsEmpty() const
    {
        bool result;
        
        easyutil_lock_mutex(this->lock);
        {
            result = RCQueue::IsEmpty();
        }
        easyutil_unlock_mutex(this->lock);

        return result;
    }
}
