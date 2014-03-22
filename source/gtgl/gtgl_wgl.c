/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include <gtgl/gtgl.h>

/* We only continue if we're on a WGL platform. */
#ifdef GTGL_WGL

#include "debugging/gtgl_debugging_common.h"

/* We'll need windows.h here for various Windows functions. Remember, gtgl_wgl.h does not include windows.h; only windef.h. */
#include <windows.h>

/*
Below are function pointers to wgl* functions from OpenGL32.dll. We dynamically load these so that the user does not need the
notion of linking to OpenGL32.dll. These are initialised in gtglLoadWGLSymbols(). Each of the symbols are prefixed with gtwgl so
as to avoid redefinitions.
*/
HMODULE GTGL_OpenGL32_DLL = NULL;

PFNWGLCREATECONTEXTPROC     gtwglCreateContext     = NULL;
PFNWGLDELETECONTEXTPROC     gtwglDeleteContext     = NULL;
PFNWGLMAKECURRENTPROC       gtwglMakeCurrent       = NULL;
PFNWGLGETCURRENTCONTEXTPROC gtwglGetCurrentContext = NULL;
PFNWGLGETPROCADDRESSPROC    gtwglGetProcAddress    = NULL;
PFNWGLSHARELISTSPROC        gtwglShareLists        = NULL;
PFNWGLGETCURRENTDCPROC      gtwglGetCurrentDC      = NULL;

GLboolean gtglLoadWGLSymbols()
{
    if (GTGL_OpenGL32_DLL == NULL)
    {
        GTGL_OpenGL32_DLL = LoadLibraryA("OpenGL32.dll");
        if (GTGL_OpenGL32_DLL != NULL)
        {
            gtwglCreateContext     = (PFNWGLCREATECONTEXTPROC)GetProcAddress(GTGL_OpenGL32_DLL, "wglCreateContext");
            gtwglDeleteContext     = (PFNWGLDELETECONTEXTPROC)GetProcAddress(GTGL_OpenGL32_DLL, "wglDeleteContext");
            gtwglMakeCurrent       = (PFNWGLMAKECURRENTPROC)GetProcAddress(GTGL_OpenGL32_DLL, "wglMakeCurrent");
            gtwglGetCurrentContext = (PFNWGLGETCURRENTCONTEXTPROC)GetProcAddress(GTGL_OpenGL32_DLL, "wglGetCurrentContext");
            gtwglGetProcAddress    = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(GTGL_OpenGL32_DLL, "wglGetProcAddress");
            gtwglShareLists        = (PFNWGLSHARELISTSPROC)GetProcAddress(GTGL_OpenGL32_DLL, "wglShareLists");
            gtwglGetCurrentDC      = (PFNWGLGETCURRENTDCPROC)GetProcAddress(GTGL_OpenGL32_DLL, "wglGetCurrentDC");

            return GL_TRUE;
        }
        else
        {
            return GL_FALSE;
        }
    }
    else
    {
        /* Symbols are already loaded. */
        return GL_TRUE;
    }
}


/* ARB Extensions */
GLboolean GTGL_WGL_ARB_buffer_region              = GL_FALSE;
GLboolean GTGL_WGL_ARB_multisample                = GL_FALSE;
GLboolean GTGL_WGL_ARB_extensions_string          = GL_FALSE;
GLboolean GTGL_WGL_ARB_pixel_format               = GL_FALSE;
GLboolean GTGL_WGL_ARB_make_current_read          = GL_FALSE;
GLboolean GTGL_WGL_ARB_pbuffer                    = GL_FALSE;
GLboolean GTGL_WGL_ARB_render_texture             = GL_FALSE;
GLboolean GTGL_WGL_ARB_pixel_format_float         = GL_FALSE;
GLboolean GTGL_WGL_ARB_framebuffer_sRGB           = GL_FALSE;
GLboolean GTGL_WGL_ARB_create_context             = GL_FALSE;
GLboolean GTGL_WGL_ARB_create_context_profile     = GL_FALSE;
GLboolean GTGL_WGL_ARB_create_context_robustness  = GL_FALSE;

/* EXT Extensions */
GLboolean GTGL_WGL_EXT_display_color_table        = GL_FALSE;
GLboolean GTGL_WGL_EXT_extensions_string          = GL_FALSE;
GLboolean GTGL_WGL_EXT_make_current_read          = GL_FALSE;
GLboolean GTGL_WGL_EXT_pbuffer                    = GL_FALSE;
GLboolean GTGL_WGL_EXT_pixel_format               = GL_FALSE;
GLboolean GTGL_WGL_EXT_swap_control               = GL_FALSE;
GLboolean GTGL_WGL_EXT_depth_float                = GL_FALSE;
GLboolean GTGL_WGL_EXT_multisample                = GL_FALSE;
GLboolean GTGL_WGL_EXT_pixel_format_packed_float  = GL_FALSE;
GLboolean GTGL_WGL_EXT_framebuffer_sRGB           = GL_FALSE;
GLboolean GTGL_WGL_EXT_create_context_es2_profile = GL_FALSE;
GLboolean GTGL_WGL_EXT_swap_control_tear          = GL_FALSE;

/* NV Extensions */
GLboolean GTGL_WGL_NV_vertex_array_range          = GL_FALSE;
GLboolean GTGL_WGL_NV_float_buffer                = GL_FALSE;
GLboolean GTGL_WGL_NV_present_video               = GL_FALSE;
GLboolean GTGL_WGL_NV_video_output                = GL_FALSE;
GLboolean GTGL_WGL_NV_swap_group                  = GL_FALSE;
GLboolean GTGL_WGL_NV_gpu_affinity                = GL_FALSE;
GLboolean GTGL_WGL_NV_video_capture               = GL_FALSE;
GLboolean GTGL_WGL_NV_copy_image                  = GL_FALSE;
GLboolean GTGL_WGL_NV_multisample_coverage        = GL_FALSE;
GLboolean GTGL_WGL_NV_DX_interop                  = GL_FALSE;
GLboolean GTGL_WGL_NV_DX_interop2                 = GL_FALSE;

/* I3D Extensions */
GLboolean GTGL_WGL_I3D_digital_video_control      = GL_FALSE;
GLboolean GTGL_WGL_I3D_gamma                      = GL_FALSE;
GLboolean GTGL_WGL_I3D_genlock                    = GL_FALSE;
GLboolean GTGL_WGL_I3D_image_buffer               = GL_FALSE;
GLboolean GTGL_WGL_I3D_swap_frame_lock            = GL_FALSE;
GLboolean GTGL_WGL_I3D_swap_frame_usage           = GL_FALSE;

/* Other Extensions */
GLboolean GTGL_WGL_3DFX_multisample               = GL_FALSE;
GLboolean GTGL_WGL_OML_sync_control               = GL_FALSE;
GLboolean GTGL_WGL_ATI_pixel_format_float         = GL_FALSE;
GLboolean GTGL_WGL_3DL_stereo_control             = GL_FALSE;
GLboolean GTGL_WGL_AMD_gpu_association            = GL_FALSE;



/* WGL_ARB_buffer_region */
PFNWGLCREATEBUFFERREGIONARBPROC  wglCreateBufferRegionARB  = NULL, __native_wglCreateBufferRegionARB  = NULL;
PFNWGLDELETEBUFFERREGIONARBPROC  wglDeleteBufferRegionARB  = NULL, __native_wglDeleteBufferRegionARB  = NULL;
PFNWGLSAVEBUFFERREGIONARBPROC    wglSaveBufferRegionARB    = NULL, __native_wglSaveBufferRegionARB    = NULL;
PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB = NULL, __native_wglRestoreBufferRegionARB = NULL;

HANDLE WINAPI __debug_wglCreateBufferRegionARB(HDC hDC, int iLayerPlane, UINT uType)        { GTGL_DEBUG_IMPL_RETURN(HANDLE, wglCreateBufferRegionARB, (hDC, iLayerPlane, uType)); }
VOID   WINAPI __debug_wglDeleteBufferRegionARB(HANDLE hRegion)                              { GTGL_DEBUG_IMPL(wglDeleteBufferRegionARB, (hRegion)); }
BOOL   WINAPI __debug_wglSaveBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglSaveBufferRegionARB, (hRegion, x, y, width, height)); }
BOOL   WINAPI __debug_wglRestoreBufferRegionARB(HANDLE hRegion, int x, int y, int width, int height, int xSrc, int ySrc) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglRestoreBufferRegionARB, (hRegion, x, y, width, height, xSrc, ySrc)); }

/* WGL_ARB_extensions_string */
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = NULL, __native_wglGetExtensionsStringARB = NULL;
const char * WINAPI __debug_wglGetExtensionsStringARB(HDC hdc) { GTGL_DEBUG_IMPL_RETURN(const char *, wglGetExtensionsStringARB, (hdc)); }

/* WGL_ARB_pixel_format */
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = NULL, __native_wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = NULL, __native_wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB      = NULL, __native_wglChoosePixelFormatARB      = NULL;

BOOL WINAPI __debug_wglGetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetPixelFormatAttribivARB, (hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues)); }
BOOL WINAPI __debug_wglGetPixelFormatAttribfvARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetPixelFormatAttribfvARB, (hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues)); }
BOOL WINAPI __debug_wglChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglChoosePixelFormatARB, (hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats)); }

/* WGL_ARB_make_current_read */
PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB = NULL, __native_wglMakeContextCurrentARB = NULL;
PFNWGLGETCURRENTREADDCARBPROC   wglGetCurrentReadDCARB   = NULL, __native_wglGetCurrentReadDCARB   = NULL;

BOOL WINAPI __debug_wglMakeContextCurrentARB(HDC hDrawDC, HDC hReadDC, HGLRC hglrc)         { GTGL_DEBUG_IMPL_RETURN(BOOL, wglMakeContextCurrentARB, (hDrawDC, hReadDC, hglrc)); }
HDC  WINAPI __debug_wglGetCurrentReadDCARB(void)                                            { GTGL_DEBUG_IMPL_RETURN(HDC, wglGetCurrentReadDCARB, ()); }

/* WGL_ARB_pbuffer */
PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB    = NULL, __native_wglCreatePbufferARB    = NULL;
PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB     = NULL, __native_wglGetPbufferDCARB     = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB = NULL, __native_wglReleasePbufferDCARB = NULL;
PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB   = NULL, __native_wglDestroyPbufferARB   = NULL;
PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB     = NULL, __native_wglQueryPbufferARB     = NULL;

HPBUFFERARB WINAPI __debug_wglCreatePbufferARB(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList) { GTGL_DEBUG_IMPL_RETURN(HPBUFFERARB, wglCreatePbufferARB, (hDC, iPixelFormat, iWidth, iHeight, piAttribList)); }
HDC WINAPI __debug_wglGetPbufferDCARB(HPBUFFERARB hPbuffer)                                 { GTGL_DEBUG_IMPL_RETURN(HDC, wglGetPbufferDCARB, (hPbuffer)); }
int WINAPI __debug_wglReleasePbufferDCARB(HPBUFFERARB hPbuffer, HDC hDC)                    { GTGL_DEBUG_IMPL_RETURN(int, wglReleasePbufferDCARB, (hPbuffer, hDC)); }
BOOL WINAPI __debug_wglDestroyPbufferARB(HPBUFFERARB hPbuffer)                              { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDestroyPbufferARB, (hPbuffer)); }
BOOL WINAPI __debug_wglQueryPbufferARB(HPBUFFERARB hPbuffer, int iAttribute, int *piValue)  { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryPbufferARB, (hPbuffer, iAttribute, piValue)); }

/* WGL_ARB_render_texture */
PFNWGLBINDTEXIMAGEARBPROC     wglBindTexImageARB     = NULL, __native_wglBindTexImageARB     = NULL;
PFNWGLRELEASETEXIMAGEARBPROC  wglReleaseTexImageARB  = NULL, __native_wglReleaseTexImageARB  = NULL;
PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB = NULL, __native_wglSetPbufferAttribARB = NULL;

