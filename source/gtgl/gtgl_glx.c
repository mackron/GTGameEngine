/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include <gtgl/gtgl.h>


/* We only continue if we're on a GLX platform. */
#ifdef GTGL_GLX

#include "debugging/gtgl_debugging_common.h"

/*
We're going to do a bit of an experiment here and see if we can avoid the need for linking to libGL.so at compile time.
*/
#include <dlfcn.h>

void* GTGL_GL_SO = NULL;

PFNGLXCHOOSEVISUALPROC          gtglXChooseVisual           = NULL;
PFNGLXCREATECONTEXTPROC         gtglXCreateContext          = NULL;
PFNGLXDESTROYCONTEXTPROC        gtglXDestroyContext         = NULL;
PFNGLXMAKECURRENTPROC           gtglXMakeCurrent            = NULL;
PFNGLXSWAPBUFFERSPROC           gtglXSwapBuffers            = NULL;
PFNGLXGETCURRENTCONTEXTPROC     gtglXGetCurrentContext      = NULL;
PFNGLXQUERYEXTENSIONSTRINGPROC  gtglXQueryExtensionsString  = NULL;
PFNGLXGETCURRENTDISPLAYPROC     gtglXGetCurrentDisplay      = NULL;
PFNGLXCHOOSEFBCONFIGPROC        gtglXChooseFBConfig         = NULL;
PFNGLXGETVISUALFROMFBCONFIGPROC gtglXGetVisualFromFBConfig  = NULL;
PFNGLXGETPROCADDRESSPROC        gtglXGetProcAddress         = NULL;


