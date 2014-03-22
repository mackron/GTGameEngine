/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_glx_h_
#define __gtgl_glx_h_

#if defined(GTGL_GLX)

/* We want to include our own local headers. They contain slight modifications in places. */
#include "GL/gl3.h"
#include "GL/glext.h"
#include "GL/glx.h"
#include "GL/glxext.h"

#ifdef __cplusplus
extern "C"
{
#endif


/**
*   \brief  Initialises the GLX API.
*/
void gtglInitGLAPI_GLX(GLboolean debugMode);

/**
*   \brief  Retrieves the GLX extensions string.
*/
const char * gtglXGetExtensionsString();


typedef XVisualInfo* (APIENTRYP PFNGLXCHOOSEVISUALPROC)             (Display *dpy, int screen, int *attribList);
typedef GLXContext   (APIENTRYP PFNGLXCREATECONTEXTPROC)            (Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct);
typedef void         (APIENTRYP PFNGLXDESTROYCONTEXTPROC)           (Display *dpy, GLXContext ctx);
typedef Bool         (APIENTRYP PFNGLXMAKECURRENTPROC)              (Display *dpy, GLXDrawable drawable, GLXContext ctx);
typedef void         (APIENTRYP PFNGLXSWAPBUFFERSPROC)              (Display *dpy, GLXDrawable drawable);
typedef GLXContext   (APIENTRYP PFNGLXGETCURRENTCONTEXTPROC)        (void);
typedef const char*  (APIENTRYP PFNGLXQUERYEXTENSIONSTRINGPROC)     (Display *dpy, int screen);
/*typedef Display*     (APIENTRYP PFNGLXGETCURRENTDISPLAYPROC)        (void);*/
/*typedef GLXFBConfig* (APIENTRYP PFNGLXCHOOSEFBCONFIGPROC)           (Display *dpy, int screen, const int *attribList, int *nitems);*/
/*typedef XVisualInfo* (APIENTRYP PFNGLXGETVISUALFROMFBCONFIGPROC)    (Display *dpy, GLXFBConfig config);*/


extern void* GTGL_GL_SO;
extern PFNGLXCHOOSEVISUALPROC           gtglXChooseVisual;
extern PFNGLXCREATECONTEXTPROC          gtglXCreateContext;
extern PFNGLXDESTROYCONTEXTPROC         gtglXDestroyContext;
extern PFNGLXMAKECURRENTPROC            gtglXMakeCurrent;
extern PFNGLXSWAPBUFFERSPROC            gtglXSwapBuffers;
extern PFNGLXGETCURRENTCONTEXTPROC      gtglXGetCurrentContext;
extern PFNGLXQUERYEXTENSIONSTRINGPROC   gtglXQueryExtensionsString;
extern PFNGLXGETCURRENTDISPLAYPROC      gtglXGetCurrentDisplay;
extern PFNGLXCHOOSEFBCONFIGPROC         gtglXChooseFBConfig;
extern PFNGLXGETVISUALFROMFBCONFIGPROC  gtglXGetVisualFromFBConfig;
extern PFNGLXGETPROCADDRESSPROC         gtglXGetProcAddress;


/** Called during gtglStartup() to load the GLX function symbols. */
GLboolean gtglLoadGLXSymbols();

/** Called during gtglShutdown() to unload the GLX function symbols. */
void gtglUnloadGLXSymbols();



/* ARB Extensions */
extern GLboolean GTGL_GLX_ARB_multisample;
extern GLboolean GTGL_GLX_ARB_fbconfig_float;
extern GLboolean GTGL_GLX_ARB_framebuffer_sRGB;
extern GLboolean GTGL_GLX_ARB_create_context;
extern GLboolean GTGL_GLX_ARB_create_context_profile;
extern GLboolean GTGL_GLX_ARB_create_context_robustness;

/* EXT Extensions */
extern GLboolean GTGL_GLX_EXT_visual_info;
extern GLboolean GTGL_GLX_EXT_visual_rating;
extern GLboolean GTGL_GLX_EXT_import_context;
extern GLboolean GTGL_GLX_EXT_fbconfig_packed_float;
extern GLboolean GTGL_GLX_EXT_framebuffer_sRGB;
extern GLboolean GTGL_GLX_EXT_texture_from_pixmap;
extern GLboolean GTGL_GLX_EXT_swap_control;
extern GLboolean GTGL_GLX_EXT_create_context_es2_profile;
extern GLboolean GTGL_GLX_EXT_swap_control_tear;

/* NV Extensions */
extern GLboolean GTGL_GLX_NV_float_buffer;
extern GLboolean GTGL_GLX_NV_present_video;
extern GLboolean GTGL_GLX_NV_video_output;
extern GLboolean GTGL_GLX_NV_swap_group;
extern GLboolean GTGL_GLX_NV_video_capture;
extern GLboolean GTGL_GLX_NV_copy_image;
extern GLboolean GTGL_GLX_NV_multisample_coverage;

/* SGI Extensions */
extern GLboolean GTGL_GLX_SGIS_multisample;
extern GLboolean GTGL_GLX_SGI_swap_control;
extern GLboolean GTGL_GLX_SGI_video_sync;
extern GLboolean GTGL_GLX_SGI_make_current_read;
extern GLboolean GTGL_GLX_SGIX_fbconfig;
extern GLboolean GTGL_GLX_SGIX_pbuffer;
extern GLboolean GTGL_GLX_SGI_cushion;
extern GLboolean GTGL_GLX_SGIX_video_resize;
extern GLboolean GTGL_GLX_SGIX_swap_group;
extern GLboolean GTGL_GLX_SGIX_swap_barrier;
extern GLboolean GTGL_GLX_SGIX_visual_select_group;
extern GLboolean GTGL_GLX_SGIX_hyperpipe;

/* MESA Extensions */
extern GLboolean GTGL_GLX_MESA_copy_sub_buffer;
extern GLboolean GTGL_GLX_MESA_pixmap_colormap;
extern GLboolean GTGL_GLX_MESA_release_buffers;
extern GLboolean GTGL_GLX_MESA_set_3dfx_mode;
extern GLboolean GTGL_GLX_MESA_agp_offset;

/* Other Extensions */
extern GLboolean GTGL_GLX_SUN_get_transparent_index;
extern GLboolean GTGL_GLX_OML_swap_method;
extern GLboolean GTGL_GLX_OML_sync_control;
extern GLboolean GTGL_GLX_INTEL_swap_event;



/* GLX_ARB_create_context */
extern PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;

/* GLX_EXT_import_context */
extern PFNGLXGETCURRENTDISPLAYEXTPROC glXGetCurrentDisplayEXT;
extern PFNGLXQUERYCONTEXTINFOEXTPROC  glXQueryContextInfoEXT;
extern PFNGLXGETCONTEXTIDEXTPROC      glXGetContextIDEXT;
extern PFNGLXIMPORTCONTEXTEXTPROC     glXImportContextEXT;
extern PFNGLXFREECONTEXTEXTPROC       glXFreeContextEXT;

/* GLX_EXT_texture_from_pixmap */
extern PFNGLXBINDTEXIMAGEEXTPROC    glXBindTexImageEXT;
extern PFNGLXRELEASETEXIMAGEEXTPROC glXReleaseTexImageEXT;

/* GLX_EXT_swap_control */
extern PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;


/* GLX_NV_present_video */
extern PFNGLXENUMERATEVIDEODEVICESNVPROC glXEnumerateVideoDevicesNV;
extern PFNGLXBINDVIDEODEVICENVPROC       glXBindVideoDeviceNV;

/* GLX_NV_video_output */
extern PFNGLXGETVIDEODEVICENVPROC     glXGetVideoDeviceNV;
extern PFNGLXRELEASEVIDEODEVICENVPROC glXReleaseVideoDeviceNV;
extern PFNGLXBINDVIDEOIMAGENVPROC     glXBindVideoImageNV;
extern PFNGLXRELEASEVIDEOIMAGENVPROC  glXReleaseVideoImageNV;
extern PFNGLXSENDPBUFFERTOVIDEONVPROC glXSendPbufferToVideoNV;
extern PFNGLXGETVIDEOINFONVPROC       glXGetVideoInfoNV;

/* GLX_NV_swap_group */
extern PFNGLXJOINSWAPGROUPNVPROC      glXJoinSwapGroupNV;
extern PFNGLXBINDSWAPBARRIERNVPROC    glXBindSwapBarrierNV;
extern PFNGLXQUERYSWAPGROUPNVPROC     glXQuerySwapGroupNV;
extern PFNGLXQUERYMAXSWAPGROUPSNVPROC glXQueryMaxSwapGroupsNV;
extern PFNGLXQUERYFRAMECOUNTNVPROC    glXQueryFrameCountNV;
extern PFNGLXRESETFRAMECOUNTNVPROC    glXResetFrameCountNV;

/* GLX_NV_video_capture */
extern PFNGLXBINDVIDEOCAPTUREDEVICENVPROC       glXBindVideoCaptureDeviceNV;
extern PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC glXEnumerateVideoCaptureDevicesNV;
extern PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC       glXLockVideoCaptureDeviceNV;
extern PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC      glXQueryVideoCaptureDeviceNV;
extern PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC    glXReleaseVideoCaptureDeviceNV;

/* GLX_NV_copy_image */
extern PFNGLXCOPYIMAGESUBDATANVPROC glXCopyImageSubDataNV;

/* GLX_SGI_swap_control */
extern PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI;

/* GLX_SGI_video_sync */
extern PFNGLXGETVIDEOSYNCSGIPROC  glXGetVideoSyncSGI;
extern PFNGLXWAITVIDEOSYNCSGIPROC glXWaitVideoSyncSGI;

/* GLX_SGI_make_current_read */
extern PFNGLXMAKECURRENTREADSGIPROC        glXMakeCurrentReadSGI;
extern PFNGLXGETCURRENTREADDRAWABLESGIPROC glXGetCurrentReadDrawableSGI;

/* GLX_SGIX_fbconfig */
extern PFNGLXGETFBCONFIGATTRIBSGIXPROC         glXGetFBConfigAttribSGIX;
extern PFNGLXCHOOSEFBCONFIGSGIXPROC            glXChooseFBConfigSGIX;
extern PFNGLXCREATEGLXPIXMAPWITHCONFIGSGIXPROC glXCreateGLXPixmapWithConfigSGIX;
extern PFNGLXCREATECONTEXTWITHCONFIGSGIXPROC   glXCreateContextWithConfigSGIX;
extern PFNGLXGETVISUALFROMFBCONFIGSGIXPROC     glXGetVisualFromFBConfigSGIX;
extern PFNGLXGETFBCONFIGFROMVISUALSGIXPROC     glXGetFBConfigFromVisualSGIX;

/* GLX_SGIX_pbuffer */
extern PFNGLXCREATEGLXPBUFFERSGIXPROC  glXCreateGLXPbufferSGIX;
extern PFNGLXDESTROYGLXPBUFFERSGIXPROC glXDestroyGLXPbufferSGIX;
extern PFNGLXQUERYGLXPBUFFERSGIXPROC   glXQueryGLXPbufferSGIX;
extern PFNGLXSELECTEVENTSGIXPROC       glXSelectEventSGIX;
extern PFNGLXGETSELECTEDEVENTSGIXPROC  glXGetSelectedEventSGIX;

/* GLX_SGI_cushion */
extern PFNGLXCUSHIONSGIPROC glXCushionSGI;

/* GLX_SGIX_video_resize */
extern PFNGLXBINDCHANNELTOWINDOWSGIXPROC glXBindChannelToWindowSGIX;
extern PFNGLXCHANNELRECTSGIXPROC         glXChannelRectSGIX;
extern PFNGLXQUERYCHANNELRECTSGIXPROC    glXQueryChannelRectSGIX;
extern PFNGLXQUERYCHANNELDELTASSGIXPROC  glXQueryChannelDeltasSGIX;
extern PFNGLXCHANNELRECTSYNCSGIXPROC     glXChannelRectSyncSGIX;

/* GLX_SGIX_swap_group */
extern PFNGLXJOINSWAPGROUPSGIXPROC glXJoinSwapGroupSGIX;

/* GLX_SGIX_swap_barrier */
extern PFNGLXBINDSWAPBARRIERSGIXPROC      glXBindSwapBarrierSGIX;
extern PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC glXQueryMaxSwapBarriersSGIX;

/* GLX_SGIX_hyperpipe */
extern PFNGLXQUERYHYPERPIPENETWORKSGIXPROC    glXQueryHyperpipeNetworkSGIX;
extern PFNGLXHYPERPIPECONFIGSGIXPROC          glXHyperpipeConfigSGIX;
extern PFNGLXQUERYHYPERPIPECONFIGSGIXPROC     glXQueryHyperpipeConfigSGIX;
extern PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC   glXDestroyHyperpipeConfigSGIX;
extern PFNGLXBINDHYPERPIPESGIXPROC            glXBindHyperpipeSGIX;
extern PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC glXQueryHyperpipeBestAttribSGIX;
extern PFNGLXHYPERPIPEATTRIBSGIXPROC          glXHyperpipeAttribSGIX;
extern PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC     glXQueryHyperpipeAttribSGIX;

/* GLX_MESA_copy_sub_buffer */
extern PFNGLXCOPYSUBBUFFERMESAPROC glXCopySubBufferMESA;

/* GLX_MESA_pixmap_colormap */
extern PFNGLXCREATEGLXPIXMAPMESAPROC glXCreateGLXPixmapMESA;

/* GLX_MESA_release_buffers */
extern PFNGLXRELEASEBUFFERSMESAPROC glXReleaseBuffersMESA;

/* GLX_MESA_set_3dfx_mode */
extern PFNGLXSET3DFXMODEMESAPROC glXSet3DfxModeMESA;

/* GLX_MESA_agp_offset */
extern PFNGLXGETAGPOFFSETMESAPROC glXGetAGPOffsetMESA;

/* GLX_SUN_get_transparent_index */
extern PFNGLXGETTRANSPARENTINDEXSUNPROC glXGetTransparentIndexSUN;

/* GLX_OML_sync_control */
extern PFNGLXGETSYNCVALUESOMLPROC  glXGetSyncValuesOML;
extern PFNGLXGETMSCRATEOMLPROC     glXGetMscRateOML;
extern PFNGLXSWAPBUFFERSMSCOMLPROC glXSwapBuffersMscOML;
extern PFNGLXWAITFORMSCOMLPROC     glXWaitForMscOML;
extern PFNGLXWAITFORSBCOMLPROC     glXWaitForSbcOML;



#ifdef __cplusplus
}
#endif

#endif

#endif
