// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "RCQueue.hpp"
#include "RenderCommand.hpp"
#include <cstring>      // For memcpy().

namespace GTEngine
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
        : RCQueue(), lock()
    {
    }

    void SynchronizedRCQueue::Append(RenderCommand &cmd)
    {
        this->lock.Lock();
        {
            RCQueue::Append(cmd);
        }
        this->lock.Unlock();
    }

    void SynchronizedRCQueue::Append(const RCQueue &other)
    {
        this->lock.Lock();
        {
            RCQueue::Append(other);
        }
        this->lock.Unlock();
    }

    void SynchronizedRCQueue::Execute()
    {
        this->lock.Lock();
        {
            RCQueue::Execute();
        }
        this->lock.Unlock();
    }

    void SynchronizedRCQueue::Clear()
    {
        this->lock.Lock();
        {
            RCQueue::Clear();
        }
        this->lock.Unlock();
    }

    bool SynchronizedRCQueue::IsEmpty() const
    {
        bool result;
        
        this->lock.Lock();
        {
            result = RCQueue::IsEmpty();
        }
        this->lock.Unlock();

        return result;
    }
}
