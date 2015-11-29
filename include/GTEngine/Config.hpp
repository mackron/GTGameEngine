// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_GE_Config
#define GT_GE_Config

#include <GTLib/Config.hpp>

//////////////////////////////////////////////////
// Configurable Section
//
// This section can be configured by hand to customize the build.


// Editor
//
// The editor can be enabled or disabled from here. When the editor is disabled, none of it's components are included in the build,
// and the editor can never be opened.
//
// Disabling the editor is useful for reducing the size of the build for when the editor isn't really needed. An example would be
// phone games where the editor would never be used in a final build.

#define GT_ENABLE_EDITOR            1



// Rendering APIs
//
// Rendering APIs can be enabled and disabled from here. By default, everything is enabled. To disable these, either comment out the line or
// change the value to 0.
//
// Disabling APIs can be useful for simplifying and reducing the size of the build.

#define GT_ENABLE_VULKAN			0
#define GT_ENABLE_D3D12				0
#define GT_ENABLE_OPENGL            1



// Asset Management
//
// The engine has built-in support for various types of assets. Whether or not support for these assets should be included in the build can
// be controlled from here.
//
// There are two reasons why you may want to disable built-in support for an asset:
//   1) The host application can do it better
//   2) The size of the executable can be reduced if the asset type is not needed.

#define GT_ENABLE_PNG       1       ///< Enable PNG images.
#define GT_ENABLE_TGA       1       ///< Enable TGA images.
#define GT_ENABLE_JPG       1       ///< Enable JPG images.
#define GT_ENABLE_PSD       1       ///< Enable PSD images. PSD files are good for rapid turnaround times with texture creation, however disabling this will save a few KB in a release build.
#define GT_ENABLE_MTL       1       ///< Enable MTL materials (OBJ materials).
#define GT_ENABLE_OBJ       1       ///< Enable OBJ models.
#define GT_ENABLE_MD2       1       ///< Enable MD2 models.
#define GT_ENABLE_OGEX      1       ///< Enable OpenGEX models.
#define GT_ENABLE_WAV       1       ///< Enable WAV sounds.
#define GT_ENABLE_VORBIS    1       ///< Enable Ogg Vorbis sounds.



//////////////////////////////////////////////////
// Non-Configurable Section
//
// This section should not be configured by hand, unless it's for the development of the engine itself.

// Editor.
#if (defined(GT_ENABLE_EDITOR) && GT_ENABLE_EDITOR == 1)
#define GT_BUILD_EDITOR
#endif



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
#if defined(GT_ENABLE_MTL) && GT_ENABLE_MTL == 1
#define GT_BUILD_MTL
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
#if defined(GT_ENABLE_VORBIS) && GT_ENABLE_VORBIS == 1
#define GT_BUILD_VORBIS
#endif

#if defined(GT_BUILD_PNG)  || \
    defined(GT_BUILD_TGA)  || \
    defined(GT_BUILD_JPG)  || \
    defined(GT_BUILD_PSD)  || \
    defined(GT_BUILD_MTL)  || \
    defined(GT_BUILD_OBJ)  || \
    defined(GT_BUILD_MD2)  || \
    defined(GT_BUILD_OGEX) || \
    defined(GT_BUILD_WAV)  || \
    defined(GT_BUILD_VORBIS)
#define GT_BUILD_DEFAULT_ASSETS
#endif


#endif
