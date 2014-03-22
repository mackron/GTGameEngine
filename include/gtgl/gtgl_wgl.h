/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_wgl_h_
#define __gtgl_wgl_h_

/* We want to include our own local headers. They contain slight modifications in places. */
#include "GL/gl3.h"
#include "GL/glext.h"
#include "GL/wglext.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
*   \brief  Initialises the WGL API.
*/
void gtglInitGLAPI_WGL(GLboolean debugMode);

/**
*   \brief  Retrieves the WGL extensions string.
*/
const char * gtwglGetExtensionsString();


/* WGL functions. */
typedef HGLRC (APIENTRYP PFNWGLCREATECONTEXTPROC)       (HDC hdc);
typedef BOOL  (APIENTRYP PFNWGLDELETECONTEXTPROC)       (HGLRC hglrc);
typedef BOOL  (APIENTRYP PFNWGLMAKECURRENTPROC)         (HDC hdc, HGLRC hglrc);
typedef HGLRC (APIENTRYP PFNWGLGETCURRENTCONTEXTPROC)   ();
typedef PROC  (APIENTRYP PFNWGLGETPROCADDRESSPROC)      (LPCSTR lpszProc);
typedef BOOL  (APIENTRYP PFNWGLSHARELISTSPROC)          (HGLRC hglrc1, HGLRC hglrc2);
typedef HDC   (APIENTRYP PFNWGLGETCURRENTDCPROC)        ();

extern HMODULE GTGL_OpenGL32_DLL;
extern PFNWGLCREATECONTEXTPROC     gtwglCreateContext;
extern PFNWGLDELETECONTEXTPROC     gtwglDeleteContext;
extern PFNWGLMAKECURRENTPROC       gtwglMakeCurrent;
extern PFNWGLGETCURRENTCONTEXTPROC gtwglGetCurrentContext;
extern PFNWGLGETPROCADDRESSPROC    gtwglGetProcAddress;
extern PFNWGLSHARELISTSPROC        gtwglShareLists;
extern PFNWGLGETCURRENTDCPROC      gtwglGetCurrentDC;

/** Called during gtglStartup() to load the WGL function symbols. */
GLboolean gtglLoadWGLSymbols();



/* ARB Extensions */
extern GLboolean GTGL_WGL_ARB_buffer_region;
extern GLboolean GTGL_WGL_ARB_multisample;
extern GLboolean GTGL_WGL_ARB_extensions_string;
extern GLboolean GTGL_WGL_ARB_pixel_format;
extern GLboolean GTGL_WGL_ARB_make_current_read;
extern GLboolean GTGL_WGL_ARB_pbuffer;
extern GLboolean GTGL_WGL_ARB_render_texture;
extern GLboolean GTGL_WGL_ARB_pixel_format_float;
extern GLboolean GTGL_WGL_ARB_framebuffer_sRGB;
extern GLboolean GTGL_WGL_ARB_create_context;
extern GLboolean GTGL_WGL_ARB_create_context_profile;
extern GLboolean GTGL_WGL_ARB_create_context_robustness;

/* EXT Extensions */
extern GLboolean GTGL_WGL_EXT_display_color_table;
extern GLboolean GTGL_WGL_EXT_extensions_string;
extern GLboolean GTGL_WGL_EXT_make_current_read;
extern GLboolean GTGL_WGL_EXT_pbuffer;
extern GLboolean GTGL_WGL_EXT_pixel_format;
extern GLboolean GTGL_WGL_EXT_swap_control;
extern GLboolean GTGL_WGL_EXT_depth_float;
extern GLboolean GTGL_WGL_EXT_multisample;
extern GLboolean GTGL_WGL_EXT_pixel_format_packed_float;
extern GLboolean GTGL_WGL_EXT_framebuffer_sRGB;
extern GLboolean GTGL_WGL_EXT_create_context_es2_profile;
extern GLboolean GTGL_WGL_EXT_swap_control_tear;

/* NV Extensions */
extern GLboolean GTGL_WGL_NV_vertex_array_range;
extern GLboolean GTGL_WGL_NV_float_buffer;
extern GLboolean GTGL_WGL_NV_present_video;
extern GLboolean GTGL_WGL_NV_video_output;
extern GLboolean GTGL_WGL_NV_swap_group;
extern GLboolean GTGL_WGL_NV_gpu_affinity;
extern GLboolean GTGL_WGL_NV_video_capture;
extern GLboolean GTGL_WGL_NV_copy_image;
extern GLboolean GTGL_WGL_NV_multisample_coverage;
extern GLboolean GTGL_WGL_NV_DX_interop;
extern GLboolean GTGL_WGL_NV_DX_interop2;

