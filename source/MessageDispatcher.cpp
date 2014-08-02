// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#include <GTEngine/MessageDispatcher.hpp>

namespace GT
{
    MessageDispatcher::MessageDispatcher()
        : m_messageHandlers()
    {
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