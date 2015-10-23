// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Regex.hpp>
#include <GTLib/Errors.hpp>
#include <GTLib/Strings/Size.hpp>

extern "C" {
    #include "slre.h"
}

namespace GTLib
{
    Regex::Regex(const char *pattern)
        : internalData(nullptr)
    {
        this->internalData = malloc(sizeof(slre));
        if (!slre_compile(reinterpret_cast<slre*>(this->internalData), pattern))
        {
            GTLib::PostError("Regex::Regex() - Compilation failed %s", reinterpret_cast<slre*>(this->internalData)->err_str);
        }
    }

    Regex::~Regex()
    {
        free(this->internalData);
    }

    bool Regex::Match(const char *subject)
    {
        return slre_match(reinterpret_cast<slre*>(this->internalData), subject, static_cast<int>(Strings::SizeInTs(subject)), nullptr) != 0;
    }
}
