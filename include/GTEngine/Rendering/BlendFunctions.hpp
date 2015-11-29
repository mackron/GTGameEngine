// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_BlendFunctions_hpp_
#define __GTEngine_Rendering_BlendFunctions_hpp_

#include <GTEngine/Core/Strings/Equal.hpp>

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


    inline bool DoesBlendFunctionUseConstantColour(BlendFunc function)
    {
        return function == BlendFunc_ConstantColour         ||
               function == BlendFunc_OneMinusConstantColour ||
               function == BlendFunc_ConstantAlpha          ||
               function == BlendFunc_OneMinusConstantAlpha;
    }


    /// Converts a string to a BlendFunction value. Returns Zero if it is invalid. Not case sensitive.
    inline BlendFunc ToBlendFunction(const char* value)
    {
        if (GTLib::Strings::Equal<false>("Zero", value))
        {
            return BlendFunc_Zero;
        }
        else if (GTLib::Strings::Equal<false>("One", value))
        {
            return BlendFunc_One;
        }

        else if (GTLib::Strings::Equal<false>("SourceColour", value))
        {
            return BlendFunc_SourceColour;
        }
        else if (GTLib::Strings::Equal<false>("OneMinusSourceColour", value))
        {
            return BlendFunc_OneMinusSourceColour;
        }
        else if (GTLib::Strings::Equal<false>("SourceAlpha", value))
        {
            return BlendFunc_SourceAlpha;
        }
        else if (GTLib::Strings::Equal<false>("OneMinusSourceAlpha", value))
        {
            return BlendFunc_OneMinusSourceAlpha;
        }

        else if (GTLib::Strings::Equal<false>("DestColour", value))
        {
            return BlendFunc_DestColour;
        }
        else if (GTLib::Strings::Equal<false>("OneMinusDestColour", value))
        {
            return BlendFunc_OneMinusDestColour;
        }
        else if (GTLib::Strings::Equal<false>("DestAlpha", value))
        {
            return BlendFunc_DestAlpha;
        }
        else if (GTLib::Strings::Equal<false>("OneMinusDestAlpha", value))
        {
            return BlendFunc_OneMinusDestAlpha;
        }

        else if (GTLib::Strings::Equal<false>("ConstantColour", value))
        {
            return BlendFunc_ConstantColour;
        }
        else if (GTLib::Strings::Equal<false>("OneMinusConstantColour", value))
        {
            return BlendFunc_OneMinusConstantColour;
        }
        else if (GTLib::Strings::Equal<false>("ConstantAlpha", value))
        {
            return BlendFunc_ConstantAlpha;
        }
        else if (GTLib::Strings::Equal<false>("OneMinusConstantAlpha", value))
        {
            return BlendFunc_OneMinusConstantAlpha;
        }

        else if (GTLib::Strings::Equal<false>("SourceAlphaSaturate", value))
        {
            return BlendFunc_SourceAlphaSaturate;
        }


        return BlendFunc_Zero;
    }
}

#endif