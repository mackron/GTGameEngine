// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/FontEventHandler.hpp>
#include <GTEngine/Core/FontServer.hpp>

namespace GT
{
    FontEventHandler::~FontEventHandler()
    {
    }

    void FontEventHandler::OnCreateFont(FontServer &, Font &)
    {
    }

    void FontEventHandler::OnDeleteFont(FontServer &, Font &)
    {
    }
}