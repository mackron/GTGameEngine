
#ifndef __GTEngine_RCQueue_hpp_
#define __GTEngine_RCQueue_hpp_

#include <GTCore/Vector.hpp>

namespace GTEngine
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

        /// Appends a single draw call to the end of the cache.
        void Append(RenderCommand &cmd);

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
        GTCore::Vector<RenderCommand*> commands;


    private:    // No copying.
        RCQueue(const RCQueue &);
        RCQueue & operator=(const RCQueue &);
    };
}

#endif