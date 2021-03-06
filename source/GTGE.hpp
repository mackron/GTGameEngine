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

#include "../include/GTGE/Config.hpp"

// Standard headers.
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <locale>
#include <new>
#include <functional>
#include <algorithm>
#include <bitset>

#if defined(_MSC_VER)
#include <direct.h>
#endif

// Platform headers.
#ifdef _WIN32
#if !defined(WINVER)
#define WINVER 0x0501
#elif WINVER < 0x0501
#undef  WINVER
#define WINVER 0x0501
#endif

#include <windows.h>
#include <windowsx.h>
#undef CreateWindow
#undef DeleteFile
#undef CopyFile
#undef SetCurrentDirectory
#undef GetCurrentDirectory
#undef CreateDirectory
#undef CreateFont
#undef DeleteFont
#undef LoadImage
#undef GetClassName
#undef GetCommandLine
#undef PostMessage
#undef min
#undef max
#undef MIN
#undef MAX
#undef next

#if defined(_MSC_VER) || defined(__clang__)
#pragma comment(lib, "msimg32.lib")
#endif
#endif  // __WIN32
#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/XKBlib.h>
#include "../include/GTGE/Core/Profiling/valgrind/callgrind.h"
#include "../include/GTGE/Core/Profiling/valgrind/helgrind.h"
#include "../include/GTGE/Core/Profiling/valgrind/memcheck.h"
#include "../include/GTGE/Core/Profiling/valgrind/valgrind.h"
#endif  // __linux__


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


// rapidxml
#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
    #pragma GCC diagnostic ignored "-Winline"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include "../include/GTGE/external/rapidxml.hpp"
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif


// STB Libraries
#if defined(GT_BUILD_VORBIS)
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_HEADER_ONLY
#include "../include/GTGE/external/stb_vorbis.c"
#endif


// dr_libs
//
// These should be placed after stb_vorbis so that dr_audio can detect it.
#include <dr_libs/dr.h>
#include <dr_libs/dr_fs.h>
#include <dr_libs/dr_fsw.h>

#if defined(GT_BUILD_FLAC)
#include <dr_libs/dr_flac.h>
#endif

#if defined(GT_BUILD_WAV)
#include <dr_libs/dr_wav.h>
#endif

#include <dr_libs/dr_audio.h>
#include <dr_libs/old/dr_mtl.h>


