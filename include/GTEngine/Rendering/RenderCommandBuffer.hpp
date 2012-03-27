
#ifndef __GTEngine_RenderCommandBuffer_hpp_
#define __GTEngine_RenderCommandBuffer_hpp_

#include "RenderCommand.hpp"
#include <GTCore/Vector.hpp>

namespace GTEngine
{
    /**
    *   \brief  Class representing a buffer of rendering commands.
    *
    *   This class does not make copies of any attached draw calls. It is the applications responsibility to manage
    *   allocation and deallocation of render commands.
    */
    class RenderCommandBuffer
    {
    public:

        /// Constructor.
        RenderCommandBuffer();

        /**
        *   \brief  Appends a single draw call to the end of the cache.
        *
        *   \remarks
        *       You can append a chain of draw calls by using Append(RenderCommand *, RenderCommand *).
        */
        void Append(RenderCommand &cmd);

        /**
        *   \brief  Executes all of the commands in the buffer, but does not clear it.
        *
        *   \remarks
        *       The rendering commands will remain in the buffer after calling this function. Use Clear() to remove
        *       the commands.
        */
        void Execute();

        /**
        *   \brief  Clears the buffer.
        */
        void Clear();

        /**
        *   \brief  Checks if the cache is empty.
        */
        bool IsEmpty() const
        {
            return this->commands.count == 0;
        }


    private:

        /// The list containing all of the rendering commands.
        GTCore::Vector<RenderCommand*> commands;


    private:    // No copying.
        RenderCommandBuffer(const RenderCommandBuffer &);
        RenderCommandBuffer & operator=(const RenderCommandBuffer &);
    };
}

#endif