/* I3D Extensions */
extern GLboolean GTGL_WGL_I3D_digital_video_control;
extern GLboolean GTGL_WGL_I3D_gamma;
extern GLboolean GTGL_WGL_I3D_genlock;
extern GLboolean GTGL_WGL_I3D_image_buffer;
extern GLboolean GTGL_WGL_I3D_swap_frame_lock;
extern GLboolean GTGL_WGL_I3D_swap_frame_usage;

/* Other Extensions */
extern GLboolean GTGL_WGL_3DFX_multisample;
extern GLboolean GTGL_WGL_OML_sync_control;
extern GLboolean GTGL_WGL_ATI_pixel_format_float;
extern GLboolean GTGL_WGL_3DL_stereo_control;
extern GLboolean GTGL_WGL_AMD_gpu_association;



/* WGL_ARB_buffer_region */
extern PFNWGLCREATEBUFFERREGIONARBPROC  wglCreateBufferRegionARB;
extern PFNWGLDELETEBUFFERREGIONARBPROC  wglDeleteBufferRegionARB;
extern PFNWGLSAVEBUFFERREGIONARBPROC    wglSaveBufferRegionARB;
extern PFNWGLRESTOREBUFFERREGIONARBPROC wglRestoreBufferRegionARB;

/* WGL_ARB_extensions_string */
extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;

/* WGL_ARB_pixel_format */
extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
extern PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;

/* WGL_ARB_make_current_read */
extern PFNWGLMAKECONTEXTCURRENTARBPROC wglMakeContextCurrentARB;
extern PFNWGLGETCURRENTREADDCARBPROC   wglGetCurrentReadDCARB;

/* WGL_ARB_pbuffer */
extern PFNWGLCREATEPBUFFERARBPROC    wglCreatePbufferARB;
extern PFNWGLGETPBUFFERDCARBPROC     wglGetPbufferDCARB;
extern PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
extern PFNWGLDESTROYPBUFFERARBPROC   wglDestroyPbufferARB;
extern PFNWGLQUERYPBUFFERARBPROC     wglQueryPbufferARB;

/* WGL_ARB_render_texture */
extern PFNWGLBINDTEXIMAGEARBPROC     wglBindTexImageARB;
extern PFNWGLRELEASETEXIMAGEARBPROC  wglReleaseTexImageARB;
extern PFNWGLSETPBUFFERATTRIBARBPROC wglSetPbufferAttribARB;

/* WGL_ARB_create_context */
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;



/* WGL_EXT_display_color_table */
extern PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC  wglCreateDisplayColorTableEXT;
extern PFNWGLLOADDISPLAYCOLORTABLEEXTPROC    wglLoadDisplayColorTableEXT;
extern PFNWGLBINDDISPLAYCOLORTABLEEXTPROC    wglBindDisplayColorTableEXT;
extern PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC wglDestroyDisplayColorTableEXT;

/* WGL_EXT_extensions_string */
extern PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;

/* WGL_EXT_make_current_read */
extern PFNWGLMAKECONTEXTCURRENTEXTPROC wglMakeContextCurrentEXT;
extern PFNWGLGETCURRENTREADDCEXTPROC   wglGetCurrentReadDCEXT;

/* WGL_EXT_pbuffer */
extern PFNWGLCREATEPBUFFEREXTPROC    wglCreatePbufferEXT;
extern PFNWGLGETPBUFFERDCEXTPROC     wglGetPbufferDCEXT;
extern PFNWGLRELEASEPBUFFERDCEXTPROC wglReleasePbufferDCEXT;
extern PFNWGLDESTROYPBUFFEREXTPROC   wglDestroyPbufferEXT;
extern PFNWGLQUERYPBUFFEREXTPROC     wglQueryPbufferEXT;

/* WGL_EXT_pixel_format */
extern PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivEXT;
extern PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvEXT;
extern PFNWGLCHOOSEPIXELFORMATEXTPROC      wglChoosePixelFormatEXT;

/* WGL_EXT_swap_control */
extern PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT;
extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;