// GTGE Headers.
#include "../include/GTGE/Core/stdlib.hpp"
#include "../include/GTGE/Core/stdio.hpp"
#include "../include/GTGE/Core/BasicBuffer.hpp"
#include "../include/GTGE/Core/BinarySearchTree.hpp"
#include "../include/GTGE/Core/BufferReader.hpp"
#include "../include/GTGE/Core/Colour.hpp"
#include "../include/GTGE/Core/Limits.hpp"
#include "../include/GTGE/Core/ReferenceCountedObject.hpp"
#include "../include/GTGE/Core/ResultCodes.hpp"
#include "../include/GTGE/Core/Strings/UnicodeBase.hpp"
#include "../include/GTGE/Core/Strings/NextChar.hpp"
#include "../include/GTGE/Core/Strings/Compare.hpp"
#include "../include/GTGE/Core/Strings/Iterator.hpp"
#include "../include/GTGE/Core/Strings/Size.hpp"
#include "../include/GTGE/Core/Strings/Copy.hpp"
#include "../include/GTGE/Core/Strings/Equal.hpp"
#include "../include/GTGE/Core/Strings/List.hpp"
#include "../include/GTGE/Core/Strings/Create.hpp"
#include "../include/GTGE/Core/Strings/Find.hpp"
#include "../include/GTGE/Core/Strings/Tokenizer.hpp"
#include "../include/GTGE/Core/Strings/LineIterator.hpp"
#include "../include/GTGE/Core/Strings/Replacer.hpp"
#include "../include/GTGE/Core/Strings/Trim.hpp"
#include "../include/GTGE/Core/String.hpp"
#include "../include/GTGE/Core/Parse.hpp"
#include "../include/GTGE/Core/ToString.hpp"
#include "../include/GTGE/Core/DateTime.hpp"
#include "../include/GTGE/Core/Cursors.hpp"
#include "../include/GTGE/Core/Math.hpp"
#include "../include/GTGE/Core/Rect.hpp"
#include "../include/GTGE/Core/Point.hpp"
#include "../include/GTGE/Core/Vector.hpp"
#include "../include/GTGE/Core/List.hpp"
#include "../include/GTGE/Core/Map.hpp"
#include "../include/GTGE/Core/Dictionary.hpp"
#include "../include/GTGE/Core/SortedVector.hpp"
#include "../include/GTGE/Core/Deserializer.hpp"
#include "../include/GTGE/Core/Serializer.hpp"
#include "../include/GTGE/Core/System.hpp"
#include "../include/GTGE/Core/Timing/TimingCommon.hpp"
#include "../include/GTGE/Core/Timing/Benchmarker.hpp"
#include "../include/GTGE/Core/Timing/Stopwatch.hpp"
#include "../include/GTGE/Core/Timing/Timer.hpp"
#include "../include/GTGE/Core/Windowing/Clipboard.hpp"
#include "../include/GTGE/Core/Windowing/InternalWindowObjects.hpp"
#include "../include/GTGE/Core/Windowing/Keys.hpp"
#include "../include/GTGE/Core/Windowing/MouseButtons.hpp"
#include "../include/GTGE/Core/Windowing/WindowEventHandler.hpp"
#include "../include/GTGE/Core/Windowing/X11/X11.hpp"
#include "../include/GTGE/Core/Windowing/X11/X11Keys.hpp"
#include "../include/GTGE/Core/Windowing/Win32/Win32.hpp"
#include "../include/GTGE/Core/Keyboard.hpp"
#include "../include/GTGE/Core/Mouse.hpp"
#include "../include/GTGE/Core/GlyphMapManager.hpp"
#include "../include/GTGE/Core/GlyphRenderingModes.hpp"
#include "../include/GTGE/Core/GlyphMetrics.hpp"
#include "../include/GTGE/Core/FontStyles.hpp"
#include "../include/GTGE/Core/FontInfo.hpp"
#include "../include/GTGE/Core/FontMetrics.hpp"
#include "../include/GTGE/Core/KerningVector.hpp"
#include "../include/GTGE/Core/FontEngine.hpp"
#include "../include/GTGE/Core/GlyphMapLayout.hpp"
#include "../include/GTGE/Core/Font.hpp"
#include "../include/GTGE/Core/FontEventHandler.hpp"
#include "../include/GTGE/Core/FontServer.hpp"
#include "../include/GTGE/Core/GlyphCache.hpp"
#include "../include/GTGE/Core/HandleManager.hpp"
#include "../include/GTGE/Core/ImageFormats.hpp"
#include "../include/GTGE/Core/Mipmap.hpp"
#include "../include/GTGE/Core/ImageFileInfo.hpp"
#include "../include/GTGE/Core/ImageLoader.hpp"
#include "../include/GTGE/Core/Image.hpp"
#include "../include/GTGE/Core/ImageDataConverter.hpp"
#include "../include/GTGE/Core/ImageUtils.hpp"
#include "../include/GTGE/Core/MipmapGenerator.hpp"
#include "../include/GTGE/Core/Random.hpp"
#include "../include/GTGE/Core/TextMesh.hpp"
#include "../include/GTGE/Core/TextManager.hpp"
#include "../include/GTGE/Core/Window.hpp"
#include "../include/GTGE/Core/WindowEvent.hpp"
#include "../include/GTGE/Core/WindowEventCallback.hpp"
#include "../include/GTGE/Core/WindowManagement.hpp"

#include "../include/GTGE/Math.hpp"
#include "../include/GTGE/Math/Plane.hpp"
#include "../include/GTGE/Script.hpp"