BOOL WINAPI __debug_wglBindTexImageARB(HPBUFFERARB hPbuffer, int iBuffer)                   { GTGL_DEBUG_IMPL_RETURN(BOOL, wglBindTexImageARB, (hPbuffer, iBuffer)); }
BOOL WINAPI __debug_wglReleaseTexImageARB(HPBUFFERARB hPbuffer, int iBuffer)                { GTGL_DEBUG_IMPL_RETURN(BOOL, wglReleaseTexImageARB, (hPbuffer, iBuffer)); }
BOOL WINAPI __debug_wglSetPbufferAttribARB(HPBUFFERARB hPbuffer, const int *piAttribList)   { GTGL_DEBUG_IMPL_RETURN(BOOL, wglSetPbufferAttribARB, (hPbuffer, piAttribList)); }

/* WGL_ARB_create_context */
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL, __native_wglCreateContextAttribsARB = NULL;
HGLRC WINAPI __debug_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList) { GTGL_DEBUG_IMPL_RETURN(HGLRC, wglCreateContextAttribsARB, (hDC, hShareContext, attribList)); }



/* WGL_EXT_display_color_table */
PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC  wglCreateDisplayColorTableEXT  = NULL, __native_wglCreateDisplayColorTableEXT  = NULL;
PFNWGLLOADDISPLAYCOLORTABLEEXTPROC    wglLoadDisplayColorTableEXT    = NULL, __native_wglLoadDisplayColorTableEXT    = NULL;
PFNWGLBINDDISPLAYCOLORTABLEEXTPROC    wglBindDisplayColorTableEXT    = NULL, __native_wglBindDisplayColorTableEXT    = NULL;
PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC wglDestroyDisplayColorTableEXT = NULL, __native_wglDestroyDisplayColorTableEXT = NULL;

GLboolean WINAPI __debug_wglCreateDisplayColorTableEXT(GLushort id)                         { GTGL_DEBUG_IMPL_RETURN(GLboolean, wglCreateDisplayColorTableEXT, (id)); }
GLboolean WINAPI __debug_wglLoadDisplayColorTableEXT(const GLushort *table, GLuint length)  { GTGL_DEBUG_IMPL_RETURN(GLboolean, wglLoadDisplayColorTableEXT, (table, length)); }
GLboolean WINAPI __debug_wglBindDisplayColorTableEXT(GLushort id)                           { GTGL_DEBUG_IMPL_RETURN(GLboolean, wglBindDisplayColorTableEXT, (id)); }
VOID WINAPI __debug_wglDestroyDisplayColorTableEXT(GLushort id)                             { GTGL_DEBUG_IMPL(wglDestroyDisplayColorTableEXT, (id)); }

/* WGL_EXT_extensions_string */
PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = NULL, __native_wglGetExtensionsStringEXT = NULL;
const char * WINAPI __debug_wglGetExtensionsStringEXT(void) { GTGL_DEBUG_IMPL_RETURN(const char *, wglGetExtensionsStringEXT, ()); }

/* WGL_EXT_make_current_read */
PFNWGLMAKECONTEXTCURRENTEXTPROC wglMakeContextCurrentEXT = NULL, __native_wglMakeContextCurrentEXT = NULL;
PFNWGLGETCURRENTREADDCEXTPROC   wglGetCurrentReadDCEXT   = NULL, __native_wglGetCurrentReadDCEXT   = NULL;

BOOL WINAPI __debug_wglMakeContextCurrentEXT(HDC hDrawDC, HDC hReadDC, HGLRC hglrc) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglMakeContextCurrentEXT, (hDrawDC, hReadDC, hglrc)); }
HDC WINAPI __debug_wglGetCurrentReadDCEXT(void) { GTGL_DEBUG_IMPL_RETURN(HDC, wglGetCurrentReadDCEXT, ()); }

/* WGL_EXT_pbuffer */
PFNWGLCREATEPBUFFEREXTPROC    wglCreatePbufferEXT    = NULL, __native_wglCreatePbufferEXT    = NULL;
PFNWGLGETPBUFFERDCEXTPROC     wglGetPbufferDCEXT     = NULL, __native_wglGetPbufferDCEXT     = NULL;
PFNWGLRELEASEPBUFFERDCEXTPROC wglReleasePbufferDCEXT = NULL, __native_wglReleasePbufferDCEXT = NULL;
PFNWGLDESTROYPBUFFEREXTPROC   wglDestroyPbufferEXT   = NULL, __native_wglDestroyPbufferEXT   = NULL;
PFNWGLQUERYPBUFFEREXTPROC     wglQueryPbufferEXT     = NULL, __native_wglQueryPbufferEXT     = NULL;

HPBUFFEREXT WINAPI __debug_wglCreatePbufferEXT(HDC hDC, int iPixelFormat, int iWidth, int iHeight, const int *piAttribList) { GTGL_DEBUG_IMPL_RETURN(HPBUFFEREXT, wglCreatePbufferEXT, (hDC, iPixelFormat, iWidth, iHeight, piAttribList)); }
HDC WINAPI __debug_wglGetPbufferDCEXT(HPBUFFEREXT hPbuffer)                                 { GTGL_DEBUG_IMPL_RETURN(HDC, wglGetPbufferDCEXT, (hPbuffer)); }
int WINAPI __debug_wglReleasePbufferDCEXT(HPBUFFEREXT hPbuffer, HDC hDC)                    { GTGL_DEBUG_IMPL_RETURN(int, wglReleasePbufferDCEXT, (hPbuffer, hDC)); }
BOOL WINAPI __debug_wglDestroyPbufferEXT(HPBUFFEREXT hPbuffer)                              { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDestroyPbufferEXT, (hPbuffer)); }
BOOL WINAPI __debug_wglQueryPbufferEXT(HPBUFFEREXT hPbuffer, int iAttribute, int *piValue)  { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryPbufferEXT, (hPbuffer, iAttribute, piValue)); }

/* WGL_EXT_pixel_format */
PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivEXT = NULL, __native_wglGetPixelFormatAttribivEXT = NULL;
PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvEXT = NULL, __native_wglGetPixelFormatAttribfvEXT = NULL;
PFNWGLCHOOSEPIXELFORMATEXTPROC      wglChoosePixelFormatEXT      = NULL, __native_wglChoosePixelFormatEXT      = NULL;

BOOL WINAPI __debug_wglGetPixelFormatAttribivEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int *piAttributes, int *piValues)    { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetPixelFormatAttribivEXT, (hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues)); }
BOOL WINAPI __debug_wglGetPixelFormatAttribfvEXT(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int *piAttributes, FLOAT *pfValues)  { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetPixelFormatAttribfvEXT, (hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues)); }
BOOL WINAPI __debug_wglChoosePixelFormatEXT(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglChoosePixelFormatEXT, (hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats)); }

/* WGL_EXT_swap_control */
PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT    = NULL, __native_wglSwapIntervalEXT    = NULL;
PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = NULL, __native_wglGetSwapIntervalEXT = NULL;

BOOL WINAPI __debug_wglSwapIntervalEXT(int interval) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglSwapIntervalEXT, (interval)); }
int  WINAPI __debug_wglGetSwapIntervalEXT(void)      { GTGL_DEBUG_IMPL_RETURN(int,  wglGetSwapIntervalEXT, ()); }


/* WGL_NV_vertex_array_range */
PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV = NULL, __native_wglAllocateMemoryNV = NULL;
PFNWGLFREEMEMORYNVPROC     wglFreeMemoryNV     = NULL, __native_wglFreeMemoryNV     = NULL;

void* WINAPI __debug_wglAllocateMemoryNV(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority) { GTGL_DEBUG_IMPL_RETURN(void*, wglAllocateMemoryNV, (size, readfreq, writefreq, priority)); }
void WINAPI __debug_wglFreeMemoryNV(void *pointer) { GTGL_DEBUG_IMPL(wglFreeMemoryNV, (pointer)); }

/* WGL_NV_present_video */
PFNWGLENUMERATEVIDEODEVICESNVPROC wglEnumerateVideoDevicesNV = NULL, __native_wglEnumerateVideoDevicesNV = NULL;
PFNWGLBINDVIDEODEVICENVPROC       wglBindVideoDeviceNV       = NULL, __native_wglBindVideoDeviceNV       = NULL;
PFNWGLQUERYCURRENTCONTEXTNVPROC   wglQueryCurrentContextNV   = NULL, __native_wglQueryCurrentContextNV   = NULL;

int WINAPI __debug_wglEnumerateVideoDevicesNV(HDC hDC, HVIDEOOUTPUTDEVICENV *phDeviceList) { GTGL_DEBUG_IMPL_RETURN(int, wglEnumerateVideoDevicesNV, (hDC, phDeviceList)); }
BOOL WINAPI __debug_wglBindVideoDeviceNV(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int *piAttribList) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglBindVideoDeviceNV, (hDC, uVideoSlot, hVideoDevice, piAttribList)); }
BOOL WINAPI __debug_wglQueryCurrentContextNV(int iAttribute, int *piValue) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryCurrentContextNV, (iAttribute, piValue)); }

/* WGL_NV_video_output */
PFNWGLGETVIDEODEVICENVPROC     wglGetVideoDeviceNV     = NULL, __native_wglGetVideoDeviceNV     = NULL;
PFNWGLRELEASEVIDEODEVICENVPROC wglReleaseVideoDeviceNV = NULL, __native_wglReleaseVideoDeviceNV = NULL;
PFNWGLBINDVIDEOIMAGENVPROC     wglBindVideoImageNV     = NULL, __native_wglBindVideoImageNV     = NULL;
PFNWGLRELEASEVIDEOIMAGENVPROC  wglReleaseVideoImageNV  = NULL, __native_wglReleaseVideoImageNV  = NULL;
PFNWGLSENDPBUFFERTOVIDEONVPROC wglSendPbufferToVideoNV = NULL, __native_wglSendPbufferToVideoNV = NULL;
PFNWGLGETVIDEOINFONVPROC       wglGetVideoInfoNV       = NULL, __native_wglGetVideoInfoNV       = NULL;

BOOL WINAPI __debug_wglGetVideoDeviceNV(HDC hDC, int numDevices, HPVIDEODEV *hVideoDevice)  { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetVideoDeviceNV, (hDC, numDevices, hVideoDevice)); }
BOOL WINAPI __debug_wglReleaseVideoDeviceNV(HPVIDEODEV hVideoDevice)                        { GTGL_DEBUG_IMPL_RETURN(BOOL, wglReleaseVideoDeviceNV, (hVideoDevice)); }
BOOL WINAPI __debug_wglBindVideoImageNV(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer, int iVideoBuffer) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglBindVideoImageNV, (hVideoDevice, hPbuffer, iVideoBuffer)); }
BOOL WINAPI __debug_wglReleaseVideoImageNV(HPBUFFERARB hPbuffer, int iVideoBuffer)          { GTGL_DEBUG_IMPL_RETURN(BOOL, wglReleaseVideoImageNV, (hPbuffer, iVideoBuffer)); }
BOOL WINAPI __debug_wglSendPbufferToVideoNV(HPBUFFERARB hPbuffer, int iBufferType, unsigned long *pulCounterPbuffer, BOOL bBlock) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglSendPbufferToVideoNV, (hPbuffer, iBufferType, pulCounterPbuffer, bBlock)); }
BOOL WINAPI __debug_wglGetVideoInfoNV(HPVIDEODEV hpVideoDevice, unsigned long *pulCounterOutputPbuffer, unsigned long *pulCounterOutputVideo) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetVideoInfoNV, (hpVideoDevice, pulCounterOutputPbuffer, pulCounterOutputVideo)); }

/* WGL_NV_swap_group */
PFNWGLJOINSWAPGROUPNVPROC      wglJoinSwapGroupNV      = NULL, __native_wglJoinSwapGroupNV      = NULL;
PFNWGLBINDSWAPBARRIERNVPROC    wglBindSwapBarrierNV    = NULL, __native_wglBindSwapBarrierNV    = NULL;
PFNWGLQUERYSWAPGROUPNVPROC     wglQuerySwapGroupNV     = NULL, __native_wglQuerySwapGroupNV     = NULL;
PFNWGLQUERYMAXSWAPGROUPSNVPROC wglQueryMaxSwapGroupsNV = NULL, __native_wglQueryMaxSwapGroupsNV = NULL;
PFNWGLQUERYFRAMECOUNTNVPROC    wglQueryFrameCountNV    = NULL, __native_wglQueryFrameCountNV    = NULL;
PFNWGLRESETFRAMECOUNTNVPROC    wglResetFrameCountNV    = NULL, __native_wglResetFrameCountNV    = NULL;

BOOL WINAPI __debug_wglJoinSwapGroupNV(HDC hDC, GLuint group)                               { GTGL_DEBUG_IMPL_RETURN(BOOL, wglJoinSwapGroupNV, (hDC, group)); }
BOOL WINAPI __debug_wglBindSwapBarrierNV(GLuint group, GLuint barrier)                      { GTGL_DEBUG_IMPL_RETURN(BOOL, wglBindSwapBarrierNV, (group, barrier)); }
BOOL WINAPI __debug_wglQuerySwapGroupNV(HDC hDC, GLuint *group, GLuint *barrier)            { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQuerySwapGroupNV, (hDC, group, barrier)); }
BOOL WINAPI __debug_wglQueryMaxSwapGroupsNV(HDC hDC, GLuint *maxGroups, GLuint *maxBarriers) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryMaxSwapGroupsNV, (hDC, maxGroups, maxBarriers)); }
BOOL WINAPI __debug_wglQueryFrameCountNV(HDC hDC, GLuint *count)                            { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryFrameCountNV, (hDC, count)); }
BOOL WINAPI __debug_wglResetFrameCountNV(HDC hDC)                                           { GTGL_DEBUG_IMPL_RETURN(BOOL, wglResetFrameCountNV, (hDC)); }

/* WGL_NV_gpu_affinity */
PFNWGLENUMGPUSNVPROC               wglEnumGpusNV               = NULL, __native_wglEnumGpusNV               = NULL;
PFNWGLENUMGPUDEVICESNVPROC         wglEnumGpuDevicesNV         = NULL, __native_wglEnumGpuDevicesNV         = NULL;
PFNWGLCREATEAFFINITYDCNVPROC       wglCreateAffinityDCNV       = NULL, __native_wglCreateAffinityDCNV       = NULL;
PFNWGLENUMGPUSFROMAFFINITYDCNVPROC wglEnumGpusFromAffinityDCNV = NULL, __native_wglEnumGpusFromAffinityDCNV = NULL;
PFNWGLDELETEDCNVPROC               wglDeleteDCNV               = NULL, __native_wglDeleteDCNV               = NULL;

BOOL WINAPI __debug_wglEnumGpusNV(UINT iGpuIndex, HGPUNV *phGpu)                            { GTGL_DEBUG_IMPL_RETURN(BOOL, wglEnumGpusNV, (iGpuIndex, phGpu)); }
BOOL WINAPI __debug_wglEnumGpuDevicesNV(HGPUNV hGpu, UINT iDeviceIndex, PGPU_DEVICE lpGpuDevice) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglEnumGpuDevicesNV, (hGpu, iDeviceIndex, lpGpuDevice)); }
HDC WINAPI __debug_wglCreateAffinityDCNV(const HGPUNV *phGpuList)                           { GTGL_DEBUG_IMPL_RETURN(HDC, wglCreateAffinityDCNV, (phGpuList)); }
BOOL WINAPI __debug_wglEnumGpusFromAffinityDCNV(HDC hAffinityDC, UINT iGpuIndex, HGPUNV *hGpu) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglEnumGpusFromAffinityDCNV, (hAffinityDC, iGpuIndex, hGpu)); }
BOOL WINAPI __debug_wglDeleteDCNV(HDC hdc)                                                  { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDeleteDCNV, (hdc)); }

/* WGL_NV_video_capture */
PFNWGLBINDVIDEOCAPTUREDEVICENVPROC       wglBindVideoCaptureDeviceNV       = NULL, __native_wglBindVideoCaptureDeviceNV       = NULL;
PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC wglEnumerateVideoCaptureDevicesNV = NULL, __native_wglEnumerateVideoCaptureDevicesNV = NULL;
PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC       wglLockVideoCaptureDeviceNV       = NULL, __native_wglLockVideoCaptureDeviceNV       = NULL;
PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC      wglQueryVideoCaptureDeviceNV      = NULL, __native_wglQueryVideoCaptureDeviceNV      = NULL;
PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC    wglReleaseVideoCaptureDeviceNV    = NULL, __native_wglReleaseVideoCaptureDeviceNV    = NULL;

BOOL WINAPI __debug_wglBindVideoCaptureDeviceNV(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglBindVideoCaptureDeviceNV, (uVideoSlot, hDevice)); }
UINT WINAPI __debug_wglEnumerateVideoCaptureDevicesNV(HDC hDc, HVIDEOINPUTDEVICENV *phDeviceList) { GTGL_DEBUG_IMPL_RETURN(UINT, wglEnumerateVideoCaptureDevicesNV, (hDc, phDeviceList)); }
BOOL WINAPI __debug_wglLockVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice)       { GTGL_DEBUG_IMPL_RETURN(BOOL, wglLockVideoCaptureDeviceNV, (hDc, hDevice)); }
BOOL WINAPI __debug_wglQueryVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int *piValue) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryVideoCaptureDeviceNV, (hDc, hDevice, iAttribute, piValue)); }
BOOL WINAPI __debug_wglReleaseVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice)    { GTGL_DEBUG_IMPL_RETURN(BOOL, wglReleaseVideoCaptureDeviceNV, (hDc, hDevice)); }

/* WGL_NV_copy_image */
PFNWGLCOPYIMAGESUBDATANVPROC wglCopyImageSubDataNV = NULL, __native_wglCopyImageSubDataNV = NULL;
BOOL WINAPI __debug_wglCopyImageSubDataNV(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, HGLRC hDstRC, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth) {
    GTGL_DEBUG_IMPL_RETURN(BOOL, wglCopyImageSubDataNV, (hSrcRC, srcName, srcTarget, srcLevel, srcX, srcY, srcZ, hDstRC, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth)); }

/* WGL_NV_DX_interop */
PFNWGLDXSETRESOURCESHAREHANDLENVPROC wglDXSetResourceShareHandleNV = NULL, __native_wglDXSetResourceShareHandleNV = NULL;
PFNWGLDXOPENDEVICENVPROC             wglDXOpenDeviceNV             = NULL, __native_wglDXOpenDeviceNV             = NULL;
PFNWGLDXCLOSEDEVICENVPROC            wglDXCloseDeviceNV            = NULL, __native_wglDXCloseDeviceNV            = NULL;
PFNWGLDXREGISTEROBJECTNVPROC         wglDXRegisterObjectNV         = NULL, __native_wglDXRegisterObjectNV         = NULL;
PFNWGLDXUNREGISTEROBJECTNVPROC       wglDXUnregisterObjectNV       = NULL, __native_wglDXUnregisterObjectNV       = NULL;
PFNWGLDXOBJECTACCESSNVPROC           wglDXObjectAccessNV           = NULL, __native_wglDXObjectAccessNV           = NULL;
PFNWGLDXLOCKOBJECTSNVPROC            wglDXLockObjectsNV            = NULL, __native_wglDXLockObjectsNV            = NULL;
PFNWGLDXUNLOCKOBJECTSNVPROC          wglDXUnlockObjectsNV          = NULL, __native_wglDXUnlockObjectsNV          = NULL;

BOOL WINAPI __debug_wglDXSetResourceShareHandleNV(void *dxObject, HANDLE shareHandle)       { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDXSetResourceShareHandleNV, (dxObject, shareHandle)); }
HANDLE WINAPI __debug_wglDXOpenDeviceNV(void *dxDevice)                                     { GTGL_DEBUG_IMPL_RETURN(HANDLE, wglDXOpenDeviceNV, (dxDevice)); }
BOOL WINAPI __debug_wglDXCloseDeviceNV(HANDLE hDevice)                                      { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDXCloseDeviceNV, (hDevice)); }
HANDLE WINAPI __debug_wglDXRegisterObjectNV(HANDLE hDevice, void *dxObject, GLuint name, GLenum type, GLenum access) { GTGL_DEBUG_IMPL_RETURN(HANDLE, wglDXRegisterObjectNV, (hDevice, dxObject, name, type, access)); }
BOOL WINAPI __debug_wglDXUnregisterObjectNV(HANDLE hDevice, HANDLE hObject)                 { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDXUnregisterObjectNV, (hDevice, hObject)); }
BOOL WINAPI __debug_wglDXObjectAccessNV(HANDLE hObject, GLenum access)                      { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDXObjectAccessNV, (hObject, access)); }
BOOL WINAPI __debug_wglDXLockObjectsNV(HANDLE hDevice, GLint count, HANDLE *hObjects)       { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDXLockObjectsNV, (hDevice, count, hObjects)); }
BOOL WINAPI __debug_wglDXUnlockObjectsNV(HANDLE hDevice, GLint count, HANDLE *hObjects)     { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDXUnlockObjectsNV, (hDevice, count, hObjects)); }


/* WGL_I3D_digital_video_control */
PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC wglGetDigitalVideoParametersI3D = NULL, __native_wglGetDigitalVideoParametersI3D = NULL;
PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC wglSetDigitalVideoParametersI3D = NULL, __native_wglSetDigitalVideoParametersI3D = NULL;

BOOL WINAPI __debug_wglGetDigitalVideoParametersI3D(HDC hDC, int iAttribute, int *piValue)  { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetDigitalVideoParametersI3D, (hDC, iAttribute, piValue)); }
BOOL WINAPI __debug_wglSetDigitalVideoParametersI3D(HDC hDC, int iAttribute, const int *piValue) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglSetDigitalVideoParametersI3D, (hDC, iAttribute, piValue)); }


/* WGL_I3D_gamma */
PFNWGLGETGAMMATABLEPARAMETERSI3DPROC wglGetGammaTableParametersI3D = NULL, __native_wglGetGammaTableParametersI3D = NULL;
PFNWGLSETGAMMATABLEPARAMETERSI3DPROC wglSetGammaTableParametersI3D = NULL, __native_wglSetGammaTableParametersI3D = NULL;
PFNWGLGETGAMMATABLEI3DPROC           wglGetGammaTableI3D           = NULL, __native_wglGetGammaTableI3D           = NULL;
PFNWGLSETGAMMATABLEI3DPROC           wglSetGammaTableI3D           = NULL, __native_wglSetGammaTableI3D           = NULL;

BOOL WINAPI __debug_wglGetGammaTableParametersI3D(HDC hDC, int iAttribute, int *piValue) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetGammaTableParametersI3D, (hDC, iAttribute, piValue)); }
BOOL WINAPI __debug_wglSetGammaTableParametersI3D(HDC hDC, int iAttribute, const int *piValue) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglSetGammaTableParametersI3D, (hDC, iAttribute, piValue)); }
BOOL WINAPI __debug_wglGetGammaTableI3D(HDC hDC, int iEntries, USHORT *puRed, USHORT *puGreen, USHORT *puBlue) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetGammaTableI3D, (hDC, iEntries, puRed, puGreen, puBlue)); }
BOOL WINAPI __debug_wglSetGammaTableI3D(HDC hDC, int iEntries, const USHORT *puRed, const USHORT *puGreen, const USHORT *puBlue) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglSetGammaTableI3D, (hDC, iEntries, puRed, puGreen, puBlue)); }


/* WGL_I3D_genlock */
PFNWGLENABLEGENLOCKI3DPROC              wglEnableGenlockI3D              = NULL, __native_wglEnableGenlockI3D              = NULL;
PFNWGLDISABLEGENLOCKI3DPROC             wglDisableGenlockI3D             = NULL, __native_wglDisableGenlockI3D             = NULL;
PFNWGLISENABLEDGENLOCKI3DPROC           wglIsEnabledGenlockI3D           = NULL, __native_wglIsEnabledGenlockI3D           = NULL;
PFNWGLGENLOCKSOURCEI3DPROC              wglGenlockSourceI3D              = NULL, __native_wglGenlockSourceI3D              = NULL;
PFNWGLGETGENLOCKSOURCEI3DPROC           wglGetGenlockSourceI3D           = NULL, __native_wglGetGenlockSourceI3D           = NULL;
PFNWGLGENLOCKSOURCEEDGEI3DPROC          wglGenlockSourceEdgeI3D          = NULL, __native_wglGenlockSourceEdgeI3D          = NULL;
PFNWGLGETGENLOCKSOURCEEDGEI3DPROC       wglGetGenlockSourceEdgeI3D       = NULL, __native_wglGetGenlockSourceEdgeI3D       = NULL;
PFNWGLGENLOCKSAMPLERATEI3DPROC          wglGenlockSampleRateI3D          = NULL, __native_wglGenlockSampleRateI3D          = NULL;
PFNWGLGETGENLOCKSAMPLERATEI3DPROC       wglGetGenlockSampleRateI3D       = NULL, __native_wglGetGenlockSampleRateI3D       = NULL;
PFNWGLGENLOCKSOURCEDELAYI3DPROC         wglGenlockSourceDelayI3D         = NULL, __native_wglGenlockSourceDelayI3D         = NULL;
PFNWGLGETGENLOCKSOURCEDELAYI3DPROC      wglGetGenlockSourceDelayI3D      = NULL, __native_wglGetGenlockSourceDelayI3D      = NULL;
PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC wglQueryGenlockMaxSourceDelayI3D = NULL, __native_wglQueryGenlockMaxSourceDelayI3D = NULL;

BOOL WINAPI __debug_wglEnableGenlockI3D(HDC hDC)                                            { GTGL_DEBUG_IMPL_RETURN(BOOL, wglEnableGenlockI3D, (hDC)); }
BOOL WINAPI __debug_wglDisableGenlockI3D(HDC hDC)                                           { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDisableGenlockI3D, (hDC)); }
BOOL WINAPI __debug_wglIsEnabledGenlockI3D(HDC hDC, BOOL *pFlag)                            { GTGL_DEBUG_IMPL_RETURN(BOOL, wglIsEnabledGenlockI3D, (hDC, pFlag)); }
BOOL WINAPI __debug_wglGenlockSourceI3D(HDC hDC, UINT uSource)                              { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGenlockSourceI3D, (hDC, uSource)); }
BOOL WINAPI __debug_wglGetGenlockSourceI3D(HDC hDC, UINT *uSource)                          { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetGenlockSourceI3D, (hDC, uSource)); }
BOOL WINAPI __debug_wglGenlockSourceEdgeI3D(HDC hDC, UINT uEdge)                            { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGenlockSourceEdgeI3D, (hDC, uEdge)); }
BOOL WINAPI __debug_wglGetGenlockSourceEdgeI3D(HDC hDC, UINT *uEdge)                        { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetGenlockSourceEdgeI3D, (hDC, uEdge)); }
BOOL WINAPI __debug_wglGenlockSampleRateI3D(HDC hDC, UINT uRate)                            { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGenlockSampleRateI3D, (hDC, uRate)); }
BOOL WINAPI __debug_wglGetGenlockSampleRateI3D(HDC hDC, UINT *uRate)                        { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetGenlockSampleRateI3D, (hDC, uRate)); }
BOOL WINAPI __debug_wglGenlockSourceDelayI3D(HDC hDC, UINT uDelay)                          { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGenlockSourceDelayI3D, (hDC, uDelay)); }
BOOL WINAPI __debug_wglGetGenlockSourceDelayI3D(HDC hDC, UINT *uDelay)                      { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetGenlockSourceDelayI3D, (hDC, uDelay)); }
BOOL WINAPI __debug_wglQueryGenlockMaxSourceDelayI3D(HDC hDC, UINT *uMaxLineDelay, UINT *uMaxPixelDelay) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryGenlockMaxSourceDelayI3D, (hDC, uMaxLineDelay, uMaxPixelDelay)); }

/* WGL_I3D_image_buffer */
PFNWGLCREATEIMAGEBUFFERI3DPROC          wglCreateImageBufferI3D          = NULL, __native_wglCreateImageBufferI3D          = NULL;
PFNWGLDESTROYIMAGEBUFFERI3DPROC         wglDestroyImageBufferI3D         = NULL, __native_wglDestroyImageBufferI3D         = NULL;
PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC wglAssociateImageBufferEventsI3D = NULL, __native_wglAssociateImageBufferEventsI3D = NULL;
PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC   wglReleaseImageBufferEventsI3D   = NULL, __native_wglReleaseImageBufferEventsI3D   = NULL;

LPVOID WINAPI __debug_wglCreateImageBufferI3D(HDC hDC, DWORD dwSize, UINT uFlags)           { GTGL_DEBUG_IMPL_RETURN(LPVOID, wglCreateImageBufferI3D, (hDC, dwSize, uFlags)); }
BOOL WINAPI __debug_wglDestroyImageBufferI3D(HDC hDC, LPVOID pAddress)                      { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDestroyImageBufferI3D, (hDC, pAddress)); }
BOOL WINAPI __debug_wglAssociateImageBufferEventsI3D(HDC hDC, const HANDLE *pEvent, const LPVOID *pAddress, const DWORD *pSize, UINT count) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglAssociateImageBufferEventsI3D, (hDC, pEvent, pAddress, pSize, count)); }
BOOL WINAPI __debug_wglReleaseImageBufferEventsI3D(HDC hDC, const LPVOID *pAddress, UINT count) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglReleaseImageBufferEventsI3D, (hDC, pAddress, count)); }


/* WGL_I3D_swap_frame_lock */
PFNWGLENABLEFRAMELOCKI3DPROC      wglEnableFrameLockI3D      = NULL, __native_wglEnableFrameLockI3D      = NULL;
PFNWGLDISABLEFRAMELOCKI3DPROC     wglDisableFrameLockI3D     = NULL, __native_wglDisableFrameLockI3D     = NULL;
PFNWGLISENABLEDFRAMELOCKI3DPROC   wglIsEnabledFrameLockI3D   = NULL, __native_wglIsEnabledFrameLockI3D   = NULL;
PFNWGLQUERYFRAMELOCKMASTERI3DPROC wglQueryFrameLockMasterI3D = NULL, __native_wglQueryFrameLockMasterI3D = NULL;

BOOL WINAPI __debug_wglEnableFrameLockI3D(void)             { GTGL_DEBUG_IMPL_RETURN(BOOL, wglEnableFrameLockI3D, ()); }
BOOL WINAPI __debug_wglDisableFrameLockI3D(void)            { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDisableFrameLockI3D, ()); }
BOOL WINAPI __debug_wglIsEnabledFrameLockI3D(BOOL *pFlag)   { GTGL_DEBUG_IMPL_RETURN(BOOL, wglIsEnabledFrameLockI3D, (pFlag)); }
BOOL WINAPI __debug_wglQueryFrameLockMasterI3D(BOOL *pFlag) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryFrameLockMasterI3D, (pFlag)); }

/* WGL_I3D_swap_frame_usage */
PFNWGLGETFRAMEUSAGEI3DPROC      wglGetFrameUsageI3D      = NULL, __native_wglGetFrameUsageI3D      = NULL;
PFNWGLBEGINFRAMETRACKINGI3DPROC wglBeginFrameTrackingI3D = NULL, __native_wglBeginFrameTrackingI3D = NULL;
PFNWGLENDFRAMETRACKINGI3DPROC   wglEndFrameTrackingI3D   = NULL, __native_wglEndFrameTrackingI3D   = NULL;
PFNWGLQUERYFRAMETRACKINGI3DPROC wglQueryFrameTrackingI3D = NULL, __native_wglQueryFrameTrackingI3D = NULL;

BOOL WINAPI __debug_wglGetFrameUsageI3D(float *pUsage)  { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetFrameUsageI3D, (pUsage)); }
BOOL WINAPI __debug_wglBeginFrameTrackingI3D(void)      { GTGL_DEBUG_IMPL_RETURN(BOOL, wglBeginFrameTrackingI3D, ()); }
BOOL WINAPI __debug_wglEndFrameTrackingI3D(void)        { GTGL_DEBUG_IMPL_RETURN(BOOL, wglEndFrameTrackingI3D, ()); }
BOOL WINAPI __debug_wglQueryFrameTrackingI3D(DWORD *pFrameCount, DWORD *pMissedFrames, float *pLastMissedUsage) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglQueryFrameTrackingI3D, (pFrameCount, pMissedFrames, pLastMissedUsage)); }


/* WGL_OML_sync_control */
PFNWGLGETSYNCVALUESOMLPROC       wglGetSyncValuesOML       = NULL, __native_wglGetSyncValuesOML       = NULL;
PFNWGLGETMSCRATEOMLPROC          wglGetMscRateOML          = NULL, __native_wglGetMscRateOML          = NULL;
PFNWGLSWAPBUFFERSMSCOMLPROC      wglSwapBuffersMscOML      = NULL, __native_wglSwapBuffersMscOML      = NULL;
PFNWGLSWAPLAYERBUFFERSMSCOMLPROC wglSwapLayerBuffersMscOML = NULL, __native_wglSwapLayerBuffersMscOML = NULL;
PFNWGLWAITFORMSCOMLPROC          wglWaitForMscOML          = NULL, __native_wglWaitForMscOML          = NULL;
PFNWGLWAITFORSBCOMLPROC          wglWaitForSbcOML          = NULL, __native_wglWaitForSbcOML          = NULL;

BOOL WINAPI __debug_wglGetSyncValuesOML(HDC hdc, INT64 *ust, INT64 *msc, INT64 *sbc)        { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetSyncValuesOML, (hdc, ust, msc, sbc)); }
BOOL WINAPI __debug_wglGetMscRateOML(HDC hdc, INT32 *numerator, INT32 *denominator)         { GTGL_DEBUG_IMPL_RETURN(BOOL, wglGetMscRateOML, (hdc, numerator, denominator)); }
INT64 WINAPI __debug_wglSwapBuffersMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder) { GTGL_DEBUG_IMPL_RETURN(INT64, wglSwapBuffersMscOML, (hdc, target_msc, divisor, remainder)); }
INT64 WINAPI __debug_wglSwapLayerBuffersMscOML(HDC hdc, int fuPlanes, INT64 target_msc, INT64 divisor, INT64 remainder) { GTGL_DEBUG_IMPL_RETURN(INT64, wglSwapLayerBuffersMscOML, (hdc, fuPlanes, target_msc, divisor, remainder)); }
BOOL WINAPI __debug_wglWaitForMscOML(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder, INT64 *ust, INT64 *msc, INT64 *sbc) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglWaitForMscOML, (hdc, target_msc, divisor, remainder, ust, msc, sbc)); }
BOOL WINAPI __debug_wglWaitForSbcOML(HDC hdc, INT64 target_sbc, INT64 *ust, INT64 *msc, INT64 *sbc) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglWaitForSbcOML, (hdc, target_sbc, ust, msc, sbc)); }

