// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_DefaultMessageHandler
#define GT_DefaultMessageHandler

#include "MessageHandler.hpp"
#include <GTGE/Core/Log.hpp>
#include <easy_fs/easy_vfs.h>

namespace GT
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
        void OpenLogFile(easyvfs_context* pVFS, const char* filePath);


        /// MessageHandler::HandleMessage()
        void HandleMessage(const Message &message) override;



    private:

        /// The log to write the messages to.
        LogFile m_log;
    };
}

#endif // !GT_DefaultMessageHandler