#include "../include/GTGE/GUI/GUIAlign.hpp"
#include "../include/GTGE/GUI/GUIGarbageCollector.hpp"
#include "../include/GTGE/GUI/GUIImageManager.hpp"
#include "../include/GTGE/GUI/Rendering/GUIMesh.hpp"
#include "../include/GTGE/GUI/Rendering/GUIElementRenderingData.hpp"
#include "../include/GTGE/GUI/Rendering/GUIRenderer.hpp"
#include "../include/GTGE/GUI/GUIElementEventHandler.hpp"
#include "../include/GTGE/GUI/GUIStyleNumber.hpp"
#include "../include/GTGE/GUI/GUIPositioning.hpp"
#include "../include/GTGE/GUI/GUIPlanes.hpp"
#include "../include/GTGE/GUI/GUIFontStyles.hpp"
#include "../include/GTGE/GUI/GUIStyleAttribute.hpp"
#include "../include/GTGE/GUI/GUIStyleClass.hpp"
#include "../include/GTGE/GUI/GUIStyleClassTree.hpp"
#include "../include/GTGE/GUI/GUIStyleStack.hpp"
#include "../include/GTGE/GUI/GUIElement.hpp"
#include "../include/GTGE/GUI/GUICaret.hpp"
#include "../include/GTGE/GUI/GUIElementTree.hpp"
#include "../include/GTGE/GUI/GUIEventCodes.hpp"
#include "../include/GTGE/GUI/GUIEvent.hpp"
#include "../include/GTGE/GUI/GUIEventQueue.hpp"
#include "../include/GTGE/GUI/GUIFontCache.hpp"
#include "../include/GTGE/GUI/GUIFontGlyphMapManager.hpp"
#include "../include/GTGE/GUI/GUILayoutManager.hpp"
#include "../include/GTGE/GUI/GUIServerEventHandler.hpp"
#include "../include/GTGE/GUI/GUIServerXMLParser.hpp"
#include "../include/GTGE/GUI/GUIMarkupLoader.hpp"
#include "../include/GTGE/GUI/GUIScriptServer.hpp"
#include "../include/GTGE/GUI/GUIStyleScriptError.hpp"
#include "../include/GTGE/GUI/GUIStyleScriptCompiler.hpp"
#include "../include/GTGE/GUI/GUIStyleServer.hpp"
#include "../include/GTGE/GUI/GUIServer.hpp"

#include "../include/GTGE/Graphics/GraphicsTypes.hpp"

#include "../include/GTGE/Rendering/TextureFilters.hpp"
#include "../include/GTGE/Rendering/TextureWrapModes.hpp"
#include "../include/GTGE/Rendering/Texture2D.hpp"
#include "../include/GTGE/Rendering/TextureCube.hpp"
#include "../include/GTGE/Texture2DLibrary.hpp"
#include "../include/GTGE/ShaderParameter.hpp"
#include "../include/GTGE/ShaderParameterCache.hpp"
#include "../include/GTGE/Rendering/BlendEquations.hpp"
#include "../include/GTGE/Rendering/BlendFunctions.hpp"
#include "../include/GTGE/Rendering/BufferTypes.hpp"
#include "../include/GTGE/Rendering/DrawModes.hpp"
#include "../include/GTGE/Rendering/PolygonModes.hpp"
#include "../include/GTGE/Rendering/RendererFunctions.hpp"
#include "../include/GTGE/Rendering/ShaderStages.hpp"
#include "../include/GTGE/Rendering/StencilOperations.hpp"
#include "../include/GTGE/Rendering/Framebuffer.hpp"
#include "../include/GTGE/Rendering/VertexFormat.hpp"
#include "../include/GTGE/Rendering/VertexAttributes.hpp"
#include "../include/GTGE/Rendering/VertexArray.hpp"
#include "../include/GTGE/Rendering/Shader.hpp"
#include "../include/GTGE/Rendering/Renderer.hpp"
#include "../include/GTGE/Rendering/DefaultGUIRenderer.hpp"

#include "../include/GTGE/AABB.hpp"
#include "../include/GTGE/AlignedType.hpp"
#include "../include/GTGE/Bone.hpp"
#include "../include/GTGE/CollisionGroups.hpp"
#include "../include/GTGE/Component.hpp"
#include "../include/GTGE/ConvexHullBuildSettings.hpp"
#include "../include/GTGE/ConvexHull.hpp"
#include "../include/GTGE/Message.hpp"
#include "../include/GTGE/MessageHandler.hpp"
#include "../include/GTGE/MessageDispatcher.hpp"
#include "../include/GTGE/DefaultGUIImageManager.hpp"

