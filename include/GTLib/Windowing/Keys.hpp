// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_Keys
#define GT_Keys

#include <cstdint>
#include <GTLib/String.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4482)   // nonstandard extension.
#endif

namespace GTLib
{
    typedef char32_t Key;

    // Keys map to unicode code points. We use the PUA in plane 15 for keys not in Unicode, such as the function keys, home/end, etc.
    // This list should include both upper- and lower-case versions of applicable characters.
    namespace Keys
    {
        const char32_t Null         = 0x00;

        // ASCII
        const char32_t Backspace    = 0x08;
        const char32_t Tab          = 0x09;
        const char32_t Return       = 0x0D;
        const char32_t Enter        = 0x0D;
        const char32_t Escape       = 0x1B;
        const char32_t Space        = 0x20;
        const char32_t Exclamation  = 0x21;
        const char32_t DoubleQuote  = 0x22;
        const char32_t Hash         = 0x23;
        const char32_t Dollar       = 0x24;
        const char32_t Percent      = 0x25;
        const char32_t Ampersand    = 0x26;
        const char32_t Apostrophe   = 0x27;
        const char32_t ParanLeft    = 0x28;
        const char32_t ParanRight   = 0x29;
        const char32_t Asterisk     = 0x2A;
        const char32_t Plus         = 0x2B;
        const char32_t Comma        = 0x2C;
        const char32_t Minus        = 0x2D;
        const char32_t Period       = 0x2E;
        const char32_t Slash        = 0x2F;
        const char32_t Zero         = 0x30;
        const char32_t One          = 0x31;
        const char32_t Two          = 0x32;
        const char32_t Three        = 0x33;
        const char32_t Four         = 0x34;
        const char32_t Five         = 0x35;
        const char32_t Six          = 0x36;
        const char32_t Seven        = 0x37;
        const char32_t Eight        = 0x38;
        const char32_t Nine         = 0x39;
        const char32_t Colon        = 0x3A;
        const char32_t Semicolon    = 0x3B;
        const char32_t Less         = 0x3C;
        const char32_t Equal        = 0x3D;
        const char32_t Greater      = 0x3E;
        const char32_t Question     = 0x3F;
        const char32_t At           = 0x40;
        const char32_t A            = 0x41;
        const char32_t B            = 0x42;
        const char32_t C            = 0x43;
        const char32_t D            = 0x44;
        const char32_t E            = 0x45;
        const char32_t F            = 0x46;
        const char32_t G            = 0x47;
        const char32_t H            = 0x48;
        const char32_t I            = 0x49;
        const char32_t J            = 0x4A;
        const char32_t K            = 0x4B;
        const char32_t L            = 0x4C;
        const char32_t M            = 0x4D;
        const char32_t N            = 0x4E;
        const char32_t O            = 0x4F;
        const char32_t P            = 0x50;
        const char32_t Q            = 0x51;
        const char32_t R            = 0x52;
        const char32_t S            = 0x53;
        const char32_t T            = 0x54;
        const char32_t U            = 0x55;
        const char32_t V            = 0x56;
        const char32_t W            = 0x57;
        const char32_t X            = 0x58;
        const char32_t Y            = 0x59;
        const char32_t Z            = 0x5A;
        const char32_t LeftBracket  = 0x5B;
        const char32_t BackSlash    = 0x5C;
        const char32_t RightBracket = 0x5D;
        const char32_t CircumAccent = 0x5E;
        const char32_t Underscore   = 0x5F;
        const char32_t GraveAccent  = 0x60;
        const char32_t a            = 0x61;
        const char32_t b            = 0x62;
        const char32_t c            = 0x63;
        const char32_t d            = 0x64;
        const char32_t e            = 0x65;
        const char32_t f            = 0x66;
        const char32_t g            = 0x67;
        const char32_t h            = 0x68;
        const char32_t i            = 0x69;
        const char32_t j            = 0x6A;
        const char32_t k            = 0x6B;
        const char32_t l            = 0x6C;
        const char32_t m            = 0x6D;
        const char32_t n            = 0x6E;
        const char32_t o            = 0x6F;
        const char32_t p            = 0x70;
        const char32_t q            = 0x71;
        const char32_t r            = 0x72;
        const char32_t s            = 0x73;
        const char32_t t            = 0x74;
        const char32_t u            = 0x75;
        const char32_t v            = 0x76;
        const char32_t w            = 0x77;
        const char32_t x            = 0x78;
        const char32_t y            = 0x79;
        const char32_t z            = 0x7A;
        const char32_t LeftBrace    = 0x7B;
        const char32_t Bar          = 0x7C;
        const char32_t RightBrace   = 0x7D;
        const char32_t Tilde        = 0x7E;
        const char32_t Delete       = 0x7F;

