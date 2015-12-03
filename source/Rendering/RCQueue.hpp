// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_RCQueue
#define GT_RCQueue

#include <GTGE/Core/Vector.hpp>
#include <easy_util/easy_util.h>

namespace GT
{
    class RenderCommand;

    /// Class representing a buffer of rendering commands.
    ///
    /// This class does not make copies of any attached draw calls. It is the applications responsibility to manage
    /// allocation and deallocation of render commands.
    class RCQueue
    {
    public:

        /// Constructor.
        RCQueue();

        /// Destructor.
        virtual ~RCQueue();


        /// Appends a single draw call to the end of the cache.
        ///
        /// @param cmd [in] The command to append to the queue.
        void Append(RenderCommand &cmd);

        /// Appends another RCQueue.
        ///
        /// @param other [in] The other RC queue to append to this one.
        ///
        /// @remarks
        ///     This will not clear 'other'.
        ///     @par
        ///     This is more efficient than appending each item idividually.
        void Append(const RCQueue &other);


        /// Executes all of the commands in the buffer, but does not clear it.
        ///
        /// @remarks
        ///     The rendering commands will remain in the buffer after calling this function. Use Clear() to remove
        ///     the commands.
        void Execute();

        /// Clears the buffer. This does not deallocate any memory.
        void Clear();

        /// Checks if the cache is empty.
        bool IsEmpty() const
        {
            return this->commands.count == 0;
        }


    private:

        /// The list containing all of the rendering commands.
        Vector<RenderCommand*> commands;


    private:    // No copying.
        RCQueue(const RCQueue &);
        RCQueue & operator=(const RCQueue &);
    };


    /// Same as RCQueue, except everything is synchronized.
    class SynchronizedRCQueue : public RCQueue
    {
    public:

        /// Constructor.
        SynchronizedRCQueue();

        /// Destructor.
        ~SynchronizedRCQueue();


        /// RCQueue::Append(RenderCommand &)
        void Append(RenderCommand &cmd);

        /// RCQueue::Append(const RCQueue &other)
        void Append(const RCQueue &other);

        /// RCQueue::Execute()
        void Execute();

        /// RCQueue::Clear()
        void Clear();

        /// RCQueue::IsEmpty()
        bool IsEmpty() const;


    private:

        /// The synchronization lock.
        mutable easyutil_mutex lock;


    private:    // No copying.
        SynchronizedRCQueue(const SynchronizedRCQueue &);
        SynchronizedRCQueue & operator=(const SynchronizedRCQueue &);
    };
}

#endif