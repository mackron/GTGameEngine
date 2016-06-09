
#include <GTGE/Config.hpp>

#define DR_VFS_IMPLEMENTATION
#include <dr_libs/dr_fs.h>

#define DR_FSW_IMPLEMENTATION
#include <dr_libs/dr_fsw.h>

#define DR_UTIL_IMPLEMENTATION
#include <dr_libs/dr.h>

#define DR_PATH_IMPLEMENTATION
#include <dr_libs/dr_path.h>

#define DR_FLAC_IMPLEMENTATION
#include <dr_libs/dr_flac.h>

#define DR_WAV_IMPLEMENTATION
#include <dr_libs/dr_wav.h>

#if defined(GT_BUILD_VORBIS)
#define STB_VORBIS_HEADER_ONLY
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