/* WGL_NV_vertex_array_range */
extern PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
extern PFNWGLFREEMEMORYNVPROC     wglFreeMemoryNV;

/* WGL_NV_present_video */
extern PFNWGLENUMERATEVIDEODEVICESNVPROC wglEnumerateVideoDevicesNV;
extern PFNWGLBINDVIDEODEVICENVPROC       wglBindVideoDeviceNV;
extern PFNWGLQUERYCURRENTCONTEXTNVPROC   wglQueryCurrentContextNV;

/* WGL_NV_video_output */
extern PFNWGLGETVIDEODEVICENVPROC     wglGetVideoDeviceNV;
extern PFNWGLRELEASEVIDEODEVICENVPROC wglReleaseVideoDeviceNV;
extern PFNWGLBINDVIDEOIMAGENVPROC     wglBindVideoImageNV;
extern PFNWGLRELEASEVIDEOIMAGENVPROC  wglReleaseVideoImageNV;
extern PFNWGLSENDPBUFFERTOVIDEONVPROC wglSendPbufferToVideoNV;
extern PFNWGLGETVIDEOINFONVPROC       wglGetVideoInfoNV;

/* WGL_NV_swap_group */
extern PFNWGLJOINSWAPGROUPNVPROC      wglJoinSwapGroupNV;
extern PFNWGLBINDSWAPBARRIERNVPROC    wglBindSwapBarrierNV;
extern PFNWGLQUERYSWAPGROUPNVPROC     wglQuerySwapGroupNV;
extern PFNWGLQUERYMAXSWAPGROUPSNVPROC wglQueryMaxSwapGroupsNV;
extern PFNWGLQUERYFRAMECOUNTNVPROC    wglQueryFrameCountNV;
extern PFNWGLRESETFRAMECOUNTNVPROC    wglResetFrameCountNV;

/* WGL_NV_gpu_affinity */
extern PFNWGLENUMGPUSNVPROC               wglEnumGpusNV;
extern PFNWGLENUMGPUDEVICESNVPROC         wglEnumGpuDevicesNV;
extern PFNWGLCREATEAFFINITYDCNVPROC       wglCreateAffinityDCNV;
extern PFNWGLENUMGPUSFROMAFFINITYDCNVPROC wglEnumGpusFromAffinityDCNV;
extern PFNWGLDELETEDCNVPROC               wglDeleteDCNV;

/* WGL_NV_video_capture */
extern PFNWGLBINDVIDEOCAPTUREDEVICENVPROC       wglBindVideoCaptureDeviceNV;
extern PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC wglEnumerateVideoCaptureDevicesNV;
extern PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC       wglLockVideoCaptureDeviceNV;
extern PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC      wglQueryVideoCaptureDeviceNV;
extern PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC    wglReleaseVideoCaptureDeviceNV;

/* WGL_NV_copy_image */
extern PFNWGLCOPYIMAGESUBDATANVPROC wglCopyImageSubDataNV;

/* WGL_NV_DX_interop */
extern PFNWGLDXSETRESOURCESHAREHANDLENVPROC wglDXSetResourceShareHandleNV;
extern PFNWGLDXOPENDEVICENVPROC             wglDXOpenDeviceNV;
extern PFNWGLDXCLOSEDEVICENVPROC            wglDXCloseDeviceNV;
extern PFNWGLDXREGISTEROBJECTNVPROC         wglDXRegisterObjectNV;
extern PFNWGLDXUNREGISTEROBJECTNVPROC       wglDXUnregisterObjectNV;
extern PFNWGLDXOBJECTACCESSNVPROC           wglDXObjectAccessNV;
extern PFNWGLDXLOCKOBJECTSNVPROC            wglDXLockObjectsNV;
extern PFNWGLDXUNLOCKOBJECTSNVPROC          wglDXUnlockObjectsNV;


/* WGL_I3D_digital_video_control */
extern PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC wglGetDigitalVideoParametersI3D;
extern PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC wglSetDigitalVideoParametersI3D;

/* WGL_I3D_gamma */
extern PFNWGLGETGAMMATABLEPARAMETERSI3DPROC wglGetGammaTableParametersI3D;
extern PFNWGLSETGAMMATABLEPARAMETERSI3DPROC wglSetGammaTableParametersI3D;
extern PFNWGLGETGAMMATABLEI3DPROC           wglGetGammaTableI3D;
extern PFNWGLSETGAMMATABLEI3DPROC           wglSetGammaTableI3D;

