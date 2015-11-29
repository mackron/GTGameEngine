// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/Window.hpp>

namespace GTLib
{
    const InternalWindowObjects & Window::GetInternalObjects() const
    { 
        return this->iwo;
    }
}