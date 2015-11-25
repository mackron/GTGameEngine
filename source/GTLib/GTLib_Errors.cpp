// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Errors.hpp>
#include <GTLib/Threading/Mutex.hpp>
#include <GTLib/stdlib.hpp>
#include <GTLib/stdio.hpp>

namespace GTLib
{
    namespace Errors
    {
        /// The mutex controlling syncs for error handlers.
        GTLib::Mutex errorMutex;

        /// The root error handler.
        ErrorHandler *rootHandler = nullptr;

        /// The last error handler.
        ErrorHandler *lastHandler = nullptr;


        ErrorHandler::ErrorHandler()
            : next(nullptr)
        {
            errorMutex.Lock();

            // If we don't have a root error handler, it will be this one.
            if (!rootHandler)
            {
                rootHandler = this;
            }

            // The newest handler is always the last one.
            if (lastHandler)
            {
                lastHandler->next = this;
            }
            lastHandler = this;

            errorMutex.Unlock();
        }

        ErrorHandler::~ErrorHandler()
        {
            errorMutex.Lock();

            // We need to remove this handler from the list.
            if (rootHandler)
            {
                ErrorHandler *i    = rootHandler;
                ErrorHandler *prev = nullptr;
                while (i)
                {
                    if (i == this)
                    {
                        if (prev)
                        {
                            prev->next = i->next;
                            if (prev->next == nullptr)
                            {
                                lastHandler = prev;
                            }
                        }
                        else
                        {
                            rootHandler = i->next;
                            if (rootHandler == nullptr)
                            {
                                lastHandler = nullptr;
                            }
                        }

                        break;
                    }

                    prev = i;
                    i = i->next;
                }
            }

            errorMutex.Unlock();
        }

        void ErrorHandler::Error(const char *)
        {
        }


        void Post(const char *msg, ...)
        {
            va_list argList1;
            va_list argList2;
            va_start(argList1, msg);
            va_start(argList2, msg);

            VPost(msg, argList1, argList2);

            va_end(argList1);
            va_end(argList2);
        }

        void VPost(const char *msg, va_list argList1, va_list argList2)
        {
            errorMutex.Lock();

            if (rootHandler)
            {
                // We need to transform the message.
                size_t msgLength = (size_t)GTLib::IO::vsnprintf(nullptr, 0, msg, argList1);
                char *newMsg     = (char *)malloc(msgLength + 1);

                GTLib::IO::vsnprintf(newMsg, msgLength + 1, msg, argList2);

                ErrorHandler *i = rootHandler;
                while (i)
                {
                    i->Error(newMsg);
                    i = i->next;
                }

                free(newMsg);
            }

            errorMutex.Unlock();
        }
    }

    void PostError(const char *msg, ...)
    {
        va_list argList1;
        va_list argList2;
        va_start(argList1, msg);
        va_start(argList2, msg);

        Errors::VPost(msg, argList1, argList2);

        va_end(argList1);
        va_end(argList2);
    }
}
