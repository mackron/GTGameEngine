// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIServerEventHandler
#define GT_GUIServerEventHandler

namespace GT
{
    /**
    *   \brief  Base class for handling events from the server.
    *
    *   An application will typically inherit from this class and then attach an object of that class
    *   to a server via the GUIServer::SetEventHandler() method.
    *
    *   By default, the event handler will use PostError() for both OnError() and OnWarning().
    */
    class GUIServerEventHandler
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
        virtual void OnChangeCursor(GT::Cursor cursor);

        /**
        *   \brief  Called when the application needs to load a font.
        */
        virtual void OnLoadFont(GT::Font &font);

        /**
        *   \brief  Called when the application needs to unload a font.
        */
        virtual void OnUnloadFont(GT::Font &font);

    
    
    public: // Static stuff.
    
        /// The default event handler.
        static GUIServerEventHandler Default;
        
        
    public:
    
        virtual ~GUIServerEventHandler() {}
    };
}

#endif
