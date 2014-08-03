// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file.

#ifndef __GT_Engine_DefaultMessageHandler_hpp_
#define __GT_Engine_DefaultMessageHandler_hpp_

#include "MessageHandler.hpp"
#include <GTLib/Log.hpp>

namespace GT
{
    namespace Engine
    {
        /// The engine's default message handler.
        ///
        /// This message handler simply writes the message to the log file.
        class DefaultMessageHandler : public MessageHandler
        {
        public:

            /// Constructor.
            DefaultMessageHandler();

            /// Destructor.
            ~DefaultMessageHandler();


            /// Opens the log file.
            ///
            /// @param filePath [in] The path of the log file to write the log to.
            void OpenLogFile(const char* filePath);


            /// MessageHandler::HandleMessage()
            void HandleMessage(const Message &message) override;



        private:

            /// The log to write the messages to.
            GTLib::Log m_log;
        };
    }
}

#endif // !__GT_Engine_DefaultMessageHandler_hpp_