// Copyright (C) 2011 - 2016 David Reid. See included LICENCE.

#ifndef GTGE_H
#define GTGE_H

#define _CRT_SECURE_NO_WARNINGS

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
#include "../include/GTGE/Core/Profiling/valgrind/callgrind.h"
#include "../include/GTGE/Core/Profiling/valgrind/helgrind.h"
#include "../include/GTGE/Core/Profiling/valgrind/memcheck.h"
#include "../include/GTGE/Core/Profiling/valgrind/valgrind.h"
#endif


// GLM
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4201)
    #pragma warning(disable:4310)
    #pragma warning(disable:4324)
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
    #if defined(__clang__)
        #pragma GCC diagnostic ignored "-Wunused-parameter"
    #endif
#endif
#define GLM_FORCE_ONLY_XYZW
#define GLM_FORCE_PURE
#define GLM_SIMD_ENABLE_XYZW_UNION
#define GLM_SIMD_ENABLE_DEFAULT_INIT
#define GLM_FORCE_RADIANS
#include "../include/glm/glm/glm.hpp"
#include "../include/glm/glm/ext.hpp"
#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__)
    #pragma GCC dianostic pop
#endif


// GTGE Headers.
#include "../include/GTGE/Core/Strings/Compare.hpp"
#include "../include/GTGE/Core/Strings/Copy.hpp"
#include "../include/GTGE/Core/Strings/Create.hpp"
#include "../include/GTGE/Core/Strings/Equal.hpp"
#include "../include/GTGE/Core/Strings/Find.hpp"
#include "../include/GTGE/Core/Strings/Iterator.hpp"
#include "../include/GTGE/Core/Strings/LineIterator.hpp"
#include "../include/GTGE/Core/Strings/List.hpp"
#include "../include/GTGE/Core/Strings/NextChar.hpp"
#include "../include/GTGE/Core/Strings/Replacer.hpp"
#include "../include/GTGE/Core/Strings/Size.hpp"
#include "../include/GTGE/Core/Strings/Tokenizer.hpp"
#include "../include/GTGE/Core/Strings/Trim.hpp"
#include "../include/GTGE/Core/Strings/UnicodeBase.hpp"
#include "../include/GTGE/Core/String.hpp"

#include "../include/GTGE/Core/Timing/Benchmarker.hpp"
#include "../include/GTGE/Core/Timing/Stopwatch.hpp"
#include "../include/GTGE/Core/Timing/Timer.hpp"
#include "../include/GTGE/Core/Timing/TimingCommon.hpp"

#include "../include/GTGE/Math.hpp"
#include "../include/GTGE/Math/Plane.hpp"

#include "../include/GTGE/AABB.hpp"

#include "../include/GTGE/Animation/AnimationKeyFrame.hpp"
#include "../include/GTGE/Animation/AnimationKey.hpp"
#include "../include/GTGE/Animation/AnimationChannel.hpp"
#include "../include/GTGE/Animation/AnimationSegment.hpp"
#include "../include/GTGE/Animation/AnimationSequence.hpp"
#include "../include/GTGE/Animation/AnimationTrack.hpp"
#include "../include/GTGE/Animation/Animation.hpp"

#include "../include/GTGE/GTEngine.hpp"

#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef GTGE_IMPLEMENTATION
#undef LoadString

// Lua
#ifndef GTGE_NO_DEFAULT_LUA
#define LUA_CORE
#include "external/lua-5.1.5/src/loslib.c"  // <-- Needs to be first.
#include "external/lua-5.1.5/src/lapi.c"
#include "external/lua-5.1.5/src/lcode.c"
#include "external/lua-5.1.5/src/ldebug.c"
#include "external/lua-5.1.5/src/ldo.c"
#include "external/lua-5.1.5/src/ldump.c"
#include "external/lua-5.1.5/src/lfunc.c"
#include "external/lua-5.1.5/src/lgc.c"
#include "external/lua-5.1.5/src/llex.c"
#include "external/lua-5.1.5/src/lmem.c"
#include "external/lua-5.1.5/src/lobject.c"
#include "external/lua-5.1.5/src/lopcodes.c"
#include "external/lua-5.1.5/src/lparser.c"
#include "external/lua-5.1.5/src/lstate.c"
#include "external/lua-5.1.5/src/lstring.c"
#include "external/lua-5.1.5/src/ltable.c"
#include "external/lua-5.1.5/src/ltm.c"
#include "external/lua-5.1.5/src/lundump.c"
#include "external/lua-5.1.5/src/lvm.c"
#include "external/lua-5.1.5/src/lzio.c"
#include "external/lua-5.1.5/src/lauxlib.c"
#include "external/lua-5.1.5/src/lbaselib.c"
#include "external/lua-5.1.5/src/ldblib.c"
#include "external/lua-5.1.5/src/liolib.c"
#include "external/lua-5.1.5/src/lmathlib.c"
#include "external/lua-5.1.5/src/ltablib.c"
#include "external/lua-5.1.5/src/lstrlib.c"
#include "external/lua-5.1.5/src/loadlib.c"
#include "external/lua-5.1.5/src/linit.c"
#endif

#include "external/lua-5.1.5/src/lua.h"
#include "external/lua-5.1.5/src/lualib.h"
#include "external/lua-5.1.5/src/lauxlib.h"


#include "Core/Strings/NextChar.cpp"
#include "Core/Timing/Stopwatch.cpp"
#include "Core/Timing/Timer.cpp"
#include "Core/Timing/TimingCommon.cpp"
#include "Core/Windowing/Clipboard.cpp"
#include "Core/Windowing/WindowEventHandler.cpp"
#include "Core/Windowing/Win32/Win32.cpp"
#include "Core/Windowing/Win32/Window_Win32.cpp"
#include "Core/Windowing/X11/X11.cpp"
#include "Core/Windowing/X11/Window_X11.cpp"
#include "Core/Windowing/X11/X11Keys.cpp"
#include "Core/BasicBuffer.cpp"
#include "Core/Colour.cpp"
#include "Core/DateTime.cpp"
#include "Core/Font.cpp"
#include "Core/FontEngine.cpp"
#include "Core/FontEngine_FCFT.cpp"
#include "Core/FontEngine_Win32.cpp"
#include "Core/FontEventHandler.cpp"
#include "Core/FontServer.cpp"
#include "Core/GlyphCache.cpp"
#include "Core/GlyphMapLayout.cpp"
#include "Core/GlyphMapManager.cpp"
#include "Core/HandleManager.cpp"
#include "Core/Image.cpp"
#include "Core/ImageDataConverter.cpp"
#include "Core/ImageLoader.cpp"
#include "Core/ImageLoader_PNG.cpp"
#include "Core/ImageUtils.cpp"
#include "Core/Keyboard.cpp"
#include "Core/Mipmap.cpp"
#include "Core/MipmapGenerator.cpp"
#include "Core/Mouse.cpp"
#include "Core/Parse.cpp"
#include "Core/stdio.cpp"
#include "Core/System.cpp"
#include "Core/TextManager.cpp"
#include "Core/TextMesh.cpp"
#include "Core/ToString.cpp"
#include "Core/Window.cpp"
#include "Core/WindowEventCallback.cpp"
#include "Core/WindowManagement.cpp"

#include "Animation/AnimationKeyFrame.cpp"
#include "Animation/AnimationKey.cpp"
#include "Animation/AnimationChannel.cpp"
#include "Animation/AnimationSequence.cpp"
#include "Animation/AnimationTrack.cpp"
#include "Animation/Animation.cpp"

#include "Assets/Asset.cpp"
#include "Assets/AssetAllocator.cpp"
#include "Assets/AssetLibrary.cpp"
#include "Assets/AssetMetadata.cpp"
#include "Assets/DefaultAssetAllocator.cpp"
#include "Assets/ImageAsset.cpp"
#include "Assets/ImageAsset_STB.cpp"
#include "Assets/MaterialAsset.cpp"
#include "Assets/MaterialAsset_MTL.cpp"
#include "Assets/ModelAsset.cpp"
#include "Assets/ModelAsset_MD2.cpp"
#include "Assets/ModelAsset_OBJ.cpp"
#include "Assets/SoundAsset.cpp"



#endif