#include "../include/GTGE/Animation/AnimationKeyFrame.hpp"
#include "../include/GTGE/Animation/AnimationKey.hpp"
#include "../include/GTGE/Animation/AnimationChannel.hpp"
#include "../include/GTGE/Animation/AnimationSegment.hpp"
#include "../include/GTGE/Animation/AnimationSequence.hpp"
#include "../include/GTGE/Animation/AnimationTrack.hpp"
#include "../include/GTGE/Animation/Animation.hpp"

#include "../include/GTGE/Assets/AssetTypes.hpp"
#include "../include/GTGE/Assets/AssetMetadata.hpp"
#include "../include/GTGE/Assets/Asset.hpp"
#include "../include/GTGE/Assets/AssetAllocator.hpp"
#include "../include/GTGE/Assets/AssetLibrary.hpp"
#include "../include/GTGE/Assets/ImageAsset.hpp"
#include "../include/GTGE/Assets/MaterialAsset.hpp"
#include "../include/GTGE/Assets/ModelAsset.hpp"
#include "../include/GTGE/Assets/SoundAsset.hpp"

#include "../include/GTGE/Audio/SoundStreamer.hpp"
#include "../include/GTGE/Audio/SoundWorld.hpp"

#include "../include/GTGE/ScriptVariableTypes.hpp"
#include "../include/GTGE/ScriptVariable.hpp"
#include "../include/GTGE/ScriptDefinition.hpp"
#include "../include/GTGE/ScriptLibrary.hpp"
#include "../include/GTGE/Context.hpp"
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
#undef PlaySound
#undef DeleteFont
#undef GetNextSibling
#undef GetPrevSibling
#undef GetFirstChild
#undef GetClassName

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4459)   // Hides global declaration (thanks Lua!)
    #pragma warning(disable:4822)   // local class member function does not have a body
#endif

// HACD
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4456)   // declaration of '...' hides previous local declaration
#endif
#include "external/HACD/hacdHACD.cpp"
#include "external/HACD/hacdGraph.cpp"
#include "external/HACD/hacdICHull.cpp"
#include "external/HACD/hacdManifoldMesh.cpp"
#include "external/HACD/hacdMeshDecimator.cpp"
#include "external/HACD/hacdMicroAllocator.cpp"
#include "external/HACD/hacdRaycastMesh.cpp"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

// GTGL
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4457)
#endif
#include "external/gtgl/gtgl.c"
#include "external/gtgl/gtgl_context.c"
#include "external/gtgl/gtgl_contextmanager.c"
#include "external/gtgl/gtgl_errors.c"
#include "external/gtgl/gtgl_extensions.c"
#include "external/gtgl/gtgl_gl.c"
#include "external/gtgl/gtgl_glx.c"
#include "external/gtgl/gtgl_utils.c"
#include "external/gtgl/gtgl_wgl.c"
#include "external/gtgl/gtgl_window.c"
#include "external/gtgl/gtgl_windowmanager.c"
#include "external/gtgl/debugging/gtgl_debugging_common.c"
#include "external/gtgl/emulators/gtgl_emulation_common.c"
#include "external/gtgl/emulators/gtgl_arb_debug_output.c"
#include "external/gtgl/win32/gtgl_context_win32.c"
#include "external/gtgl/win32/gtgl_window_win32.c"
#include "external/gtgl/win32/gtgl_windowmanager_win32.c"
#include "external/gtgl/x11/gtgl_context_x11.c"
#include "external/gtgl/x11/gtgl_window_x11.c"
#include "external/gtgl/x11/gtgl_windowmanager_x11.c"
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif


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


