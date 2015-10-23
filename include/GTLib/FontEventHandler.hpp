// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_FontEventHandler_hpp_
#define __GTLib_FontEventHandler_hpp_

namespace GTLib
{
    class FontServer;
    class Font;

    class FontEventHandler
    {
    public:

        /**
        *   \brief  Destructor.
        */
        virtual ~FontEventHandler();

        /**
        *   \brief  Called after a font is created.
        */
        virtual void OnCreateFont(FontServer &server, Font &font);

        /**
        *   \brief  Called just before a font is destroyed.
        */
        virtual void OnDeleteFont(FontServer &server, Font &font);
    };
}

#endif