GLboolean gtglLoadGLXSymbols()
{
    if (GTGL_GL_SO == NULL)
    {
        GTGL_GL_SO = dlopen("libGL.so", RTLD_NOW | RTLD_GLOBAL);
        if (GTGL_GL_SO != NULL)
        {
            gtglXChooseVisual          =          (PFNGLXCHOOSEVISUALPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXChooseVisual"));
            gtglXCreateContext         =         (PFNGLXCREATECONTEXTPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXCreateContext"));
            gtglXDestroyContext        =        (PFNGLXDESTROYCONTEXTPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXDestroyContext"));
            gtglXMakeCurrent           =           (PFNGLXMAKECURRENTPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXMakeCurrent"));
            gtglXSwapBuffers           =           (PFNGLXSWAPBUFFERSPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXSwapBuffers"));
            gtglXGetCurrentContext     =     (PFNGLXGETCURRENTCONTEXTPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXGetCurrentContext"));
            gtglXQueryExtensionsString =  (PFNGLXQUERYEXTENSIONSTRINGPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXQueryExtensionsString"));
            gtglXGetCurrentDisplay     =     (PFNGLXGETCURRENTDISPLAYPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXGetCurrentDisplay"));
            gtglXChooseFBConfig        =        (PFNGLXCHOOSEFBCONFIGPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXChooseFBConfig"));
            gtglXGetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXGetVisualFromFBConfig"));
            gtglXGetProcAddress        =        (PFNGLXGETPROCADDRESSPROC)voidp_to_function(dlsym(GTGL_GL_SO, "glXGetProcAddress"));

            return GL_TRUE;
        }
        else
        {
            /* Failed to open the library. */
            return GL_FALSE;
        }
    }
    else
    {
        /* Symbols are already loaded. */
        return GL_TRUE;
    }
}

void gtglUnloadGLXSymbols()
{
    if (GTGL_GL_SO != NULL)
    {
        dlclose(GTGL_GL_SO);

        gtglXChooseVisual          = NULL;
        gtglXCreateContext         = NULL;
        gtglXDestroyContext        = NULL;
        gtglXMakeCurrent           = NULL;
        gtglXSwapBuffers           = NULL;
        gtglXGetCurrentContext     = NULL;
        gtglXQueryExtensionsString = NULL;
        gtglXGetCurrentDisplay     = NULL;
        gtglXChooseFBConfig        = NULL;
        gtglXGetVisualFromFBConfig = NULL;
        gtglXGetProcAddress        = NULL;
    }
}



/* ARB Extensions */
GLboolean GTGL_GLX_ARB_multisample                = GL_FALSE;
GLboolean GTGL_GLX_ARB_fbconfig_float             = GL_FALSE;
GLboolean GTGL_GLX_ARB_framebuffer_sRGB           = GL_FALSE;
GLboolean GTGL_GLX_ARB_create_context             = GL_FALSE;
GLboolean GTGL_GLX_ARB_create_context_profile     = GL_FALSE;
GLboolean GTGL_GLX_ARB_create_context_robustness  = GL_FALSE;

/* EXT Extensions */
GLboolean GTGL_GLX_EXT_visual_info                = GL_FALSE;
GLboolean GTGL_GLX_EXT_visual_rating              = GL_FALSE;
GLboolean GTGL_GLX_EXT_import_context             = GL_FALSE;
GLboolean GTGL_GLX_EXT_fbconfig_packed_float      = GL_FALSE;
GLboolean GTGL_GLX_EXT_framebuffer_sRGB           = GL_FALSE;
GLboolean GTGL_GLX_EXT_texture_from_pixmap        = GL_FALSE;
GLboolean GTGL_GLX_EXT_swap_control               = GL_FALSE;
GLboolean GTGL_GLX_EXT_create_context_es2_profile = GL_FALSE;
GLboolean GTGL_GLX_EXT_swap_control_tear          = GL_FALSE;

/* NV Extensions */
GLboolean GTGL_GLX_NV_float_buffer                = GL_FALSE;
GLboolean GTGL_GLX_NV_present_video               = GL_FALSE;
GLboolean GTGL_GLX_NV_video_output                = GL_FALSE;
GLboolean GTGL_GLX_NV_swap_group                  = GL_FALSE;
GLboolean GTGL_GLX_NV_video_capture               = GL_FALSE;
GLboolean GTGL_GLX_NV_copy_image                  = GL_FALSE;
GLboolean GTGL_GLX_NV_multisample_coverage        = GL_FALSE;

/* SGI Extensions */
GLboolean GTGL_GLX_SGIS_multisample               = GL_FALSE;
GLboolean GTGL_GLX_SGI_swap_control               = GL_FALSE;
GLboolean GTGL_GLX_SGI_video_sync                 = GL_FALSE;
GLboolean GTGL_GLX_SGI_make_current_read          = GL_FALSE;
GLboolean GTGL_GLX_SGIX_fbconfig                  = GL_FALSE;
GLboolean GTGL_GLX_SGIX_pbuffer                   = GL_FALSE;
GLboolean GTGL_GLX_SGI_cushion                    = GL_FALSE;
GLboolean GTGL_GLX_SGIX_video_resize              = GL_FALSE;
GLboolean GTGL_GLX_SGIX_swap_group                = GL_FALSE;
GLboolean GTGL_GLX_SGIX_swap_barrier              = GL_FALSE;
GLboolean GTGL_GLX_SGIX_visual_select_group       = GL_FALSE;
GLboolean GTGL_GLX_SGIX_hyperpipe                 = GL_FALSE;

/* MESA Extensions */
GLboolean GTGL_GLX_MESA_copy_sub_buffer           = GL_FALSE;
GLboolean GTGL_GLX_MESA_pixmap_colormap           = GL_FALSE;
GLboolean GTGL_GLX_MESA_release_buffers           = GL_FALSE;
GLboolean GTGL_GLX_MESA_set_3dfx_mode             = GL_FALSE;
GLboolean GTGL_GLX_MESA_agp_offset                = GL_FALSE;

/* Other Extensions */
GLboolean GTGL_GLX_SUN_get_transparent_index      = GL_FALSE;
GLboolean GTGL_GLX_OML_swap_method                = GL_FALSE;
GLboolean GTGL_GLX_OML_sync_control               = GL_FALSE;
GLboolean GTGL_GLX_INTEL_swap_event               = GL_FALSE;


/* GLX_ARB_create_context */
PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = NULL, __native_glXCreateContextAttribsARB = NULL;
GLXContext __debug_glXCreateContextAttribsARB(Display *dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list) { GTGL_DEBUG_IMPL_RETURN(GLXContext, glXCreateContextAttribsARB, (dpy, config, share_context, direct, attrib_list)); }


/* GLX_EXT_import_context */
PFNGLXGETCURRENTDISPLAYEXTPROC glXGetCurrentDisplayEXT = NULL, __native_glXGetCurrentDisplayEXT = NULL;
PFNGLXQUERYCONTEXTINFOEXTPROC  glXQueryContextInfoEXT  = NULL, __native_glXQueryContextInfoEXT  = NULL;
PFNGLXGETCONTEXTIDEXTPROC      glXGetContextIDEXT      = NULL, __native_glXGetContextIDEXT      = NULL;
PFNGLXIMPORTCONTEXTEXTPROC     glXImportContextEXT     = NULL, __native_glXImportContextEXT     = NULL;
PFNGLXFREECONTEXTEXTPROC       glXFreeContextEXT       = NULL, __native_glXFreeContextEXT       = NULL;

Display * __debug_glXGetCurrentDisplayEXT(void)                                             { GTGL_DEBUG_IMPL_RETURN(Display *, glXGetCurrentDisplayEXT, ()); }
int __debug_glXQueryContextInfoEXT(Display *dpy, GLXContext context, int attribute, int *value) { GTGL_DEBUG_IMPL_RETURN(int, glXQueryContextInfoEXT, (dpy, context, attribute, value)); }
GLXContextID __debug_glXGetContextIDEXT(const GLXContext context)                           { GTGL_DEBUG_IMPL_RETURN(GLXContextID, glXGetContextIDEXT, (context)); }
GLXContext __debug_glXImportContextEXT(Display *dpy, GLXContextID contextID)                { GTGL_DEBUG_IMPL_RETURN(GLXContext, glXImportContextEXT, (dpy, contextID)); }
void __debug_glXFreeContextEXT(Display *dpy, GLXContext context)                            { GTGL_DEBUG_IMPL(glXFreeContextEXT, (dpy, context)); }

/* GLX_EXT_texture_from_pixmap */
PFNGLXBINDTEXIMAGEEXTPROC    glXBindTexImageEXT    = NULL, __native_glXBindTexImageEXT    = NULL;
PFNGLXRELEASETEXIMAGEEXTPROC glXReleaseTexImageEXT = NULL, __native_glXReleaseTexImageEXT = NULL;

void __debug_glXBindTexImageEXT(Display *dpy, GLXDrawable drawable, int buffer, const int *attrib_list) { GTGL_DEBUG_IMPL(glXBindTexImageEXT, (dpy, drawable, buffer, attrib_list)); }
void __debug_glXReleaseTexImageEXT(Display *dpy, GLXDrawable drawable, int buffer) { GTGL_DEBUG_IMPL(glXReleaseTexImageEXT, (dpy, drawable, buffer)); }


/* GLX_EXT_swap_control */
PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = NULL, __native_glXSwapIntervalEXT = NULL;
void APIENTRY __debug_glXSwapIntervalEXT(Display *dpy, GLXDrawable drawable, int interval) { GTGL_DEBUG_IMPL(glXSwapIntervalEXT, (dpy, drawable, interval)); }


/* GLX_NV_present_video */
PFNGLXENUMERATEVIDEODEVICESNVPROC glXEnumerateVideoDevicesNV = NULL, __native_glXEnumerateVideoDevicesNV = NULL;
PFNGLXBINDVIDEODEVICENVPROC       glXBindVideoDeviceNV       = NULL, __native_glXBindVideoDeviceNV       = NULL;

unsigned int * __debug_glXEnumerateVideoDevicesNV(Display *dpy, int screen, int *nelements) { GTGL_DEBUG_IMPL_RETURN(unsigned int *, glXEnumerateVideoDevicesNV, (dpy, screen, nelements)); }
int __debug_glXBindVideoDeviceNV(Display *dpy, unsigned int video_slot, unsigned int video_device, const int *attrib_list) { GTGL_DEBUG_IMPL_RETURN(int, glXBindVideoDeviceNV, (dpy, video_slot, video_device, attrib_list)); }


/* GLX_NV_video_output */
PFNGLXGETVIDEODEVICENVPROC     glXGetVideoDeviceNV     = NULL, __native_glXGetVideoDeviceNV     = NULL;
PFNGLXRELEASEVIDEODEVICENVPROC glXReleaseVideoDeviceNV = NULL, __native_glXReleaseVideoDeviceNV = NULL;
PFNGLXBINDVIDEOIMAGENVPROC     glXBindVideoImageNV     = NULL, __native_glXBindVideoImageNV     = NULL;
PFNGLXRELEASEVIDEOIMAGENVPROC  glXReleaseVideoImageNV  = NULL, __native_glXReleaseVideoImageNV  = NULL;
PFNGLXSENDPBUFFERTOVIDEONVPROC glXSendPbufferToVideoNV = NULL, __native_glXSendPbufferToVideoNV = NULL;
PFNGLXGETVIDEOINFONVPROC       glXGetVideoInfoNV       = NULL, __native_glXGetVideoInfoNV       = NULL;

int __debug_glXGetVideoDeviceNV(Display *dpy, int screen, int numVideoDevices, GLXVideoDeviceNV *pVideoDevice) { GTGL_DEBUG_IMPL_RETURN(int, glXGetVideoDeviceNV, (dpy, screen, numVideoDevices, pVideoDevice)); }
int __debug_glXReleaseVideoDeviceNV(Display *dpy, int screen, GLXVideoDeviceNV VideoDevice)   { GTGL_DEBUG_IMPL_RETURN(int, glXReleaseVideoDeviceNV, (dpy, screen, VideoDevice)); }
int __debug_glXBindVideoImageNV(Display *dpy, GLXVideoDeviceNV VideoDevice, GLXPbuffer pbuf, int iVideoBuffer) { GTGL_DEBUG_IMPL_RETURN(int, glXBindVideoImageNV, (dpy, VideoDevice, pbuf, iVideoBuffer)); }
int __debug_glXReleaseVideoImageNV(Display *dpy, GLXPbuffer pbuf)                             { GTGL_DEBUG_IMPL_RETURN(int, glXReleaseVideoImageNV, (dpy, pbuf)); }
int __debug_glXSendPbufferToVideoNV(Display *dpy, GLXPbuffer pbuf, int iBufferType, unsigned long *pulCounterPbuffer, GLboolean bBlock) { GTGL_DEBUG_IMPL_RETURN(int, glXSendPbufferToVideoNV, (dpy, pbuf, iBufferType, pulCounterPbuffer, bBlock)); }
int __debug_glXGetVideoInfoNV(Display *dpy, int screen, GLXVideoDeviceNV VideoDevice, unsigned long *pulCounterOutputPbuffer, unsigned long *pulCounterOutputVideo) { GTGL_DEBUG_IMPL_RETURN(int, glXGetVideoInfoNV, (dpy, screen, VideoDevice, pulCounterOutputPbuffer, pulCounterOutputVideo)); }


/* GLX_NV_swap_group */
PFNGLXJOINSWAPGROUPNVPROC      glXJoinSwapGroupNV      = NULL, __native_glXJoinSwapGroupNV      = NULL;
PFNGLXBINDSWAPBARRIERNVPROC    glXBindSwapBarrierNV    = NULL, __native_glXBindSwapBarrierNV    = NULL;
PFNGLXQUERYSWAPGROUPNVPROC     glXQuerySwapGroupNV     = NULL, __native_glXQuerySwapGroupNV     = NULL;
PFNGLXQUERYMAXSWAPGROUPSNVPROC glXQueryMaxSwapGroupsNV = NULL, __native_glXQueryMaxSwapGroupsNV = NULL;
PFNGLXQUERYFRAMECOUNTNVPROC    glXQueryFrameCountNV    = NULL, __native_glXQueryFrameCountNV    = NULL;
PFNGLXRESETFRAMECOUNTNVPROC    glXResetFrameCountNV    = NULL, __native_glXResetFrameCountNV    = NULL;

Bool __debug_glXJoinSwapGroupNV(Display *dpy, GLXDrawable drawable, GLuint group)           { GTGL_DEBUG_IMPL_RETURN(Bool, glXJoinSwapGroupNV, (dpy, drawable, group)); }
Bool __debug_glXBindSwapBarrierNV(Display *dpy, GLuint group, GLuint barrier)               { GTGL_DEBUG_IMPL_RETURN(Bool, glXBindSwapBarrierNV, (dpy, group, barrier)); }
Bool __debug_glXQuerySwapGroupNV(Display *dpy, GLXDrawable drawable, GLuint *group, GLuint *barrier) { GTGL_DEBUG_IMPL_RETURN(Bool, glXQuerySwapGroupNV, (dpy, drawable, group, barrier)); }
Bool __debug_glXQueryMaxSwapGroupsNV(Display *dpy, int screen, GLuint *maxGroups, GLuint *maxBarriers) { GTGL_DEBUG_IMPL_RETURN(Bool, glXQueryMaxSwapGroupsNV, (dpy, screen, maxGroups, maxBarriers)); }
Bool __debug_glXQueryFrameCountNV(Display *dpy, int screen, GLuint *count)                  { GTGL_DEBUG_IMPL_RETURN(Bool, glXQueryFrameCountNV, (dpy, screen, count)); }
Bool __debug_glXResetFrameCountNV(Display *dpy, int screen)                                 { GTGL_DEBUG_IMPL_RETURN(Bool, glXResetFrameCountNV, (dpy, screen)); }

/* GLX_NV_video_capture */
PFNGLXBINDVIDEOCAPTUREDEVICENVPROC       glXBindVideoCaptureDeviceNV       = NULL, __native_glXBindVideoCaptureDeviceNV       = NULL;
PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC glXEnumerateVideoCaptureDevicesNV = NULL, __native_glXEnumerateVideoCaptureDevicesNV = NULL;
PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC       glXLockVideoCaptureDeviceNV       = NULL, __native_glXLockVideoCaptureDeviceNV       = NULL;
PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC      glXQueryVideoCaptureDeviceNV      = NULL, __native_glXQueryVideoCaptureDeviceNV      = NULL;
PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC    glXReleaseVideoCaptureDeviceNV    = NULL, __native_glXReleaseVideoCaptureDeviceNV    = NULL;

int __debug_glXBindVideoCaptureDeviceNV(Display *dpy, unsigned int video_capture_slot, GLXVideoCaptureDeviceNV device) { GTGL_DEBUG_IMPL_RETURN(int, glXBindVideoCaptureDeviceNV, (dpy, video_capture_slot, device)); }
GLXVideoCaptureDeviceNV * __debug_glXEnumerateVideoCaptureDevicesNV(Display *dpy, int screen, int *nelements) { GTGL_DEBUG_IMPL_RETURN(GLXVideoCaptureDeviceNV *, glXEnumerateVideoCaptureDevicesNV, (dpy, screen, nelements)); }
void __debug_glXLockVideoCaptureDeviceNV(Display *dpy, GLXVideoCaptureDeviceNV device)      { GTGL_DEBUG_IMPL(glXLockVideoCaptureDeviceNV, (dpy, device)); }
int __debug_glXQueryVideoCaptureDeviceNV(Display *dpy, GLXVideoCaptureDeviceNV device, int attribute, int *value) { GTGL_DEBUG_IMPL_RETURN(int, glXQueryVideoCaptureDeviceNV, (dpy, device, attribute, value)); }
void __debug_glXReleaseVideoCaptureDeviceNV(Display *dpy, GLXVideoCaptureDeviceNV device)   { GTGL_DEBUG_IMPL(glXReleaseVideoCaptureDeviceNV, (dpy, device)); }


/* GLX_NV_copy_image */
PFNGLXCOPYIMAGESUBDATANVPROC glXCopyImageSubDataNV = NULL, __native_glXCopyImageSubDataNV = NULL;
void __debug_glXCopyImageSubDataNV(Display *dpy, GLXContext srcCtx, GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLXContext dstCtx, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei width, GLsizei height, GLsizei depth) {
    GTGL_DEBUG_IMPL(glXCopyImageSubDataNV, (dpy, srcCtx, srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstCtx, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, width, height, depth)); }


/* GLX_SGI_swap_control */
PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = NULL, __native_glXSwapIntervalSGI = NULL;
int __debug_glXSwapIntervalSGI(int interval) { GTGL_DEBUG_IMPL_RETURN(int, glXSwapIntervalSGI, (interval)); }


/* GLX_SGI_video_sync */
PFNGLXGETVIDEOSYNCSGIPROC  glXGetVideoSyncSGI  = NULL, __native_glXGetVideoSyncSGI  = NULL;
PFNGLXWAITVIDEOSYNCSGIPROC glXWaitVideoSyncSGI = NULL, __native_glXWaitVideoSyncSGI = NULL;

int __debug_glXGetVideoSyncSGI(unsigned int *count)                                         { GTGL_DEBUG_IMPL_RETURN(int, glXGetVideoSyncSGI, (count)); }
int __debug_glXWaitVideoSyncSGI(int divisor, int remainder, unsigned int *count)            { GTGL_DEBUG_IMPL_RETURN(int, glXWaitVideoSyncSGI, (divisor, remainder, count)); }


/* GLX_SGI_make_current_read */
PFNGLXMAKECURRENTREADSGIPROC        glXMakeCurrentReadSGI        = NULL, __native_glXMakeCurrentReadSGI        = NULL;
PFNGLXGETCURRENTREADDRAWABLESGIPROC glXGetCurrentReadDrawableSGI = NULL, __native_glXGetCurrentReadDrawableSGI = NULL;

Bool __debug_glXMakeCurrentReadSGI(Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx) { GTGL_DEBUG_IMPL_RETURN(Bool, glXMakeCurrentReadSGI, (dpy, draw, read, ctx)); }
GLXDrawable __debug_glXGetCurrentReadDrawableSGI(void) { GTGL_DEBUG_IMPL_RETURN(GLXDrawable, glXGetCurrentReadDrawableSGI, ()); }


/* GLX_SGIX_fbconfig */
PFNGLXGETFBCONFIGATTRIBSGIXPROC         glXGetFBConfigAttribSGIX         = NULL, __native_glXGetFBConfigAttribSGIX         = NULL;
PFNGLXCHOOSEFBCONFIGSGIXPROC            glXChooseFBConfigSGIX            = NULL, __native_glXChooseFBConfigSGIX            = NULL;
PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC glXCreateGLXPixmapWithConfigSGIX = NULL, __native_glXCreateGLXPixmapWithConfigSGIX = NULL;
PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC   glXCreateContextWithConfigSGIX   = NULL, __native_glXCreateContextWithConfigSGIX   = NULL;
PFNGLXGETVISUALFROMFBCONFIGSGIXPROC     glXGetVisualFromFBConfigSGIX     = NULL, __native_glXGetVisualFromFBConfigSGIX     = NULL;
PFNGLXGETFBCONFIGFROMVISUALSGIXPROC     glXGetFBConfigFromVisualSGIX     = NULL, __native_glXGetFBConfigFromVisualSGIX     = NULL;

int __debug_glXGetFBConfigAttribSGIX(Display *dpy, GLXFBConfigSGIX config, int attribute, int *value) { GTGL_DEBUG_IMPL_RETURN(int, glXGetFBConfigAttribSGIX, (dpy, config, attribute, value)); }
GLXFBConfigSGIX * __debug_glXChooseFBConfigSGIX(Display *dpy, int screen, int *attrib_list, int *nelements) { GTGL_DEBUG_IMPL_RETURN(GLXFBConfigSGIX *, glXChooseFBConfigSGIX, (dpy, screen, attrib_list, nelements)); }
GLXPixmap __debug_glXCreateGLXPixmapWithConfigSGIX(Display *dpy, GLXFBConfigSGIX config, Pixmap pixmap) { GTGL_DEBUG_IMPL_RETURN(GLXPixmap, glXCreateGLXPixmapWithConfigSGIX, (dpy, config, pixmap)); }
GLXContext __debug_glXCreateContextWithConfigSGIX(Display *dpy, GLXFBConfigSGIX config, int render_type, GLXContext share_list, Bool direct) { GTGL_DEBUG_IMPL_RETURN(GLXContext, glXCreateContextWithConfigSGIX, (dpy, config, render_type, share_list, direct)); }
XVisualInfo * __debug_glXGetVisualFromFBConfigSGIX(Display *dpy, GLXFBConfigSGIX config)    { GTGL_DEBUG_IMPL_RETURN(XVisualInfo *, glXGetVisualFromFBConfigSGIX, (dpy, config)); }
GLXFBConfigSGIX __debug_glXGetFBConfigFromVisualSGIX(Display *dpy, XVisualInfo *vis)        { GTGL_DEBUG_IMPL_RETURN(GLXFBConfigSGIX, glXGetFBConfigFromVisualSGIX, (dpy, vis)); }

/* GLX_SGIX_pbuffer */
PFNGLXCREATEGLXPBUFFERSGIXPROC  glXCreateGLXPbufferSGIX  = NULL, __native_glXCreateGLXPbufferSGIX  = NULL;
PFNGLXDESTROYGLXPBUFFERSGIXPROC glXDestroyGLXPbufferSGIX = NULL, __native_glXDestroyGLXPbufferSGIX = NULL;
PFNGLXQUERYGLXPBUFFERSGIXPROC   glXQueryGLXPbufferSGIX   = NULL, __native_glXQueryGLXPbufferSGIX   = NULL;
PFNGLXSELECTEVENTSGIXPROC       glXSelectEventSGIX       = NULL, __native_glXSelectEventSGIX       = NULL;
PFNGLXGETSELECTEDEVENTSGIXPROC  glXGetSelectedEventSGIX  = NULL, __native_glXGetSelectedEventSGIX  = NULL;

GLXPbufferSGIX __debug_glXCreateGLXPbufferSGIX(Display *dpy, GLXFBConfigSGIX config, unsigned int width, unsigned int height, int *attrib_list) { GTGL_DEBUG_IMPL_RETURN(GLXPbufferSGIX, glXCreateGLXPbufferSGIX, (dpy, config, width, height, attrib_list)); }
void __debug_glXDestroyGLXPbufferSGIX(Display *dpy, GLXPbufferSGIX pbuf)                    { GTGL_DEBUG_IMPL(glXDestroyGLXPbufferSGIX, (dpy, pbuf)); }
int __debug_glXQueryGLXPbufferSGIX(Display *dpy, GLXPbufferSGIX pbuf, int attribute, unsigned int *value) { GTGL_DEBUG_IMPL_RETURN(int, glXQueryGLXPbufferSGIX, (dpy, pbuf, attribute, value)); }
void __debug_glXSelectEventSGIX(Display *dpy, GLXDrawable drawable, unsigned long mask)     { GTGL_DEBUG_IMPL(glXSelectEventSGIX, (dpy, drawable, mask)); }
void __debug_glXGetSelectedEventSGIX(Display *dpy, GLXDrawable drawable, unsigned long *mask) { GTGL_DEBUG_IMPL(glXGetSelectedEventSGIX, (dpy, drawable, mask)); }

/* GLX_SGI_cushion */
PFNGLXCUSHIONSGIPROC glXCushionSGI = NULL, __native_glXCushionSGI = NULL;
void __debug_glXCushionSGI(Display *dpy, Window window, float cushion) { GTGL_DEBUG_IMPL(glXCushionSGI, (dpy, window, cushion)); }

/* GLX_SGIX_video_resize */
PFNGLXBINDCHANNELTOWINDOWSGIXPROC glXBindChannelToWindowSGIX = NULL, __native_glXBindChannelToWindowSGIX = NULL;
PFNGLXCHANNELRECTSGIXPROC         glXChannelRectSGIX         = NULL, __native_glXChannelRectSGIX         = NULL;
PFNGLXQUERYCHANNELRECTSGIXPROC    glXQueryChannelRectSGIX    = NULL, __native_glXQueryChannelRectSGIX    = NULL;
PFNGLXQUERYCHANNELDELTASSGIXPROC  glXQueryChannelDeltasSGIX  = NULL, __native_glXQueryChannelDeltasSGIX  = NULL;
PFNGLXCHANNELRECTSYNCSGIXPROC     glXChannelRectSyncSGIX     = NULL, __native_glXChannelRectSyncSGIX     = NULL;

int __debug_glXBindChannelToWindowSGIX(Display *display, int screen, int channel, Window window) { GTGL_DEBUG_IMPL_RETURN(int, glXBindChannelToWindowSGIX, (display, screen, channel, window)); }
int __debug_glXChannelRectSGIX(Display *display, int screen, int channel, int x, int y, int w, int h) { GTGL_DEBUG_IMPL_RETURN(int, glXChannelRectSGIX, (display, screen, channel, x, y, w, h)); }
int __debug_glXQueryChannelRectSGIX(Display *display, int screen, int channel, int *dx, int *dy, int *dw, int *dh) { GTGL_DEBUG_IMPL_RETURN(int, glXQueryChannelRectSGIX, (display, screen, channel, dx, dy, dw, dh)); }
int __debug_glXQueryChannelDeltasSGIX(Display *display, int screen, int channel, int *x, int *y, int *w, int *h) { GTGL_DEBUG_IMPL_RETURN(int, glXQueryChannelDeltasSGIX, (display, screen, channel, x, y, w, h)); }
int __debug_glXChannelRectSyncSGIX(Display *display, int screen, int channel, GLenum synctype) { GTGL_DEBUG_IMPL_RETURN(int, glXChannelRectSyncSGIX, (display, screen, channel, synctype)); }

/* GLX_SGIX_swap_group */
PFNGLXJOINSWAPGROUPSGIXPROC glXJoinSwapGroupSGIX = NULL, __native_glXJoinSwapGroupSGIX = NULL;
void __debug_glXJoinSwapGroupSGIX(Display *dpy, GLXDrawable drawable, GLXDrawable member) { GTGL_DEBUG_IMPL(glXJoinSwapGroupSGIX, (dpy, drawable, member)); }

/* GLX_SGIX_swap_barrier */
PFNGLXBINDSWAPBARRIERSGIXPROC      glXBindSwapBarrierSGIX      = NULL, __native_glXBindSwapBarrierSGIX      = NULL;
PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC glXQueryMaxSwapBarriersSGIX = NULL, __native_glXQueryMaxSwapBarriersSGIX = NULL;

void __debug_glXBindSwapBarrierSGIX(Display *dpy, GLXDrawable drawable, int barrier) { GTGL_DEBUG_IMPL(glXBindSwapBarrierSGIX, (dpy, drawable, barrier)); }
Bool __debug_glXQueryMaxSwapBarriersSGIX(Display *dpy, int screen, int *max)         { GTGL_DEBUG_IMPL_RETURN(Bool, glXQueryMaxSwapBarriersSGIX, (dpy, screen, max)); }

/* GLX_SGIX_hyperpipe */
PFNGLXQUERYHYPERPIPENETWORKSGIXPROC    glXQueryHyperpipeNetworkSGIX    = NULL, __native_glXQueryHyperpipeNetworkSGIX    = NULL;
PFNGLXHYPERPIPECONFIGSGIXPROC          glXHyperpipeConfigSGIX          = NULL, __native_glXHyperpipeConfigSGIX          = NULL;
PFNGLXQUERYHYPERPIPECONFIGSGIXPROC     glXQueryHyperpipeConfigSGIX     = NULL, __native_glXQueryHyperpipeConfigSGIX     = NULL;
PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC   glXDestroyHyperpipeConfigSGIX   = NULL, __native_glXDestroyHyperpipeConfigSGIX   = NULL;
PFNGLXBINDHYPERPIPESGIXPROC            glXBindHyperpipeSGIX            = NULL, __native_glXBindHyperpipeSGIX            = NULL;
PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC glXQueryHyperpipeBestAttribSGIX = NULL, __native_glXQueryHyperpipeBestAttribSGIX = NULL;
PFNGLXHYPERPIPEATTRIBSGIXPROC          glXHyperpipeAttribSGIX          = NULL, __native_glXHyperpipeAttribSGIX          = NULL;
PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC     glXQueryHyperpipeAttribSGIX     = NULL, __native_glXQueryHyperpipeAttribSGIX     = NULL;

GLXHyperpipeNetworkSGIX * __debug_glXQueryHyperpipeNetworkSGIX(Display *dpy, int *npipes)   { GTGL_DEBUG_IMPL_RETURN(GLXHyperpipeNetworkSGIX *, glXQueryHyperpipeNetworkSGIX, (dpy, npipes)); }
int __debug_glXHyperpipeConfigSGIX(Display *dpy, int networkId, int npipes, GLXHyperpipeConfigSGIX *cfg, int *hpId) { GTGL_DEBUG_IMPL_RETURN(int, glXHyperpipeConfigSGIX, (dpy, networkId, npipes, cfg, hpId)); }
GLXHyperpipeConfigSGIX * __debug_glXQueryHyperpipeConfigSGIX(Display *dpy, int hpId, int *npipes) { GTGL_DEBUG_IMPL_RETURN(GLXHyperpipeConfigSGIX *, glXQueryHyperpipeConfigSGIX, (dpy, hpId, npipes)); }
int __debug_glXDestroyHyperpipeConfigSGIX(Display *dpy, int hpId)                           { GTGL_DEBUG_IMPL_RETURN(int, glXDestroyHyperpipeConfigSGIX, (dpy, hpId)); }
int __debug_glXBindHyperpipeSGIX(Display *dpy, int hpId)                                    { GTGL_DEBUG_IMPL_RETURN(int, glXBindHyperpipeSGIX, (dpy, hpId)); }
int __debug_glXQueryHyperpipeBestAttribSGIX(Display *dpy, int timeSlice, int attrib, int size, void *attribList, void *returnAttribList) { GTGL_DEBUG_IMPL_RETURN(int, glXQueryHyperpipeBestAttribSGIX, (dpy, timeSlice, attrib, size, attribList, returnAttribList)); }
int __debug_glXHyperpipeAttribSGIX(Display *dpy, int timeSlice, int attrib, int size, void *attribList) { GTGL_DEBUG_IMPL_RETURN(int, glXHyperpipeAttribSGIX, (dpy, timeSlice, attrib, size, attribList)); }
int __debug_glXQueryHyperpipeAttribSGIX(Display *dpy, int timeSlice, int attrib, int size, void *returnAttribList) { GTGL_DEBUG_IMPL_RETURN(int, glXQueryHyperpipeAttribSGIX, (dpy, timeSlice, attrib, size, returnAttribList)); }

/* GLX_MESA_copy_sub_buffer */
PFNGLXCOPYSUBBUFFERMESAPROC glXCopySubBufferMESA = NULL, __native_glXCopySubBufferMESA = NULL;
void __debug_glXCopySubBufferMESA(Display *dpy, GLXDrawable drawable, int x, int y, int width, int height) { GTGL_DEBUG_IMPL(glXCopySubBufferMESA, (dpy, drawable, x, y, width, height)); }

/* GLX_MESA_pixmap_colormap */
PFNGLXCREATEGLXPIXMAPMESAPROC glXCreateGLXPixmapMESA = NULL, __native_glXCreateGLXPixmapMESA = NULL;
GLXPixmap __debug_glXCreateGLXPixmapMESA(Display *dpy, XVisualInfo *visual, Pixmap pixmap, Colormap cmap) { GTGL_DEBUG_IMPL_RETURN(GLXPixmap, glXCreateGLXPixmapMESA, (dpy, visual, pixmap, cmap)); }

/* GLX_MESA_release_buffers */
PFNGLXRELEASEBUFFERSMESAPROC glXReleaseBuffersMESA = NULL, __native_glXReleaseBuffersMESA = NULL;
Bool __debug_glXReleaseBuffersMESA(Display *dpy, GLXDrawable drawable) { GTGL_DEBUG_IMPL_RETURN(Bool, glXReleaseBuffersMESA, (dpy, drawable)); }

/* GLX_MESA_set_3dfx_mode */
PFNGLXSET3DFXMODEMESAPROC glXSet3DfxModeMESA = NULL, __native_glXSet3DfxModeMESA = NULL;
Bool __debug_glXSet3DfxModeMESA(int mode) { GTGL_DEBUG_IMPL_RETURN(Bool, glXSet3DfxModeMESA, (mode)); }

/*GLX_MESA_agp_offset */
PFNGLXGETAGPOFFSETMESAPROC glXGetAGPOffsetMESA = NULL, __native_glXGetAGPOffsetMESA = NULL;
unsigned int __debug_glXGetAGPOffsetMESA(const void *pointer) { GTGL_DEBUG_IMPL_RETURN(unsigned int, glXGetAGPOffsetMESA, (pointer)); }


/* GLX_SUN_get_transparent_index */
PFNGLXGETTRANSPARENTINDEXSUNPROC glXGetTransparentIndexSUN = NULL, __native_glXGetTransparentIndexSUN = NULL;
Status __debug_glXGetTransparentIndexSUN(Display *dpy, Window overlay, Window underlay, long *pTransparentIndex) { GTGL_DEBUG_IMPL_RETURN(Status, glXGetTransparentIndexSUN, (dpy, overlay, underlay, pTransparentIndex)); }

/* GLX_OML_sync_control */
PFNGLXGETSYNCVALUESOMLPROC  glXGetSyncValuesOML  = NULL, __native_glXGetSyncValuesOML  = NULL;
PFNGLXGETMSCRATEOMLPROC     glXGetMscRateOML     = NULL, __native_glXGetMscRateOML     = NULL;
PFNGLXSWAPBUFFERSMSCOMLPROC glXSwapBuffersMscOML = NULL, __native_glXSwapBuffersMscOML = NULL;
PFNGLXWAITFORMSCOMLPROC     glXWaitForMscOML     = NULL, __native_glXWaitForMscOML     = NULL;
PFNGLXWAITFORSBCOMLPROC     glXWaitForSbcOML     = NULL, __native_glXWaitForSbcOML     = NULL;

Bool __debug_glXGetSyncValuesOML(Display *dpy, GLXDrawable drawable, int64_t *ust, int64_t *msc, int64_t *sbc) { GTGL_DEBUG_IMPL_RETURN(Bool, glXGetSyncValuesOML, (dpy, drawable, ust, msc, sbc)); }
Bool __debug_glXGetMscRateOML(Display *dpy, GLXDrawable drawable, int32_t *numerator, int32_t *denominator) { GTGL_DEBUG_IMPL_RETURN(Bool, glXGetMscRateOML, (dpy, drawable, numerator, denominator)); }
int64_t __debug_glXSwapBuffersMscOML(Display *dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder) { GTGL_DEBUG_IMPL_RETURN(int64_t, glXSwapBuffersMscOML, (dpy, drawable, target_msc, divisor, remainder)); }
Bool __debug_glXWaitForMscOML(Display *dpy, GLXDrawable drawable, int64_t target_msc, int64_t divisor, int64_t remainder, int64_t *ust, int64_t *msc, int64_t *sbc) { GTGL_DEBUG_IMPL_RETURN(Bool, glXWaitForMscOML, (dpy, drawable, target_msc, divisor, remainder, ust, msc, sbc)); }
Bool __debug_glXWaitForSbcOML(Display *dpy, GLXDrawable drawable, int64_t target_sbc, int64_t *ust, int64_t *msc, int64_t *sbc) { GTGL_DEBUG_IMPL_RETURN(Bool, glXWaitForSbcOML, (dpy, drawable, target_sbc, ust, msc, sbc)); }


void gtglInitGLAPI_GLX(GLboolean debugMode)
{
    /* ARB Extensions */
    if (gtglIsExtensionSupported("GLX_ARB_multisample"))
    {
        GTGL_GLX_ARB_multisample = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_ARB_fbconfig_float"))
    {
        GTGL_GLX_ARB_fbconfig_float = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_ARB_framebuffer_sRGB"))
    {
        GTGL_GLX_ARB_framebuffer_sRGB = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_ARB_create_context"))
    {
        GTGL_GLX_ARB_create_context = GL_TRUE;
        glXCreateContextAttribsARB = __native_glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)gtglGetProcAddress("glXCreateContextAttribsARB");

        if (debugMode)
        {
            glXCreateContextAttribsARB = __debug_glXCreateContextAttribsARB;
        }
    }

    if (gtglIsExtensionSupported("GLX_ARB_create_context_profile"))
    {
        GTGL_GLX_ARB_create_context_profile = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_ARB_create_context_robustness"))
    {
        GTGL_GLX_ARB_create_context_robustness = GL_TRUE;
    }




    /* EXT Extensions */
    if (gtglIsExtensionSupported("GLX_EXT_visual_info"))
    {
        GTGL_GLX_EXT_visual_info = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_EXT_visual_rating"))
    {
        GTGL_GLX_EXT_visual_rating = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_EXT_import_context"))
    {
        GTGL_GLX_EXT_import_context = GL_TRUE;
        glXGetCurrentDisplayEXT = __native_glXGetCurrentDisplayEXT = (PFNGLXGETCURRENTDISPLAYEXTPROC)gtglGetProcAddress("glXGetCurrentDisplayEXT");
        glXQueryContextInfoEXT  = __native_glXQueryContextInfoEXT  = (PFNGLXQUERYCONTEXTINFOEXTPROC)gtglGetProcAddress("glXQueryContextInfoEXT");
        glXGetContextIDEXT      = __native_glXGetContextIDEXT      = (PFNGLXGETCONTEXTIDEXTPROC)gtglGetProcAddress("glXGetContextIDEXT");
        glXImportContextEXT     = __native_glXImportContextEXT     = (PFNGLXIMPORTCONTEXTEXTPROC)gtglGetProcAddress("glXImportContextEXT");
        glXFreeContextEXT       = __native_glXFreeContextEXT       = (PFNGLXFREECONTEXTEXTPROC)gtglGetProcAddress("glXFreeContextEXT");

        if (debugMode)
        {
            glXGetCurrentDisplayEXT = __debug_glXGetCurrentDisplayEXT;
            glXQueryContextInfoEXT  = __debug_glXQueryContextInfoEXT;
            glXGetContextIDEXT      = __debug_glXGetContextIDEXT;
            glXImportContextEXT     = __debug_glXImportContextEXT;
            glXFreeContextEXT       = __debug_glXFreeContextEXT;
        }
    }

    if (gtglIsExtensionSupported("GLX_EXT_fbconfig_packed_float"))
    {
        GTGL_GLX_EXT_fbconfig_packed_float = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_EXT_framebuffer_sRGB"))
    {
        GTGL_GLX_EXT_framebuffer_sRGB = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_EXT_texture_from_pixmap"))
    {
        GTGL_GLX_EXT_texture_from_pixmap = GL_TRUE;
        glXBindTexImageEXT    = __native_glXBindTexImageEXT    = (PFNGLXBINDTEXIMAGEEXTPROC)gtglGetProcAddress("glXBindTexImageEXT");
        glXReleaseTexImageEXT = __native_glXReleaseTexImageEXT = (PFNGLXRELEASETEXIMAGEEXTPROC)gtglGetProcAddress("glXReleaseTexImageEXT");

        if (debugMode)
        {
            glXBindTexImageEXT    = __debug_glXBindTexImageEXT;
            glXReleaseTexImageEXT = __debug_glXReleaseTexImageEXT;
        }
    }

    if (gtglIsExtensionSupported("GLX_EXT_swap_control"))
    {
        GTGL_GLX_EXT_swap_control = GL_TRUE;
        glXSwapIntervalEXT = __native_glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)gtglGetProcAddress("glXSwapIntervalEXT");

        if (debugMode)
        {
            glXSwapIntervalEXT = __debug_glXSwapIntervalEXT;
        }
    }

    if (gtglIsExtensionSupported("GLX_EXT_create_context_es2_profile"))
    {
        GTGL_GLX_EXT_create_context_es2_profile = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_EXT_swap_control_tear"))
    {
        GTGL_GLX_EXT_swap_control_tear = GL_TRUE;
    }


    /* NV Extensions */
    if (gtglIsExtensionSupported("GLX_NV_float_buffer"))
    {
        GTGL_GLX_NV_float_buffer = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_NV_present_video"))
    {
        GTGL_GLX_NV_present_video = GL_TRUE;
        glXEnumerateVideoDevicesNV = __native_glXEnumerateVideoDevicesNV = (PFNGLXENUMERATEVIDEODEVICESNVPROC)gtglGetProcAddress("glXEnumerateVideoDevicesNV");
        glXBindVideoDeviceNV       = __native_glXBindVideoDeviceNV       = (PFNGLXBINDVIDEODEVICENVPROC)gtglGetProcAddress("glXBindVideoDeviceNV");

        if (debugMode)
        {
            glXEnumerateVideoDevicesNV = __debug_glXEnumerateVideoDevicesNV;
            glXBindVideoDeviceNV       = __debug_glXBindVideoDeviceNV;
        }
    }

    if (gtglIsExtensionSupported("GLX_NV_video_output"))
    {
        GTGL_GLX_NV_video_output = GL_TRUE;
        glXGetVideoDeviceNV     = __native_glXGetVideoDeviceNV     = (PFNGLXGETVIDEODEVICENVPROC)gtglGetProcAddress("glXGetVideoDeviceNV");
        glXReleaseVideoDeviceNV = __native_glXReleaseVideoDeviceNV = (PFNGLXRELEASEVIDEODEVICENVPROC)gtglGetProcAddress("glXReleaseVideoDeviceNV");
        glXBindVideoImageNV     = __native_glXBindVideoImageNV     = (PFNGLXBINDVIDEOIMAGENVPROC)gtglGetProcAddress("glXBindVideoImageNV");
        glXReleaseVideoImageNV  = __native_glXReleaseVideoImageNV  = (PFNGLXRELEASEVIDEOIMAGENVPROC)gtglGetProcAddress("glXReleaseVideoImageNV");
        glXSendPbufferToVideoNV = __native_glXSendPbufferToVideoNV = (PFNGLXSENDPBUFFERTOVIDEONVPROC)gtglGetProcAddress("glXSendPbufferToVideoNV");
        glXGetVideoInfoNV       = __native_glXGetVideoInfoNV       = (PFNGLXGETVIDEOINFONVPROC)gtglGetProcAddress("glXGetVideoInfoNV");

        if (debugMode)
        {
            glXGetVideoDeviceNV     = __debug_glXGetVideoDeviceNV;
            glXReleaseVideoDeviceNV = __debug_glXReleaseVideoDeviceNV;
            glXBindVideoImageNV     = __debug_glXBindVideoImageNV;
            glXReleaseVideoImageNV  = __debug_glXReleaseVideoImageNV;
            glXSendPbufferToVideoNV = __debug_glXSendPbufferToVideoNV;
            glXGetVideoInfoNV       = __debug_glXGetVideoInfoNV;
        }
    }

    if (gtglIsExtensionSupported("GLX_NV_swap_group"))
    {
        GTGL_GLX_NV_swap_group = GL_FALSE;
        glXJoinSwapGroupNV      = __native_glXJoinSwapGroupNV      = (PFNGLXJOINSWAPGROUPNVPROC)gtglGetProcAddress("glXJoinSwapGroupNV");
        glXBindSwapBarrierNV    = __native_glXBindSwapBarrierNV    = (PFNGLXBINDSWAPBARRIERNVPROC)gtglGetProcAddress("glXBindSwapBarrierNV");
        glXQuerySwapGroupNV     = __native_glXQuerySwapGroupNV     = (PFNGLXQUERYSWAPGROUPNVPROC)gtglGetProcAddress("glXQuerySwapGroupNV");
        glXQueryMaxSwapGroupsNV = __native_glXQueryMaxSwapGroupsNV = (PFNGLXQUERYMAXSWAPGROUPSNVPROC)gtglGetProcAddress("glXQueryMaxSwapGroupsNV");
        glXQueryFrameCountNV    = __native_glXQueryFrameCountNV    = (PFNGLXQUERYFRAMECOUNTNVPROC)gtglGetProcAddress("glXQueryFrameCountNV");
        glXResetFrameCountNV    = __native_glXResetFrameCountNV    = (PFNGLXRESETFRAMECOUNTNVPROC)gtglGetProcAddress("glXResetFrameCountNV");

        if (debugMode)
        {
            glXJoinSwapGroupNV      = __debug_glXJoinSwapGroupNV;
            glXBindSwapBarrierNV    = __debug_glXBindSwapBarrierNV;
            glXQuerySwapGroupNV     = __debug_glXQuerySwapGroupNV;
            glXQueryMaxSwapGroupsNV = __debug_glXQueryMaxSwapGroupsNV;
            glXQueryFrameCountNV    = __debug_glXQueryFrameCountNV;
            glXResetFrameCountNV    = __debug_glXResetFrameCountNV;
        }
    }

    if (gtglIsExtensionSupported("GLX_NV_video_capture"))
    {
        GTGL_GLX_NV_video_capture = GL_TRUE;
        glXBindVideoCaptureDeviceNV       = __native_glXBindVideoCaptureDeviceNV       = (PFNGLXBINDVIDEOCAPTUREDEVICENVPROC)gtglGetProcAddress("glXBindVideoCaptureDeviceNV");
        glXEnumerateVideoCaptureDevicesNV = __native_glXEnumerateVideoCaptureDevicesNV = (PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC)gtglGetProcAddress("glXEnumerateVideoCaptureDevicesNV");
        glXLockVideoCaptureDeviceNV       = __native_glXLockVideoCaptureDeviceNV       = (PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC)gtglGetProcAddress("glXLockVideoCaptureDeviceNV");
        glXQueryVideoCaptureDeviceNV      = __native_glXQueryVideoCaptureDeviceNV      = (PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC)gtglGetProcAddress("glXQueryVideoCaptureDeviceNV");
        glXReleaseVideoCaptureDeviceNV    = __native_glXReleaseVideoCaptureDeviceNV    = (PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC)gtglGetProcAddress("glXReleaseVideoCaptureDeviceNV");

        if (debugMode)
        {
            glXBindVideoCaptureDeviceNV       = __debug_glXBindVideoCaptureDeviceNV;
            glXEnumerateVideoCaptureDevicesNV = __debug_glXEnumerateVideoCaptureDevicesNV;
            glXLockVideoCaptureDeviceNV       = __debug_glXLockVideoCaptureDeviceNV;
            glXQueryVideoCaptureDeviceNV      = __debug_glXQueryVideoCaptureDeviceNV;
            glXReleaseVideoCaptureDeviceNV    = __debug_glXReleaseVideoCaptureDeviceNV;
        }
    }

    if (gtglIsExtensionSupported("GLX_NV_copy_image"))
    {
        GTGL_GLX_NV_copy_image = GL_TRUE;
        glXCopyImageSubDataNV = __native_glXCopyImageSubDataNV = (PFNGLXCOPYIMAGESUBDATANVPROC)gtglGetProcAddress("glXCopyImageSubDataNV");

        if (debugMode)
        {
            glXCopyImageSubDataNV = __debug_glXCopyImageSubDataNV;
        }
    }

    if (gtglIsExtensionSupported("GLX_NV_multisample_coverage"))
    {
        GTGL_GLX_NV_multisample_coverage = GL_TRUE;
    }


    /* SGI Extensions */
    if (gtglIsExtensionSupported("GLX_SGIS_multisample"))
    {
        GTGL_GLX_SGIS_multisample = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_SGI_swap_control"))
    {
        GTGL_GLX_SGI_swap_control = GL_TRUE;
        glXSwapIntervalSGI = __native_glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)gtglGetProcAddress("glXSwapIntervalSGI");

        if (debugMode)
        {
            glXSwapIntervalSGI = __debug_glXSwapIntervalSGI;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGI_video_sync"))
    {
        GTGL_GLX_SGI_video_sync = GL_TRUE;
        glXGetVideoSyncSGI  = __native_glXGetVideoSyncSGI  = (PFNGLXGETVIDEOSYNCSGIPROC)gtglGetProcAddress("glXGetVideoSyncSGI");
        glXWaitVideoSyncSGI = __native_glXWaitVideoSyncSGI = (PFNGLXWAITVIDEOSYNCSGIPROC)gtglGetProcAddress("glXWaitVideoSyncSGI");

        if (debugMode)
        {
            glXGetVideoSyncSGI  = __debug_glXGetVideoSyncSGI;
            glXWaitVideoSyncSGI = __debug_glXWaitVideoSyncSGI;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGI_make_current_read"))
    {
        GTGL_GLX_SGI_make_current_read = GL_TRUE;
        glXMakeCurrentReadSGI        = __native_glXMakeCurrentReadSGI        = (PFNGLXMAKECURRENTREADSGIPROC)gtglGetProcAddress("glXMakeCurrentReadSGI");
        glXGetCurrentReadDrawableSGI = __native_glXGetCurrentReadDrawableSGI = (PFNGLXGETCURRENTREADDRAWABLESGIPROC)gtglGetProcAddress("glXGetCurrentReadDrawableSGI");

        if (debugMode)
        {
            glXMakeCurrentReadSGI        = __debug_glXMakeCurrentReadSGI;
            glXGetCurrentReadDrawableSGI = __debug_glXGetCurrentReadDrawableSGI;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGIX_fbconfig"))
    {
        GTGL_GLX_SGIX_fbconfig = GL_TRUE;
        glXGetFBConfigAttribSGIX         = __native_glXGetFBConfigAttribSGIX         = (PFNGLXGETFBCONFIGATTRIBSGIXPROC)gtglGetProcAddress("glXGetFBConfigAttribSGIX");
        glXChooseFBConfigSGIX            = __native_glXChooseFBConfigSGIX            = (PFNGLXCHOOSEFBCONFIGSGIXPROC)gtglGetProcAddress("glXChooseFBConfigSGIX");
        glXCreateGLXPixmapWithConfigSGIX = __native_glXCreateGLXPixmapWithConfigSGIX = (PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC)gtglGetProcAddress("glXCreateGLXPixmapWithConfigSGIX");
        glXCreateContextWithConfigSGIX   = __native_glXCreateContextWithConfigSGIX   = (PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC)gtglGetProcAddress("glXCreateContextWithConfigSGIX");
        glXGetVisualFromFBConfigSGIX     = __native_glXGetVisualFromFBConfigSGIX     = (PFNGLXGETVISUALFROMFBCONFIGSGIXPROC)gtglGetProcAddress("glXGetVisualFromFBConfigSGIX");
        glXGetFBConfigFromVisualSGIX     = __native_glXGetFBConfigFromVisualSGIX     = (PFNGLXGETFBCONFIGFROMVISUALSGIXPROC)gtglGetProcAddress("glXGetFBConfigFromVisualSGIX");

        if (debugMode)
        {
            glXGetFBConfigAttribSGIX         = __debug_glXGetFBConfigAttribSGIX;
            glXChooseFBConfigSGIX            = __debug_glXChooseFBConfigSGIX;
            glXCreateGLXPixmapWithConfigSGIX = __debug_glXCreateGLXPixmapWithConfigSGIX;
            glXCreateContextWithConfigSGIX   = __debug_glXCreateContextWithConfigSGIX;
            glXGetVisualFromFBConfigSGIX     = __debug_glXGetVisualFromFBConfigSGIX;
            glXGetFBConfigFromVisualSGIX     = __debug_glXGetFBConfigFromVisualSGIX;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGIX_pbuffer"))
    {
        GTGL_GLX_SGIX_pbuffer = GL_TRUE;
        glXCreateGLXPbufferSGIX  = __native_glXCreateGLXPbufferSGIX  = (PFNGLXCREATEGLXPBUFFERSGIXPROC)gtglGetProcAddress("glXCreateGLXPbufferSGIX");
        glXDestroyGLXPbufferSGIX = __native_glXDestroyGLXPbufferSGIX = (PFNGLXDESTROYGLXPBUFFERSGIXPROC)gtglGetProcAddress("glXDestroyGLXPbufferSGIX");
        glXQueryGLXPbufferSGIX   = __native_glXQueryGLXPbufferSGIX   = (PFNGLXQUERYGLXPBUFFERSGIXPROC)gtglGetProcAddress("glXQueryGLXPbufferSGIX");
        glXSelectEventSGIX       = __native_glXSelectEventSGIX       = (PFNGLXSELECTEVENTSGIXPROC)gtglGetProcAddress("glXSelectEventSGIX");
        glXGetSelectedEventSGIX  = __native_glXGetSelectedEventSGIX  = (PFNGLXGETSELECTEDEVENTSGIXPROC)gtglGetProcAddress("glXGetSelectedEventSGIX");

        if (debugMode)
        {
            glXCreateGLXPbufferSGIX  = __debug_glXCreateGLXPbufferSGIX;
            glXDestroyGLXPbufferSGIX = __debug_glXDestroyGLXPbufferSGIX;
            glXQueryGLXPbufferSGIX   = __debug_glXQueryGLXPbufferSGIX;
            glXSelectEventSGIX       = __debug_glXSelectEventSGIX;
            glXGetSelectedEventSGIX  = __debug_glXGetSelectedEventSGIX;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGI_cushion"))
    {
        GTGL_GLX_SGI_cushion = GL_TRUE;
        glXCushionSGI = __native_glXCushionSGI = (PFNGLXCUSHIONSGIPROC)gtglGetProcAddress("glXCushionSGI");

        if (debugMode)
        {
            glXCushionSGI = __debug_glXCushionSGI;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGIX_video_resize"))
    {
        GTGL_GLX_SGIX_video_resize = GL_TRUE;
        glXBindChannelToWindowSGIX = __native_glXBindChannelToWindowSGIX = (PFNGLXBINDCHANNELTOWINDOWSGIXPROC)gtglGetProcAddress("glXBindChannelToWindowSGIX");
        glXChannelRectSGIX         = __native_glXChannelRectSGIX         = (PFNGLXCHANNELRECTSGIXPROC)gtglGetProcAddress("glXChannelRectSGIX");
        glXQueryChannelRectSGIX    = __native_glXQueryChannelRectSGIX    = (PFNGLXQUERYCHANNELRECTSGIXPROC)gtglGetProcAddress("glXQueryChannelRectSGIX");
        glXQueryChannelDeltasSGIX  = __native_glXQueryChannelDeltasSGIX  = (PFNGLXQUERYCHANNELDELTASSGIXPROC)gtglGetProcAddress("glXQueryChannelDeltasSGIX");
        glXChannelRectSyncSGIX     = __native_glXChannelRectSyncSGIX     = (PFNGLXCHANNELRECTSYNCSGIXPROC)gtglGetProcAddress("glXChannelRectSyncSGIX");

        if (debugMode)
        {
            glXBindChannelToWindowSGIX = __debug_glXBindChannelToWindowSGIX;
            glXChannelRectSGIX         = __debug_glXChannelRectSGIX;
            glXQueryChannelRectSGIX    = __debug_glXQueryChannelRectSGIX;
            glXQueryChannelDeltasSGIX  = __debug_glXQueryChannelDeltasSGIX;
            glXChannelRectSyncSGIX     = __debug_glXChannelRectSyncSGIX;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGIX_swap_group"))
    {
        GTGL_GLX_SGIX_swap_group = GL_TRUE;
        glXJoinSwapGroupSGIX = __native_glXJoinSwapGroupSGIX = (PFNGLXJOINSWAPGROUPSGIXPROC)gtglGetProcAddress("glXJoinSwapGroupSGIX");

        if (debugMode)
        {
            glXJoinSwapGroupSGIX = __debug_glXJoinSwapGroupSGIX;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGIX_swap_barrier"))
    {
        GTGL_GLX_SGIX_swap_barrier = GL_TRUE;
        glXBindSwapBarrierSGIX      = __native_glXBindSwapBarrierSGIX      = (PFNGLXBINDSWAPBARRIERSGIXPROC)gtglGetProcAddress("glXBindSwapBarrierSGIX");
        glXQueryMaxSwapBarriersSGIX = __native_glXQueryMaxSwapBarriersSGIX = (PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC)gtglGetProcAddress("glXQueryMaxSwapBarriersSGIX");

        if (debugMode)
        {
            glXBindSwapBarrierSGIX      = __debug_glXBindSwapBarrierSGIX;
            glXQueryMaxSwapBarriersSGIX = __debug_glXQueryMaxSwapBarriersSGIX;
        }
    }

    if (gtglIsExtensionSupported("GLX_SGIX_visual_select_group"))
    {
        GTGL_GLX_SGIX_visual_select_group = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_SGIX_hyperpipe"))
    {
        GTGL_GLX_SGIX_hyperpipe = GL_TRUE;
        glXQueryHyperpipeNetworkSGIX    = __native_glXQueryHyperpipeNetworkSGIX    = (PFNGLXQUERYHYPERPIPENETWORKSGIXPROC)gtglGetProcAddress("glXQueryHyperpipeNetworkSGIX");
        glXHyperpipeConfigSGIX          = __native_glXHyperpipeConfigSGIX          = (PFNGLXHYPERPIPECONFIGSGIXPROC)gtglGetProcAddress("glXHyperpipeConfigSGIX");
        glXQueryHyperpipeConfigSGIX     = __native_glXQueryHyperpipeConfigSGIX     = (PFNGLXQUERYHYPERPIPECONFIGSGIXPROC)gtglGetProcAddress("glXQueryHyperpipeConfigSGIX");
        glXDestroyHyperpipeConfigSGIX   = __native_glXDestroyHyperpipeConfigSGIX   = (PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC)gtglGetProcAddress("glXDestroyHyperpipeConfigSGIX");
        glXBindHyperpipeSGIX            = __native_glXBindHyperpipeSGIX            = (PFNGLXBINDHYPERPIPESGIXPROC)gtglGetProcAddress("glXBindHyperpipeSGIX");
        glXQueryHyperpipeBestAttribSGIX = __native_glXQueryHyperpipeBestAttribSGIX = (PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC)gtglGetProcAddress("glXQueryHyperpipeBestAttribSGIX");
        glXHyperpipeAttribSGIX          = __native_glXHyperpipeAttribSGIX          = (PFNGLXHYPERPIPEATTRIBSGIXPROC)gtglGetProcAddress("glXHyperpipeAttribSGIX");
        glXQueryHyperpipeAttribSGIX     = __native_glXQueryHyperpipeAttribSGIX     = (PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC)gtglGetProcAddress("glXQueryHyperpipeAttribSGIX");

        if (debugMode)
        {
            glXQueryHyperpipeNetworkSGIX    = __debug_glXQueryHyperpipeNetworkSGIX;
            glXHyperpipeConfigSGIX          = __debug_glXHyperpipeConfigSGIX;
            glXQueryHyperpipeConfigSGIX     = __debug_glXQueryHyperpipeConfigSGIX;
            glXDestroyHyperpipeConfigSGIX   = __debug_glXDestroyHyperpipeConfigSGIX;
            glXBindHyperpipeSGIX            = __debug_glXBindHyperpipeSGIX;
            glXQueryHyperpipeBestAttribSGIX = __debug_glXQueryHyperpipeBestAttribSGIX;
            glXHyperpipeAttribSGIX          = __debug_glXHyperpipeAttribSGIX;
            glXQueryHyperpipeAttribSGIX     = __debug_glXQueryHyperpipeAttribSGIX;
        }
    }

    /* MESA Extensions */
    if (gtglIsExtensionSupported("GLX_MESA_copy_sub_buffer"))
    {
        GTGL_GLX_MESA_copy_sub_buffer = GL_TRUE;
        glXCopySubBufferMESA = __native_glXCopySubBufferMESA = (PFNGLXCOPYSUBBUFFERMESAPROC)gtglGetProcAddress("glXCopySubBufferMESA");

        if (debugMode)
        {
            glXCopySubBufferMESA = __debug_glXCopySubBufferMESA;
        }
    }

    if (gtglIsExtensionSupported("GLX_MESA_pixmap_colormap"))
    {
        GTGL_GLX_MESA_pixmap_colormap = GL_TRUE;
        glXCreateGLXPixmapMESA = __native_glXCreateGLXPixmapMESA = (PFNGLXCREATEGLXPIXMAPMESAPROC)gtglGetProcAddress("glXCreateGLXPixmapMESA");

        if (debugMode)
        {
            glXCreateGLXPixmapMESA = __debug_glXCreateGLXPixmapMESA;
        }
    }

    if (gtglIsExtensionSupported("GLX_MESA_release_buffers"))
    {
        GTGL_GLX_MESA_release_buffers = GL_TRUE;
        glXReleaseBuffersMESA = __native_glXReleaseBuffersMESA = (PFNGLXRELEASEBUFFERSMESAPROC)gtglGetProcAddress("glXReleaseBuffersMESA");

        if (debugMode)
        {
            glXReleaseBuffersMESA = __debug_glXReleaseBuffersMESA;
        }
    }

    if (gtglIsExtensionSupported("GLX_MESA_set_3dfx_mode"))
    {
        GTGL_GLX_MESA_set_3dfx_mode = GL_TRUE;
        glXSet3DfxModeMESA = __native_glXSet3DfxModeMESA = (PFNGLXSET3DFXMODEMESAPROC)gtglGetProcAddress("glXSet3DfxModeMESA");

        if (debugMode)
        {
            glXSet3DfxModeMESA = __debug_glXSet3DfxModeMESA;
        }
    }

    if (gtglIsExtensionSupported("GLX_MESA_agp_offset"))
    {
        GTGL_GLX_MESA_agp_offset = GL_TRUE;
        glXGetAGPOffsetMESA = __native_glXGetAGPOffsetMESA = (PFNGLXGETAGPOFFSETMESAPROC)gtglGetProcAddress("glXGetAGPOffsetMESA");

        if (debugMode)
        {
            glXGetAGPOffsetMESA = __debug_glXGetAGPOffsetMESA;
        }
    }

    /* Other Extensions */
    if (gtglIsExtensionSupported("GLX_SUN_get_transparent_index"))
    {
        GTGL_GLX_SUN_get_transparent_index = GL_TRUE;
        glXGetTransparentIndexSUN = __native_glXGetTransparentIndexSUN = (PFNGLXGETTRANSPARENTINDEXSUNPROC)gtglGetProcAddress("glXGetTransparentIndexSUN");

        if (debugMode)
        {
            glXGetTransparentIndexSUN = __debug_glXGetTransparentIndexSUN;
        }
    }

    if (gtglIsExtensionSupported("GLX_OML_swap_method"))
    {
        GTGL_GLX_OML_swap_method = GL_TRUE;
    }

    if (gtglIsExtensionSupported("GLX_OML_sync_control"))
    {
        GTGL_GLX_OML_sync_control = GL_TRUE;
        glXGetSyncValuesOML  = __native_glXGetSyncValuesOML  = (PFNGLXGETSYNCVALUESOMLPROC)gtglGetProcAddress("glXGetSyncValuesOML");
        glXGetMscRateOML     = __native_glXGetMscRateOML     = (PFNGLXGETMSCRATEOMLPROC)gtglGetProcAddress("glXGetMscRateOML");
        glXSwapBuffersMscOML = __native_glXSwapBuffersMscOML = (PFNGLXSWAPBUFFERSMSCOMLPROC)gtglGetProcAddress("glXSwapBuffersMscOML");
        glXWaitForMscOML     = __native_glXWaitForMscOML     = (PFNGLXWAITFORMSCOMLPROC)gtglGetProcAddress("glXWaitForMscOML");
        glXWaitForSbcOML     = __native_glXWaitForSbcOML     = (PFNGLXWAITFORSBCOMLPROC)gtglGetProcAddress("glXWaitForSbcOML");

        if (debugMode)
        {
            glXGetSyncValuesOML  = __debug_glXGetSyncValuesOML;
            glXGetMscRateOML     = __debug_glXGetMscRateOML;
            glXSwapBuffersMscOML = __debug_glXSwapBuffersMscOML;
            glXWaitForMscOML     = __debug_glXWaitForMscOML;
            glXWaitForSbcOML     = __debug_glXWaitForSbcOML;
        }
    }

    if (gtglIsExtensionSupported("GLX_INTEL_swap_event"))
    {
        GTGL_GLX_INTEL_swap_event = GL_TRUE;
    }
}

const char * gtglXGetExtensionsString()
{
    Display *dpy = gtglXGetCurrentDisplay();
    return gtglXQueryExtensionsString(dpy, DefaultScreen(dpy));
}

#endif
