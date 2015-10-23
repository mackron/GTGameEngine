// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Errors_hpp_
#define __GTLib_Errors_hpp_

#include "IO.hpp"
#include <cstdarg>  // For variable arguments.

namespace GTLib
{
    namespace Errors
    {
		/**
        *   \brief  Posts an error which will be intercepted by each ErrorHandler instance.
        *
        *   \remarks
        *       This function is thread-safe. It will always post the error to every handler before posting other errors.
        */
        void Post(const char *msg, ...);
        void VPost(const char *msg, va_list argList1, va_list argList2);

        /**
        *   \brief  Class used to handle errors as their received.
        */
        class ErrorHandler
        {
        public:
        
            /// Constructor.
            ErrorHandler();
            
            /// Destructor.
            virtual ~ErrorHandler();
        
            /**
            *   \brief  Called after an error has been posted. This will call the handler for any descendants, too.
            */
            virtual void Error(const char *msg);
            
        private:
        
            friend void GTLib::Errors::VPost(const char *, va_list, va_list);
        
            /// A pointer to the next error handler.
            ErrorHandler *next;
            
            
        private:    // No copying.
        
            ErrorHandler(const ErrorHandler &);
            ErrorHandler & operator=(const ErrorHandler &);
        };
    }
    
    /// Convenience typedef.
    typedef Errors::ErrorHandler ErrorHandler;
    
    /// A helper function for errors::post.
    void PostError(const char *msg, ...);
}

#endif
