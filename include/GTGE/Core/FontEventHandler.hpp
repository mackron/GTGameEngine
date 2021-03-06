// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_FontEventHandler
#define GT_FontEventHandler

namespace GT
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