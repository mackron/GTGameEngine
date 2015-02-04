// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GE_Config_hpp_
#define __GT_GE_Config_hpp_

#include <GTLib/Config.hpp>

//////////////////////////////////////////////////
// Configurable Section
//
// This section can be configured by hand to customize the build.

// Rendering APIs
//
// Rendering APIs can be enabled and disabled from here. By default, everything is enabled. To disable these, either comment out the line or
// change the value to 0.
//
// Disabling APIs can be useful for simplifying and reducing the size of the build.

#define GT_GE_ENABLE_D3D11          1
#define GT_GE_ENABLE_OPENGL45       1
#define GT_GE_ENABLE_D3D9           1
#define GT_GE_ENABLE_OPENGL21       1



//////////////////////////////////////////////////
// Non-Configurable Section
//
// This section should not be configured by hand, unless it's for the development of the engine itself.

// D3D11 Support.
//
// Currently, only Visual C++ 2013 has been tested for D3D11, so this is restricted to VC compilers for now.
#if (defined(GT_GE_ENABLE_D3D11) && GT_GE_ENABLE_D3D11 == 1) && defined(GT_PLATFORM_WINDOWS) && defined(GT_COMPILER_VC)
#define GT_GE_BUILD_D3D11
#endif

// OpenGL 4.5 Support.
#if (defined(GT_GE_ENABLE_OPENGL45) && GT_GE_ENABLE_OPENGL45 == 1)
#define GT_GE_BUILD_OPENGL45
#endif

// D3D9 Support.
//
// Currently, only Visual C++ 2013 has been tested for D3D11, so this is restricted to VC compilers for now.
#if (defined(GT_GE_ENABLE_D3D9) && GT_GE_ENABLE_D3D9 == 1) && defined(GT_PLATFORM_WINDOWS) && defined(GT_COMPILER_VC)
#define GT_GE_BUILD_D3D9
#endif

// OpenGL 2.1 Support.
#if (defined(GT_GE_ENABLE_OPENGL21) && GT_GE_ENABLE_OPENGL21 == 1)
#define GT_GE_BUILD_OPENGL21
#endif


#endif