/* WGL_I3D_genlock */
extern PFNWGLENABLEGENLOCKI3DPROC              wglEnableGenlockI3D;
extern PFNWGLDISABLEGENLOCKI3DPROC             wglDisableGenlockI3D;
extern PFNWGLISENABLEDGENLOCKI3DPROC           wglIsEnabledGenlockI3D;
extern PFNWGLGENLOCKSOURCEI3DPROC              wglGenlockSourceI3D;
extern PFNWGLGETGENLOCKSOURCEI3DPROC           wglGetGenlockSourceI3D;
extern PFNWGLGENLOCKSOURCEEDGEI3DPROC          wglGenlockSourceEdgeI3D;
extern PFNWGLGETGENLOCKSOURCEEDGEI3DPROC       wglGetGenlockSourceEdgeI3D;
extern PFNWGLGENLOCKSAMPLERATEI3DPROC          wglGenlockSampleRateI3D;
extern PFNWGLGETGENLOCKSAMPLERATEI3DPROC       wglGetGenlockSampleRateI3D;
extern PFNWGLGENLOCKSOURCEDELAYI3DPROC         wglGenlockSourceDelayI3D;
extern PFNWGLGETGENLOCKSOURCEDELAYI3DPROC      wglGetGenlockSourceDelayI3D;
extern PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC wglQueryGenlockMaxSourceDelayI3D;

/* WGL_I3D_image_buffer */
extern PFNWGLCREATEIMAGEBUFFERI3DPROC          wglCreateImageBufferI3D;
extern PFNWGLDESTROYIMAGEBUFFERI3DPROC         wglDestroyImageBufferI3D;
extern PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC wglAssociateImageBufferEventsI3D;
extern PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC   wglReleaseImageBufferEventsI3D;

/* WGL_I3D_swap_frame_lock */
extern PFNWGLENABLEFRAMELOCKI3DPROC      wglEnableFrameLockI3D;
extern PFNWGLDISABLEFRAMELOCKI3DPROC     wglDisableFrameLockI3D;
extern PFNWGLISENABLEDFRAMELOCKI3DPROC   wglIsEnabledFrameLockI3D;
extern PFNWGLQUERYFRAMELOCKMASTERI3DPROC wglQueryFrameLockMasterI3D;

/* WGL_I3D_swap_frame_usage */
extern PFNWGLGETFRAMEUSAGEI3DPROC      wglGetFrameUsageI3D;
extern PFNWGLBEGINFRAMETRACKINGI3DPROC wglBeginFrameTrackingI3D;
extern PFNWGLENDFRAMETRACKINGI3DPROC   wglEndFrameTrackingI3D;
extern PFNWGLQUERYFRAMETRACKINGI3DPROC wglQueryFrameTrackingI3D;


/* WGL_OML_sync_control */
extern PFNWGLGETSYNCVALUESOMLPROC       wglGetSyncValuesOML;
extern PFNWGLGETMSCRATEOMLPROC          wglGetMscRateOML;
extern PFNWGLSWAPBUFFERSMSCOMLPROC      wglSwapBuffersMscOML;
extern PFNWGLSWAPLAYERBUFFERSMSCOMLPROC wglSwapLayerBuffersMscOML;
extern PFNWGLWAITFORMSCOMLPROC          wglWaitForMscOML;
extern PFNWGLWAITFORSBCOMLPROC          wglWaitForSbcOML;

/* WGL_3DL_stereo_control */
extern PFNWGLSETSTEREOEMITTERSTATE3DLPROC wglSetStereoEmitterState3DL;

/* WGL_AMD_gpu_association */
extern PFNWGLGETGPUIDSAMDPROC                      wglGetGPUIDsAMD;
extern PFNWGLGETGPUINFOAMDPROC                     wglGetGPUInfoAMD;
extern PFNWGLGETCONTEXTGPUIDAMDPROC                wglGetContextGPUIDAMD;
extern PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC        wglCreateAssociatedContextAMD;
extern PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC wglCreateAssociatedContextAttribsAMD;
extern PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC        wglDeleteAssociatedContextAMD;
extern PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC   wglMakeAssociatedContextCurrentAMD;
extern PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC    wglGetCurrentAssociatedContextAMD;
extern PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC         wglBlitContextFramebufferAMD;


#ifdef __cplusplus
}
#endif

#endif
