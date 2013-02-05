// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Rendering/RCQueue.hpp>
#include <GTEngine/Rendering/RenderCommand.hpp>
#include <cstring>      // For memcpy().

namespace GTEngine
{
    RCQueue::RCQueue()
        : commands()
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
}
