// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GT_MessageHandler_hpp_
#define __GT_MessageHandler_hpp_

#include "Message.hpp"

namespace GT
{
    /// Base class used for intercepting and handling messages that are dispatched via the MessageDispatcher.
    class MessageHandler
    {
    public:

        /// Constructor.
        MessageHandler();

        /// Destructor.
        virtual ~MessageHandler();


        /// Virtual method for handling a message.
        ///
        /// @param message [in] The message to handle.
        ///
        /// @remarks
        ///     The default implementation will do nothing.
        virtual void HandleMessage(const Message &message);


    private:    // No copying.
        MessageHandler(const MessageHandler &);
        MessageHandler & operator=(const MessageHandler &);
    };
}

#endif