        const char32_t Pause        = 0x13;
        const char32_t ScrollLock   = 0x14;

        // Non-printable keyboard keys. These are on the PUA in plane 15.
        const char32_t Shift        = 0xF0000;
        const char32_t Ctrl         = 0xF0001;
        const char32_t Alt          = 0xF0002;
        const char32_t Super        = 0xF0003;      // <-- AKA the Windows Key.
        const char32_t CapsLock     = 0xF0004;

        const char32_t ArrowLeft    = 0xF0009;
        const char32_t ArrowRight   = 0xF000A;
        const char32_t ArrowUp      = 0xF000B;
        const char32_t ArrowDown    = 0xF000C;

        const char32_t F1           = 0xF000D;
        const char32_t F2           = 0xF000E;
        const char32_t F3           = 0xF000F;
        const char32_t F4           = 0xF0010;
        const char32_t F5           = 0xF0011;
        const char32_t F6           = 0xF0012;
        const char32_t F7           = 0xF0013;
        const char32_t F8           = 0xF0014;
        const char32_t F9           = 0xF0015;
        const char32_t F10          = 0xF0016;
        const char32_t F11          = 0xF0017;
        const char32_t F12          = 0xF0018;
        const char32_t F13          = 0xF0019;
        const char32_t F14          = 0xF001A;
        const char32_t F15          = 0xF001B;
        const char32_t F16          = 0xF001C;
        const char32_t F17          = 0xF001D;
        const char32_t F18          = 0xF001E;
        const char32_t F19          = 0xF001F;
        const char32_t F20          = 0xF0020;
        const char32_t F21          = 0xF0021;
        const char32_t F22          = 0xF0022;
        const char32_t F23          = 0xF0023;
        const char32_t F24          = 0xF0024;

        const char32_t End          = 0xF0025;
        const char32_t Home         = 0xF0026;
        const char32_t PageUp       = 0xF0027;
        const char32_t PageDown     = 0xF0028;

        const char32_t Select       = 0xF0029;
        const char32_t Print        = 0xF002A;
        const char32_t Execute      = 0xF002B;
        const char32_t Insert       = 0xF002C;
        const char32_t Undo         = 0xF002D;
        const char32_t Redo         = 0xF002E;
        const char32_t Menu         = 0xF002F;
        const char32_t Find         = 0xF0030;
        const char32_t Cancel       = 0xF0031;
        const char32_t Help         = 0xF0032;
        const char32_t Break        = 0xF0033;
        const char32_t ModeSwitch   = 0xF0034;
        const char32_t NumLock      = 0xF0035;


        // Aliases
        const char32_t LeftRoundBracket   = ParanLeft;
        const char32_t RightRoundBracket  = ParanRight;
        const char32_t LeftSquareBracket  = LeftBracket;
        const char32_t RightSquareBracket = RightBracket;
        const char32_t Grave              = GraveAccent;
        const char32_t LeftCurlyBracket   = LeftBrace;
        const char32_t RightCurlyBracket  = RightBrace;
    }


    /// Determines if a key is printable.
    inline bool IsKeyPrintable(Key key)
    {
        if (key >= Keys::Space && key <= Keys::Tilde)
        {
            return true;
        }

        return false;
    }



    /// Structure representing a key combination.
    ///
    /// A key combination can have up to 2 system keys (CTRL, ALT, Shift) and up to 2 printable keys.
    struct KeyCombination
    {
        KeyCombination()
            : systemKey1(GTLib::Keys::Null),    systemKey2(GTLib::Keys::Null),
              printableKey1(GTLib::Keys::Null), printableKey2(GTLib::Keys::Null)
        {
        }

        KeyCombination(GTLib::Key systemKey, GTLib::Key printableKey)
            : systemKey1(systemKey),       systemKey2(GTLib::Keys::Null),
              printableKey1(printableKey), printableKey2(GTLib::Keys::Null)
        {
        }


        /// Determines whether or not the given key is one of the system keys.
        bool IsSystemKey(GTLib::Key key) const
        {
            return key == this->systemKey1 || key == this->systemKey2;
        }

        /// Determines whether or not the given key is one of the printable keys.
        bool IsPrintableKey(GTLib::Key key) const
        {
            return key == this->printableKey1 || key == this->printableKey2;
        }


        /// Determines whether or not any key is non-null.
        bool IsAnyKeySet() const
        {
            return this->systemKey1 != GTLib::Keys::Null || this->systemKey2 != GTLib::Keys::Null || this->printableKey1 != GTLib::Keys::Null || this->printableKey2 != GTLib::Keys::Null;
        }



        GTLib::Key systemKey1;
        GTLib::Key systemKey2;
        GTLib::Key printableKey1;
        GTLib::Key printableKey2;
    };
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
