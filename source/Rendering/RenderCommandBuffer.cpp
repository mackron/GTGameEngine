
#include <GTEngine/Rendering/RenderCommandBuffer.hpp>
#include <GTEngine/Material.hpp>

namespace GTEngine
{
    RenderCommandBuffer::RenderCommandBuffer()
        : commands()
    {
    }
    
    void RenderCommandBuffer::Append(RenderCommand &cmd)
    {
        this->commands.PushBack(&cmd);
    }

    void RenderCommandBuffer::Execute()
    {
        for (size_t i = 0; i < this->commands.count; ++i)
        {
            this->commands.buffer[i]->Execute();
        }
    }

    void RenderCommandBuffer::Clear()
    {
        // Just to make sure we don't cycle over and attempt to call destructors, we'll simply set the count to 0
        // which is enough to clear the buffer. We would otherwise use this->commands.Clear();
        this->commands.count = 0;
    }
}
