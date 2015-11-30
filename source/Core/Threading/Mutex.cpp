// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTEngine/Core/Threading/Mutex.hpp>
#include <easy_util/easy_util.h>

namespace GT
{
    Mutex::Mutex()
        : data(nullptr)
    {
        this->data = (void*)easyutil_create_mutex();
    }

    Mutex::~Mutex()
    {
        easyutil_delete_mutex((easyutil_mutex)this->data);
    }

    void Mutex::Lock()
    {
        easyutil_lock_mutex((easyutil_mutex)this->data);
    }

    void Mutex::Unlock()
    {
        easyutil_unlock_mutex((easyutil_mutex)this->data);
    }
}
