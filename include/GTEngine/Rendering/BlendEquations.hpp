// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_Rendering_BlendEquations_hpp_
#define __GTEngine_Rendering_BlendEquations_hpp_

#include <GTLib/Strings/Equal.hpp>

namespace GTEngine
{
    enum BlendEquation
    {
        BlendEquation_Add,
        BlendEquation_Subtract,
        BlendEquation_ReverseSubtract,
        BlendEquation_Min,
        BlendEquation_Max
    };


    /// Converts a string to a BlendFunction value. Returns Add if it is invalid. Not case sensitive.
    inline BlendEquation ToBlendEquation(const char* value)
    {
        if (GTLib::Strings::Equal<false>("Add", value))
        {
            return BlendEquation_Add;
        }
        else if (GTLib::Strings::Equal<false>("Subtract", value))
        {
            return BlendEquation_Subtract;
        }
        else if (GTLib::Strings::Equal<false>("ReverseSubtract", value))
        {
            return BlendEquation_ReverseSubtract;
        }
        else if (GTLib::Strings::Equal<false>("Min", value))
        {
            return BlendEquation_Min;
        }
        else if (GTLib::Strings::Equal<false>("Max", value))
        {
            return BlendEquation_Max;
        }
        
        return BlendEquation_Add;
    }
}

#endif