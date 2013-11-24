// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/GameEventQueue.hpp>
#include <cassert>

namespace GTEngine
{
    GameEventQueue::GameEventQueue()
        : buffer(nullptr), bufferSize(0), eventCount(0), nextEventIndex(0)
    {
    }

    GameEventQueue::~GameEventQueue()
    {
        delete [] this->buffer;
    }

    void GameEventQueue::Push(const GameEvent &e)
    {
        assert(this->bufferSize >= this->eventCount);

        // We don't want consecutive OnSize events. If we get into this situation, we just replace the last event
        // with the new one instead of adding it.
        if (e.code == EventCodes::OnSize)
        {
            GameEvent *lastEvent = this->PeekLast();
            if (lastEvent != nullptr && lastEvent->code == EventCodes::OnSize)
            {
                *lastEvent = e;
                return;
            }
        }

        // First we need to ensure we have enough space...
        if (this->bufferSize == this->eventCount)
        {
            this->ResizeBuffer(this->bufferSize * 2 + 1);
        }

        // Now it's just a matter of making a copy in the correct location...
        this->buffer[(this->nextEventIndex + this->eventCount) % this->bufferSize] = e;
        ++this->eventCount;
    }

    bool GameEventQueue::Next(GameEvent &e)
    {
        if (eventCount > 0)
        {
            // There is an event in the buffer...
            e = this->buffer[this->nextEventIndex];

            // The next index needs to move forward by one. If it overflows the buffer, we simply move back to the start.
            this->nextEventIndex = (this->nextEventIndex + 1) % this->bufferSize;

            // And we also have one less event...
            --this->eventCount;

            return true;
        }
        else
        {
            // There are no events.
            e.code = EventCodes::Unknown;
            return false;
        }
    }

    GameEvent * GameEventQueue::PeekLast()
    {
        if (eventCount > 0)
        {
            return this->buffer + ((this->nextEventIndex + this->eventCount - 1) % this->bufferSize);
        }

        return nullptr;
    }

    void GameEventQueue::ResizeBuffer(size_t newBufferSize)
    {
        // First we need to allocate a new buffer, making sure we keep a hold of the old buffer so we can
        // copy the existing events back over to the new buffer.
        GameEvent *oldBuffer = this->buffer;
        GameEvent *newBuffer = new GameEvent[newBufferSize];

        // If we have items, they need to be copied over. We'll place the next event and index 0.
        if (this->eventCount > 0)
        {
            for (size_t i = 0; i < this->eventCount; ++i)
            {
                newBuffer[i] = oldBuffer[(this->nextEventIndex + i) % this->bufferSize];
            }
        }

        // Now we need to set our members. The next event will be at index 0. The event count remains unchanged.
        this->buffer         = newBuffer;
        this->bufferSize     = newBufferSize;
        this->nextEventIndex = 0;

        // Finally, the old buffer needs to be deleted...
        delete [] oldBuffer;
    }
}
