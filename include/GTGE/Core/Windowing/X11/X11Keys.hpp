// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifdef __linux__

namespace GT
{
    namespace X11
    {
        /// Converts an X11 KeySym key to a Key.
        Key FromX11VirtualKey(KeySym key);

        /// Determines if the given X11 key is printable.
        ///
        /// @remarks
        ///     This will return true for the backspace key because it needs to be handled like any other printable character
        ///     when the user is typing.
        bool IsX11KeyPrintable(KeySym key);
        
        /// Retrieves the GTLib key code from the given xkey event.
        ///
        /// @remarks
        ///     If the key is unknown, Keys::Null will be returned.
        Key GetKeyFromX11KeyEvent(XEvent &e);
    }
}

#endif // __linux__
