// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

// This is a speciail header for cstdlib which will also include crtdbg.h in MSVC builds. We use crtdbg.h to do memory leak detection.
#ifndef __GTLib_stdlib_hpp_
#define __GTLib_stdlib_hpp_

#include <cstdlib>

#if defined(_MSC_VER)
#include <crtdbg.h>
#endif

#endif