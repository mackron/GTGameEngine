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

#define GT_ENABLE_VULKAN			1
#define GT_ENABLE_D3D12				1
#define GT_ENABLE_OPENGL            1

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


// Asset Management
//
// The engine has built-in support for various types of assets. Whether or not support for these assets should be included in the build can
// be controlled from here.
//
// There are two reasons why you may want to disable built-in support for an asset:
//   1) The host application can do it better
//   2) The size of the executable can be reduced if the asset type is not needed.

#define GT_ENABLE_PNG   1       //< Enable PNG images.
#define GT_ENABLE_TGA   1       //< Enable TGA images.
#define GT_ENABLE_JPG   1       //< Enable JPG images.
#define GT_ENABLE_PSD   1       //< Enable PSD images. PSD files are good for rapid turnaround times with texture creation, however disabling this will save a few KB in a release build.
#define GT_ENABLE_OBJ   1       //< Enable OBJ models.
#define GT_ENABLE_MD2   1       //< Enable MD2 models.
#define GT_ENABLE_OGEX  1       //< Enable OpenGEX models.
#define GT_ENABLE_WAV   1       //< Enable WAV sounds.
#define GT_ENABLE_OGG   1       //< Enable OGG sounds.




//////////////////////////////////////////////////
// Non-Configurable Section
//
// This section should not be configured by hand, unless it's for the development of the engine itself.

// Vulkan Support.
#if (defined(GT_ENABLE_VULKAN) && GT_ENABLE_VULKAN == 1)
#define GT_BUILD_VULKAN
#endif

// D3D12 Support.
//
// Currently, only Visual C++ has been tested for D3D12, so this is restricted to VC compilers for now.
#if (defined(GT_ENABLE_D3D12) && GT_ENABLE_D3D12 == 1) && defined(GT_PLATFORM_WINDOWS) && defined(GT_COMPILER_VC)
#define GT_BUILD_D3D12
#endif

// OpenGL Support.
#if (defined(GT_ENABLE_OPENGL) && GT_ENABLE_OPENGL == 1)
#define GT_BUILD_OPENGL
#endif

#if defined(GT_BUILD_VULKAN) || \
    defined(GT_BUILD_D3D12)  || \
    defined(GT_BUILD_OPENGL)
#define GT_BUILD_DEFAULT_GRAPHICS_INTERFACES
#endif



// Assets
#if defined(GT_ENABLE_PNG) && GT_ENABLE_PNG == 1
#define GT_BUILD_PNG
#endif
#if defined(GT_ENABLE_TGA) && GT_ENABLE_TGA == 1
#define GT_BUILD_TGA
#endif
#if defined(GT_ENABLE_JPG) && GT_ENABLE_JPG == 1
#define GT_BUILD_JPG
#endif
#if defined(GT_ENABLE_PSD) && GT_ENABLE_PSD == 1
#define GT_BUILD_PSD
#endif
#if defined(GT_ENABLE_OBJ) && GT_ENABLE_OBJ == 1
#define GT_BUILD_OBJ
#endif
#if defined(GT_ENABLE_MD2) && GT_ENABLE_MD2 == 1
#define GT_BUILD_MD2
#endif
#if defined(GT_ENABLE_OGEX) && GT_ENABLE_OGEX == 1
#define GT_BUILD_OGEX
#endif
#if defined(GT_ENABLE_WAV) && GT_ENABLE_WAV == 1
#define GT_BUILD_WAV
#endif
#if defined(GT_ENABLE_OGG) && GT_ENABLE_OGG == 1
#define GT_BUILD_OGG
#endif

#if defined(GT_BUILD_PNG)  || \
    defined(GT_BUILD_TGA)  || \
    defined(GT_BUILD_JPG)  || \
    defined(GT_BUILD_PSD)  || \
    defined(GT_BUILD_OBJ)  || \
    defined(GT_BUILD_MD2)  || \
    defined(GT_BUILD_OGEX) || \
    defined(GT_BUILD_WAV)  || \
    defined(GT_BUILD_OGG)
#define GT_BUILD_DEFAULT_ASSETS
#endif




// TODO: Remove D3D11 and OpenGL 4 support (replaced with D3D12 and Vulkan, with OpenGL as a legacy fallback.
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
