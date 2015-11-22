// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_GUI_ServerEventHandler_hpp_
#define __GTLib_GUI_ServerEventHandler_hpp_

#include <GTLib/Cursors.hpp>
#include <GTLib/Font.hpp>

namespace GTGUI
{
    /**
    *   \brief  Base class for handling events from the server.
    *
    *   An application will typically inherit from this class and then attach an object of that class
    *   to a server via the Server::SetEventHandler() method.
    *
    *   By default, the event handler will use GTLib::PostError() for both OnError() and OnWarning().
    */
    class ServerEventHandler
    {
    public:
    
        /**
        *   \brief           Called when there is an error.
        *   \param  msg [in] The error message.
        */
        virtual void OnError(const char *msg);
        
        /**
        *   \brief           Called when there is a warning.
        *   \param  msg [in] The warning message.
        */
        virtual void OnWarning(const char *msg);
        
        /**
        *   \brief           Called when something is being logged.
        *   \param  msg [in] The log message.
        */
        virtual void OnLog(const char *msg);

        /**
        *   \brief  Called when the applications needs to change the cursor.
        */
        virtual void OnChangeCursor(GTLib::Cursor cursor);

        /**
        *   \brief  Called when the application needs to load a font.
        */
        virtual void OnLoadFont(GTLib::Font &font);

        /**
        *   \brief  Called when the application needs to unload a font.
        */
        virtual void OnUnloadFont(GTLib::Font &font);

    
    
    public: // Static stuff.
    
        /// The default event handler.
        static ServerEventHandler Default;
        
        
    public:
    
        virtual ~ServerEventHandler() {}
    };
}

#endif