// STB
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4996)   // This function or variable may be unsafe.
    #pragma warning(disable:4189)   // local variable is initialized but not referenced
    #pragma warning(disable:4244)   // conversion from '...' to '...', possible loss of data
    #pragma warning(disable:4100)   // unreferenced formal parameter
    #pragma warning(disable:4456)   // declaration of '...' hides previous local declaration
    #pragma warning(disable:4457)   // declaration of '...' hides function parameter
    #pragma warning(disable:4245)   // conversion from '...' to '...', signed/unsigned mismatch
    #pragma warning(disable:4701)   // potentially uninitialized local variable '...' used
    #pragma warning(disable:4127)   // conditional expression is constant
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_BMP
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#if !defined(GT_BUILD_PNG)
#define STBI_NO_PNG
#endif
#if !defined(GT_BUILD_TGA)
#define STBI_NO_TGA
#endif
#if !defined(GT_BUILD_JPG)
#define STBI_NO_JPEG
#endif
#if !defined(GT_BUILD_PSD)
#define STBI_NO_PSD
#endif
#include "../include/GTGE/external/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/GTGE/external/stb_image_write.h"

#define STB_DXT_IMPLEMENTATION
#include "../include/GTGE/external/stb_dxt.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../include/GTGE/external/stb_truetype.h"

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif





// GTGE Source Files
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

#include "Audio/SoundStreamer.cpp"
#include "Audio/SoundWorld.cpp"

#include "Components/CameraComponent.cpp"
#include "Components/CollisionShapeComponent.cpp"
#include "Components/ConeTwistConstraintComponent.cpp"
#include "Components/DynamicsComponent.cpp"
#include "Components/EditorMetadataComponent.cpp"
#include "Components/GenericConstraintComponent.cpp"
#include "Components/LightComponent.cpp"
#include "Components/ModelComponent.cpp"
#include "Components/OccluderComponent.cpp"
#include "Components/ParticleSystemComponent.cpp"
#include "Components/PointToPointConstraintComponent.cpp"
#include "Components/PrefabComponent.cpp"
#include "Components/ProximityComponent.cpp"
#include "Components/ScriptComponent.cpp"

#include "DefaultSceneRenderer/DefaultSceneRenderer.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_DirectionalLight.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_Framebuffers.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_LightGroup.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_LightManager.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_LuminanceChain.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_MaterialShaderID.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_MaterialShaders.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_MaterialUniformNameCache.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_Mesh.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_MultiPassPipeline.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_PointLight.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_ShaderBuilder.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_ShadowVisibilityProcessor.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_SinglePassPipeline.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_SpotLight.cpp"
#include "DefaultSceneRenderer/DefaultSceneRenderer_VisibilityProcessor.cpp"

#if 0
#include "Graphics/DefaultGraphicsWorld_Null.cpp"
#include "Graphics/DefaultGraphicsWorld_OpenGL.cpp"
#include "Graphics/GraphicsAPI.cpp"
#include "Graphics/GraphicsAPI_Null.cpp"
#include "Graphics/GraphicsAPI_OpenGL.cpp"
#include "Graphics/GraphicsAssetResourceManager.cpp"
#include "Graphics/GraphicsAssetResources.cpp"
#include "Graphics/GraphicsModelObject.cpp"
#include "Graphics/GraphicsWorld.cpp"
#include "Graphics/GraphicsWorldObject.cpp"
#endif

#include "GUI/Rendering/GUIElementRenderingData.cpp"
#include "GUI/Rendering/GUIMesh.cpp"
#include "GUI/Rendering/GUIRenderer.cpp"
#include "GUI/GUICaret.cpp"
#include "GUI/GUIElement.cpp"
#include "GUI/GUIElementEventHandler.cpp"
#include "GUI/GUIElementTree.cpp"
#include "GUI/GUIEventQueue.cpp"
#include "GUI/GUIFontCache.cpp"
#include "GUI/GUIFontGlyphMapManager.cpp"
#include "GUI/GUIGarbageCollector.cpp"
#include "GUI/GUIImageManager.cpp"
#include "GUI/GUILayoutManager.cpp"
#include "GUI/GUIMarkupLoader.cpp"
#include "GUI/GUIScriptServer.cpp"
#include "GUI/GUIServer.cpp"
#include "GUI/GUIServerEventHandler.cpp"
#include "GUI/GUIServerXMLParser.cpp"
#include "GUI/GUIStandardLibrary.cpp"
#include "GUI/GUIStandardLibrary_Images.cpp"
#include "GUI/GUIStyleAttributeHandler.cpp"
#include "GUI/GUIStyleClass.cpp"
#include "GUI/GUIStyleClassTree.cpp"
#include "GUI/GUIStyleScriptCompiler.cpp"
#include "GUI/GUIStyleServer.cpp"
#include "GUI/GUIStyleStack.cpp"
#include "GUI/GUIStyleTokenizer.cpp"

