// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/Threading/Job.hpp>

namespace GTLib
{
    namespace Threading
    {
        Job::Job()
            : next(nullptr)
        {
        }

        Job::~Job()
        {
        }
    }
}
