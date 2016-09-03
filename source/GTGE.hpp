// Copyright (C) 2011 - 2016 David Reid. See included LICENCE.

#ifndef GTGE_H
#define GTGE_H

// These #defines enable us to load large files on Linux platforms. They need to be placed before including any headers.
#ifndef _WIN32
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#endif

// Standard headers.
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

// Platform headers.
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __linux__
// TODO: Implement me.
#endif


// GTGE Headers.
#include "../include/GTGE/Math.hpp"

#include "../include/GTGE/Animation/AnimationKeyFrame.hpp"
#include "../include/GTGE/Animation/AnimationKey.hpp"
#include "../include/GTGE/Animation/AnimationChannel.hpp"
#include "../include/GTGE/Animation/AnimationSegment.hpp"
#include "../include/GTGE/Animation/AnimationSequence.hpp"
#include "../include/GTGE/Animation/AnimationTrack.hpp"
#include "../include/GTGE/Animation/Animation.hpp"

#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifdef GTGE_IMPLEMENTATION

#include "Animation/AnimationKeyFrame.cpp"
#include "Animation/AnimationKey.cpp"
#include "Animation/AnimationChannel.cpp"
#include "Animation/AnimationSequence.cpp"
#include "Animation/AnimationTrack.cpp"
#include "Animation/Animation.cpp"


#endif