#include "Physics/BaseCollisionWorld.cpp"
#include "Physics/btEllipsoidShape.cpp"
#include "Physics/CollisionObject.cpp"
#include "Physics/CollisionWorld.cpp"
#include "Physics/ConeTwistConstraint.cpp"
#include "Physics/DynamicsWorld.cpp"
#include "Physics/GenericConstraint.cpp"
#include "Physics/GhostObject.cpp"
#include "Physics/MeshCollisionShape.cpp"
#include "Physics/PointToPointConstraint.cpp"
#include "Physics/RigidBody.cpp"
#include "Physics/SceneNodeMotionState.cpp"
#include "Physics/StaticMeshCollisionShape.cpp"

#include "Rendering/OpenGL/OpenGL21/Framebuffer_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/FramebufferState_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/Renderer_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/ServerState_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/Shader_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/ShaderState_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/State_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/Texture2D_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/TextureCube_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/TextureState_OpenGL21.cpp"
#include "Rendering/OpenGL/OpenGL21/VertexArray_OpenGL21.cpp"
#include "Rendering/OpenGL/Debugging_OpenGL.cpp"
#include "Rendering/DefaultGUIRenderer.cpp"
#include "Rendering/Framebuffer.cpp"
#include "Rendering/Renderer.cpp"
#include "Rendering/Shader.cpp"
#include "Rendering/Texture2D.cpp"
#include "Rendering/TextureCube.cpp"
#include "Rendering/VertexArray.cpp"
#include "Rendering/VertexFormat.cpp"

#include "Scripting/Scripting_Animation.cpp"
#include "Scripting/Scripting_Audio.cpp"
#include "Scripting/Scripting_Components.cpp"
#include "Scripting/Scripting_Editor.cpp"
#include "Scripting/Scripting_Game.cpp"
#include "Scripting/Scripting_Math.cpp"
#include "Scripting/Scripting_Particles.cpp"
#include "Scripting/Scripting_Rendering.cpp"
#include "Scripting/Scripting_Scene.cpp"
#include "Scripting/Scripting_SceneNode.cpp"

#include "Utilities/DynamicCharacterController.cpp"

#include "Bone.cpp"
#include "Component.cpp"
#include "Context.cpp"
#include "ConvexHull.cpp"
#include "CPUVertexShader.cpp"
#include "CPUVertexShader_SimpleTransform.cpp"
#include "CPUVertexShader_Skinning.cpp"
#include "DefaultGUIImageManager.cpp"
#include "DefaultPrefabLinker.cpp"
#include "DefaultSceneCullingManager.cpp"
#include "DefaultScenePhysicsManager.cpp"
#include "DefaultSceneUpdateManager.cpp"
#include "Editor.cpp"
#include "GameEventFilter.cpp"
#include "GameEventQueue.cpp"
#include "GamePackager.cpp"
#include "GameScript.cpp"
#include "GameStateManager.cpp"
#include "GameWindowEventHandler.cpp"
#include "GTEngine.cpp"
#include "GUIEventHandler.cpp"
#include "IO.cpp"
#include "Material.cpp"
#include "MaterialLibrary.cpp"
#include "MaterialShaderCache.cpp"
#include "Math.cpp"
#include "Mesh.cpp"
#include "MeshBuilder.cpp"
#include "Message.cpp"
#include "MessageDispatcher.cpp"
#include "MessageHandler.cpp"
#include "Model.cpp"
#include "ModelDefinition.cpp"
#include "ModelDefinition_Assimp.cpp"
#include "ModelLibrary.cpp"
#include "NavigationMesh.cpp"
#include "Particle.cpp"
#include "ParticleEmitter.cpp"
#include "ParticleFunction.cpp"
#include "ParticleList.cpp"
#include "ParticleSystem.cpp"
#include "ParticleSystemDefinition.cpp"
#include "ParticleSystemLibrary.cpp"
#include "Prefab.cpp"
#include "PrefabLibrary.cpp"
#include "PrefabLinker.cpp"
#include "Profiler.cpp"
#include "Projectile.cpp"
#include "Scene.cpp"
#include "SceneCullingDbvtPolicy.cpp"
#include "SceneDeserializeCallback.cpp"
#include "SceneEventHandler.cpp"
#include "SceneNode.cpp"
#include "SceneNodeEventHandler.cpp"
#include "SceneNodeMap.cpp"
#include "SceneRenderer.cpp"
#include "SceneStateStack.cpp"
#include "SceneStateStackBranch.cpp"
#include "SceneStateStackFrame.cpp"
#include "SceneStateStackRestoreCommands.cpp"
#include "SceneStateStackStagingArea.cpp"
#include "SceneViewport.cpp"
#include "Script.cpp"
#include "ScriptDefinition.cpp"
#include "Scripting.cpp"
#include "ScriptLibrary.cpp"
#include "ScriptVariable.cpp"
#include "ShaderLibrary.cpp"
#include "ShaderParameter.cpp"
#include "ShaderParameterCache.cpp"
#include "ShadowVolume.cpp"
#include "Texture2DLibrary.cpp"
#include "VertexArrayLibrary.cpp"

