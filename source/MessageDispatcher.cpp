// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/MessageDispatcher.hpp>

namespace GT
{
    MessageDispatcher::MessageDispatcher()
        : m_messageHandlers()
    {
    }


    void MessageDispatcher::PostMessage(const Message &message)
    {
        for (size_t iMessageHandler = 0; iMessageHandler < m_messageHandlers.GetCount(); ++iMessageHandler)
        {
            auto messageHandler = m_messageHandlers[iMessageHandler];
            assert(messageHandler != nullptr);
            {
                messageHandler->HandleMessage(message);
            }
        }
    }

    void MessageDispatcher::PostMessage(MessageType type, const char* content)
    {
        Message message(type, content);
        this->PostMessage(message);
    }

    void MessageDispatcher::PostErrorMessage(const char* message)
    {
        this->PostMessage(MessageType_Error, message);
    }

    void MessageDispatcher::PostWarningMessage(const char* message)
    {
        this->PostMessage(MessageType_Warning, message);
    }

    void MessageDispatcher::PostLogMessage(const char* message)
    {
        this->PostMessage(MessageType_Log, message);
    }


    void MessageDispatcher::AddMessageHandler(MessageHandler &messageHandler)
    {
        if (!m_messageHandlers.Exists(&messageHandler))
        {
            m_messageHandlers.PushBack(&messageHandler);
        }
    }

    void MessageDispatcher::RemoveMessageHandler(MessageHandler &messageHandler)
    {
        m_messageHandlers.RemoveFirstOccuranceOf(&messageHandler);
    }


    size_t MessageDispatcher::GetMessageHandlerCount() const
    {
        return m_messageHandlers.GetCount();
    }

    MessageHandler & MessageDispatcher::GetMessageHandlerByIndex(size_t index)
    {
        auto messageHandler = m_messageHandlers[index];
        assert(messageHandler != nullptr);
        {
            return *messageHandler;
        }
    }
}