/* WGL_3DL_stereo_control */
PFNWGLSETSTEREOEMITTERSTATE3DLPROC wglSetStereoEmitterState3DL = NULL, __native_wglSetStereoEmitterState3DL = NULL;
BOOL WINAPI __debug_wglSetStereoEmitterState3DL(HDC hDC, UINT uState) { GTGL_DEBUG_IMPL_RETURN(BOOL, wglSetStereoEmitterState3DL, (hDC, uState)); }

/* WGL_AMD_gpu_association */
PFNWGLGETGPUIDSAMDPROC                      wglGetGPUIDsAMD                      = NULL, __native_wglGetGPUIDsAMD                      = NULL;
PFNWGLGETGPUINFOAMDPROC                     wglGetGPUInfoAMD                     = NULL, __native_wglGetGPUInfoAMD                     = NULL;
PFNWGLGETCONTEXTGPUIDAMDPROC                wglGetContextGPUIDAMD                = NULL, __native_wglGetContextGPUIDAMD                = NULL;
PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC        wglCreateAssociatedContextAMD        = NULL, __native_wglCreateAssociatedContextAMD        = NULL;
PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC wglCreateAssociatedContextAttribsAMD = NULL, __native_wglCreateAssociatedContextAttribsAMD = NULL;
PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC        wglDeleteAssociatedContextAMD        = NULL, __native_wglDeleteAssociatedContextAMD        = NULL;
PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC   wglMakeAssociatedContextCurrentAMD   = NULL, __native_wglMakeAssociatedContextCurrentAMD   = NULL;
PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC    wglGetCurrentAssociatedContextAMD    = NULL, __native_wglGetCurrentAssociatedContextAMD    = NULL;
PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC         wglBlitContextFramebufferAMD         = NULL, __native_wglBlitContextFramebufferAMD         = NULL;

UINT WINAPI __debug_wglGetGPUIDsAMD(UINT maxCount, UINT *ids)                               { GTGL_DEBUG_IMPL_RETURN(UINT, wglGetGPUIDsAMD, (maxCount, ids)); }
INT WINAPI __debug_wglGetGPUInfoAMD(UINT id, int property, GLenum dataType, UINT size, void *data) { GTGL_DEBUG_IMPL_RETURN(INT, wglGetGPUInfoAMD, (id, property, dataType, size, data)); }
UINT WINAPI __debug_wglGetContextGPUIDAMD(HGLRC hglrc)                                      { GTGL_DEBUG_IMPL_RETURN(UINT, wglGetContextGPUIDAMD, (hglrc)); }
HGLRC WINAPI __debug_wglCreateAssociatedContextAMD(UINT id)                                 { GTGL_DEBUG_IMPL_RETURN(HGLRC, wglCreateAssociatedContextAMD, (id)); }
HGLRC WINAPI __debug_wglCreateAssociatedContextAttribsAMD(UINT id, HGLRC hShareContext, const int *attribList) { GTGL_DEBUG_IMPL_RETURN(HGLRC, wglCreateAssociatedContextAttribsAMD, (id, hShareContext, attribList)); }
BOOL WINAPI __debug_wglDeleteAssociatedContextAMD(HGLRC hglrc)                              { GTGL_DEBUG_IMPL_RETURN(BOOL, wglDeleteAssociatedContextAMD, (hglrc)); }
BOOL WINAPI __debug_wglMakeAssociatedContextCurrentAMD(HGLRC hglrc)                         { GTGL_DEBUG_IMPL_RETURN(BOOL, wglMakeAssociatedContextCurrentAMD, (hglrc)); }
HGLRC WINAPI __debug_wglGetCurrentAssociatedContextAMD(void)                                { GTGL_DEBUG_IMPL_RETURN(HGLRC, wglGetCurrentAssociatedContextAMD, ()); }
VOID WINAPI __debug_wglBlitContextFramebufferAMD(HGLRC dstCtx, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) { GTGL_DEBUG_IMPL(wglBlitContextFramebufferAMD, (dstCtx, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter)); }



