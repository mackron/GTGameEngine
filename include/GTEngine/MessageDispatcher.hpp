// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#ifndef __GT_MessageDispatcher_hpp_
#define __GT_MessageDispatcher_hpp_

#include "MessageHandler.hpp"
#include <GTLib/Vector.hpp>

namespace GT
{
    /// Class used for dispatching messages to the various message handlers.
    class MessageDispatcher
    {
    public:

        /// Constructor.
        MessageDispatcher();

        
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