#include "Editor/ImageEditor/ImageEditor.cpp"
#include "Editor/MaterialEditor/MaterialEditor.cpp"
#include "Editor/ModelEditor/ModelEditor.cpp"
#include "Editor/PackagingToolEditor/PackagingToolEditor.cpp"
#include "Editor/ParticleEditor/ParticleEditor.cpp"
#include "Editor/SceneEditor/SceneEditor.cpp"
#include "Editor/SceneEditor/SceneEditorCommand.cpp"
#include "Editor/SceneEditor/SceneEditorGameEventFilter.cpp"
#include "Editor/SceneEditor/SceneEditorPhysicsManager.cpp"
#include "Editor/SceneEditor/SceneEditorPrefabLinker.cpp"
#include "Editor/SceneEditor/SceneEditorSceneEventHandler.cpp"
#include "Editor/SceneEditor/SceneEditorSceneUpdateManager.cpp"
#include "Editor/TextEditor/TextEditor.cpp"
#include "Editor/Editor3DViewportEventHandler.cpp"
#include "Editor/EditorAxisArrows.cpp"
#include "Editor/EditorGrid.cpp"
#include "Editor/PivotPoint.cpp"
#include "Editor/SubEditor.cpp"
#include "Editor/SubEditorTabEventHandler.cpp"
#include "Editor/TransformGizmo.cpp"


// dr_libs
//
// These should be placed after stb_vorbis so that dr_audio can detect it.
#define DR_IMPLEMENTATION
#include <dr_libs/dr.h>

#define DR_FS_IMPLEMENTATION
#include <dr_libs/dr_fs.h>

#define DR_FSW_IMPLEMENTATION
#include <dr_libs/dr_fsw.h>

#if defined(GT_BUILD_FLAC)
#define DR_FLAC_IMPLEMENTATION
#include <dr_libs/dr_flac.h>
#endif

#if defined(GT_BUILD_WAV)
#define DR_WAV_IMPLEMENTATION
#include <dr_libs/dr_wav.h>
#endif

#define DR_AUDIO_IMPLEMENTATION
#include <dr_libs/dr_audio.h>

#define DR_MTL_IMPLEMENTATION
#include <dr_libs/old/dr_mtl.h>

#if defined(GT_BUILD_VORBIS)
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4244)   // conversion from '...' to '...', possible loss of data
    #pragma warning(disable:4100)   // unreferenced formal parameter
    #pragma warning(disable:4456)   // declaration of '...' hides previous local declaration
    #pragma warning(disable:4457)   // declaration of '...' hides function parameter
    #pragma warning(disable:4701)   // potentially uninitialized local variable '...' used
#endif
#undef STB_VORBIS_HEADER_ONLY
#include <GTGE/external/stb_vorbis.c>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
#endif


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif