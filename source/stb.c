// This file is only used for stb implementations.

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
#endif


#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_BMP
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include <GTEngine/external/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <GTEngine/external/stb_image_write.h>

#define STB_DXT_IMPLEMENTATION
#include <GTEngine/external/stb_dxt.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <GTEngine/external/stb_truetype.h>


#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#include <GTEngine/external/stb_vorbis.c>


#if defined(_MSC_VER)
    #pragma warning(pop)
#endif