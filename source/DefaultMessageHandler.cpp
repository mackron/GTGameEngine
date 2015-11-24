// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/DefaultMessageHandler.hpp>

namespace GT
{
    namespace Engine
    {
        DefaultMessageHandler::DefaultMessageHandler()
            : MessageHandler(),
              m_log()
        {
        }

        DefaultMessageHandler::~DefaultMessageHandler()
        {
            m_log.Close();
        }


        void DefaultMessageHandler::OpenLogFile(easyvfs_context* pVFS, const char* filePath)
        {
            m_log.Open(pVFS, filePath, "GTEngine");
        }


        void DefaultMessageHandler::HandleMessage(const Message &message)
        {
            GTLib::String content;
            
            switch (message.GetType())
            {
            case MessageType_Error:
                {
                    content.Append("Error: ");
                    break;
                }

            case MessageType_Warning:
                {
                    content.Append("Warning: ");
                    break;
                }

            case MessageType_Log:
            case MessageType_Other:
            default:
                {
                    break;
                }
            }

            content.Append(message.GetContent());


            // The content string has been built, so now we just write it to the log.
            m_log.WriteString(content.c_str());

            // We also want to write it to stdout.
            printf("%s\n", content.c_str());
        }
    }
}