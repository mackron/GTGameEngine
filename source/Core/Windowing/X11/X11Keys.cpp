// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifdef __linux__

#include <GTEngine/Core/Windowing/X11/X11Keys.hpp>

namespace GT
{
    namespace X11
    {
        Key FromX11VirtualKey(KeySym key)
        {
            if (key >= XK_space && key <= XK_asciitilde)
            {
                return (Key)key;
            }

            if (key >= XK_KP_0 && key <= XK_KP_9)
            {
                return (key - XK_KP_0) + Keys::Zero;
            }


            switch (key)
            {
            case XK_Pause:          return Keys::Pause;
            case XK_Scroll_Lock:    return Keys::ScrollLock;

            case XK_KP_Equal:       return static_cast<Key>('=');
            case XK_KP_Multiply:    return static_cast<Key>('*');
            case XK_KP_Divide:      return static_cast<Key>('/');
            case XK_KP_Add:         return static_cast<Key>('+');
            case XK_KP_Subtract:    return static_cast<Key>('-');
            case XK_KP_Decimal:     return static_cast<Key>('.');
            case XK_KP_Separator:   return static_cast<Key>(',');   // <-- Is this correct?


            case XK_space:
            case XK_KP_Space:
            {
                return Keys::Space;
            }

            case XK_Tab:
            case XK_KP_Tab:
            {
                return Keys::Tab;
            }

            case XK_Return:
            case XK_KP_Enter:
            {
                return Keys::Return;
            }

            case XK_Delete:
            case XK_KP_Delete:
            {
                return Keys::Delete;
            }

            case XK_BackSpace:  return Keys::Backspace;
            case XK_Escape:     return Keys::Escape;

                /*
            case XK_Shift_L:    return Keys::LeftShift;
            case XK_Shift_R:    return Keys::RightShift;
            case XK_Control_L:  return Keys::LeftCtrl;
            case XK_Control_R:  return Keys::RightCtrl;
            case XK_Alt_L:      return Keys::LeftAlt;
            case XK_Alt_R:      return Keys::RightAlt;
                */

            case XK_Shift_L:    return Keys::Shift;
            case XK_Shift_R:    return Keys::Shift;
            case XK_Control_L:  return Keys::Ctrl;
            case XK_Control_R:  return Keys::Ctrl;
            case XK_Alt_L:      return Keys::Alt;
            case XK_Alt_R:      return Keys::Alt;
            case XK_Super_L:    return Keys::Super;
            case XK_Super_R:    return Keys::Super;
            case XK_Caps_Lock:  return Keys::CapsLock;

            case XK_Left:
            case XK_KP_Left:
            {
                return Keys::ArrowLeft;
            }

            case XK_Right:
            case XK_KP_Right:
            {
                return Keys::ArrowRight;
            }

            case XK_Up:
            case XK_KP_Up:
            {
                return Keys::ArrowUp;
            }

            case XK_Down:
            case XK_KP_Down:
            {
                return Keys::ArrowDown;
            }

            case XK_F1:
            case XK_KP_F1:
            {
                return Keys::F1;
            }

            case XK_F2:
            case XK_KP_F2:
            {
                return Keys::F2;
            }

            case XK_F3:
            case XK_KP_F3:
            {
                return Keys::F3;
            }

            case XK_F4:
            case XK_KP_F4:
            {
                return Keys::F4;
            }

            case XK_F5:         return Keys::F5;
            case XK_F6:         return Keys::F6;
            case XK_F7:         return Keys::F7;
            case XK_F8:         return Keys::F8;
            case XK_F9:         return Keys::F9;
            case XK_F10:        return Keys::F10;
            case XK_F11:        return Keys::F11;
            case XK_F12:        return Keys::F12;
            case XK_F13:        return Keys::F13;
            case XK_F14:        return Keys::F14;
            case XK_F15:        return Keys::F15;
            case XK_F16:        return Keys::F16;
            case XK_F17:        return Keys::F17;
            case XK_F18:        return Keys::F18;
            case XK_F19:        return Keys::F19;
            case XK_F20:        return Keys::F20;
            case XK_F21:        return Keys::F21;
            case XK_F22:        return Keys::F22;
            case XK_F23:        return Keys::F23;
            case XK_F24:        return Keys::F24;

            case XK_End:
            case XK_KP_End:
            {
                return Keys::End;
            }

            case XK_Home:
            case XK_KP_Home:
            {
                return Keys::Home;
            }

            case XK_Page_Up:
            case XK_KP_Page_Up:
            {
                return Keys::PageUp;
            }

            case XK_Page_Down:
            case XK_KP_Page_Down:
            {
                return Keys::PageDown;
            }


            case XK_Select:         return Keys::Select;
            case XK_Print:          return Keys::Print;
            case XK_Execute:        return Keys::Execute;
            case XK_Insert:         return Keys::Insert;
            case XK_Undo:           return Keys::Undo;
            case XK_Redo:           return Keys::Redo;
            case XK_Menu:           return Keys::Menu;
            case XK_Find:           return Keys::Find;
            case XK_Cancel:         return Keys::Cancel;
            case XK_Help:           return Keys::Help;
            case XK_Break:          return Keys::Break;
            case XK_Mode_switch:    return Keys::ModeSwitch;
            case XK_Num_Lock:       return Keys::NumLock;


            case XK_ISO_Left_Tab:   return Keys::Tab;       // <-- When the tab key is hit while shift is pressed.


            default: break;
            }

            return Keys::Null;
        }
        
        bool IsX11KeyPrintable(KeySym key)
        {
            // Some specific keys need to be considered printable.
            if (key == XK_BackSpace)
            {
                return true;
            }

            // If we get here we can just use the cross-platform version.
            return IsKeyPrintable(FromX11VirtualKey(key));
        }
        
        Key GetKeyFromX11KeyEvent(XEvent &e)
        {
            KeySym  tempKeySym;
            char    tempStr[8];
            XLookupString(&e.xkey, tempStr, 8, &tempKeySym, nullptr);
            
            return FromX11VirtualKey(tempKeySym);
        }
    }
}

#endif // __linux__
