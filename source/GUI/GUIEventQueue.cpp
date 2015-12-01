// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/GUI/GUIEventQueue.hpp>
#include <cassert>

namespace GTGUI
{
    GUIEventQueue::GUIEventQueue()
        : buffer(nullptr), bufferSize(0), eventCount(0), nextEventIndex(0)
    {
    }

    GUIEventQueue::~GUIEventQueue()
    {
        delete [] this->buffer;
    }

    void GUIEventQueue::Push(const GUIEvent &e)
    {
        assert(this->bufferSize >= this->eventCount);

        // First we need to ensure we have enough space...
        if (this->bufferSize == this->eventCount)
        {
            this->ResizeBuffer(this->bufferSize * 2 + 1);
        }

        // We need to grab the location in the buffer that we'll be placing the event.
        size_t eventIndex = (this->nextEventIndex + this->eventCount) % this->bufferSize;

        // Now it's just a matter of making a copy in the correct location...
        this->buffer[eventIndex] = e;

        // And we have one more event in the list...
        ++this->eventCount;
    }

    bool GUIEventQueue::Next(GUIEvent &e)
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
            e.code = GUIEventCode_Unknown;
            return false;
        }
    }

    GUIEvent* GUIEventQueue::PeekLast()
    {
        if (this->eventCount > 0)
        {
            return this->buffer + ((this->nextEventIndex + this->eventCount - 1) % this->bufferSize);
        }

        return nullptr;
    }

    GUIEvent* GUIEventQueue::PeekNext()
    {
        if (this->eventCount > 0)
        {
            return this->buffer + this->nextEventIndex;
        }

        return nullptr;
    }


    void GUIEventQueue::NullifyEventsOfElement(GUIElement* element)
    {
        for (size_t i = 0; i < this->eventCount; ++i)
        {
            auto &e = this->buffer[(this->nextEventIndex + i) % this->bufferSize];
            if (e.element == element)
            {
                e.code = GUIEventCode_Null;
            }
        }
    }

    void GUIEventQueue::RemoveEventsOfElement(GUIElement* element)
    {
        for (size_t i = 0; i < this->eventCount; )
        {
            auto &e = this->buffer[(this->nextEventIndex + i) % this->bufferSize];
            if (e.element == element)
            {
                this->Remove(i);
            }
            else
            {
                ++i;
            }
        }
    }


    void GUIEventQueue::ResizeBuffer(size_t newBufferSize)
    {
        // First we need to allocate a new buffer, making sure we keep a hold of the old buffer so we can
        // copy the existing events back over to the new buffer.
        GUIEvent *oldBuffer = this->buffer;
        GUIEvent *newBuffer = new GUIEvent[newBufferSize];

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

        // It's a bit slower, but I want to ensure the unused slots are set to logical defaults. We don't want to use a default construct
        // in the GUIEvent structure.
        for (size_t i = this->eventCount; i < this->bufferSize; ++i)
        {
            this->buffer[i].element = nullptr;
            this->buffer[i].code    = GUIEventCode_Unknown;
        }

        // Finally, the old buffer needs to be deleted...
        delete [] oldBuffer;
    }

    void GUIEventQueue::Remove(size_t index)
    {
        // We just move everything down a position.
        for (size_t i = index + 1; i < this->eventCount; ++i)
        {
            this->buffer[(this->nextEventIndex + (i - 1)) % this->bufferSize] = this->buffer[(this->nextEventIndex + i) % this->bufferSize];
        }

        this->eventCount -= 1;
    }
}