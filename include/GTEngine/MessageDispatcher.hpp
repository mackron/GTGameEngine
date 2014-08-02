// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#ifndef __GT_MessageDispatcher_hpp_
#define __GT_MessageDispatcher_hpp_

#include "MessageHandler.hpp"
#include <GTLib/Vector.hpp>

#undef PostMessage

namespace GT
{
    /// Class used for dispatching messages to the various message handlers.
    class MessageDispatcher
    {
    public:

        /// Constructor.
        MessageDispatcher();


        /// Posts a message.
        ///
        /// @param message [in] A reference to the Message object.
        void PostMessage(const Message &message);

        /// Helper for posting a simple message.
        ///
        /// @param type    [in] The message type.
        /// @param message [in] The message content.
        void PostMessage(MessageType type, const char* message);

        /// Helper for posting a simple error message.
        ///
        /// @param message [in] The error message.
        void PostErrorMessage(const char* message);

        /// Helper for posting a simple warning message.
        ///
        /// @param message [in] The warning message.
        void PostWarningMessage(const char* message);

        /// Helper for posting a simple log message.
        ///
        /// @param message [in] The log message.
        void PostLogMessage(const char* message);


        
        /// Adds a message handler.
        ///
        /// @param messageHandler [in] A reference to the message handler to add.
        void AddMessageHandler(MessageHandler &messageHandler);

        /// Removes a message handler.
        ///
        /// @param messageHandler [in] A reference to the message handler to remove.
        void RemoveMessageHandler(MessageHandler &messageHandler);


        /// Retrieves the number of message handlers that are attached to the dispatcher.
        size_t GetMessageHandlerCount() const;

        /// Retrieves a reference to the message handler at the given index.
        ///
        /// @param index [in] The index of the message handler to retrieve.
        ///
        /// @return A reference to the message handler at the given index.
        MessageHandler & GetMessageHandlerByIndex(size_t index);



    private:

        /// The list of message handlers which will have messages dispatched to them.
        GTLib::Vector<MessageHandler*> m_messageHandlers;


    private:    // No copying.
        MessageDispatcher(const MessageDispatcher &);
        MessageDispatcher & operator=(const MessageDispatcher &);
    };
}

#endif // !__GT_MessageDispatcher_hpp_
