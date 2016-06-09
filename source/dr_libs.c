
#include <GTGE/Config.hpp>

// Platform libraries, for simplifying MSVC builds.
#ifdef _WIN32
#if defined(_MSC_VER) || defined(__clang__)
#pragma comment(lib, "msimg32.lib")
#endif
#endif

#define DR_IMPLEMENTATION
#include <dr_libs/dr.h>

#define DR_FS_IMPLEMENTATION
#include <dr_libs/dr_fs.h>

#define DR_FSW_IMPLEMENTATION
#include <dr_libs/dr_fsw.h>

#define DR_PATH_IMPLEMENTATION
#include <dr_libs/dr_path.h>

#if defined(GT_BUILD_FLAC)
#define DR_FLAC_IMPLEMENTATION
#include <dr_libs/dr_flac.h>
#endif

#if defined(GT_BUILD_WAV)
#define DR_WAV_IMPLEMENTATION
#include <dr_libs/dr_wav.h>
#endif

#if defined(GT_BUILD_VORBIS)
#define STB_VORBIS_HEADER_ONLY
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#include <GTGE/external/stb_vorbis.c>
#endif

#define DR_AUDIO_IMPLEMENTATION
#include <dr_libs/dr_audio.h>

#define DR_GUI_IMPLEMENTATION
#include <dr_libs/dr_gui.h>

#define DR_2D_IMPLEMENTATION
#include <dr_libs/dr_2d.h>

#define DR_MTL_IMPLEMENTATION
#include <dr_libs/dr_mtl.h>
