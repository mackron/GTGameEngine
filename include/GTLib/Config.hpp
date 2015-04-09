// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_Config_hpp_
#define __GTLib_Config_hpp_

// Platform detection. If a library needs to detect platforms, you can copy/paste this section and simply rename GT_PLATFORM_WINDOWS, etc.
#if   !defined(GT_PLATFORM_WINDOWS) && (defined(__WIN32__) || defined(_WIN32) || defined(_WIN64))
#define GT_PLATFORM_WINDOWS
#elif !defined(GT_PLATFORM_LINUX)   &&  defined(__linux__)
#define GT_PLATFORM_LINUX
#elif !defined(GT_PLATFORM_MAC)     && (defined(__APPLE__) && defined(__MACH__))
#define	GT_PLATFORM_MAC
#endif

// Compiler detection.
#if   !defined(GT_COMPILER_VC)    && (defined(_MSC_VER))
#define GT_COMPILER_VC
#elif !defined(GT_COMPILER_GCC)   && (defined(__GNUC__))
#define GT_COMPILER_GCC
#elif !defined(GT_COMPILER_CLANG) && (defined(__clang__))
#define GT_COMPILER_CLANG
#endif


#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif
#endif

#define ALIGNED_TYPE(t,x) t ALIGNED_(x)


#endif
