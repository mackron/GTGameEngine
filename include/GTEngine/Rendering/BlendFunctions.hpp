// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Rendering_BlendFunctions_hpp_
#define __GTEngine_Rendering_BlendFunctions_hpp_

#include <GTCore/Strings/Equal.hpp>

namespace GTEngine
{
    enum BlendFunc
    {
        BlendFunc_Zero,
        BlendFunc_One,

        BlendFunc_SourceColour,
        BlendFunc_OneMinusSourceColour,
        BlendFunc_SourceAlpha,
        BlendFunc_OneMinusSourceAlpha,

        BlendFunc_DestColour,
        BlendFunc_OneMinusDestColour,
        BlendFunc_DestAlpha,
        BlendFunc_OneMinusDestAlpha,

        BlendFunc_ConstantColour,
        BlendFunc_OneMinusConstantColour,
        BlendFunc_ConstantAlpha,
        BlendFunc_OneMinusConstantAlpha,

        BlendFunc_SourceAlphaSaturate,
    };


    /// Converts a string to a BlendFunction value. Returns Zero if it is invalid. Not case sensitive.
    inline BlendFunc ToBlendFunction(const char* value)
    {
        if (GTCore::Strings::Equal<false>("Zero", value))
        {
            return BlendFunc_Zero;
        }
        else if (GTCore::Strings::Equal<false>("One", value))
        {
            return BlendFunc_One;
        }

        else if (GTCore::Strings::Equal<false>("SourceColour", value))
        {
            return BlendFunc_SourceColour;
        }
        else if (GTCore::Strings::Equal<false>("OneMinusSourceColour", value))
        {
            return BlendFunc_OneMinusSourceColour;
        }
        else if (GTCore::Strings::Equal<false>("SourceAlpha", value))
        {
            return BlendFunc_SourceAlpha;
        }
        else if (GTCore::Strings::Equal<false>("OneMinusSourceAlpha", value))
        {
            return BlendFunc_OneMinusSourceAlpha;
        }

        else if (GTCore::Strings::Equal<false>("DestColour", value))
        {
            return BlendFunc_DestColour;
        }
        else if (GTCore::Strings::Equal<false>("OneMinusDestColour", value))
        {
            return BlendFunc_OneMinusDestColour;
        }
        else if (GTCore::Strings::Equal<false>("DestAlpha", value))
        {
            return BlendFunc_DestAlpha;
        }
        else if (GTCore::Strings::Equal<false>("OneMinusDestAlpha", value))
        {
            return BlendFunc_OneMinusDestAlpha;
        }

        else if (GTCore::Strings::Equal<false>("ConstantColour", value))
        {
            return BlendFunc_ConstantColour;
        }
        else if (GTCore::Strings::Equal<false>("OneMinusConstantColour", value))
        {
            return BlendFunc_OneMinusConstantColour;
        }
        else if (GTCore::Strings::Equal<false>("ConstantAlpha", value))
        {
            return BlendFunc_ConstantAlpha;
        }
        else if (GTCore::Strings::Equal<false>("OneMinusConstantAlpha", value))
        {
            return BlendFunc_OneMinusConstantAlpha;
        }

        else if (GTCore::Strings::Equal<false>("SourceAlphaSaturate", value))
        {
            return BlendFunc_SourceAlphaSaturate;
        }


        return BlendFunc_Zero;
    }
}

#endif