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
#define GT_GE_ENABLE_OPENGL4        1


// Optimize for Single Rendering Device
//
// The engine can support multiple rendering devices (contexts), however there is some overhead with it, in particular with OpenGL. In the
// case of OpenGL, the appropriate context needs to be made current at a global level before making the applicable rendering command. What
// this means is that for every draw call, a check needs to be performed to ensure the correct context is current at the time of calling. This
// check involves a comparison plus a call to wglGetCurrentContext() - this occurs for every function that calls an OpenGL function.
//
// In most applications however, only a single rendering device will be used. We can therefore offer an optimization that essentially assumes
// the application will only ever use a single rendering device per thread per rendering API. Set the variable below to 1 to enable this
// optimization, however please note that using this means continued use of multiple rendering devices on the same thread results in undefined
// behaviour.
//
// When using OpenGL, this optimization can be safely enabled if only a single rendering device is used per-thread. For example, consider an
// application with two threads, with each of those threads having their own rendering device object. In this case there are two rendering
// devices, but because they will only ever be used on a single thread, it should be safe to enable this optimization.
#define GT_GE_OPTIMIZE_FOR_SINGLE_RENDERING_DEVICE      0





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

// OpenGL 4.x Support.
#if (defined(GT_GE_ENABLE_OPENGL4) && GT_GE_ENABLE_OPENGL4 == 1)
#define GT_GE_BUILD_OPENGL4
#endif


#endif