void gtglInitGLAPI_WGL(GLboolean debugMode)
{
    /* ARB Extensions */
    if (gtglIsExtensionSupported("WGL_ARB_buffer_region"))
    {
        GTGL_WGL_ARB_buffer_region = GL_TRUE;
        wglCreateBufferRegionARB  = __native_wglCreateBufferRegionARB  = (PFNWGLCREATEBUFFERREGIONARBPROC)gtglGetProcAddress("wglCreateBufferRegionARB");
        wglDeleteBufferRegionARB  = __native_wglDeleteBufferRegionARB  = (PFNWGLDELETEBUFFERREGIONARBPROC)gtglGetProcAddress("wglDeleteBufferRegionARB");
        wglSaveBufferRegionARB    = __native_wglSaveBufferRegionARB    = (PFNWGLSAVEBUFFERREGIONARBPROC)gtglGetProcAddress("wglSaveBufferRegionARB");
        wglRestoreBufferRegionARB = __native_wglRestoreBufferRegionARB = (PFNWGLRESTOREBUFFERREGIONARBPROC)gtglGetProcAddress("wglRestoreBufferRegionARB");

        if (debugMode)
        {
            wglCreateBufferRegionARB  = __debug_wglCreateBufferRegionARB;
            wglDeleteBufferRegionARB  = __debug_wglDeleteBufferRegionARB;
            wglSaveBufferRegionARB    = __debug_wglSaveBufferRegionARB;
            wglRestoreBufferRegionARB = __debug_wglRestoreBufferRegionARB;
        }
    }

    if (gtglIsExtensionSupported("WGL_ARB_multisample"))
    {
        GTGL_WGL_ARB_multisample = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_ARB_extensions_string"))
    {
        GTGL_WGL_ARB_extensions_string = GL_TRUE;
        wglGetExtensionsStringARB = __native_wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)gtglGetProcAddress("wglGetExtensionsStringARB");

        if (debugMode)
        {
            wglGetExtensionsStringARB = __debug_wglGetExtensionsStringARB;
        }
    }

    if (gtglIsExtensionSupported("WGL_ARB_pixel_format"))
    {
        GTGL_WGL_ARB_pixel_format = GL_TRUE;
        wglGetPixelFormatAttribivARB = __native_wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)gtglGetProcAddress("wglGetPixelFormatAttribivARB");
        wglGetPixelFormatAttribfvARB = __native_wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)gtglGetProcAddress("wglGetPixelFormatAttribfvARB");
        wglChoosePixelFormatARB      = __native_wglChoosePixelFormatARB      = (PFNWGLCHOOSEPIXELFORMATARBPROC)gtglGetProcAddress("wglChoosePixelFormatARB");

        if (debugMode)
        {
            wglGetPixelFormatAttribivARB = __debug_wglGetPixelFormatAttribivARB;
            wglGetPixelFormatAttribfvARB = __debug_wglGetPixelFormatAttribfvARB;
            wglChoosePixelFormatARB      = __debug_wglChoosePixelFormatARB;
        }
    }

    if (gtglIsExtensionSupported("WGL_ARB_make_current_read"))
    {
        GTGL_WGL_ARB_make_current_read = GL_TRUE;
        wglMakeContextCurrentARB = __native_wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)gtglGetProcAddress("wglMakeContextCurrentARB");
        wglGetCurrentReadDCARB   = __native_wglGetCurrentReadDCARB   = (PFNWGLGETCURRENTREADDCARBPROC)gtglGetProcAddress("wglGetCurrentReadDCARB");

        if (debugMode)
        {
            wglMakeContextCurrentARB = __debug_wglMakeContextCurrentARB;
            wglGetCurrentReadDCARB   = __debug_wglGetCurrentReadDCARB;
        }
    }

    if (gtglIsExtensionSupported("WGL_ARB_pbuffer"))
    {
        GTGL_WGL_ARB_pbuffer = GL_TRUE;
        wglCreatePbufferARB    = __native_wglCreatePbufferARB    = (PFNWGLCREATEPBUFFERARBPROC)gtglGetProcAddress("wglCreatePbufferARB");
        wglGetPbufferDCARB     = __native_wglGetPbufferDCARB     = (PFNWGLGETPBUFFERDCARBPROC)gtglGetProcAddress("wglGetPbufferDCARB");
        wglReleasePbufferDCARB = __native_wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)gtglGetProcAddress("wglReleasePbufferDCARB");
        wglDestroyPbufferARB   = __native_wglDestroyPbufferARB   = (PFNWGLDESTROYPBUFFERARBPROC)gtglGetProcAddress("wglDestroyPbufferARB");
        wglQueryPbufferARB     = __native_wglQueryPbufferARB     = (PFNWGLQUERYPBUFFERARBPROC)gtglGetProcAddress("wglQueryPbufferARB");

        if (debugMode)
        {
            wglCreatePbufferARB    = __debug_wglCreatePbufferARB;
            wglGetPbufferDCARB     = __debug_wglGetPbufferDCARB;
            wglReleasePbufferDCARB = __debug_wglReleasePbufferDCARB;
            wglDestroyPbufferARB   = __debug_wglDestroyPbufferARB;
            wglQueryPbufferARB     = __debug_wglQueryPbufferARB;
        }
    }

    if (gtglIsExtensionSupported("WGL_ARB_render_texture"))
    {
        GTGL_WGL_ARB_render_texture = GL_TRUE;
        wglBindTexImageARB     = __native_wglBindTexImageARB     = (PFNWGLBINDTEXIMAGEARBPROC)gtglGetProcAddress("wglBindTexImageARB");
        wglReleaseTexImageARB  = __native_wglReleaseTexImageARB  = (PFNWGLRELEASETEXIMAGEARBPROC)gtglGetProcAddress("wglReleaseTexImageARB");
        wglSetPbufferAttribARB = __native_wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)gtglGetProcAddress("wglSetPbufferAttribARB");

        if (debugMode)
        {
            wglBindTexImageARB     = __debug_wglBindTexImageARB;
            wglReleaseTexImageARB  = __debug_wglReleaseTexImageARB;
            wglSetPbufferAttribARB = __debug_wglSetPbufferAttribARB;
        }
    }

    if (gtglIsExtensionSupported("WGL_ARB_pixel_format_float"))
    {
        GTGL_WGL_ARB_pixel_format_float = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_ARB_framebuffer_sRGB"))
    {
        GTGL_WGL_ARB_framebuffer_sRGB = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_ARB_create_context"))
    {
        GTGL_WGL_ARB_create_context = GL_TRUE;
        wglCreateContextAttribsARB = __native_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)gtglGetProcAddress("wglCreateContextAttribsARB");

        if (debugMode)
        {
            wglCreateContextAttribsARB = __debug_wglCreateContextAttribsARB;
        }
    }

    if (gtglIsExtensionSupported("WGL_ARB_create_context_profile"))
    {
        GTGL_WGL_ARB_create_context_profile = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_ARB_create_context_robustness"))
    {
        GTGL_WGL_ARB_create_context_robustness = GL_TRUE;
    }




    /* EXT Extensions */
    if (gtglIsExtensionSupported("WGL_EXT_display_color_table"))
    {
        GTGL_WGL_EXT_display_color_table = GL_TRUE;
        wglCreateDisplayColorTableEXT  = __native_wglCreateDisplayColorTableEXT  = (PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC)gtglGetProcAddress("wglCreateDisplayColorTableEXT");
        wglLoadDisplayColorTableEXT    = __native_wglLoadDisplayColorTableEXT    = (PFNWGLLOADDISPLAYCOLORTABLEEXTPROC)gtglGetProcAddress("wglLoadDisplayColorTableEXT");
        wglBindDisplayColorTableEXT    = __native_wglBindDisplayColorTableEXT    = (PFNWGLBINDDISPLAYCOLORTABLEEXTPROC)gtglGetProcAddress("wglBindDisplayColorTableEXT");
        wglDestroyDisplayColorTableEXT = __native_wglDestroyDisplayColorTableEXT = (PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC)gtglGetProcAddress("wglDestroyDisplayColorTableEXT");

        if (debugMode)
        {
            wglCreateDisplayColorTableEXT  = __debug_wglCreateDisplayColorTableEXT;
            wglLoadDisplayColorTableEXT    = __debug_wglLoadDisplayColorTableEXT;
            wglBindDisplayColorTableEXT    = __debug_wglBindDisplayColorTableEXT;
            wglDestroyDisplayColorTableEXT = __debug_wglDestroyDisplayColorTableEXT;
        }
    }

    if (gtglIsExtensionSupported("WGL_EXT_extensions_string"))
    {
        GTGL_WGL_EXT_extensions_string = GL_TRUE;
        wglGetExtensionsStringEXT = __native_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)gtglGetProcAddress("wglGetExtensionsStringEXT");

        if (debugMode)
        {
            wglGetExtensionsStringEXT = __debug_wglGetExtensionsStringEXT;
        }
    }

    if (gtglIsExtensionSupported("WGL_EXT_make_current_read"))
    {
        GTGL_WGL_EXT_make_current_read = GL_TRUE;
        wglMakeContextCurrentEXT = __native_wglMakeContextCurrentEXT = (PFNWGLMAKECONTEXTCURRENTEXTPROC)gtglGetProcAddress("wglMakeContextCurrentEXT");
        wglGetCurrentReadDCEXT   = __native_wglGetCurrentReadDCEXT   = (PFNWGLGETCURRENTREADDCEXTPROC)gtglGetProcAddress("wglGetCurrentReadDCEXT");

        if (debugMode)
        {
            wglMakeContextCurrentEXT = __debug_wglMakeContextCurrentEXT;
            wglGetCurrentReadDCEXT   = __debug_wglGetCurrentReadDCEXT;
        }
    }

    if (gtglIsExtensionSupported("WGL_EXT_pbuffer"))
    {
        GTGL_WGL_EXT_pbuffer = GL_TRUE;
        wglCreatePbufferEXT    = __native_wglCreatePbufferEXT    = (PFNWGLCREATEPBUFFEREXTPROC)gtglGetProcAddress("wglCreatePbufferEXT");
        wglGetPbufferDCEXT     = __native_wglGetPbufferDCEXT     = (PFNWGLGETPBUFFERDCEXTPROC)gtglGetProcAddress("wglGetPbufferDCEXT");
        wglReleasePbufferDCEXT = __native_wglReleasePbufferDCEXT = (PFNWGLRELEASEPBUFFERDCEXTPROC)gtglGetProcAddress("wglReleasePbufferDCEXT");
        wglDestroyPbufferEXT   = __native_wglDestroyPbufferEXT   = (PFNWGLDESTROYPBUFFEREXTPROC)gtglGetProcAddress("wglDestroyPbufferEXT");
        wglQueryPbufferEXT     = __native_wglQueryPbufferEXT     = (PFNWGLQUERYPBUFFEREXTPROC)gtglGetProcAddress("wglQueryPbufferEXT");

        if (debugMode)
        {
            wglCreatePbufferEXT    = __debug_wglCreatePbufferEXT;
            wglGetPbufferDCEXT     = __debug_wglGetPbufferDCEXT;
            wglReleasePbufferDCEXT = __debug_wglReleasePbufferDCEXT;
            wglDestroyPbufferEXT   = __debug_wglDestroyPbufferEXT;
            wglQueryPbufferEXT     = __debug_wglQueryPbufferEXT;
        }
    }

    if (gtglIsExtensionSupported("WGL_EXT_pixel_format"))
    {
        GTGL_WGL_EXT_pixel_format = GL_TRUE;
        wglGetPixelFormatAttribivEXT = __native_wglGetPixelFormatAttribivEXT = (PFNWGLGETPIXELFORMATATTRIBIVEXTPROC)gtglGetProcAddress("wglGetPixelFormatAttribivEXT");
        wglGetPixelFormatAttribfvEXT = __native_wglGetPixelFormatAttribfvEXT = (PFNWGLGETPIXELFORMATATTRIBFVEXTPROC)gtglGetProcAddress("wglGetPixelFormatAttribfvEXT");
        wglChoosePixelFormatEXT      = __native_wglChoosePixelFormatEXT      = (PFNWGLCHOOSEPIXELFORMATEXTPROC)gtglGetProcAddress("wglChoosePixelFormatEXT");

        if (debugMode)
        {
            wglGetPixelFormatAttribivEXT = __debug_wglGetPixelFormatAttribivEXT;
            wglGetPixelFormatAttribfvEXT = __debug_wglGetPixelFormatAttribfvEXT;
            wglChoosePixelFormatEXT      = __debug_wglChoosePixelFormatEXT;
        }
    }

    if (gtglIsExtensionSupported("WGL_EXT_swap_control"))
    {
        GTGL_WGL_EXT_swap_control = GL_TRUE;
        wglSwapIntervalEXT    = __native_wglSwapIntervalEXT    = (PFNWGLSWAPINTERVALEXTPROC)gtglGetProcAddress("wglSwapIntervalEXT");
        wglGetSwapIntervalEXT = __native_wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)gtglGetProcAddress("wglGetSwapIntervalEXT");

        if (debugMode)
        {
            wglSwapIntervalEXT    = __debug_wglSwapIntervalEXT;
            wglGetSwapIntervalEXT = __debug_wglGetSwapIntervalEXT;
        }
    }

    if (gtglIsExtensionSupported("WGL_EXT_depth_float"))
    {
        GTGL_WGL_EXT_depth_float = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_EXT_multisample"))
    {
        GTGL_WGL_EXT_multisample = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_EXT_pixel_format_packed_float"))
    {
        GTGL_WGL_EXT_pixel_format_packed_float = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_EXT_framebuffer_sRGB"))
    {
        GTGL_WGL_EXT_framebuffer_sRGB = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_EXT_create_context_es2_profile"))
    {
        GTGL_WGL_EXT_create_context_es2_profile = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_EXT_swap_control_tear"))
    {
        GTGL_WGL_EXT_swap_control_tear = GL_TRUE;
    }


    /* NV Extensions */
    if (gtglIsExtensionSupported("WGL_NV_vertex_array_range"))
    {
        GTGL_WGL_NV_vertex_array_range = GL_TRUE;
        wglAllocateMemoryNV = __native_wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)gtglGetProcAddress("wglAllocateMemoryNV");
        wglFreeMemoryNV     = __native_wglFreeMemoryNV     = (PFNWGLFREEMEMORYNVPROC)gtglGetProcAddress("wglFreeMemoryNV");

        if (debugMode)
        {
            wglAllocateMemoryNV = __debug_wglAllocateMemoryNV;
            wglFreeMemoryNV     = __debug_wglFreeMemoryNV;
        }
    }

    if (gtglIsExtensionSupported("WGL_NV_float_buffer"))
    {
        GTGL_WGL_NV_float_buffer = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_NV_present_video"))
    {
        GTGL_WGL_NV_present_video = GL_TRUE;
        wglEnumerateVideoDevicesNV = __native_wglEnumerateVideoDevicesNV = (PFNWGLENUMERATEVIDEODEVICESNVPROC)gtglGetProcAddress("wglEnumerateVideoDevicesNV");
        wglBindVideoDeviceNV       = __native_wglBindVideoDeviceNV       = (PFNWGLBINDVIDEODEVICENVPROC)gtglGetProcAddress("wglBindVideoDeviceNV");
        wglQueryCurrentContextNV   = __native_wglQueryCurrentContextNV   = (PFNWGLQUERYCURRENTCONTEXTNVPROC)gtglGetProcAddress("wglQueryCurrentContextNV");

        if (debugMode)
        {
            wglEnumerateVideoDevicesNV = __debug_wglEnumerateVideoDevicesNV;
            wglBindVideoDeviceNV       = __debug_wglBindVideoDeviceNV;
            wglQueryCurrentContextNV   = __debug_wglQueryCurrentContextNV;
        }
    }

    if (gtglIsExtensionSupported("WGL_NV_video_output"))
    {
        GTGL_WGL_NV_video_output = GL_TRUE;
        wglGetVideoDeviceNV     = __native_wglGetVideoDeviceNV     = (PFNWGLGETVIDEODEVICENVPROC)gtglGetProcAddress("wglGetVideoDeviceNV");
        wglReleaseVideoDeviceNV = __native_wglReleaseVideoDeviceNV = (PFNWGLRELEASEVIDEODEVICENVPROC)gtglGetProcAddress("wglReleaseVideoDeviceNV");
        wglBindVideoImageNV     = __native_wglBindVideoImageNV     = (PFNWGLBINDVIDEOIMAGENVPROC)gtglGetProcAddress("wglBindVideoImageNV");
        wglReleaseVideoImageNV  = __native_wglReleaseVideoImageNV  = (PFNWGLRELEASEVIDEOIMAGENVPROC)gtglGetProcAddress("wglReleaseVideoImageNV");
        wglSendPbufferToVideoNV = __native_wglSendPbufferToVideoNV = (PFNWGLSENDPBUFFERTOVIDEONVPROC)gtglGetProcAddress("wglSendPbufferToVideoNV");
        wglGetVideoInfoNV       = __native_wglGetVideoInfoNV       = (PFNWGLGETVIDEOINFONVPROC)gtglGetProcAddress("wglGetVideoInfoNV");

        if (debugMode)
        {
            wglGetVideoDeviceNV     = __debug_wglGetVideoDeviceNV;
            wglReleaseVideoDeviceNV = __debug_wglReleaseVideoDeviceNV;
            wglBindVideoImageNV     = __debug_wglBindVideoImageNV;
            wglReleaseVideoImageNV  = __debug_wglReleaseVideoImageNV;
            wglSendPbufferToVideoNV = __debug_wglSendPbufferToVideoNV;
            wglGetVideoInfoNV       = __debug_wglGetVideoInfoNV;
        }
    }

    if (gtglIsExtensionSupported("WGL_NV_swap_group"))
    {
        GTGL_WGL_NV_swap_group = GL_TRUE;
        wglJoinSwapGroupNV      = __native_wglJoinSwapGroupNV      = (PFNWGLJOINSWAPGROUPNVPROC)gtglGetProcAddress("wglJoinSwapGroupNV");
        wglBindSwapBarrierNV    = __native_wglBindSwapBarrierNV    = (PFNWGLBINDSWAPBARRIERNVPROC)gtglGetProcAddress("wglBindSwapBarrierNV");
        wglQuerySwapGroupNV     = __native_wglQuerySwapGroupNV     = (PFNWGLQUERYSWAPGROUPNVPROC)gtglGetProcAddress("wglQuerySwapGroupNV");
        wglQueryMaxSwapGroupsNV = __native_wglQueryMaxSwapGroupsNV = (PFNWGLQUERYMAXSWAPGROUPSNVPROC)gtglGetProcAddress("wglQueryMaxSwapGroupsNV");
        wglQueryFrameCountNV    = __native_wglQueryFrameCountNV    = (PFNWGLQUERYFRAMECOUNTNVPROC)gtglGetProcAddress("wglQueryFrameCountNV");
        wglResetFrameCountNV    = __native_wglResetFrameCountNV    = (PFNWGLRESETFRAMECOUNTNVPROC)gtglGetProcAddress("wglResetFrameCountNV");

        if (debugMode)
        {
            wglJoinSwapGroupNV      = __debug_wglJoinSwapGroupNV;
            wglBindSwapBarrierNV    = __debug_wglBindSwapBarrierNV;
            wglQuerySwapGroupNV     = __debug_wglQuerySwapGroupNV;
            wglQueryMaxSwapGroupsNV = __debug_wglQueryMaxSwapGroupsNV;
            wglQueryFrameCountNV    = __debug_wglQueryFrameCountNV;
            wglResetFrameCountNV    = __debug_wglResetFrameCountNV;
        }
    }

    if (gtglIsExtensionSupported("WGL_NV_gpu_affinity"))
    {
        GTGL_WGL_NV_gpu_affinity = GL_TRUE;
        wglEnumGpusNV               = __native_wglEnumGpusNV               = (PFNWGLENUMGPUSNVPROC)gtglGetProcAddress("wglEnumGpusNV");
        wglEnumGpuDevicesNV         = __native_wglEnumGpuDevicesNV         = (PFNWGLENUMGPUDEVICESNVPROC)gtglGetProcAddress("wglEnumGpuDevicesNV");
        wglCreateAffinityDCNV       = __native_wglCreateAffinityDCNV       = (PFNWGLCREATEAFFINITYDCNVPROC)gtglGetProcAddress("wglCreateAffinityDCNV");
        wglEnumGpusFromAffinityDCNV = __native_wglEnumGpusFromAffinityDCNV = (PFNWGLENUMGPUSFROMAFFINITYDCNVPROC)gtglGetProcAddress("wglEnumGpusFromAffinityDCNV");
        wglDeleteDCNV               = __native_wglDeleteDCNV               = (PFNWGLDELETEDCNVPROC)gtglGetProcAddress("wglDeleteDCNV");

        if (debugMode)
        {
            wglEnumGpusNV               = __debug_wglEnumGpusNV;
            wglEnumGpuDevicesNV         = __debug_wglEnumGpuDevicesNV;
            wglCreateAffinityDCNV       = __debug_wglCreateAffinityDCNV;
            wglEnumGpusFromAffinityDCNV = __debug_wglEnumGpusFromAffinityDCNV;
            wglDeleteDCNV               = __debug_wglDeleteDCNV;
        }
    }

    if (gtglIsExtensionSupported("WGL_NV_video_capture"))
    {
        GTGL_WGL_NV_video_capture = GL_TRUE;
        wglBindVideoCaptureDeviceNV       = __native_wglBindVideoCaptureDeviceNV       = (PFNWGLBINDVIDEOCAPTUREDEVICENVPROC)gtglGetProcAddress("wglBindVideoCaptureDeviceNV");
        wglEnumerateVideoCaptureDevicesNV = __native_wglEnumerateVideoCaptureDevicesNV = (PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC)gtglGetProcAddress("wglEnumerateVideoCaptureDevicesNV");
        wglLockVideoCaptureDeviceNV       = __native_wglLockVideoCaptureDeviceNV       = (PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC)gtglGetProcAddress("wglLockVideoCaptureDeviceNV");
        wglQueryVideoCaptureDeviceNV      = __native_wglQueryVideoCaptureDeviceNV      = (PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC)gtglGetProcAddress("wglQueryVideoCaptureDeviceNV");
        wglReleaseVideoCaptureDeviceNV    = __native_wglReleaseVideoCaptureDeviceNV    = (PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC)gtglGetProcAddress("wglReleaseVideoCaptureDeviceNV");

        if (debugMode)
        {
            wglBindVideoCaptureDeviceNV       = __debug_wglBindVideoCaptureDeviceNV;
            wglEnumerateVideoCaptureDevicesNV = __debug_wglEnumerateVideoCaptureDevicesNV;
            wglLockVideoCaptureDeviceNV       = __debug_wglLockVideoCaptureDeviceNV;
            wglQueryVideoCaptureDeviceNV      = __debug_wglQueryVideoCaptureDeviceNV;
            wglReleaseVideoCaptureDeviceNV    = __debug_wglReleaseVideoCaptureDeviceNV;
        }
    }

    if (gtglIsExtensionSupported("WGL_NV_copy_image"))
    {
        GTGL_WGL_NV_copy_image = GL_TRUE;
        wglCopyImageSubDataNV = __native_wglCopyImageSubDataNV = (PFNWGLCOPYIMAGESUBDATANVPROC)gtglGetProcAddress("wglCopyImageSubDataNV");

        if (debugMode)
        {
            wglCopyImageSubDataNV = __debug_wglCopyImageSubDataNV;
        }
    }

    if (gtglIsExtensionSupported("WGL_NV_multisample_coverage"))
    {
        GTGL_WGL_NV_multisample_coverage = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_NV_DX_interop"))
    {
        GTGL_WGL_NV_DX_interop = GL_TRUE;
        wglDXSetResourceShareHandleNV = __native_wglDXSetResourceShareHandleNV = (PFNWGLDXSETRESOURCESHAREHANDLENVPROC)gtglGetProcAddress("wglDXSetResourceShareHandleNV");
        wglDXOpenDeviceNV             = __native_wglDXOpenDeviceNV             = (PFNWGLDXOPENDEVICENVPROC)gtglGetProcAddress("wglDXOpenDeviceNV");
        wglDXCloseDeviceNV            = __native_wglDXCloseDeviceNV            = (PFNWGLDXCLOSEDEVICENVPROC)gtglGetProcAddress("wglDXCloseDeviceNV");
        wglDXRegisterObjectNV         = __native_wglDXRegisterObjectNV         = (PFNWGLDXREGISTEROBJECTNVPROC)gtglGetProcAddress("wglDXRegisterObjectNV");
        wglDXUnregisterObjectNV       = __native_wglDXUnregisterObjectNV       = (PFNWGLDXUNREGISTEROBJECTNVPROC)gtglGetProcAddress("wglDXUnregisterObjectNV");
        wglDXObjectAccessNV           = __native_wglDXObjectAccessNV           = (PFNWGLDXOBJECTACCESSNVPROC)gtglGetProcAddress("wglDXObjectAccessNV");
        wglDXLockObjectsNV            = __native_wglDXLockObjectsNV            = (PFNWGLDXLOCKOBJECTSNVPROC)gtglGetProcAddress("wglDXLockObjectsNV");
        wglDXUnlockObjectsNV          = __native_wglDXUnlockObjectsNV          = (PFNWGLDXUNLOCKOBJECTSNVPROC)gtglGetProcAddress("wglDXUnlockObjectsNV");

        if (debugMode)
        {
            wglDXSetResourceShareHandleNV = __debug_wglDXSetResourceShareHandleNV;
            wglDXOpenDeviceNV             = __debug_wglDXOpenDeviceNV;
            wglDXCloseDeviceNV            = __debug_wglDXCloseDeviceNV;
            wglDXRegisterObjectNV         = __debug_wglDXRegisterObjectNV;
            wglDXUnregisterObjectNV       = __debug_wglDXUnregisterObjectNV;
            wglDXObjectAccessNV           = __debug_wglDXObjectAccessNV;
            wglDXLockObjectsNV            = __debug_wglDXLockObjectsNV;
            wglDXUnlockObjectsNV          = __debug_wglDXUnlockObjectsNV;
        }
    }

    if (gtglIsExtensionSupported("WGL_NV_DX_interop2"))
    {
        GTGL_WGL_NV_DX_interop2 = GL_TRUE;
    }

    /* I3D Extensions */
    if (gtglIsExtensionSupported("WGL_I3D_digital_video_control"))
    {
        GTGL_WGL_I3D_digital_video_control = GL_TRUE;
        wglGetDigitalVideoParametersI3D = __native_wglGetDigitalVideoParametersI3D = (PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC)gtglGetProcAddress("wglGetDigitalVideoParametersI3D");
        wglSetDigitalVideoParametersI3D = __native_wglSetDigitalVideoParametersI3D = (PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC)gtglGetProcAddress("wglSetDigitalVideoParametersI3D");

        if (debugMode)
        {
            wglGetDigitalVideoParametersI3D = __debug_wglGetDigitalVideoParametersI3D;
            wglSetDigitalVideoParametersI3D = __debug_wglSetDigitalVideoParametersI3D;
        }
    }

    if (gtglIsExtensionSupported("WGL_I3D_gamma"))
    {
        GTGL_WGL_I3D_gamma = GL_TRUE;
        wglGetGammaTableParametersI3D = __native_wglGetGammaTableParametersI3D = (PFNWGLGETGAMMATABLEPARAMETERSI3DPROC)gtglGetProcAddress("wglGetGammaTableParametersI3D");
        wglSetGammaTableParametersI3D = __native_wglSetGammaTableParametersI3D = (PFNWGLSETGAMMATABLEPARAMETERSI3DPROC)gtglGetProcAddress("wglSetGammaTableParametersI3D");
        wglGetGammaTableI3D           = __native_wglGetGammaTableI3D           = (PFNWGLGETGAMMATABLEI3DPROC)gtglGetProcAddress("wglGetGammaTableI3D");
        wglSetGammaTableI3D           = __native_wglSetGammaTableI3D           = (PFNWGLSETGAMMATABLEI3DPROC)gtglGetProcAddress("wglSetGammaTableI3D");

        if (debugMode)
        {
            wglGetGammaTableParametersI3D = __debug_wglGetGammaTableParametersI3D;
            wglSetGammaTableParametersI3D = __debug_wglSetGammaTableParametersI3D;
            wglGetGammaTableI3D           = __debug_wglGetGammaTableI3D;
            wglSetGammaTableI3D           = __debug_wglSetGammaTableI3D;
        }
    }

    if (gtglIsExtensionSupported("WGL_I3D_genlock"))
    {
        GTGL_WGL_I3D_genlock = GL_TRUE;
        wglEnableGenlockI3D              = __native_wglEnableGenlockI3D              = (PFNWGLENABLEGENLOCKI3DPROC)gtglGetProcAddress("wglEnableGenlockI3D");
        wglDisableGenlockI3D             = __native_wglDisableGenlockI3D             = (PFNWGLDISABLEGENLOCKI3DPROC)gtglGetProcAddress("wglDisableGenlockI3D");
        wglIsEnabledGenlockI3D           = __native_wglIsEnabledGenlockI3D           = (PFNWGLISENABLEDGENLOCKI3DPROC)gtglGetProcAddress("wglIsEnabledGenlockI3D");
        wglGenlockSourceI3D              = __native_wglGenlockSourceI3D              = (PFNWGLGENLOCKSOURCEI3DPROC)gtglGetProcAddress("wglGenlockSourceI3D");
        wglGetGenlockSourceI3D           = __native_wglGetGenlockSourceI3D           = (PFNWGLGETGENLOCKSOURCEI3DPROC)gtglGetProcAddress("wglGetGenlockSourceI3D");
        wglGenlockSourceEdgeI3D          = __native_wglGenlockSourceEdgeI3D          = (PFNWGLGENLOCKSOURCEEDGEI3DPROC)gtglGetProcAddress("wglGenlockSourceEdgeI3D");
        wglGetGenlockSourceEdgeI3D       = __native_wglGetGenlockSourceEdgeI3D       = (PFNWGLGETGENLOCKSOURCEEDGEI3DPROC)gtglGetProcAddress("wglGetGenlockSourceEdgeI3D");
        wglGenlockSampleRateI3D          = __native_wglGenlockSampleRateI3D          = (PFNWGLGENLOCKSAMPLERATEI3DPROC)gtglGetProcAddress("wglGenlockSampleRateI3D");
        wglGetGenlockSampleRateI3D       = __native_wglGetGenlockSampleRateI3D       = (PFNWGLGETGENLOCKSAMPLERATEI3DPROC)gtglGetProcAddress("wglGetGenlockSampleRateI3D");
        wglGenlockSourceDelayI3D         = __native_wglGenlockSourceDelayI3D         = (PFNWGLGENLOCKSOURCEDELAYI3DPROC)gtglGetProcAddress("wglGenlockSourceDelayI3D");
        wglGetGenlockSourceDelayI3D      = __native_wglGetGenlockSourceDelayI3D      = (PFNWGLGETGENLOCKSOURCEDELAYI3DPROC)gtglGetProcAddress("wglGetGenlockSourceDelayI3D");
        wglQueryGenlockMaxSourceDelayI3D = __native_wglQueryGenlockMaxSourceDelayI3D = (PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC)gtglGetProcAddress("wglQueryGenlockMaxSourceDelayI3D");

        if (debugMode)
        {
            wglEnableGenlockI3D              = __debug_wglEnableGenlockI3D;
            wglDisableGenlockI3D             = __debug_wglDisableGenlockI3D;
            wglIsEnabledGenlockI3D           = __debug_wglIsEnabledGenlockI3D;
            wglGenlockSourceI3D              = __debug_wglGenlockSourceI3D;
            wglGetGenlockSourceI3D           = __debug_wglGetGenlockSourceI3D;
            wglGenlockSourceEdgeI3D          = __debug_wglGenlockSourceEdgeI3D;
            wglGetGenlockSourceEdgeI3D       = __debug_wglGetGenlockSourceEdgeI3D;
            wglGenlockSampleRateI3D          = __debug_wglGenlockSampleRateI3D;
            wglGetGenlockSampleRateI3D       = __debug_wglGetGenlockSampleRateI3D;
            wglGenlockSourceDelayI3D         = __debug_wglGenlockSourceDelayI3D;
            wglGetGenlockSourceDelayI3D      = __debug_wglGetGenlockSourceDelayI3D;
            wglQueryGenlockMaxSourceDelayI3D = __debug_wglQueryGenlockMaxSourceDelayI3D;
        }
    }

    if (gtglIsExtensionSupported("WGL_I3D_image_buffer"))
    {
        GTGL_WGL_I3D_image_buffer = GL_TRUE;
        wglCreateImageBufferI3D          = __native_wglCreateImageBufferI3D          = (PFNWGLCREATEIMAGEBUFFERI3DPROC)gtglGetProcAddress("wglCreateImageBufferI3D");
        wglDestroyImageBufferI3D         = __native_wglDestroyImageBufferI3D         = (PFNWGLDESTROYIMAGEBUFFERI3DPROC)gtglGetProcAddress("wglDestroyImageBufferI3D");
        wglAssociateImageBufferEventsI3D = __native_wglAssociateImageBufferEventsI3D = (PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC)gtglGetProcAddress("wglAssociateImageBufferEventsI3D");
        wglReleaseImageBufferEventsI3D   = __native_wglReleaseImageBufferEventsI3D   = (PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC)gtglGetProcAddress("wglReleaseImageBufferEventsI3D");

        if (debugMode)
        {
            wglCreateImageBufferI3D          = __debug_wglCreateImageBufferI3D;
            wglDestroyImageBufferI3D         = __debug_wglDestroyImageBufferI3D;
            wglAssociateImageBufferEventsI3D = __debug_wglAssociateImageBufferEventsI3D;
            wglReleaseImageBufferEventsI3D   = __debug_wglReleaseImageBufferEventsI3D;
        }
    }

    if (gtglIsExtensionSupported("WGL_I3D_swap_frame_lock"))
    {
        GTGL_WGL_I3D_swap_frame_lock = GL_TRUE;
        wglEnableFrameLockI3D      = __native_wglEnableFrameLockI3D      = (PFNWGLENABLEFRAMELOCKI3DPROC)gtglGetProcAddress("wglEnableFrameLockI3D");
        wglDisableFrameLockI3D     = __native_wglDisableFrameLockI3D     = (PFNWGLDISABLEFRAMELOCKI3DPROC)gtglGetProcAddress("wglDisableFrameLockI3D");
        wglIsEnabledFrameLockI3D   = __native_wglIsEnabledFrameLockI3D   = (PFNWGLISENABLEDFRAMELOCKI3DPROC)gtglGetProcAddress("wglIsEnabledFrameLockI3D");
        wglQueryFrameLockMasterI3D = __native_wglQueryFrameLockMasterI3D = (PFNWGLQUERYFRAMELOCKMASTERI3DPROC)gtglGetProcAddress("wglQueryFrameLockMasterI3D");

        if (debugMode)
        {
            wglEnableFrameLockI3D      = __debug_wglEnableFrameLockI3D;
            wglDisableFrameLockI3D     = __debug_wglDisableFrameLockI3D;
            wglIsEnabledFrameLockI3D   = __debug_wglIsEnabledFrameLockI3D;
            wglQueryFrameLockMasterI3D = __debug_wglQueryFrameLockMasterI3D;
        }
    }

    if (gtglIsExtensionSupported("WGL_I3D_swap_frame_usage"))
    {
        GTGL_WGL_I3D_swap_frame_usage = GL_TRUE;
        wglGetFrameUsageI3D      = __native_wglGetFrameUsageI3D      = (PFNWGLGETFRAMEUSAGEI3DPROC)gtglGetProcAddress("wglGetFrameUsageI3D");
        wglBeginFrameTrackingI3D = __native_wglBeginFrameTrackingI3D = (PFNWGLBEGINFRAMETRACKINGI3DPROC)gtglGetProcAddress("wglBeginFrameTrackingI3D");
        wglEndFrameTrackingI3D   = __native_wglEndFrameTrackingI3D   = (PFNWGLENDFRAMETRACKINGI3DPROC)gtglGetProcAddress("wglEndFrameTrackingI3D");
        wglQueryFrameTrackingI3D = __native_wglQueryFrameTrackingI3D = (PFNWGLQUERYFRAMETRACKINGI3DPROC)gtglGetProcAddress("wglQueryFrameTrackingI3D");

        if (debugMode)
        {
            wglGetFrameUsageI3D      = __debug_wglGetFrameUsageI3D;
            wglBeginFrameTrackingI3D = __debug_wglBeginFrameTrackingI3D;
            wglEndFrameTrackingI3D   = __debug_wglEndFrameTrackingI3D;
            wglQueryFrameTrackingI3D = __debug_wglQueryFrameTrackingI3D;
        }
    }

    /* Other Extensions */
    if (gtglIsExtensionSupported("WGL_3DFX_multisample"))
    {
        GTGL_WGL_3DFX_multisample = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_OML_sync_control"))
    {
        GTGL_WGL_OML_sync_control = GL_TRUE;
        wglGetSyncValuesOML       = __native_wglGetSyncValuesOML       = (PFNWGLGETSYNCVALUESOMLPROC)gtglGetProcAddress("wglGetSyncValuesOML");
        wglGetMscRateOML          = __native_wglGetMscRateOML          = (PFNWGLGETMSCRATEOMLPROC)gtglGetProcAddress("wglGetMscRateOML");
        wglSwapBuffersMscOML      = __native_wglSwapBuffersMscOML      = (PFNWGLSWAPBUFFERSMSCOMLPROC)gtglGetProcAddress("wglSwapBuffersMscOML");
        wglSwapLayerBuffersMscOML = __native_wglSwapLayerBuffersMscOML = (PFNWGLSWAPLAYERBUFFERSMSCOMLPROC)gtglGetProcAddress("wglSwapLayerBuffersMscOML");
        wglWaitForMscOML          = __native_wglWaitForMscOML          = (PFNWGLWAITFORMSCOMLPROC)gtglGetProcAddress("wglWaitForMscOML");
        wglWaitForSbcOML          = __native_wglWaitForSbcOML          = (PFNWGLWAITFORSBCOMLPROC)gtglGetProcAddress("wglWaitForSbcOML");

        if (debugMode)
        {
            wglGetSyncValuesOML       = __debug_wglGetSyncValuesOML;
            wglGetMscRateOML          = __debug_wglGetMscRateOML;
            wglSwapBuffersMscOML      = __debug_wglSwapBuffersMscOML;
            wglSwapLayerBuffersMscOML = __debug_wglSwapLayerBuffersMscOML;
            wglWaitForMscOML          = __debug_wglWaitForMscOML;
            wglWaitForSbcOML          = __debug_wglWaitForSbcOML;
        }
    }

    if (gtglIsExtensionSupported("WGL_ATI_pixel_format_float"))
    {
        GTGL_WGL_ATI_pixel_format_float = GL_TRUE;
    }

    if (gtglIsExtensionSupported("WGL_3DL_stereo_control"))
    {
        GTGL_WGL_3DL_stereo_control = GL_TRUE;
        wglSetStereoEmitterState3DL = __native_wglSetStereoEmitterState3DL = (PFNWGLSETSTEREOEMITTERSTATE3DLPROC)gtglGetProcAddress("wglSetStereoEmitterState3DL");

        if (debugMode)
        {
            wglSetStereoEmitterState3DL = __debug_wglSetStereoEmitterState3DL;
        }
    }

    if (gtglIsExtensionSupported("WGL_AMD_gpu_association"))
    {
        GTGL_WGL_AMD_gpu_association = GL_TRUE;
        wglGetGPUIDsAMD                      = __native_wglGetGPUIDsAMD                      = (PFNWGLGETGPUIDSAMDPROC)gtglGetProcAddress("wglGetGPUIDsAMD");
        wglGetGPUInfoAMD                     = __native_wglGetGPUInfoAMD                     = (PFNWGLGETGPUINFOAMDPROC)gtglGetProcAddress("wglGetGPUInfoAMD");
        wglGetContextGPUIDAMD                = __native_wglGetContextGPUIDAMD                = (PFNWGLGETCONTEXTGPUIDAMDPROC)gtglGetProcAddress("wglGetContextGPUIDAMD");
        wglCreateAssociatedContextAMD        = __native_wglCreateAssociatedContextAMD        = (PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC)gtglGetProcAddress("wglCreateAssociatedContextAMD");
        wglCreateAssociatedContextAttribsAMD = __native_wglCreateAssociatedContextAttribsAMD = (PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC)gtglGetProcAddress("wglCreateAssociatedContextAttribsAMD");
        wglDeleteAssociatedContextAMD        = __native_wglDeleteAssociatedContextAMD        = (PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC)gtglGetProcAddress("wglDeleteAssociatedContextAMD");
        wglMakeAssociatedContextCurrentAMD   = __native_wglMakeAssociatedContextCurrentAMD   = (PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC)gtglGetProcAddress("wglMakeAssociatedContextCurrentAMD");
        wglGetCurrentAssociatedContextAMD    = __native_wglGetCurrentAssociatedContextAMD    = (PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC)gtglGetProcAddress("wglGetCurrentAssociatedContextAMD");
        wglBlitContextFramebufferAMD         = __native_wglBlitContextFramebufferAMD         = (PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC)gtglGetProcAddress("wglBlitContextFramebufferAMD");

        if (debugMode)
        {
            wglGetGPUIDsAMD                      = __debug_wglGetGPUIDsAMD;
            wglGetGPUInfoAMD                     = __debug_wglGetGPUInfoAMD;
            wglGetContextGPUIDAMD                = __debug_wglGetContextGPUIDAMD;
            wglCreateAssociatedContextAMD        = __debug_wglCreateAssociatedContextAMD;
            wglCreateAssociatedContextAttribsAMD = __debug_wglCreateAssociatedContextAttribsAMD;
            wglDeleteAssociatedContextAMD        = __debug_wglDeleteAssociatedContextAMD;
            wglMakeAssociatedContextCurrentAMD   = __debug_wglMakeAssociatedContextCurrentAMD;
            wglGetCurrentAssociatedContextAMD    = __debug_wglGetCurrentAssociatedContextAMD;
            wglBlitContextFramebufferAMD         = __debug_wglBlitContextFramebufferAMD;
        }
    }
}

const char * gtwglGetExtensionsString()
{
    PFNWGLGETEXTENSIONSSTRINGARBPROC __temp_wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)gtglGetProcAddress("wglGetExtensionsStringARB");
    if (__temp_wglGetExtensionsStringARB != NULL)
    {
        return __temp_wglGetExtensionsStringARB(gtwglGetCurrentDC());
    }

    return "";
}

#endif
