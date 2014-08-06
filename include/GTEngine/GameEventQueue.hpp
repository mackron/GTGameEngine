// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_GameEventQueue_hpp_
#define __GTEngine_GameEventQueue_hpp_

#include "GameEvent.hpp"
#include <cstddef>

namespace GTEngine
{
    class GameEventQueue
    {
    public:

        /// Constructor.
        GameEventQueue();

        /// Destructor.
        ~GameEventQueue();

        /// \brief         Pushes an event onto the queue.
        /// \param  e [in] The event to copy into the queue.
        ///
        /// \remarks
        ///     The a copy of the event is created when pushed onto the queue. Therefore, it is safe to delete \e after calling this method.
        void Push(const GameEvent &e);

        /// \brief          Retrieves the next event and removes it from the queue.
        /// \param  e [out] A reference to the event that will receive the next event.
        /// \return         True if there are more events waiting to be handled; false otherwise.
        bool Next(GameEvent &e);

        /**
        *   \brief  Retrieves a pointer to the last event in the queue, but does not remove it.
        *
        *   \remarks
        *       This function will return null if there are no events in the queue.
        *       \par
        *       The caller is free to modify the returned event if required.
        */
        GameEvent* PeekLast();


    private:

        /// Resizes the internal buffer.
        void ResizeBuffer(size_t newBufferSize);


    private:

        /// The buffer containing the events.
        GameEvent* buffer;

        /// The size of the buffer, in GameEvents.
        size_t bufferSize;

        /// The number of events in the buffer.
        size_t eventCount;

        /// The index of the next event.
        size_t nextEventIndex;


    private:    // No copying.
        GameEventQueue(const GameEventQueue &);
        GameEventQueue & operator=(const GameEventQueue &);
    };
}

#endif