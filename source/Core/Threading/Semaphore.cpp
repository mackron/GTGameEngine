// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTGE/Core/Threading/Semaphore.hpp>
#include <easy_util/easy_util.h>

namespace GT
{
    Semaphore::Semaphore(int value)
        : data(nullptr)
    {
        this->data = easyutil_create_semaphore(value);
    }

    Semaphore::~Semaphore()
    {
        easyutil_delete_semaphore((easyutil_semaphore)this->data);
    }

    void Semaphore::Wait()
    {
        easyutil_wait_semaphore((easyutil_semaphore)this->data);
    }

    void Semaphore::Release()
    {
        easyutil_release_semaphore((easyutil_semaphore)this->data);
    }
}
