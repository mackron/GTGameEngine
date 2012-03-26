
#include <GTEngine/Rendering/RenderCommands.hpp>
#include <GTEngine/Rendering/Renderer.hpp>

namespace GTEngine
{
    RenderCommand::RenderCommand()
        : next(nullptr), prev(nullptr)
    {
    }

    RenderCommand::~RenderCommand()
    {
        this->SetPrevious(nullptr);
        this->SetNext(nullptr);
    }

    void RenderCommand::SetNext(RenderCommand *command)
    {
        if (command != nullptr)
        {
            if (this->next != nullptr)
            {
                command->SetNext(this->next);
                this->next->SetPrevious(command);
            }

            this->next = command;
            command->SetPrevious(this);
        }
        else
        {
            if (this->next != nullptr)
            {
                this->next->SetPrevious(nullptr);
            }

            this->next = nullptr;
        }
    }

    void RenderCommand::SetPrevious(RenderCommand *command)
    {
        if (command != nullptr)
        {
            if (this->prev != nullptr)
            {
                command->SetPrevious(this->prev);
                this->prev->SetNext(command);
            }

            this->prev = command;
            command->SetNext(this);
        }
        else
        {
            if (this->prev != nullptr)
            {
                this->prev->SetNext(nullptr);
            }

            this->prev = nullptr;
        }
    }

    void RenderCommand::RemoveNext()
    {
        if (this->next != nullptr)
        {
            this->next->prev = nullptr;
            this->next = this->next->next;
        }
    }

    void RenderCommand::RemovePrevious()
    {
        if (this->prev != nullptr)
        {
            this->prev->next = nullptr;
            this->prev = this->prev->prev;
        }
    }
}

// ExecuteRCBuffer
namespace GTEngine
{
    void RenderCommand_ExecuteRCBuffer::Execute()
    {
        if (this->rcBuffer != nullptr)
        {
            this->rcBuffer->Execute();
        }
    }
}