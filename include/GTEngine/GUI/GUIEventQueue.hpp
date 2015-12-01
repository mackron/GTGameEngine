// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIEventQueue
#define GT_GUIEventQueue

#include "GUIEvent.hpp"

namespace GTGUI
{
    /// Class representing a queue of events.
    ///
    /// Each server has an event queue. The queue is emptied by the server at the end of every step.
    ///
    /// An important detail to remember with this class is that when the queue receives an OnSize event, it will nullify any
    /// previously queued OnSize events for that element. Thus, only one OnSize event will be present for any given element at
    /// a given time. The main reason for this is so that individual width and height OnSize events can be combined into a
    /// single event.
    class GUIEventQueue
    {
    public:

        /// Constructor.
        GUIEventQueue();

        /// Destructor.
        ~GUIEventQueue();

        /// \brief         Pushes an event onto the queue.
        /// \param  e [in] The event to copy into the queue.
        ///
        /// \remarks
        ///     The a copy of the event is created when pushed onto the queue. Therefore, it is safe to delete \e after calling this method.
        void Push(const GUIEvent &e);

        /// \brief          Retrieves the next event and removes it from the queue.
        /// \param  e [out] A reference to the event that will receive the next event.
        /// \return         True if there are more events waiting to be handled; false otherwise.
        bool Next(GUIEvent &e);

        /**
        *   \brief  Retrieves a pointer to the last event in the queue, but does not remove it.
        *
        *   \remarks
        *       This function will return null if there are no events in the queue.
        *       \par
        *       The caller is free to modify the returned event if required.
        */
        GUIEvent* PeekLast();

        /// Retrieves a pointer to the next event in the queue, but does not remove it.
        ///
        /// @remarks
        ///     This function will return null if there are no events in the queue.
        ///     @par
        ///     The caller is free to modify the returned event if required.
        GUIEvent* PeekNext();


        /// Nullifies every event that uses the given element.
        ///
        /// @param element [in] The element whose events are being nullified.
        void NullifyEventsOfElement(GUIElement* element);

        /// Removes every event that uses the given element.
        ///
        /// @param element [in] The element whose events are being deleted.
        void RemoveEventsOfElement(GUIElement* element);


    private:

        /// Resizes the internal buffer.
        void ResizeBuffer(size_t newBufferSize);

        /// Removes the element at the given index offset.
        void Remove(size_t index);


    private:

        /// The buffer containing the events.
        GUIEvent* buffer;

        /// The size of the buffer, in GameEvents.
        size_t bufferSize;

        /// The number of events in the buffer.
        size_t eventCount;

        /// The index of the next event.
        size_t nextEventIndex;


    private:    // No copying.
        GUIEventQueue(const GUIEventQueue &);
        GUIEventQueue & operator=(const GUIEventQueue &);
    };
}

#endif