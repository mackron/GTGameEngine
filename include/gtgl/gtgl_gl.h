/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_gl_h_
#define __gtgl_gl_h_

/*
The following list of extensions have been intentionally excluded. We excluded extensions if they are not longer needed. It is
mainly vendor-specific extensions that have been promoted to EXT or ARB status. We also intentionally exclude extensions that
were made core in OpenGL 1.1.

    GL_EXT_vertex_array       - Core in OpenGL 1.1
    GL_EXT_polygon_offset     - Core in OpenGL 1.1
    GL_EXT_texture            - Core in OpenGL 1.1
    GL_EXT_copy_texture       - Core in OpenGL 1.1
    GL_EXT_subtexture         - Core in OpenGL 1.1
    GL_EXT_texture_object     - Core in OpenGL 1.1
    GL_EXT_vertex_shader      - Use GL_ARB_vertex_shader instead
    GL_EXT_pixel_transform    - No implementation support
    GL_EXT_light_texture      - No implementation support
    GL_NV_point_sprite        - Use GL_ARB_point_sprite instead
    GL_NV_occlusion_query     - Use GL_ARB_occlusion_query instead
    GL_NV_evaluators          - No longer supported
    GL_AMD_debug_output       - Use GL_ARB_debug_output instead
    GL_AMD_draw_buffers_blend - Use GL_ARB_draw_buffers_blend instead
    GL_ATI_draw_buffers       - Use GL_ARB_draw_buffers instead
*/

/* Platform. */
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN32__) || defined(__MINGW32__)
    #define GTGL_WGL
#elif defined(__linux__)
    #define GTGL_GLX
#elif defined(__APPLE__) && defined(__MACH__)
    #define GTGL_AGL
#endif


#ifndef __clang_analyzer__
    #ifndef __gtgl_h_
    #error "Do not include gtgl_gl.h directly. Use gtgl.h instead."
    #endif

    #ifdef __gl_h_
/*    #error "Do not include gl/gl.h directly. Use gtgl.h instead. There is no need to include the standard OpenGL headers." */
    #endif
#endif

/* This avoids multiple-declaration errors if gl.h is included after this one. */
#define __gl_h_

#if   defined(GTGL_WGL)
#include "gtgl_wgl.h"
#elif defined(GTGL_GLX)
#include "gtgl_glx.h"
#elif defined(GTGL_AGL)
#include "gtgl_agl.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
*   \brief                 Initialises the base OpenGL API.
*   \param  debugMode [in] Whether or not a debug functions should be used. This will be true for debug context's.
*/
void gtglInitGLAPI_GL(GLboolean debugMode);

/* ARB Extensions. */
extern GLboolean GTGL_ARB_multitexture;                      /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_transpose_matrix;                  /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_multisample;                       /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_texture_env_add;                   /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_texture_cube_map;                  /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_texture_compression;               /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_texture_border_clamp;              /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_point_parameters;                  /* Core in OpenGL 1.4 */
extern GLboolean GTGL_ARB_vertex_blend;
extern GLboolean GTGL_ARB_matrix_palette;
extern GLboolean GTGL_ARB_texture_env_combine;               /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_texture_env_crossbar;              /* Core in OpengL 1.4 */
extern GLboolean GTGL_ARB_texture_env_dot3;                  /* Core in OpenGL 1.3 */
extern GLboolean GTGL_ARB_texture_mirrored_repeat;           /* Core in OpenGL 1.4 */
extern GLboolean GTGL_ARB_depth_texture;                     /* Core in OpenGL 1.4 */
extern GLboolean GTGL_ARB_shadow;                            /* Core in OpenGL 1.4 */
extern GLboolean GTGL_ARB_shadow_ambient;
extern GLboolean GTGL_ARB_window_pos;                        /* Core in OpenGL 1.4 */
extern GLboolean GTGL_ARB_vertex_program;
extern GLboolean GTGL_ARB_fragment_program;
extern GLboolean GTGL_ARB_vertex_buffer_object;              /* Core in OpenGL 1.5 */
extern GLboolean GTGL_ARB_occlusion_query;                   /* Core in OpenGL 1.5 */
extern GLboolean GTGL_ARB_shader_objects;                    /* Core in OpenGL 2.0 */
extern GLboolean GTGL_ARB_vertex_shader;                     /* Core in OpenGL 2.0 */
extern GLboolean GTGL_ARB_fragment_shader;                   /* Core in OpenGL 2.0 */
extern GLboolean GTGL_ARB_shading_language_100;              /* Core in OpenGL 2.0 */
extern GLboolean GTGL_ARB_texture_non_power_of_two;          /* Core in OpenGL 2.0 */
extern GLboolean GTGL_ARB_point_sprite;                      /* Core in OpenGL 2.0 */
extern GLboolean GTGL_ARB_fragment_program_shadow;
extern GLboolean GTGL_ARB_draw_buffers;                      /* Core in OpenGL 2.0 */
extern GLboolean GTGL_ARB_texture_rectangle;                 /* Core in OpenGL 3.1 */
extern GLboolean GTGL_ARB_color_buffer_float;                /* Core in OpenGL 3.0 */
extern GLboolean GTGL_ARB_half_float_pixel;                  /* Core in OpenGL 3.0 */
extern GLboolean GTGL_ARB_texture_float;                     /* Core in OpenGL 3.0 */
extern GLboolean GTGL_ARB_pixel_buffer_object;               /* Core in OpenGL 2.1 */
extern GLboolean GTGL_ARB_depth_buffer_float;
extern GLboolean GTGL_ARB_draw_instanced;                    /* Core in OpenGL 3.1 */
extern GLboolean GTGL_ARB_framebuffer_object;                /* Core in OpenGL 3.0 */
extern GLboolean GTGL_ARB_framebuffer_sRGB;
extern GLboolean GTGL_ARB_geometry_shader4;                  /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_half_float_vertex;
extern GLboolean GTGL_ARB_instanced_arrays;                  /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_map_buffer_range;                  /* Core in OpenGL 3.0 */
extern GLboolean GTGL_ARB_texture_buffer_object;             /* Core in OpenGL 3.1 */
extern GLboolean GTGL_ARB_texture_compression_rgtc;          /* Core in OpenGL 3.0 */
extern GLboolean GTGL_ARB_texture_rg;
extern GLboolean GTGL_ARB_vertex_array_object;               /* Core in OpenGL 3.0 */
extern GLboolean GTGL_ARB_uniform_buffer_object;             /* Core in OpenGL 3.1 */
extern GLboolean GTGL_ARB_compatibility;
extern GLboolean GTGL_ARB_copy_buffer;                       /* Core in OpenGL 3.1 */
extern GLboolean GTGL_ARB_shader_texture_lod;
extern GLboolean GTGL_ARB_depth_clamp;                       /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_draw_elements_base_vertex;         /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_fragment_coord_conventions;        /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_provoking_vertex;                  /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_seamless_cube_map;                 /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_sync;                              /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_texture_multisample;               /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_vertex_array_bgra;                 /* Core in OpenGL 3.2 */
extern GLboolean GTGL_ARB_draw_buffers_blend;                /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_sample_shading;                    /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_texture_cube_map_array;            /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_texture_gather;                    /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_texture_query_lod;                 /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_shading_language_include;
extern GLboolean GTGL_ARB_texture_compression_bptc;          /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_blend_func_extended;               /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_explicit_attrib_location;          /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_occlusion_query2;                  /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_sampler_objects;                   /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_shader_bit_encoding;               /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_texture_rgb10_a2ui;                /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_texture_swizzle;                   /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_timer_query;                       /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_vertex_type_2_10_10_10_rev;        /* Core in OpenGL 3.3 */
extern GLboolean GTGL_ARB_draw_indirect;                     /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_gpu_shader5;                       /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_gpu_shader_fp64;                   /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_shader_subroutine;                 /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_tessellation_shader;               /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_texture_buffer_object_rgb32;       /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_transform_feedback2;               /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_transform_feedback3;               /* Core in OpenGL 4.0 */
extern GLboolean GTGL_ARB_ES2_compatibility;                 /* Core in OpenGL 4.1 */
extern GLboolean GTGL_ARB_get_program_binary;                /* Core in OpenGL 4.1 */
extern GLboolean GTGL_ARB_separate_shader_objects;           /* Core in OpenGL 4.1 */
extern GLboolean GTGL_ARB_shader_precision;                  /* Core in OpenGL 4.1 */
extern GLboolean GTGL_ARB_vertex_attrib_64bit;               /* Core in OpenGL 4.1 */
extern GLboolean GTGL_ARB_viewport_array;                    /* Core in OpenGL 4.1 */
extern GLboolean GTGL_ARB_cl_event;
extern GLboolean GTGL_ARB_debug_output;
extern GLboolean GTGL_ARB_robustness;
extern GLboolean GTGL_ARB_shader_stencil_export;
extern GLboolean GTGL_ARB_base_instance;                     /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_shading_language_420pack;          /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_transform_feedback_instanced;      /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_compressed_texture_pixel_storage;  /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_conservative_depth;                /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_internalformat_query;              /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_map_buffer_alignment;              /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_shader_atomic_counters;            /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_shader_image_load_store;           /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_shading_language_packing;          /* Core in OpenGL 4.2 */
extern GLboolean GTGL_ARB_texture_storage;                   /* Core in OpenGL 4.2 */

/* EXT Extensions */
extern GLboolean GTGL_EXT_abgr;

extern GLboolean GTGL_EXT_blend_color;                       /* Core in OpenGL 1.2 */
extern PFNGLBLENDCOLOREXTPROC glBlendColorEXT;

extern GLboolean GTGL_EXT_texture3D;                         /* Core in OpenGL 1.2 */
extern PFNGLTEXIMAGE3DEXTPROC    glTexImage3DEXT;
extern PFNGLTEXSUBIMAGE3DEXTPROC glTexSubImage3DEXT;

extern GLboolean GTGL_EXT_histogram;                         /* Core in OpenGL 1.2 */
extern PFNGLGETHISTOGRAMEXTPROC            glGetHistogramEXT;
extern PFNGLGETHISTOGRAMPARAMETERFVEXTPROC glGetHistogramParameterfvEXT;
extern PFNGLGETHISTOGRAMPARAMETERIVEXTPROC glGetHistogramParameterivEXT;
extern PFNGLGETMINMAXEXTPROC               glGetMinmaxEXT;
extern PFNGLGETMINMAXPARAMETERFVEXTPROC    glGetMinmaxParameterfvEXT;
extern PFNGLGETMINMAXPARAMETERIVEXTPROC    glGetMinmaxParameterivEXT;
extern PFNGLHISTOGRAMEXTPROC               glHistogramEXT;
extern PFNGLMINMAXEXTPROC                  glMinmaxEXT;
extern PFNGLRESETHISTOGRAMEXTPROC          glResetHistogramEXT;
extern PFNGLRESETMINMAXEXTPROC             glResetMinmaxEXT;

extern GLboolean GTGL_EXT_convolution;                       /* Core in OpenGL 1.2 */
extern PFNGLCONVOLUTIONFILTER1DEXTPROC       glConvolutionFilter1DEXT;
extern PFNGLCONVOLUTIONFILTER2DEXTPROC       glConvolutionFilter2DEXT;
extern PFNGLCONVOLUTIONPARAMETERFEXTPROC     glConvolutionParameterfEXT;
extern PFNGLCONVOLUTIONPARAMETERFVEXTPROC    glConvolutionParameterfvEXT;
extern PFNGLCONVOLUTIONPARAMETERIEXTPROC     glConvolutionParameteriEXT;
extern PFNGLCONVOLUTIONPARAMETERIVEXTPROC    glConvolutionParameterivEXT;
extern PFNGLCOPYCONVOLUTIONFILTER1DEXTPROC   glCopyConvolutionFilter1DEXT;
extern PFNGLCOPYCONVOLUTIONFILTER2DEXTPROC   glCopyConvolutionFilter2DEXT;
extern PFNGLGETCONVOLUTIONFILTEREXTPROC      glGetConvolutionFilterEXT;
extern PFNGLGETCONVOLUTIONPARAMETERFVEXTPROC glGetConvolutionParameterfvEXT;
extern PFNGLGETCONVOLUTIONPARAMETERIVEXTPROC glGetConvolutionParameterivEXT;
extern PFNGLGETSEPARABLEFILTEREXTPROC        glGetSeparableFilterEXT;
extern PFNGLSEPARABLEFILTER2DEXTPROC         glSeparableFilter2DEXT;

extern GLboolean GTGL_EXT_cmyka;
extern GLboolean GTGL_EXT_packed_pixels;                     /* Core in OpenGL 1.2 */
extern GLboolean GTGL_EXT_rescale_normal;                    /* Core in OpenGL 1.2 */
extern GLboolean GTGL_EXT_misc_attribute;

extern GLboolean GTGL_EXT_blend_minmax;                      /* Core in OpenGL 1.2 */
extern PFNGLBLENDEQUATIONEXTPROC glBlendEquationEXT;

extern GLboolean GTGL_EXT_blend_subtract;                    /* Core in OpenGL 1.2 */
extern GLboolean GTGL_EXT_blend_logic_op;

extern GLboolean GTGL_EXT_point_parameters;
extern PFNGLPOINTPARAMETERFEXTPROC  glPointParameterfEXT;
extern PFNGLPOINTPARAMETERFVEXTPROC glPointParameterfvEXT;

extern GLboolean GTGL_EXT_color_subtable;                    /* Core in OpenGL 1.2 */
extern PFNGLCOLORSUBTABLEEXTPROC     glColorSubTableEXT;
extern PFNGLCOPYCOLORSUBTABLEEXTPROC glCopyColorSubTableEXT;

extern GLboolean GTGL_EXT_paletted_texture;
extern PFNGLCOLORTABLEEXTPROC               glColorTableEXT;
extern PFNGLGETCOLORTABLEEXTPROC            glGetColorTableEXT;
extern PFNGLGETCOLORTABLEPARAMETERIVEXTPROC glGetColorTableParameterivEXT;
extern PFNGLGETCOLORTABLEPARAMETERFVEXTPROC glGetColorTableParameterfvEXT;

extern GLboolean GTGL_EXT_clip_volume_hint;
extern GLboolean GTGL_EXT_index_texture;

extern GLboolean GTGL_EXT_index_material;
extern PFNGLINDEXMATERIALEXTPROC glIndexMaterialEXT;

extern GLboolean GTGL_EXT_index_func;
extern PFNGLINDEXFUNCEXTPROC glIndexFuncEXT;

extern GLboolean GTGL_EXT_index_array_formats;

extern GLboolean GTGL_EXT_compiled_vertex_array;
extern PFNGLLOCKARRAYSEXTPROC   glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

extern GLboolean GTGL_EXT_cull_vertex;
extern PFNGLCULLPARAMETERDVEXTPROC glCullParameterdvEXT;
extern PFNGLCULLPARAMETERFVEXTPROC glCullParameterfvEXT;

extern GLboolean GTGL_EXT_draw_range_elements;               /* Core in OpenGL 1.2 */
extern PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT;

extern GLboolean GTGL_EXT_bgra;                              /* Core in OpenGL 1.2 */
extern GLboolean GTGL_EXT_pixel_transform_color_table;
extern GLboolean GTGL_EXT_shared_texture_palette;
extern GLboolean GTGL_EXT_separate_specular_color;           /* Core in OpenGL 1.2 */

extern GLboolean GTGL_EXT_secondary_color;                   /* Core in OpenGL 1.4 */
extern PFNGLSECONDARYCOLOR3BEXTPROC      glSecondaryColor3bEXT;
extern PFNGLSECONDARYCOLOR3BVEXTPROC     glSecondaryColor3bvEXT;
extern PFNGLSECONDARYCOLOR3DEXTPROC      glSecondaryColor3dEXT;
extern PFNGLSECONDARYCOLOR3DVEXTPROC     glSecondaryColor3dvEXT;
extern PFNGLSECONDARYCOLOR3FEXTPROC      glSecondaryColor3fEXT;
extern PFNGLSECONDARYCOLOR3FVEXTPROC     glSecondaryColor3fvEXT;
extern PFNGLSECONDARYCOLOR3IEXTPROC      glSecondaryColor3iEXT;
extern PFNGLSECONDARYCOLOR3IVEXTPROC     glSecondaryColor3ivEXT;
extern PFNGLSECONDARYCOLOR3SEXTPROC      glSecondaryColor3sEXT;
extern PFNGLSECONDARYCOLOR3SVEXTPROC     glSecondaryColor3svEXT;
extern PFNGLSECONDARYCOLOR3UBEXTPROC     glSecondaryColor3ubEXT;
extern PFNGLSECONDARYCOLOR3UBVEXTPROC    glSecondaryColor3ubvEXT;
extern PFNGLSECONDARYCOLOR3UIEXTPROC     glSecondaryColor3uiEXT;
extern PFNGLSECONDARYCOLOR3UIVEXTPROC    glSecondaryColor3uivEXT;
extern PFNGLSECONDARYCOLOR3USEXTPROC     glSecondaryColor3usEXT;
extern PFNGLSECONDARYCOLOR3USVEXTPROC    glSecondaryColor3usvEXT;
extern PFNGLSECONDARYCOLORPOINTEREXTPROC glSecondaryColorPointerEXT;

extern GLboolean GTGL_EXT_texture_perturb_normal;
extern PFNGLTEXTURENORMALEXTPROC glTextureNormalEXT;

extern GLboolean GTGL_EXT_multi_draw_arrays;                 /* Core in OpenGL 1.4 */
extern PFNGLMULTIDRAWARRAYSEXTPROC   glMultiDrawArraysEXT;
extern PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;

extern GLboolean GTGL_EXT_fog_coord;                         /* Core in OpenGL 1.4 */
extern PFNGLFOGCOORDFEXTPROC       glFogCoordfEXT;
extern PFNGLFOGCOORDFVEXTPROC      glFogCoordfvEXT;
extern PFNGLFOGCOORDDEXTPROC       glFogCoorddEXT;
extern PFNGLFOGCOORDDVEXTPROC      glFogCoorddvEXT;
extern PFNGLFOGCOORDPOINTEREXTPROC glFogCoordPointerEXT;

extern GLboolean GTGL_EXT_coordinate_frame;
extern PFNGLTANGENT3BEXTPROC       glTangent3bEXT;
extern PFNGLTANGENT3BVEXTPROC      glTangent3bvEXT;
extern PFNGLTANGENT3DEXTPROC       glTangent3dEXT;
extern PFNGLTANGENT3DVEXTPROC      glTangent3dvEXT;
extern PFNGLTANGENT3FEXTPROC       glTangent3fEXT;
extern PFNGLTANGENT3FVEXTPROC      glTangent3fvEXT;
extern PFNGLTANGENT3IEXTPROC       glTangent3iEXT;
extern PFNGLTANGENT3IVEXTPROC      glTangent3ivEXT;
extern PFNGLTANGENT3SEXTPROC       glTangent3sEXT;
extern PFNGLTANGENT3SVEXTPROC      glTangent3svEXT;
extern PFNGLBINORMAL3BEXTPROC      glBinormal3bEXT;
extern PFNGLBINORMAL3BVEXTPROC     glBinormal3bvEXT;
extern PFNGLBINORMAL3DEXTPROC      glBinormal3dEXT;
extern PFNGLBINORMAL3DVEXTPROC     glBinormal3dvEXT;
extern PFNGLBINORMAL3FEXTPROC      glBinormal3fEXT;
extern PFNGLBINORMAL3FVEXTPROC     glBinormal3fvEXT;
extern PFNGLBINORMAL3IEXTPROC      glBinormal3iEXT;
extern PFNGLBINORMAL3IVEXTPROC     glBinormal3ivEXT;
extern PFNGLBINORMAL3SEXTPROC      glBinormal3sEXT;
extern PFNGLBINORMAL3SVEXTPROC     glBinormal3svEXT;
extern PFNGLTANGENTPOINTEREXTPROC  glTangentPointerEXT;
extern PFNGLBINORMALPOINTEREXTPROC glBinormalPointerEXT;

extern GLboolean GTGL_EXT_texture_env_combine;

extern GLboolean GTGL_EXT_blend_func_separate;               /* Core in OpenGL 1.4 */
extern PFNGLBLENDFUNCSEPARATEEXTPROC glBlendFuncSeparateEXT;

extern GLboolean GTGL_EXT_stencil_wrap;                      /* Core in OpenGL 1.4 */
extern GLboolean GTGL_EXT_422_pixels;
extern GLboolean GTGL_EXT_texture_env_add;
extern GLboolean GTGL_EXT_texture_lod_bias;                  /* Core in OpenGL 1.4 */
extern GLboolean GTGL_EXT_texture_filter_anisotropic;

extern GLboolean GTGL_EXT_vertex_weighting;
extern PFNGLVERTEXWEIGHTFEXTPROC       glVertexWeightfEXT;
extern PFNGLVERTEXWEIGHTFVEXTPROC      glVertexWeightfvEXT;
extern PFNGLVERTEXWEIGHTPOINTEREXTPROC glVertexWeightPointerEXT;

extern GLboolean GTGL_EXT_texture_compression_s3tc;

extern GLboolean GTGL_EXT_multisample;
extern PFNGLSAMPLEMASKEXTPROC    glSampleMaskEXT;
extern PFNGLSAMPLEPATTERNEXTPROC glSamplePatternEXT;

extern GLboolean GTGL_EXT_texture_env_dot3;
extern GLboolean GTGL_EXT_shadow_funcs;                      /* Core in OpenGL 1.5 */

extern GLboolean GTGL_EXT_stencil_two_side;                  /* Core in OpenGL 2.0 */
extern PFNGLACTIVESTENCILFACEEXTPROC glActiveStencilFaceEXT;

extern GLboolean GTGL_EXT_depth_bounds_test;
extern PFNGLDEPTHBOUNDSEXTPROC glDepthBoundsEXT;

extern GLboolean GTGL_EXT_texture_mirror_clamp;

extern GLboolean GTGL_EXT_blend_equation_separate;           /* Core in OpenGL 2.0 */
extern PFNGLBLENDEQUATIONSEPARATEEXTPROC glBlendEquationSeparateEXT;

extern GLboolean GTGL_EXT_pixel_buffer_object;

extern GLboolean GTGL_EXT_framebuffer_object;                /* OpenGL 3.0 */
extern PFNGLISRENDERBUFFEREXTPROC                      glIsRenderbufferEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC                    glBindRenderbufferEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC                 glDeleteRenderbuffersEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC                    glGenRenderbuffersEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC                 glRenderbufferStorageEXT;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC          glGetRenderbufferParameterivEXT;
extern PFNGLISFRAMEBUFFEREXTPROC                       glIsFramebufferEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC                     glBindFramebufferEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC                  glDeleteFramebuffersEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC                     glGenFramebuffersEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC              glCheckFramebufferStatusEXT;
extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC                glFramebufferTexture1DEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC                glFramebufferTexture3DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC             glFramebufferRenderbufferEXT;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
extern PFNGLGENERATEMIPMAPEXTPROC                      glGenerateMipmapEXT;

extern GLboolean GTGL_EXT_packed_depth_stencil;              /* Core in OpenGL 3.0 */

extern GLboolean GTGL_EXT_stencil_clear_tag;
extern PFNGLSTENCILCLEARTAGEXTPROC glStencilClearTagEXT;

extern GLboolean GTGL_EXT_texture_sRGB;                      /* Core in OpenGL 2.1 */

extern GLboolean GTGL_EXT_framebuffer_blit;                  /* Core in OpenGL 3.0 */
extern PFNGLBLITFRAMEBUFFEREXTPROC glBlitFramebufferEXT;

extern GLboolean GTGL_EXT_framebuffer_multisample;           /* Core in OpenGL 3.0 */
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT;

extern GLboolean GTGL_EXT_timer_query;
extern PFNGLGETQUERYOBJECTI64VEXTPROC  glGetQueryObjecti64vEXT;
extern PFNGLGETQUERYOBJECTUI64VEXTPROC glGetQueryObjectui64vEXT;

extern GLboolean GTGL_EXT_gpu_program_parameters;
extern PFNGLPROGRAMENVPARAMETERS4FVEXTPROC   glProgramEnvParameters4fvEXT;
extern PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC glProgramLocalParameters4fvEXT;

extern GLboolean GTGL_EXT_geometry_shader4;
extern PFNGLPROGRAMPARAMETERIEXTPROC glProgramParameteriEXT;

extern GLboolean GTGL_EXT_gpu_shader4;                       /* Core in OpenGL 3.0 */
extern PFNGLGETUNIFORMUIVEXTPROC        glGetUniformuivEXT;
extern PFNGLBINDFRAGDATALOCATIONEXTPROC glBindFragDataLocationEXT;
extern PFNGLGETFRAGDATALOCATIONEXTPROC  glGetFragDataLocationEXT;
extern PFNGLUNIFORM1UIEXTPROC           glUniform1uiEXT;
extern PFNGLUNIFORM2UIEXTPROC           glUniform2uiEXT;
extern PFNGLUNIFORM3UIEXTPROC           glUniform3uiEXT;
extern PFNGLUNIFORM4UIEXTPROC           glUniform4uiEXT;
extern PFNGLUNIFORM1UIVEXTPROC          glUniform1uivEXT;
extern PFNGLUNIFORM2UIVEXTPROC          glUniform2uivEXT;
extern PFNGLUNIFORM3UIVEXTPROC          glUniform3uivEXT;
extern PFNGLUNIFORM4UIVEXTPROC          glUniform4uivEXT;

extern GLboolean GTGL_EXT_draw_instanced;
extern PFNGLDRAWARRAYSINSTANCEDEXTPROC   glDrawArraysInstancedEXT;
extern PFNGLDRAWELEMENTSINSTANCEDEXTPROC glDrawElementsInstancedEXT;

extern GLboolean GTGL_EXT_packed_float;                      /* Core in OpenGL 3.0 */
extern GLboolean GTGL_EXT_texture_array;                     /* Core in OpenGL 3.0 */

extern GLboolean GTGL_EXT_texture_buffer_object;
extern PFNGLTEXBUFFEREXTPROC glTexBufferEXT;

extern GLboolean GTGL_EXT_texture_compression_latc;
extern GLboolean GTGL_EXT_texture_compression_rgtc;          /* Core in OpenGL 3.0 */
extern GLboolean GTGL_EXT_texture_shared_exponent;           /* Core in OpenGL 3.0 */
extern GLboolean GTGL_EXT_framebuffer_sRGB;                  /* Core in OpenGL 3.0 */

extern GLboolean GTGL_EXT_draw_buffers2;                     /* Core in OpenGL 3.0 */
extern PFNGLCOLORMASKINDEXEDEXTPROC   glColorMaskIndexedEXT;
extern PFNGLGETBOOLEANINDEXEDVEXTPROC glGetBooleanIndexedvEXT;
extern PFNGLGETINTEGERINDEXEDVEXTPROC glGetIntegerIndexedvEXT;
extern PFNGLENABLEINDEXEDEXTPROC      glEnableIndexedEXT;
extern PFNGLDISABLEINDEXEDEXTPROC     glDisableIndexedEXT;
extern PFNGLISENABLEDINDEXEDEXTPROC   glIsEnabledIndexedEXT;

extern GLboolean GTGL_EXT_bindable_uniform;
extern PFNGLUNIFORMBUFFEREXTPROC        glUniformBufferEXT;
extern PFNGLGETUNIFORMBUFFERSIZEEXTPROC glGetUniformBufferSizeEXT;
extern PFNGLGETUNIFORMOFFSETEXTPROC     glGetUniformOffsetEXT;

extern GLboolean GTGL_EXT_texture_integer;                   /* Core in OpenGL 3.0 */
extern PFNGLTEXPARAMETERIIVEXTPROC     glTexParameterIivEXT;
extern PFNGLTEXPARAMETERIUIVEXTPROC    glTexParameterIuivEXT;
extern PFNGLGETTEXPARAMETERIIVEXTPROC  glGetTexParameterIivEXT;
extern PFNGLGETTEXPARAMETERIUIVEXTPROC glGetTexParameterIuivEXT;
extern PFNGLCLEARCOLORIIEXTPROC        glClearColorIiEXT;
extern PFNGLCLEARCOLORIUIEXTPROC       glClearColorIuiEXT;

extern GLboolean GTGL_EXT_transform_feedback;                /* Core in OpenGL 3.0 */
extern PFNGLBEGINTRANSFORMFEEDBACKEXTPROC      glBeginTransformFeedbackEXT;
extern PFNGLENDTRANSFORMFEEDBACKEXTPROC        glEndTransformFeedbackEXT;
extern PFNGLBINDBUFFERRANGEEXTPROC             glBindBufferRangeEXT;
extern PFNGLBINDBUFFEROFFSETEXTPROC            glBindBufferOffsetEXT;
extern PFNGLBINDBUFFERBASEEXTPROC              glBindBufferBaseEXT;
extern PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC   glTransformFeedbackVaryingsEXT;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC glGetTransformFeedbackVaryingEXT;

extern GLboolean GTGL_EXT_direct_state_access;
extern PFNGLCLIENTATTRIBDEFAULTEXTPROC                         glClientAttribDefaultEXT;
extern PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC                     glPushClientAttribDefaultEXT;
extern PFNGLMATRIXLOADFEXTPROC                                 glMatrixLoadfEXT;
extern PFNGLMATRIXLOADDEXTPROC                                 glMatrixLoaddEXT;
extern PFNGLMATRIXMULTFEXTPROC                                 glMatrixMultfEXT;
extern PFNGLMATRIXMULTDEXTPROC                                 glMatrixMultdEXT;
extern PFNGLMATRIXLOADIDENTITYEXTPROC                          glMatrixLoadIdentityEXT;
extern PFNGLMATRIXROTATEFEXTPROC                               glMatrixRotatefEXT;
extern PFNGLMATRIXROTATEDEXTPROC                               glMatrixRotatedEXT;
extern PFNGLMATRIXSCALEFEXTPROC                                glMatrixScalefEXT;
extern PFNGLMATRIXSCALEDEXTPROC                                glMatrixScaledEXT;
extern PFNGLMATRIXTRANSLATEFEXTPROC                            glMatrixTranslatefEXT;
extern PFNGLMATRIXTRANSLATEDEXTPROC                            glMatrixTranslatedEXT;
extern PFNGLMATRIXFRUSTUMEXTPROC                               glMatrixFrustumEXT;
extern PFNGLMATRIXORTHOEXTPROC                                 glMatrixOrthoEXT;
extern PFNGLMATRIXPOPEXTPROC                                   glMatrixPopEXT;
extern PFNGLMATRIXPUSHEXTPROC                                  glMatrixPushEXT;
extern PFNGLMATRIXLOADTRANSPOSEFEXTPROC                        glMatrixLoadTransposefEXT;
extern PFNGLMATRIXLOADTRANSPOSEDEXTPROC                        glMatrixLoadTransposedEXT;
extern PFNGLMATRIXMULTTRANSPOSEFEXTPROC                        glMatrixMultTransposefEXT;
extern PFNGLMATRIXMULTTRANSPOSEDEXTPROC                        glMatrixMultTransposedEXT;
extern PFNGLTEXTUREPARAMETERFEXTPROC                           glTextureParameterfEXT;
extern PFNGLTEXTUREPARAMETERFVEXTPROC                          glTextureParameterfvEXT;
extern PFNGLTEXTUREPARAMETERIEXTPROC                           glTextureParameteriEXT;
extern PFNGLTEXTUREPARAMETERIVEXTPROC                          glTextureParameterivEXT;
extern PFNGLTEXTUREIMAGE1DEXTPROC                              glTextureImage1DEXT;
extern PFNGLTEXTUREIMAGE2DEXTPROC                              glTextureImage2DEXT;
extern PFNGLTEXTURESUBIMAGE1DEXTPROC                           glTextureSubImage1DEXT;
extern PFNGLTEXTURESUBIMAGE2DEXTPROC                           glTextureSubImage2DEXT;
extern PFNGLCOPYTEXTUREIMAGE1DEXTPROC                          glCopyTextureImage1DEXT;
extern PFNGLCOPYTEXTUREIMAGE2DEXTPROC                          glCopyTextureImage2DEXT;
extern PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC                       glCopyTextureSubImage1DEXT;
extern PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC                       glCopyTextureSubImage2DEXT;
extern PFNGLGETTEXTUREIMAGEEXTPROC                             glGetTextureImageEXT;
extern PFNGLGETTEXTUREPARAMETERFVEXTPROC                       glGetTextureParameterfvEXT;
extern PFNGLGETTEXTUREPARAMETERIVEXTPROC                       glGetTextureParameterivEXT;
extern PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC                  glGetTextureLevelParameterfvEXT;
extern PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC                  glGetTextureLevelParameterivEXT;
extern PFNGLTEXTUREIMAGE3DEXTPROC                              glTextureImage3DEXT;
extern PFNGLTEXTURESUBIMAGE3DEXTPROC                           glTextureSubImage3DEXT;
extern PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC                       glCopyTextureSubImage3DEXT;
extern PFNGLMULTITEXPARAMETERFEXTPROC                          glMultiTexParameterfEXT;
extern PFNGLMULTITEXPARAMETERFVEXTPROC                         glMultiTexParameterfvEXT;
extern PFNGLMULTITEXPARAMETERIEXTPROC                          glMultiTexParameteriEXT;
extern PFNGLMULTITEXPARAMETERIVEXTPROC                         glMultiTexParameterivEXT;
extern PFNGLMULTITEXIMAGE1DEXTPROC                             glMultiTexImage1DEXT;
extern PFNGLMULTITEXIMAGE2DEXTPROC                             glMultiTexImage2DEXT;
extern PFNGLMULTITEXSUBIMAGE1DEXTPROC                          glMultiTexSubImage1DEXT;
extern PFNGLMULTITEXSUBIMAGE2DEXTPROC                          glMultiTexSubImage2DEXT;
extern PFNGLCOPYMULTITEXIMAGE1DEXTPROC                         glCopyMultiTexImage1DEXT;
extern PFNGLCOPYMULTITEXIMAGE2DEXTPROC                         glCopyMultiTexImage2DEXT;
extern PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC                      glCopyMultiTexSubImage1DEXT;
extern PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC                      glCopyMultiTexSubImage2DEXT;
extern PFNGLGETMULTITEXIMAGEEXTPROC                            glGetMultiTexImageEXT;
extern PFNGLGETMULTITEXPARAMETERFVEXTPROC                      glGetMultiTexParameterfvEXT;
extern PFNGLGETMULTITEXPARAMETERIVEXTPROC                      glGetMultiTexParameterivEXT;
extern PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC                 glGetMultiTexLevelParameterfvEXT;
extern PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC                 glGetMultiTexLevelParameterivEXT;
extern PFNGLMULTITEXIMAGE3DEXTPROC                             glMultiTexImage3DEXT;
extern PFNGLMULTITEXSUBIMAGE3DEXTPROC                          glMultiTexSubImage3DEXT;
extern PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC                      glCopyMultiTexSubImage3DEXT;
extern PFNGLBINDMULTITEXTUREEXTPROC                            glBindMultiTextureEXT;
extern PFNGLENABLECLIENTSTATEINDEXEDEXTPROC                    glEnableClientStateIndexedEXT;
extern PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC                   glDisableClientStateIndexedEXT;
extern PFNGLMULTITEXCOORDPOINTEREXTPROC                        glMultiTexCoordPointerEXT;
extern PFNGLMULTITEXENVFEXTPROC                                glMultiTexEnvfEXT;
extern PFNGLMULTITEXENVFVEXTPROC                               glMultiTexEnvfvEXT;
extern PFNGLMULTITEXENVIEXTPROC                                glMultiTexEnviEXT;
extern PFNGLMULTITEXENVIVEXTPROC                               glMultiTexEnvivEXT;
extern PFNGLMULTITEXGENDEXTPROC                                glMultiTexGendEXT;
extern PFNGLMULTITEXGENDVEXTPROC                               glMultiTexGendvEXT;
extern PFNGLMULTITEXGENFEXTPROC                                glMultiTexGenfEXT;
extern PFNGLMULTITEXGENFVEXTPROC                               glMultiTexGenfvEXT;
extern PFNGLMULTITEXGENIEXTPROC                                glMultiTexGeniEXT;
extern PFNGLMULTITEXGENIVEXTPROC                               glMultiTexGenivEXT;
extern PFNGLGETMULTITEXENVFVEXTPROC                            glGetMultiTexEnvfvEXT;
extern PFNGLGETMULTITEXENVIVEXTPROC                            glGetMultiTexEnvivEXT;
extern PFNGLGETMULTITEXGENDVEXTPROC                            glGetMultiTexGendvEXT;
extern PFNGLGETMULTITEXGENFVEXTPROC                            glGetMultiTexGenfvEXT;
extern PFNGLGETMULTITEXGENIVEXTPROC                            glGetMultiTexGenivEXT;
extern PFNGLGETFLOATINDEXEDVEXTPROC                            glGetFloatIndexedvEXT;
extern PFNGLGETDOUBLEINDEXEDVEXTPROC                           glGetDoubleIndexedvEXT;
extern PFNGLGETPOINTERINDEXEDVEXTPROC                          glGetPointerIndexedvEXT;
extern PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC                    glCompressedTextureImage3DEXT;
extern PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC                    glCompressedTextureImage2DEXT;
extern PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC                    glCompressedTextureImage1DEXT;
extern PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC                 glCompressedTextureSubImage3DEXT;
extern PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC                 glCompressedTextureSubImage2DEXT;
extern PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC                 glCompressedTextureSubImage1DEXT;
extern PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC                   glGetCompressedTextureImageEXT;
extern PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC                   glCompressedMultiTexImage3DEXT;
extern PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC                   glCompressedMultiTexImage2DEXT;
extern PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC                   glCompressedMultiTexImage1DEXT;
extern PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC                glCompressedMultiTexSubImage3DEXT;
extern PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC                glCompressedMultiTexSubImage2DEXT;
extern PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC                glCompressedMultiTexSubImage1DEXT;
extern PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC                  glGetCompressedMultiTexImageEXT;
extern PFNGLNAMEDPROGRAMSTRINGEXTPROC                          glNamedProgramStringEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC                glNamedProgramLocalParameter4dEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC               glNamedProgramLocalParameter4dvEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC                glNamedProgramLocalParameter4fEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC               glNamedProgramLocalParameter4fvEXT;
extern PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC             glGetNamedProgramLocalParameterdvEXT;
extern PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC             glGetNamedProgramLocalParameterfvEXT;
extern PFNGLGETNAMEDPROGRAMIVEXTPROC                           glGetNamedProgramivEXT;
extern PFNGLGETNAMEDPROGRAMSTRINGEXTPROC                       glGetNamedProgramStringEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC              glNamedProgramLocalParameters4fvEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC               glNamedProgramLocalParameterI4iEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC              glNamedProgramLocalParameterI4ivEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC             glNamedProgramLocalParametersI4ivEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC              glNamedProgramLocalParameterI4uiEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC             glNamedProgramLocalParameterI4uivEXT;
extern PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC            glNamedProgramLocalParametersI4uivEXT;
extern PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC            glGetNamedProgramLocalParameterIivEXT;
extern PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC           glGetNamedProgramLocalParameterIuivEXT;
extern PFNGLTEXTUREPARAMETERIIVEXTPROC                         glTextureParameterIivEXT;
extern PFNGLTEXTUREPARAMETERIUIVEXTPROC                        glTextureParameterIuivEXT;
extern PFNGLGETTEXTUREPARAMETERIIVEXTPROC                      glGetTextureParameterIivEXT;
extern PFNGLGETTEXTUREPARAMETERIUIVEXTPROC                     glGetTextureParameterIuivEXT;
extern PFNGLMULTITEXPARAMETERIIVEXTPROC                        glMultiTexParameterIivEXT;
extern PFNGLMULTITEXPARAMETERIUIVEXTPROC                       glMultiTexParameterIuivEXT;
extern PFNGLGETMULTITEXPARAMETERIIVEXTPROC                     glGetMultiTexParameterIivEXT;
extern PFNGLGETMULTITEXPARAMETERIUIVEXTPROC                    glGetMultiTexParameterIuivEXT;
extern PFNGLPROGRAMUNIFORM1FEXTPROC                            glProgramUniform1fEXT;
extern PFNGLPROGRAMUNIFORM2FEXTPROC                            glProgramUniform2fEXT;
extern PFNGLPROGRAMUNIFORM3FEXTPROC                            glProgramUniform3fEXT;
extern PFNGLPROGRAMUNIFORM4FEXTPROC                            glProgramUniform4fEXT;
extern PFNGLPROGRAMUNIFORM1IEXTPROC                            glProgramUniform1iEXT;
extern PFNGLPROGRAMUNIFORM2IEXTPROC                            glProgramUniform2iEXT;
extern PFNGLPROGRAMUNIFORM3IEXTPROC                            glProgramUniform3iEXT;
extern PFNGLPROGRAMUNIFORM4IEXTPROC                            glProgramUniform4iEXT;
extern PFNGLPROGRAMUNIFORM1FVEXTPROC                           glProgramUniform1fvEXT;
extern PFNGLPROGRAMUNIFORM2FVEXTPROC                           glProgramUniform2fvEXT;
extern PFNGLPROGRAMUNIFORM3FVEXTPROC                           glProgramUniform3fvEXT;
extern PFNGLPROGRAMUNIFORM4FVEXTPROC                           glProgramUniform4fvEXT;
extern PFNGLPROGRAMUNIFORM1IVEXTPROC                           glProgramUniform1ivEXT;
extern PFNGLPROGRAMUNIFORM2IVEXTPROC                           glProgramUniform2ivEXT;
extern PFNGLPROGRAMUNIFORM3IVEXTPROC                           glProgramUniform3ivEXT;
extern PFNGLPROGRAMUNIFORM4IVEXTPROC                           glProgramUniform4ivEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC                     glProgramUniformMatrix2fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC                     glProgramUniformMatrix3fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC                     glProgramUniformMatrix4fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC                   glProgramUniformMatrix2x3fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC                   glProgramUniformMatrix3x2fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC                   glProgramUniformMatrix2x4fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC                   glProgramUniformMatrix4x2fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC                   glProgramUniformMatrix3x4fvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC                   glProgramUniformMatrix4x3fvEXT;
extern PFNGLPROGRAMUNIFORM1UIEXTPROC                           glProgramUniform1uiEXT;
extern PFNGLPROGRAMUNIFORM2UIEXTPROC                           glProgramUniform2uiEXT;
extern PFNGLPROGRAMUNIFORM3UIEXTPROC                           glProgramUniform3uiEXT;
extern PFNGLPROGRAMUNIFORM4UIEXTPROC                           glProgramUniform4uiEXT;
extern PFNGLPROGRAMUNIFORM1UIVEXTPROC                          glProgramUniform1uivEXT;
extern PFNGLPROGRAMUNIFORM2UIVEXTPROC                          glProgramUniform2uivEXT;
extern PFNGLPROGRAMUNIFORM3UIVEXTPROC                          glProgramUniform3uivEXT;
extern PFNGLPROGRAMUNIFORM4UIVEXTPROC                          glProgramUniform4uivEXT;
extern PFNGLNAMEDBUFFERDATAEXTPROC                             glNamedBufferDataEXT;
extern PFNGLNAMEDBUFFERSUBDATAEXTPROC                          glNamedBufferSubDataEXT;
extern PFNGLMAPNAMEDBUFFEREXTPROC                              glMapNamedBufferEXT;
extern PFNGLUNMAPNAMEDBUFFEREXTPROC                            glUnmapNamedBufferEXT;
extern PFNGLMAPNAMEDBUFFERRANGEEXTPROC                         glMapNamedBufferRangeEXT;
extern PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEEXTPROC                 glFlushMappedNamedBufferRangeEXT;
extern PFNGLNAMEDCOPYBUFFERSUBDATAEXTPROC                      glNamedCopyBufferSubDataEXT;
extern PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC                   glGetNamedBufferParameterivEXT;
extern PFNGLGETNAMEDBUFFERPOINTERVEXTPROC                      glGetNamedBufferPointervEXT;
extern PFNGLGETNAMEDBUFFERSUBDATAEXTPROC                       glGetNamedBufferSubDataEXT;
extern PFNGLTEXTUREBUFFEREXTPROC                               glTextureBufferEXT;
extern PFNGLMULTITEXBUFFEREXTPROC                              glMultiTexBufferEXT;
extern PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC                    glNamedRenderbufferStorageEXT;
extern PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC             glGetNamedRenderbufferParameterivEXT;
extern PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC                 glCheckNamedFramebufferStatusEXT;
extern PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC                   glNamedFramebufferTexture1DEXT;
extern PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC                   glNamedFramebufferTexture2DEXT;
extern PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC                   glNamedFramebufferTexture3DEXT;
extern PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC                glNamedFramebufferRenderbufferEXT;
extern PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC    glGetNamedFramebufferAttachmentParameterivEXT;
extern PFNGLGENERATETEXTUREMIPMAPEXTPROC                       glGenerateTextureMipmapEXT;
extern PFNGLGENERATEMULTITEXMIPMAPEXTPROC                      glGenerateMultiTexMipmapEXT;
extern PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC                       glFramebufferDrawBufferEXT;
extern PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC                      glFramebufferDrawBuffersEXT;
extern PFNGLFRAMEBUFFERREADBUFFEREXTPROC                       glFramebufferReadBufferEXT;
extern PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC                   glGetFramebufferParameterivEXT;
extern PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC         glNamedRenderbufferStorageMultisampleEXT;
extern PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC glNamedRenderbufferStorageMultisampleCoverageEXT;
extern PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC                     glNamedFramebufferTextureEXT;
extern PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC                glNamedFramebufferTextureLayerEXT;
extern PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC                 glNamedFramebufferTextureFaceEXT;
extern PFNGLTEXTURERENDERBUFFEREXTPROC                         glTextureRenderbufferEXT;
extern PFNGLMULTITEXRENDERBUFFEREXTPROC                        glMultiTexRenderbufferEXT;
extern PFNGLPROGRAMUNIFORM1DEXTPROC                            glProgramUniform1dEXT;
extern PFNGLPROGRAMUNIFORM2DEXTPROC                            glProgramUniform2dEXT;
extern PFNGLPROGRAMUNIFORM3DEXTPROC                            glProgramUniform3dEXT;
extern PFNGLPROGRAMUNIFORM4DEXTPROC                            glProgramUniform4dEXT;
extern PFNGLPROGRAMUNIFORM1DVEXTPROC                           glProgramUniform1dvEXT;
extern PFNGLPROGRAMUNIFORM2DVEXTPROC                           glProgramUniform2dvEXT;
extern PFNGLPROGRAMUNIFORM3DVEXTPROC                           glProgramUniform3dvEXT;
extern PFNGLPROGRAMUNIFORM4DVEXTPROC                           glProgramUniform4dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2DVEXTPROC                     glProgramUniformMatrix2dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3DVEXTPROC                     glProgramUniformMatrix3dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4DVEXTPROC                     glProgramUniformMatrix4dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2X3DVEXTPROC                   glProgramUniformMatrix2x3dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX2X4DVEXTPROC                   glProgramUniformMatrix2x4dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3X2DVEXTPROC                   glProgramUniformMatrix3x2dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX3X4DVEXTPROC                   glProgramUniformMatrix3x4dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4X2DVEXTPROC                   glProgramUniformMatrix4x2dvEXT;
extern PFNGLPROGRAMUNIFORMMATRIX4X3DVEXTPROC                   glProgramUniformMatrix4x3dvEXT;

extern GLboolean GTGL_EXT_vertex_array_bgra;
extern GLboolean GTGL_EXT_texture_swizzle;

extern GLboolean GTGL_EXT_provoking_vertex;
extern PFNGLPROVOKINGVERTEXEXTPROC glProvokingVertexEXT;

extern GLboolean GTGL_EXT_texture_snorm;

extern GLboolean GTGL_EXT_separate_shader_objects;
extern PFNGLUSESHADERPROGRAMEXTPROC    glUseShaderProgramEXT;
extern PFNGLACTIVEPROGRAMEXTPROC       glActiveProgramEXT;
extern PFNGLCREATESHADERPROGRAMEXTPROC glCreateShaderProgramEXT;

extern GLboolean GTGL_EXT_shader_image_load_store;
extern PFNGLBINDIMAGETEXTUREEXTPROC glBindImageTextureEXT;
extern PFNGLMEMORYBARRIEREXTPROC    glMemoryBarrierEXT;

extern GLboolean GTGL_EXT_vertex_attrib_64bit;
extern PFNGLVERTEXATTRIBL1DEXTPROC                glVertexAttribL1dEXT;
extern PFNGLVERTEXATTRIBL2DEXTPROC                glVertexAttribL2dEXT;
extern PFNGLVERTEXATTRIBL3DEXTPROC                glVertexAttribL3dEXT;
extern PFNGLVERTEXATTRIBL4DEXTPROC                glVertexAttribL4dEXT;
extern PFNGLVERTEXATTRIBL1DVEXTPROC               glVertexAttribL1dvEXT;
extern PFNGLVERTEXATTRIBL2DVEXTPROC               glVertexAttribL2dvEXT;
extern PFNGLVERTEXATTRIBL3DVEXTPROC               glVertexAttribL3dvEXT;
extern PFNGLVERTEXATTRIBL4DVEXTPROC               glVertexAttribL4dvEXT;
extern PFNGLVERTEXATTRIBLPOINTEREXTPROC           glVertexAttribLPointerEXT;
extern PFNGLGETVERTEXATTRIBLDVEXTPROC             glGetVertexAttribLdvEXT;
extern PFNGLVERTEXARRAYVERTEXATTRIBLOFFSETEXTPROC glVertexArrayVertexAttribLOffsetEXT;

extern GLboolean GTGL_EXT_texture_sRGB_decode;

extern GLboolean GTGL_EXT_x11_sync_object;
extern PFNGLIMPORTSYNCEXTPROC glImportSyncEXT;

extern GLboolean GTGL_EXT_framebuffer_multisample_blit_scaled;

/* SGI Extensions */
extern GLboolean GTGL_SGIS_texture_filter4;
extern PFNGLGETTEXFILTERFUNCSGISPROC glGetTexFilterFuncSGIS;
extern PFNGLTEXFILTERFUNCSGISPROC    glTexFilterFuncSGIS;

extern GLboolean GTGL_SGI_color_matrix;                      /* Core in OpenGL 1.2 */

extern GLboolean GTGL_SGI_color_table;                       /* Core in OpenGL 1.2 */
extern PFNGLCOLORTABLESGIPROC               glColorTableSGI;
extern PFNGLCOLORTABLEPARAMETERFVSGIPROC    glColorTableParameterfvSGI;
extern PFNGLCOLORTABLEPARAMETERIVSGIPROC    glColorTableParameterivSGI;
extern PFNGLCOPYCOLORTABLESGIPROC           glCopyColorTableSGI;
extern PFNGLGETCOLORTABLESGIPROC            glGetColorTableSGI;
extern PFNGLGETCOLORTABLEPARAMETERFVSGIPROC glGetColorTableParameterfvSGI;
extern PFNGLGETCOLORTABLEPARAMETERIVSGIPROC glGetColorTableParameterivSGI;

extern GLboolean GTGL_SGIX_pixel_texture;
extern PFNGLPIXELTEXGENSGIXPROC glPixelTexGenSGIX;

extern GLboolean GTGL_SGIS_pixel_texture;
extern PFNGLPIXELTEXGENPARAMETERISGISPROC     glPixelTexGenParameteriSGIS;
extern PFNGLPIXELTEXGENPARAMETERIVSGISPROC    glPixelTexGenParameterivSGIS;
extern PFNGLPIXELTEXGENPARAMETERFSGISPROC     glPixelTexGenParameterfSGIS;
extern PFNGLPIXELTEXGENPARAMETERFVSGISPROC    glPixelTexGenParameterfvSGIS;
extern PFNGLGETPIXELTEXGENPARAMETERIVSGISPROC glGetPixelTexGenParameterivSGIS;
extern PFNGLGETPIXELTEXGENPARAMETERFVSGISPROC glGetPixelTexGenParameterfvSGIS;

extern GLboolean GTGL_SGIS_texture4D;
extern PFNGLTEXIMAGE4DSGISPROC    glTexImage4DSGIS;
extern PFNGLTEXSUBIMAGE4DSGISPROC glTexSubImage4DSGIS;

extern GLboolean GTGL_SGI_texture_color_table;

extern GLboolean GTGL_SGIS_detail_texture;
extern PFNGLDETAILTEXFUNCSGISPROC    glDetailTexFuncSGIS;
extern PFNGLGETDETAILTEXFUNCSGISPROC glGetDetailTexFuncSGIS;

extern GLboolean GTGL_SGIS_sharpen_texture;
extern PFNGLSHARPENTEXFUNCSGISPROC    glSharpenTexFuncSGIS;
extern PFNGLGETSHARPENTEXFUNCSGISPROC glGetSharpenTexFuncSGIS;

extern GLboolean GTGL_SGIS_texture_lod;                      /* Core in OpenGL 1.2 */

extern GLboolean GTGL_SGIS_multisample;
extern PFNGLSAMPLEMASKSGISPROC    glSampleMaskSGIS;
extern PFNGLSAMPLEPATTERNSGISPROC glSamplePatternSGIS;

extern GLboolean GTGL_SGIS_generate_mipmap;                  /* Core in OpenGL 1.4 */
extern GLboolean GTGL_SGIX_clipmap;
extern GLboolean GTGL_SGIX_shadow;
extern GLboolean GTGL_SGIS_texture_edge_clamp;               /* Core in OpenGL 1.2 */
extern GLboolean GTGL_SGIS_texture_border_clamp;
extern GLboolean GTGL_SGIX_interlace;
extern GLboolean GTGL_SGIX_pixel_tiles;
extern GLboolean GTGL_SGIX_texture_select;

extern GLboolean GTGL_SGIX_sprite;
extern PFNGLSPRITEPARAMETERFSGIXPROC  glSpriteParameterfSGIX;
extern PFNGLSPRITEPARAMETERFVSGIXPROC glSpriteParameterfvSGIX;
extern PFNGLSPRITEPARAMETERISGIXPROC  glSpriteParameteriSGIX;
extern PFNGLSPRITEPARAMETERIVSGIXPROC glSpriteParameterivSGIX;

extern GLboolean GTGL_SGIX_texture_multi_buffer;

extern GLboolean GTGL_SGIS_point_parameters;
extern PFNGLPOINTPARAMETERFSGISPROC  glPointParameterfSGIS;
extern PFNGLPOINTPARAMETERFVSGISPROC glPointParameterfvSGIS;

extern GLboolean GTGL_SGIX_instruments;
extern PFNGLGETINSTRUMENTSSGIXPROC    glGetInstrumentsSGIX;
extern PFNGLINSTRUMENTSBUFFERSGIXPROC glInstrumentsBufferSGIX;
extern PFNGLPOLLINSTRUMENTSSGIXPROC   glPollInstrumentsSGIX;
extern PFNGLREADINSTRUMENTSSGIXPROC   glReadInstrumentsSGIX;
extern PFNGLSTARTINSTRUMENTSSGIXPROC  glStartInstrumentsSGIX;
extern PFNGLSTOPINSTRUMENTSSGIXPROC   glStopInstrumentsSGIX;

extern GLboolean GTGL_SGIX_texture_scale_bias;

extern GLboolean GTGL_SGIX_framezoom;
extern PFNGLFRAMEZOOMSGIXPROC glFrameZoomSGIX;

extern GLboolean GTGL_SGIX_tag_sample_buffer;
extern PFNGLTAGSAMPLEBUFFERSGIXPROC glTagSampleBufferSGIX;

extern GLboolean GTGL_SGIX_polynomial_ffd;
extern PFNGLDEFORMATIONMAP3DSGIXPROC           glDeformationMap3dSGIX;
extern PFNGLDEFORMATIONMAP3FSGIXPROC           glDeformationMap3fSGIX;
extern PFNGLDEFORMSGIXPROC                     glDeformSGIX;
extern PFNGLLOADIDENTITYDEFORMATIONMAPSGIXPROC glLoadIdentityDeformationMapSGIX;

extern GLboolean GTGL_SGIX_reference_plane;
extern PFNGLREFERENCEPLANESGIXPROC glReferencePlaneSGIX;

extern GLboolean GTGL_SGIX_flush_raster;
extern PFNGLFLUSHRASTERSGIXPROC glFlushRasterSGIX;

extern GLboolean GTGL_SGIX_depth_texture;

extern GLboolean GTGL_SGIS_fog_function;
extern PFNGLFOGFUNCSGISPROC    glFogFuncSGIS;
extern PFNGLGETFOGFUNCSGISPROC glGetFogFuncSGIS;

extern GLboolean GTGL_SGIX_fog_offset;
extern GLboolean GTGL_SGIX_texture_add_env;

extern GLboolean GTGL_SGIX_list_priority;
extern PFNGLGETLISTPARAMETERFVSGIXPROC glGetListParameterfvSGIX;
extern PFNGLGETLISTPARAMETERIVSGIXPROC glGetListParameterivSGIX;
extern PFNGLLISTPARAMETERFSGIXPROC     glListParameterfSGIX;
extern PFNGLLISTPARAMETERFVSGIXPROC    glListParameterfvSGIX;
extern PFNGLLISTPARAMETERISGIXPROC     glListParameteriSGIX;
extern PFNGLLISTPARAMETERIVSGIXPROC    glListParameterivSGIX;

extern GLboolean GTGL_SGIX_ir_instrument1;
extern GLboolean GTGL_SGIX_calligraphic_fragment;
extern GLboolean GTGL_SGIX_texture_lod_bias;
extern GLboolean GTGL_SGIX_shadow_ambient;
extern GLboolean GTGL_SGIX_ycrcb;

extern GLboolean GTGL_SGIX_fragment_lighting;
extern PFNGLFRAGMENTCOLORMATERIALSGIXPROC glFragmentColorMaterialSGIX;
extern PFNGLFRAGMENTLIGHTFSGIXPROC        glFragmentLightfSGIX;
extern PFNGLFRAGMENTLIGHTFVSGIXPROC       glFragmentLightfvSGIX;
extern PFNGLFRAGMENTLIGHTISGIXPROC        glFragmentLightiSGIX;
extern PFNGLFRAGMENTLIGHTIVSGIXPROC       glFragmentLightivSGIX;
extern PFNGLFRAGMENTLIGHTMODELFSGIXPROC   glFragmentLightModelfSGIX;
extern PFNGLFRAGMENTLIGHTMODELFVSGIXPROC  glFragmentLightModelfvSGIX;
extern PFNGLFRAGMENTLIGHTMODELISGIXPROC   glFragmentLightModeliSGIX;
extern PFNGLFRAGMENTLIGHTMODELIVSGIXPROC  glFragmentLightModelivSGIX;
extern PFNGLFRAGMENTMATERIALFSGIXPROC     glFragmentMaterialfSGIX;
extern PFNGLFRAGMENTMATERIALFVSGIXPROC    glFragmentMaterialfvSGIX;
extern PFNGLFRAGMENTMATERIALISGIXPROC     glFragmentMaterialiSGIX;
extern PFNGLFRAGMENTMATERIALIVSGIXPROC    glFragmentMaterialivSGIX;
extern PFNGLGETFRAGMENTLIGHTFVSGIXPROC    glGetFragmentLightfvSGIX;
extern PFNGLGETFRAGMENTLIGHTIVSGIXPROC    glGetFragmentLightivSGIX;
extern PFNGLGETFRAGMENTMATERIALFVSGIXPROC glGetFragmentMaterialfvSGIX;
extern PFNGLGETFRAGMENTMATERIALIVSGIXPROC glGetFragmentMaterialivSGIX;
extern PFNGLLIGHTENVISGIXPROC             glLightEnviSGIX;

extern GLboolean GTGL_SGIX_blend_alpha_minmax;

extern GLboolean GTGL_SGIX_async;
extern PFNGLASYNCMARKERSGIXPROC        glAsyncMarkerSGIX;
extern PFNGLFINISHASYNCSGIXPROC        glFinishAsyncSGIX;
extern PFNGLPOLLASYNCSGIXPROC          glPollAsyncSGIX;
extern PFNGLGENASYNCMARKERSSGIXPROC    glGenAsyncMarkersSGIX;
extern PFNGLDELETEASYNCMARKERSSGIXPROC glDeleteAsyncMarkersSGIX;
extern PFNGLISASYNCMARKERSGIXPROC      glIsAsyncMarkerSGIX;

extern GLboolean GTGL_SGIX_async_pixel;
extern GLboolean GTGL_SGIX_async_histogram;
extern GLboolean GTGL_SGIX_fog_scale;
extern GLboolean GTGL_SGIX_subsample;
extern GLboolean GTGL_SGIX_ycrcba;
extern GLboolean GTGL_SGIX_ycrcb_subsample;
extern GLboolean GTGL_SGIX_depth_pass_instrument;
extern GLboolean GTGL_SGIX_vertex_preclip;
extern GLboolean GTGL_SGIX_convolution_accuracy;
extern GLboolean GTGL_SGIX_resample;
extern GLboolean GTGL_SGIS_point_line_texgen;

extern GLboolean GTGL_SGIS_texture_color_mask;
extern PFNGLTEXTURECOLORMASKSGISPROC glTextureColorMaskSGIS;

extern GLboolean GTGL_SGIX_igloo_interface;
extern PFNGLIGLOOINTERFACESGIXPROC glIglooInterfaceSGIX;

extern GLboolean GTGL_SGIX_texture_coordinate_clamp;
extern GLboolean GTGL_SGIX_scalebias_hint;

/* NV Extensions */
extern GLboolean GTGL_NV_texgen_reflection;
extern GLboolean GTGL_NV_light_max_exponent;

extern GLboolean GTGL_NV_vertex_array_range;
extern PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV;
extern PFNGLVERTEXARRAYRANGENVPROC      glVertexArrayRangeNV;

extern GLboolean GTGL_NV_register_combiners;
extern PFNGLCOMBINERPARAMETERFVNVPROC              glCombinerParameterfvNV;
extern PFNGLCOMBINERPARAMETERFNVPROC               glCombinerParameterfNV;
extern PFNGLCOMBINERPARAMETERIVNVPROC              glCombinerParameterivNV;
extern PFNGLCOMBINERPARAMETERINVPROC               glCombinerParameteriNV;
extern PFNGLCOMBINERINPUTNVPROC                    glCombinerInputNV;
extern PFNGLCOMBINEROUTPUTNVPROC                   glCombinerOutputNV;
extern PFNGLFINALCOMBINERINPUTNVPROC               glFinalCombinerInputNV;
extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC      glGetCombinerInputParameterfvNV;
extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC      glGetCombinerInputParameterivNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC     glGetCombinerOutputParameterfvNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC     glGetCombinerOutputParameterivNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;

extern GLboolean GTGL_NV_fog_distance;
extern GLboolean GTGL_NV_texgen_emboss;
extern GLboolean GTGL_NV_blend_square;                       /* Core in OpenGL 1.4 */
extern GLboolean GTGL_NV_texture_env_combine4;

extern GLboolean GTGL_NV_fence;
extern PFNGLDELETEFENCESNVPROC glDeleteFencesNV;
extern PFNGLGENFENCESNVPROC    glGenFencesNV;
extern PFNGLISFENCENVPROC      glIsFenceNV;
extern PFNGLTESTFENCENVPROC    glTestFenceNV;
extern PFNGLGETFENCEIVNVPROC   glGetFenceivNV;
extern PFNGLFINISHFENCENVPROC  glFinishFenceNV;
extern PFNGLSETFENCENVPROC     glSetFenceNV;

extern GLboolean GTGL_NV_packed_depth_stencil;

extern GLboolean GTGL_NV_register_combiners2;
extern PFNGLCOMBINERSTAGEPARAMETERFVNVPROC    glCombinerStageParameterfvNV;
extern PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC glGetCombinerStageParameterfvNV;

extern GLboolean GTGL_NV_texture_compression_vtc;
extern GLboolean GTGL_NV_texture_rectangle;
extern GLboolean GTGL_NV_texture_shader;
extern GLboolean GTGL_NV_texture_shader2;
extern GLboolean GTGL_NV_vertex_array_range2;

extern GLboolean GTGL_NV_vertex_program;
extern PFNGLAREPROGRAMSRESIDENTNVPROC     glAreProgramsResidentNV;
extern PFNGLBINDPROGRAMNVPROC             glBindProgramNV;
extern PFNGLDELETEPROGRAMSNVPROC          glDeleteProgramsNV;
extern PFNGLEXECUTEPROGRAMNVPROC          glExecuteProgramNV;
extern PFNGLGENPROGRAMSNVPROC             glGenProgramsNV;
extern PFNGLGETPROGRAMPARAMETERDVNVPROC   glGetProgramParameterdvNV;
extern PFNGLGETPROGRAMPARAMETERFVNVPROC   glGetProgramParameterfvNV;
extern PFNGLGETPROGRAMIVNVPROC            glGetProgramivNV;
extern PFNGLGETPROGRAMSTRINGNVPROC        glGetProgramStringNV;
extern PFNGLGETTRACKMATRIXIVNVPROC        glGetTrackMatrixivNV;
extern PFNGLGETVERTEXATTRIBDVNVPROC       glGetVertexAttribdvNV;
extern PFNGLGETVERTEXATTRIBFVNVPROC       glGetVertexAttribfvNV;
extern PFNGLGETVERTEXATTRIBIVNVPROC       glGetVertexAttribivNV;
extern PFNGLGETVERTEXATTRIBPOINTERVNVPROC glGetVertexAttribPointervNV;
extern PFNGLISPROGRAMNVPROC               glIsProgramNV;
extern PFNGLLOADPROGRAMNVPROC             glLoadProgramNV;
extern PFNGLPROGRAMPARAMETER4DNVPROC      glProgramParameter4dNV;
extern PFNGLPROGRAMPARAMETER4DVNVPROC     glProgramParameter4dvNV;
extern PFNGLPROGRAMPARAMETER4FNVPROC      glProgramParameter4fNV;
extern PFNGLPROGRAMPARAMETER4FVNVPROC     glProgramParameter4fvNV;
extern PFNGLPROGRAMPARAMETERS4DVNVPROC    glProgramParameters4dvNV;
extern PFNGLPROGRAMPARAMETERS4FVNVPROC    glProgramParameters4fvNV;
extern PFNGLREQUESTRESIDENTPROGRAMSNVPROC glRequestResidentProgramsNV;
extern PFNGLTRACKMATRIXNVPROC             glTrackMatrixNV;
extern PFNGLVERTEXATTRIBPOINTERNVPROC     glVertexAttribPointerNV;
extern PFNGLVERTEXATTRIB1DNVPROC          glVertexAttrib1dNV;
extern PFNGLVERTEXATTRIB1DVNVPROC         glVertexAttrib1dvNV;
extern PFNGLVERTEXATTRIB1FNVPROC          glVertexAttrib1fNV;
extern PFNGLVERTEXATTRIB1FVNVPROC         glVertexAttrib1fvNV;
extern PFNGLVERTEXATTRIB1SNVPROC          glVertexAttrib1sNV;
extern PFNGLVERTEXATTRIB1SVNVPROC         glVertexAttrib1svNV;
extern PFNGLVERTEXATTRIB2DNVPROC          glVertexAttrib2dNV;
extern PFNGLVERTEXATTRIB2DVNVPROC         glVertexAttrib2dvNV;
extern PFNGLVERTEXATTRIB2FNVPROC          glVertexAttrib2fNV;
extern PFNGLVERTEXATTRIB2FVNVPROC         glVertexAttrib2fvNV;
extern PFNGLVERTEXATTRIB2SNVPROC          glVertexAttrib2sNV;
extern PFNGLVERTEXATTRIB2SVNVPROC         glVertexAttrib2svNV;
extern PFNGLVERTEXATTRIB3DNVPROC          glVertexAttrib3dNV;
extern PFNGLVERTEXATTRIB3DVNVPROC         glVertexAttrib3dvNV;
extern PFNGLVERTEXATTRIB3FNVPROC          glVertexAttrib3fNV;
extern PFNGLVERTEXATTRIB3FVNVPROC         glVertexAttrib3fvNV;
extern PFNGLVERTEXATTRIB3SNVPROC          glVertexAttrib3sNV;
extern PFNGLVERTEXATTRIB3SVNVPROC         glVertexAttrib3svNV;
extern PFNGLVERTEXATTRIB4DNVPROC          glVertexAttrib4dNV;
extern PFNGLVERTEXATTRIB4DVNVPROC         glVertexAttrib4dvNV;
extern PFNGLVERTEXATTRIB4FNVPROC          glVertexAttrib4fNV;
extern PFNGLVERTEXATTRIB4FVNVPROC         glVertexAttrib4fvNV;
extern PFNGLVERTEXATTRIB4SNVPROC          glVertexAttrib4sNV;
extern PFNGLVERTEXATTRIB4SVNVPROC         glVertexAttrib4svNV;
extern PFNGLVERTEXATTRIB4UBNVPROC         glVertexAttrib4ubNV;
extern PFNGLVERTEXATTRIB4UBVNVPROC        glVertexAttrib4ubvNV;
extern PFNGLVERTEXATTRIBS1DVNVPROC        glVertexAttribs1dvNV;
extern PFNGLVERTEXATTRIBS1FVNVPROC        glVertexAttribs1fvNV;
extern PFNGLVERTEXATTRIBS1SVNVPROC        glVertexAttribs1svNV;
extern PFNGLVERTEXATTRIBS2DVNVPROC        glVertexAttribs2dvNV;
extern PFNGLVERTEXATTRIBS2FVNVPROC        glVertexAttribs2fvNV;
extern PFNGLVERTEXATTRIBS2SVNVPROC        glVertexAttribs2svNV;
extern PFNGLVERTEXATTRIBS3DVNVPROC        glVertexAttribs3dvNV;
extern PFNGLVERTEXATTRIBS3FVNVPROC        glVertexAttribs3fvNV;
extern PFNGLVERTEXATTRIBS3SVNVPROC        glVertexAttribs3svNV;
extern PFNGLVERTEXATTRIBS4DVNVPROC        glVertexAttribs4dvNV;
extern PFNGLVERTEXATTRIBS4FVNVPROC        glVertexAttribs4fvNV;
extern PFNGLVERTEXATTRIBS4SVNVPROC        glVertexAttribs4svNV;
extern PFNGLVERTEXATTRIBS4UBVNVPROC       glVertexAttribs4ubvNV;

extern GLboolean GTGL_NV_copy_depth_to_color;
extern GLboolean GTGL_NV_multisample_filter_hint;
extern GLboolean GTGL_NV_depth_clamp;
extern GLboolean GTGL_NV_texture_shader3;
extern GLboolean GTGL_NV_vertex_program1_1;
extern GLboolean GTGL_NV_float_buffer;

extern GLboolean GTGL_NV_fragment_program;
extern PFNGLPROGRAMNAMEDPARAMETER4FNVPROC    glProgramNamedParameter4fNV;
extern PFNGLPROGRAMNAMEDPARAMETER4DNVPROC    glProgramNamedParameter4dNV;
extern PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC   glProgramNamedParameter4fvNV;
extern PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC   glProgramNamedParameter4dvNV;
extern PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC glGetProgramNamedParameterfvNV;
extern PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC glGetProgramNamedParameterdvNV;

extern GLboolean GTGL_NV_half_float;                         /* Core in OpenGL 3.0 */
extern PFNGLVERTEX2HNVPROC          glVertex2hNV;
extern PFNGLVERTEX2HVNVPROC         glVertex2hvNV;
extern PFNGLVERTEX3HNVPROC          glVertex3hNV;
extern PFNGLVERTEX3HVNVPROC         glVertex3hvNV;
extern PFNGLVERTEX4HNVPROC          glVertex4hNV;
extern PFNGLVERTEX4HVNVPROC         glVertex4hvNV;
extern PFNGLNORMAL3HNVPROC          glNormal3hNV;
extern PFNGLNORMAL3HVNVPROC         glNormal3hvNV;
extern PFNGLCOLOR3HNVPROC           glColor3hNV;
extern PFNGLCOLOR3HVNVPROC          glColor3hvNV;
extern PFNGLCOLOR4HNVPROC           glColor4hNV;
extern PFNGLCOLOR4HVNVPROC          glColor4hvNV;
extern PFNGLTEXCOORD1HNVPROC        glTexCoord1hNV;
extern PFNGLTEXCOORD1HVNVPROC       glTexCoord1hvNV;
extern PFNGLTEXCOORD2HNVPROC        glTexCoord2hNV;
extern PFNGLTEXCOORD2HVNVPROC       glTexCoord2hvNV;
extern PFNGLTEXCOORD3HNVPROC        glTexCoord3hNV;
extern PFNGLTEXCOORD3HVNVPROC       glTexCoord3hvNV;
extern PFNGLTEXCOORD4HNVPROC        glTexCoord4hNV;
extern PFNGLTEXCOORD4HVNVPROC       glTexCoord4hvNV;
extern PFNGLMULTITEXCOORD1HNVPROC   glMultiTexCoord1hNV;
extern PFNGLMULTITEXCOORD1HVNVPROC  glMultiTexCoord1hvNV;
extern PFNGLMULTITEXCOORD2HNVPROC   glMultiTexCoord2hNV;
extern PFNGLMULTITEXCOORD2HVNVPROC  glMultiTexCoord2hvNV;
extern PFNGLMULTITEXCOORD3HNVPROC   glMultiTexCoord3hNV;
extern PFNGLMULTITEXCOORD3HVNVPROC  glMultiTexCoord3hvNV;
extern PFNGLMULTITEXCOORD4HNVPROC   glMultiTexCoord4hNV;
extern PFNGLMULTITEXCOORD4HVNVPROC  glMultiTexCoord4hvNV;
extern PFNGLFOGCOORDHNVPROC         glFogCoordhNV;
extern PFNGLFOGCOORDHVNVPROC        glFogCoordhvNV;
extern PFNGLSECONDARYCOLOR3HNVPROC  glSecondaryColor3hNV;
extern PFNGLSECONDARYCOLOR3HVNVPROC glSecondaryColor3hvNV;
extern PFNGLVERTEXWEIGHTHNVPROC     glVertexWeighthNV;
extern PFNGLVERTEXWEIGHTHVNVPROC    glVertexWeighthvNV;
extern PFNGLVERTEXATTRIB1HNVPROC    glVertexAttrib1hNV;
extern PFNGLVERTEXATTRIB1HVNVPROC   glVertexAttrib1hvNV;
extern PFNGLVERTEXATTRIB2HNVPROC    glVertexAttrib2hNV;
extern PFNGLVERTEXATTRIB2HVNVPROC   glVertexAttrib2hvNV;
extern PFNGLVERTEXATTRIB3HNVPROC    glVertexAttrib3hNV;
extern PFNGLVERTEXATTRIB3HVNVPROC   glVertexAttrib3hvNV;
extern PFNGLVERTEXATTRIB4HNVPROC    glVertexAttrib4hNV;
extern PFNGLVERTEXATTRIB4HVNVPROC   glVertexAttrib4hvNV;
extern PFNGLVERTEXATTRIBS1HVNVPROC  glVertexAttribs1hvNV;
extern PFNGLVERTEXATTRIBS2HVNVPROC  glVertexAttribs2hvNV;
extern PFNGLVERTEXATTRIBS3HVNVPROC  glVertexAttribs3hvNV;
extern PFNGLVERTEXATTRIBS4HVNVPROC  glVertexAttribs4hvNV;

extern GLboolean GTGL_NV_pixel_data_range;
extern PFNGLPIXELDATARANGENVPROC      glPixelDataRangeNV;
extern PFNGLFLUSHPIXELDATARANGENVPROC glFlushPixelDataRangeNV;

extern GLboolean GTGL_NV_primitive_restart;                  /* Core in OpenGL 3.0 */
extern PFNGLPRIMITIVERESTARTNVPROC      glPrimitiveRestartNV;
extern PFNGLPRIMITIVERESTARTINDEXNVPROC glPrimitiveRestartIndexNV;

extern GLboolean GTGL_NV_texture_expand_normal;
extern GLboolean GTGL_NV_vertex_program2;
extern GLboolean GTGL_NV_fragment_program_option;
extern GLboolean GTGL_NV_fragment_program2;
extern GLboolean GTGL_NV_vertex_program2_option;
extern GLboolean GTGL_NV_vertex_program3;

extern GLboolean GTGL_NV_gpu_program4;
extern PFNGLPROGRAMLOCALPARAMETERI4INVPROC     glProgramLocalParameterI4iNV;
extern PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC    glProgramLocalParameterI4ivNV;
extern PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC   glProgramLocalParametersI4ivNV;
extern PFNGLPROGRAMLOCALPARAMETERI4UINVPROC    glProgramLocalParameterI4uiNV;
extern PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC   glProgramLocalParameterI4uivNV;
extern PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC  glProgramLocalParametersI4uivNV;
extern PFNGLPROGRAMENVPARAMETERI4INVPROC       glProgramEnvParameterI4iNV;
extern PFNGLPROGRAMENVPARAMETERI4IVNVPROC      glProgramEnvParameterI4ivNV;
extern PFNGLPROGRAMENVPARAMETERSI4IVNVPROC     glProgramEnvParametersI4ivNV;
extern PFNGLPROGRAMENVPARAMETERI4UINVPROC      glProgramEnvParameterI4uiNV;
extern PFNGLPROGRAMENVPARAMETERI4UIVNVPROC     glProgramEnvParameterI4uivNV;
extern PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC    glProgramEnvParametersI4uivNV;
extern PFNGLGETPROGRAMLOCALPARAMETERIIVNVPROC  glGetProgramLocalParameterIivNV;
extern PFNGLGETPROGRAMLOCALPARAMETERIUIVNVPROC glGetProgramLocalParameterIuivNV;
extern PFNGLGETPROGRAMENVPARAMETERIIVNVPROC    glGetProgramEnvParameterIivNV;
extern PFNGLGETPROGRAMENVPARAMETERIUIVNVPROC   glGetProgramEnvParameterIuivNV;

extern GLboolean GTGL_NV_geometry_program4;
extern PFNGLPROGRAMVERTEXLIMITNVPROC       glProgramVertexLimitNV;
extern PFNGLFRAMEBUFFERTEXTUREEXTPROC      glFramebufferTextureEXT;
extern PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC glFramebufferTextureLayerEXT;
extern PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC  glFramebufferTextureFaceEXT;

extern GLboolean GTGL_NV_vertex_program4;
extern PFNGLVERTEXATTRIBI1IEXTPROC      glVertexAttribI1iEXT;
extern PFNGLVERTEXATTRIBI2IEXTPROC      glVertexAttribI2iEXT;
extern PFNGLVERTEXATTRIBI3IEXTPROC      glVertexAttribI3iEXT;
extern PFNGLVERTEXATTRIBI4IEXTPROC      glVertexAttribI4iEXT;
extern PFNGLVERTEXATTRIBI1UIEXTPROC     glVertexAttribI1uiEXT;
extern PFNGLVERTEXATTRIBI2UIEXTPROC     glVertexAttribI2uiEXT;
extern PFNGLVERTEXATTRIBI3UIEXTPROC     glVertexAttribI3uiEXT;
extern PFNGLVERTEXATTRIBI4UIEXTPROC     glVertexAttribI4uiEXT;
extern PFNGLVERTEXATTRIBI1IVEXTPROC     glVertexAttribI1ivEXT;
extern PFNGLVERTEXATTRIBI2IVEXTPROC     glVertexAttribI2ivEXT;
extern PFNGLVERTEXATTRIBI3IVEXTPROC     glVertexAttribI3ivEXT;
extern PFNGLVERTEXATTRIBI4IVEXTPROC     glVertexAttribI4ivEXT;
extern PFNGLVERTEXATTRIBI1UIVEXTPROC    glVertexAttribI1uivEXT;
extern PFNGLVERTEXATTRIBI2UIVEXTPROC    glVertexAttribI2uivEXT;
extern PFNGLVERTEXATTRIBI3UIVEXTPROC    glVertexAttribI3uivEXT;
extern PFNGLVERTEXATTRIBI4UIVEXTPROC    glVertexAttribI4uivEXT;
extern PFNGLVERTEXATTRIBI4BVEXTPROC     glVertexAttribI4bvEXT;
extern PFNGLVERTEXATTRIBI4SVEXTPROC     glVertexAttribI4svEXT;
extern PFNGLVERTEXATTRIBI4UBVEXTPROC    glVertexAttribI4ubvEXT;
extern PFNGLVERTEXATTRIBI4USVEXTPROC    glVertexAttribI4usvEXT;
extern PFNGLVERTEXATTRIBIPOINTEREXTPROC glVertexAttribIPointerEXT;
extern PFNGLGETVERTEXATTRIBIIVEXTPROC   glGetVertexAttribIivEXT;
extern PFNGLGETVERTEXATTRIBIUIVEXTPROC  glGetVertexAttribIuivEXT;

extern GLboolean GTGL_NV_depth_buffer_float;                 /* Core in OpenGL 3.0 */
extern PFNGLDEPTHRANGEDNVPROC  glDepthRangedNV;
extern PFNGLCLEARDEPTHDNVPROC  glClearDepthdNV;
extern PFNGLDEPTHBOUNDSDNVPROC glDepthBoundsdNV;

extern GLboolean GTGL_NV_fragment_program4;

extern GLboolean GTGL_NV_framebuffer_multisample_coverage;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC glRenderbufferStorageMultisampleCoverageNV;

extern GLboolean GTGL_NV_geometry_shader4;

extern GLboolean GTGL_NV_parameter_buffer_object;
extern PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC   glProgramBufferParametersfvNV;
extern PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC  glProgramBufferParametersIivNV;
extern PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC glProgramBufferParametersIuivNV;

extern GLboolean GTGL_NV_transform_feedback;
extern PFNGLBEGINTRANSFORMFEEDBACKNVPROC         glBeginTransformFeedbackNV;
extern PFNGLENDTRANSFORMFEEDBACKNVPROC           glEndTransformFeedbackNV;
extern PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC       glTransformFeedbackAttribsNV;
extern PFNGLBINDBUFFERRANGENVPROC                glBindBufferRangeNV;
extern PFNGLBINDBUFFEROFFSETNVPROC               glBindBufferOffsetNV;
extern PFNGLBINDBUFFERBASENVPROC                 glBindBufferBaseNV;
extern PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC      glTransformFeedbackVaryingsNV;
extern PFNGLACTIVEVARYINGNVPROC                  glActiveVaryingNV;
extern PFNGLGETVARYINGLOCATIONNVPROC             glGetVaryingLocationNV;
extern PFNGLGETACTIVEVARYINGNVPROC               glGetActiveVaryingNV;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC    glGetTransformFeedbackVaryingNV;
extern PFNGLTRANSFORMFEEDBACKSTREAMATTRIBSNVPROC glTransformFeedbackStreamAttribsNV;

extern GLboolean GTGL_NV_conditional_render;                 /* Core in OpenGL 3.0 */
extern PFNGLBEGINCONDITIONALRENDERNVPROC glBeginConditionalRenderNV;
extern PFNGLENDCONDITIONALRENDERNVPROC   glEndConditionalRenderNV;

extern GLboolean GTGL_NV_present_video;
extern PFNGLPRESENTFRAMEKEYEDNVPROC    glPresentFrameKeyedNV;
extern PFNGLPRESENTFRAMEDUALFILLNVPROC glPresentFrameDualFillNV;
extern PFNGLGETVIDEOIVNVPROC           glGetVideoivNV;
extern PFNGLGETVIDEOUIVNVPROC          glGetVideouivNV;
extern PFNGLGETVIDEOI64VNVPROC         glGetVideoi64vNV;
extern PFNGLGETVIDEOUI64VNVPROC        glGetVideoui64vNV;

extern GLboolean GTGL_NV_explicit_multisample;
extern PFNGLGETMULTISAMPLEFVNVPROC  glGetMultisamplefvNV;
extern PFNGLSAMPLEMASKINDEXEDNVPROC glSampleMaskIndexedNV;
extern PFNGLTEXRENDERBUFFERNVPROC   glTexRenderbufferNV;

extern GLboolean GTGL_NV_transform_feedback2;
extern PFNGLBINDTRANSFORMFEEDBACKNVPROC    glBindTransformFeedbackNV;
extern PFNGLDELETETRANSFORMFEEDBACKSNVPROC glDeleteTransformFeedbacksNV;
extern PFNGLGENTRANSFORMFEEDBACKSNVPROC    glGenTransformFeedbacksNV;
extern PFNGLISTRANSFORMFEEDBACKNVPROC      glIsTransformFeedbackNV;
extern PFNGLPAUSETRANSFORMFEEDBACKNVPROC   glPauseTransformFeedbackNV;
extern PFNGLRESUMETRANSFORMFEEDBACKNVPROC  glResumeTransformFeedbackNV;
extern PFNGLDRAWTRANSFORMFEEDBACKNVPROC    glDrawTransformFeedbackNV;

extern GLboolean GTGL_NV_video_capture;
extern PFNGLBEGINVIDEOCAPTURENVPROC             glBeginVideoCaptureNV;
extern PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC  glBindVideoCaptureStreamBufferNV;
extern PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC glBindVideoCaptureStreamTextureNV;
extern PFNGLENDVIDEOCAPTURENVPROC               glEndVideoCaptureNV;
extern PFNGLGETVIDEOCAPTUREIVNVPROC             glGetVideoCaptureivNV;
extern PFNGLGETVIDEOCAPTURESTREAMIVNVPROC       glGetVideoCaptureStreamivNV;
extern PFNGLGETVIDEOCAPTURESTREAMFVNVPROC       glGetVideoCaptureStreamfvNV;
extern PFNGLGETVIDEOCAPTURESTREAMDVNVPROC       glGetVideoCaptureStreamdvNV;
extern PFNGLVIDEOCAPTURENVPROC                  glVideoCaptureNV;
extern PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC glVideoCaptureStreamParameterivNV;
extern PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC glVideoCaptureStreamParameterfvNV;
extern PFNGLVIDEOCAPTURESTREAMPARAMETERDVNVPROC glVideoCaptureStreamParameterdvNV;

extern GLboolean GTGL_NV_copy_image;
extern PFNGLCOPYIMAGESUBDATANVPROC glCopyImageSubDataNV;

extern GLboolean GTGL_NV_parameter_buffer_object2;

extern GLboolean GTGL_NV_shader_buffer_load;
extern PFNGLMAKEBUFFERRESIDENTNVPROC           glMakeBufferResidentNV;
extern PFNGLMAKEBUFFERNONRESIDENTNVPROC        glMakeBufferNonResidentNV;
extern PFNGLISBUFFERRESIDENTNVPROC             glIsBufferResidentNV;
extern PFNGLMAKENAMEDBUFFERRESIDENTNVPROC      glMakeNamedBufferResidentNV;
extern PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC   glMakeNamedBufferNonResidentNV;
extern PFNGLISNAMEDBUFFERRESIDENTNVPROC        glIsNamedBufferResidentNV;
extern PFNGLGETBUFFERPARAMETERUI64VNVPROC      glGetBufferParameterui64vNV;
extern PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC glGetNamedBufferParameterui64vNV;
extern PFNGLGETINTEGERUI64VNVPROC              glGetIntegerui64vNV;
extern PFNGLUNIFORMUI64NVPROC                  glUniformui64NV;
extern PFNGLUNIFORMUI64VNVPROC                 glUniformui64vNV;
extern PFNGLGETUNIFORMUI64VNVPROC              glGetUniformui64vNV;
extern PFNGLPROGRAMUNIFORMUI64NVPROC           glProgramUniformui64NV;
extern PFNGLPROGRAMUNIFORMUI64VNVPROC          glProgramUniformui64vNV;

extern GLboolean GTGL_NV_vertex_buffer_unified_memory;
extern PFNGLBUFFERADDRESSRANGENVPROC   glBufferAddressRangeNV;
extern PFNGLVERTEXFORMATNVPROC         glVertexFormatNV;
extern PFNGLNORMALFORMATNVPROC         glNormalFormatNV;
extern PFNGLCOLORFORMATNVPROC          glColorFormatNV;
extern PFNGLINDEXFORMATNVPROC          glIndexFormatNV;
extern PFNGLTEXCOORDFORMATNVPROC       glTexCoordFormatNV;
extern PFNGLEDGEFLAGFORMATNVPROC       glEdgeFlagFormatNV;
extern PFNGLSECONDARYCOLORFORMATNVPROC glSecondaryColorFormatNV;
extern PFNGLFOGCOORDFORMATNVPROC       glFogCoordFormatNV;
extern PFNGLVERTEXATTRIBFORMATNVPROC   glVertexAttribFormatNV;
extern PFNGLVERTEXATTRIBIFORMATNVPROC  glVertexAttribIFormatNV;
extern PFNGLGETINTEGERUI64I_VNVPROC    glGetIntegerui64i_vNV;

extern GLboolean GTGL_NV_texture_barrier;
extern PFNGLTEXTUREBARRIERNVPROC glTextureBarrierNV;

extern GLboolean GTGL_NV_gpu_program5;
extern PFNGLPROGRAMSUBROUTINEPARAMETERSUIVNVPROC   glProgramSubroutineParametersuivNV;
extern PFNGLGETPROGRAMSUBROUTINEPARAMETERUIVNVPROC glGetProgramSubroutineParameteruivNV;

extern GLboolean GTGL_NV_gpu_shader5;
extern PFNGLUNIFORM1I64NVPROC          glUniform1i64NV;
extern PFNGLUNIFORM2I64NVPROC          glUniform2i64NV;
extern PFNGLUNIFORM3I64NVPROC          glUniform3i64NV;
extern PFNGLUNIFORM4I64NVPROC          glUniform4i64NV;
extern PFNGLUNIFORM1I64VNVPROC         glUniform1i64vNV;
extern PFNGLUNIFORM2I64VNVPROC         glUniform2i64vNV;
extern PFNGLUNIFORM3I64VNVPROC         glUniform3i64vNV;
extern PFNGLUNIFORM4I64VNVPROC         glUniform4i64vNV;
extern PFNGLUNIFORM1UI64NVPROC         glUniform1ui64NV;
extern PFNGLUNIFORM2UI64NVPROC         glUniform2ui64NV;
extern PFNGLUNIFORM3UI64NVPROC         glUniform3ui64NV;
extern PFNGLUNIFORM4UI64NVPROC         glUniform4ui64NV;
extern PFNGLUNIFORM1UI64VNVPROC        glUniform1ui64vNV;
extern PFNGLUNIFORM2UI64VNVPROC        glUniform2ui64vNV;
extern PFNGLUNIFORM3UI64VNVPROC        glUniform3ui64vNV;
extern PFNGLUNIFORM4UI64VNVPROC        glUniform4ui64vNV;
extern PFNGLGETUNIFORMI64VNVPROC       glGetUniformi64vNV;
extern PFNGLPROGRAMUNIFORM1I64NVPROC   glProgramUniform1i64NV;
extern PFNGLPROGRAMUNIFORM2I64NVPROC   glProgramUniform2i64NV;
extern PFNGLPROGRAMUNIFORM3I64NVPROC   glProgramUniform3i64NV;
extern PFNGLPROGRAMUNIFORM4I64NVPROC   glProgramUniform4i64NV;
extern PFNGLPROGRAMUNIFORM1I64VNVPROC  glProgramUniform1i64vNV;
extern PFNGLPROGRAMUNIFORM2I64VNVPROC  glProgramUniform2i64vNV;
extern PFNGLPROGRAMUNIFORM3I64VNVPROC  glProgramUniform3i64vNV;
extern PFNGLPROGRAMUNIFORM4I64VNVPROC  glProgramUniform4i64vNV;
extern PFNGLPROGRAMUNIFORM1UI64NVPROC  glProgramUniform1ui64NV;
extern PFNGLPROGRAMUNIFORM2UI64NVPROC  glProgramUniform2ui64NV;
extern PFNGLPROGRAMUNIFORM3UI64NVPROC  glProgramUniform3ui64NV;
extern PFNGLPROGRAMUNIFORM4UI64NVPROC  glProgramUniform4ui64NV;
extern PFNGLPROGRAMUNIFORM1UI64VNVPROC glProgramUniform1ui64vNV;
extern PFNGLPROGRAMUNIFORM2UI64VNVPROC glProgramUniform2ui64vNV;
extern PFNGLPROGRAMUNIFORM3UI64VNVPROC glProgramUniform3ui64vNV;
extern PFNGLPROGRAMUNIFORM4UI64VNVPROC glProgramUniform4ui64vNV;

extern GLboolean GTGL_NV_shader_buffer_store;
extern GLboolean GTGL_NV_tessellation_program5;

extern GLboolean GTGL_NV_vertex_attrib_integer_64bit;
extern PFNGLVERTEXATTRIBL1I64NVPROC     glVertexAttribL1i64NV;
extern PFNGLVERTEXATTRIBL2I64NVPROC     glVertexAttribL2i64NV;
extern PFNGLVERTEXATTRIBL3I64NVPROC     glVertexAttribL3i64NV;
extern PFNGLVERTEXATTRIBL4I64NVPROC     glVertexAttribL4i64NV;
extern PFNGLVERTEXATTRIBL1I64VNVPROC    glVertexAttribL1i64vNV;
extern PFNGLVERTEXATTRIBL2I64VNVPROC    glVertexAttribL2i64vNV;
extern PFNGLVERTEXATTRIBL3I64VNVPROC    glVertexAttribL3i64vNV;
extern PFNGLVERTEXATTRIBL4I64VNVPROC    glVertexAttribL4i64vNV;
extern PFNGLVERTEXATTRIBL1UI64NVPROC    glVertexAttribL1ui64NV;
extern PFNGLVERTEXATTRIBL2UI64NVPROC    glVertexAttribL2ui64NV;
extern PFNGLVERTEXATTRIBL3UI64NVPROC    glVertexAttribL3ui64NV;
extern PFNGLVERTEXATTRIBL4UI64NVPROC    glVertexAttribL4ui64NV;
extern PFNGLVERTEXATTRIBL1UI64VNVPROC   glVertexAttribL1ui64vNV;
extern PFNGLVERTEXATTRIBL2UI64VNVPROC   glVertexAttribL2ui64vNV;
extern PFNGLVERTEXATTRIBL3UI64VNVPROC   glVertexAttribL3ui64vNV;
extern PFNGLVERTEXATTRIBL4UI64VNVPROC   glVertexAttribL4ui64vNV;
extern PFNGLGETVERTEXATTRIBLI64VNVPROC  glGetVertexAttribLi64vNV;
extern PFNGLGETVERTEXATTRIBLUI64VNVPROC glGetVertexAttribLui64vNV;
extern PFNGLVERTEXATTRIBLFORMATNVPROC   glVertexAttribLFormatNV;

extern GLboolean GTGL_NV_multisample_coverage;

extern GLboolean GTGL_NV_vdpau_interop;
extern PFNGLVDPAUINITNVPROC                  glVDPAUInitNV;
extern PFNGLVDPAUFININVPROC                  glVDPAUFiniNV;
extern PFNGLVDPAUREGISTERVIDEOSURFACENVPROC  glVDPAURegisterVideoSurfaceNV;
extern PFNGLVDPAUREGISTEROUTPUTSURFACENVPROC glVDPAURegisterOutputSurfaceNV;
extern PFNGLVDPAUISSURFACENVPROC             glVDPAUIsSurfaceNV;
extern PFNGLVDPAUUNREGISTERSURFACENVPROC     glVDPAUUnregisterSurfaceNV;
extern PFNGLVDPAUGETSURFACEIVNVPROC          glVDPAUGetSurfaceivNV;
extern PFNGLVDPAUSURFACEACCESSNVPROC         glVDPAUSurfaceAccessNV;
extern PFNGLVDPAUMAPSURFACESNVPROC           glVDPAUMapSurfacesNV;
extern PFNGLVDPAUUNMAPSURFACESNVPROC         glVDPAUUnmapSurfacesNV;

extern GLboolean GTGL_NV_texture_multisample;
extern PFNGLTEXIMAGE2DMULTISAMPLECOVERAGENVPROC     glTexImage2DMultisampleCoverageNV;
extern PFNGLTEXIMAGE3DMULTISAMPLECOVERAGENVPROC     glTexImage3DMultisampleCoverageNV;
extern PFNGLTEXTUREIMAGE2DMULTISAMPLENVPROC         glTextureImage2DMultisampleNV;
extern PFNGLTEXTUREIMAGE3DMULTISAMPLENVPROC         glTextureImage3DMultisampleNV;
extern PFNGLTEXTUREIMAGE2DMULTISAMPLECOVERAGENVPROC glTextureImage2DMultisampleCoverageNV;
extern PFNGLTEXTUREIMAGE3DMULTISAMPLECOVERAGENVPROC glTextureImage3DMultisampleCoverageNV;

extern GLboolean GTGL_NV_path_rendering;
extern PFNGLGENPATHSNVPROC                   glGenPathsNV;
extern PFNGLDELETEPATHSNVPROC                glDeletePathsNV;
extern PFNGLISPATHNVPROC                     glIsPathNV;
extern PFNGLPATHCOMMANDSNVPROC               glPathCommandsNV;
extern PFNGLPATHCOORDSNVPROC                 glPathCoordsNV;
extern PFNGLPATHSUBCOMMANDSNVPROC            glPathSubCommandsNV;
extern PFNGLPATHSUBCOORDSNVPROC              glPathSubCoordsNV;
extern PFNGLPATHSTRINGNVPROC                 glPathStringNV;
extern PFNGLPATHGLYPHSNVPROC                 glPathGlyphsNV;
extern PFNGLPATHGLYPHRANGENVPROC             glPathGlyphRangeNV;
extern PFNGLWEIGHTPATHSNVPROC                glWeightPathsNV;
extern PFNGLCOPYPATHNVPROC                   glCopyPathNV;
extern PFNGLINTERPOLATEPATHSNVPROC           glInterpolatePathsNV;
extern PFNGLTRANSFORMPATHNVPROC              glTransformPathNV;
extern PFNGLPATHPARAMETERIVNVPROC            glPathParameterivNV;
extern PFNGLPATHPARAMETERINVPROC             glPathParameteriNV;
extern PFNGLPATHPARAMETERFVNVPROC            glPathParameterfvNV;
extern PFNGLPATHPARAMETERFNVPROC             glPathParameterfNV;
extern PFNGLPATHDASHARRAYNVPROC              glPathDashArrayNV;
extern PFNGLPATHSTENCILFUNCNVPROC            glPathStencilFuncNV;
extern PFNGLPATHSTENCILDEPTHOFFSETNVPROC     glPathStencilDepthOffsetNV;
extern PFNGLSTENCILFILLPATHNVPROC            glStencilFillPathNV;
extern PFNGLSTENCILSTROKEPATHNVPROC          glStencilStrokePathNV;
extern PFNGLSTENCILFILLPATHINSTANCEDNVPROC   glStencilFillPathInstancedNV;
extern PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC glStencilStrokePathInstancedNV;
extern PFNGLPATHCOVERDEPTHFUNCNVPROC         glPathCoverDepthFuncNV;
extern PFNGLPATHCOLORGENNVPROC               glPathColorGenNV;
extern PFNGLPATHTEXGENNVPROC                 glPathTexGenNV;
extern PFNGLPATHFOGGENNVPROC                 glPathFogGenNV;
extern PFNGLCOVERFILLPATHNVPROC              glCoverFillPathNV;
extern PFNGLCOVERSTROKEPATHNVPROC            glCoverStrokePathNV;
extern PFNGLCOVERFILLPATHINSTANCEDNVPROC     glCoverFillPathInstancedNV;
extern PFNGLCOVERSTROKEPATHINSTANCEDNVPROC   glCoverStrokePathInstancedNV;
extern PFNGLGETPATHPARAMETERIVNVPROC         glGetPathParameterivNV;
extern PFNGLGETPATHPARAMETERFVNVPROC         glGetPathParameterfvNV;
extern PFNGLGETPATHCOMMANDSNVPROC            glGetPathCommandsNV;
extern PFNGLGETPATHCOORDSNVPROC              glGetPathCoordsNV;
extern PFNGLGETPATHDASHARRAYNVPROC           glGetPathDashArrayNV;
extern PFNGLGETPATHMETRICSNVPROC             glGetPathMetricsNV;
extern PFNGLGETPATHMETRICRANGENVPROC         glGetPathMetricRangeNV;
extern PFNGLGETPATHSPACINGNVPROC             glGetPathSpacingNV;
extern PFNGLGETPATHCOLORGENIVNVPROC          glGetPathColorGenivNV;
extern PFNGLGETPATHCOLORGENFVNVPROC          glGetPathColorGenfvNV;
extern PFNGLGETPATHTEXGENIVNVPROC            glGetPathTexGenivNV;
extern PFNGLGETPATHTEXGENFVNVPROC            glGetPathTexGenfvNV;
extern PFNGLISPOINTINFILLPATHNVPROC          glIsPointInFillPathNV;
extern PFNGLISPOINTINSTROKEPATHNVPROC        glIsPointInStrokePathNV;
extern PFNGLGETPATHLENGTHNVPROC              glGetPathLengthNV;
extern PFNGLPOINTALONGPATHNVPROC             glPointAlongPathNV;

extern GLboolean GTGL_NV_bindless_texture;
extern PFNGLGETTEXTUREHANDLENVPROC             glGetTextureHandleNV;
extern PFNGLGETTEXTURESAMPLERHANDLENVPROC      glGetTextureSamplerHandleNV;
extern PFNGLMAKETEXTUREHANDLERESIDENTNVPROC    glMakeTextureHandleResidentNV;
extern PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC glMakeTextureHandleNonResidentNV;
extern PFNGLGETIMAGEHANDLENVPROC               glGetImageHandleNV;
extern PFNGLMAKEIMAGEHANDLERESIDENTNVPROC      glMakeImageHandleResidentNV;
extern PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC   glMakeImageHandleNonResidentNV;
extern PFNGLUNIFORMHANDLEUI64NVPROC            glUniformHandleui64NV;
extern PFNGLUNIFORMHANDLEUI64VNVPROC           glUniformHandleui64vNV;
extern PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC     glProgramUniformHandleui64NV;
extern PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC    glProgramUniformHandleui64vNV;
extern PFNGLISTEXTUREHANDLERESIDENTNVPROC      glIsTextureHandleResidentNV;
extern PFNGLISIMAGEHANDLERESIDENTNVPROC        glIsImageHandleResidentNV;

extern GLboolean GTGL_NV_shader_atomic_float;


/* ATI Extensions */
extern GLboolean GTGL_ATI_texture_mirror_once;

extern GLboolean GTGL_ATI_envmap_bumpmap;
extern PFNGLTEXBUMPPARAMETERIVATIPROC    glTexBumpParameterivATI;
extern PFNGLTEXBUMPPARAMETERFVATIPROC    glTexBumpParameterfvATI;
extern PFNGLGETTEXBUMPPARAMETERIVATIPROC glGetTexBumpParameterivATI;
extern PFNGLGETTEXBUMPPARAMETERFVATIPROC glGetTexBumpParameterfvATI;

extern GLboolean GTGL_ATI_fragment_shader;
extern PFNGLGENFRAGMENTSHADERSATIPROC        glGenFragmentShadersATI;
extern PFNGLBINDFRAGMENTSHADERATIPROC        glBindFragmentShaderATI;
extern PFNGLDELETEFRAGMENTSHADERATIPROC      glDeleteFragmentShaderATI;
extern PFNGLBEGINFRAGMENTSHADERATIPROC       glBeginFragmentShaderATI;
extern PFNGLENDFRAGMENTSHADERATIPROC         glEndFragmentShaderATI;
extern PFNGLPASSTEXCOORDATIPROC              glPassTexCoordATI;
extern PFNGLSAMPLEMAPATIPROC                 glSampleMapATI;
extern PFNGLCOLORFRAGMENTOP1ATIPROC          glColorFragmentOp1ATI;
extern PFNGLCOLORFRAGMENTOP2ATIPROC          glColorFragmentOp2ATI;
extern PFNGLCOLORFRAGMENTOP3ATIPROC          glColorFragmentOp3ATI;
extern PFNGLALPHAFRAGMENTOP1ATIPROC          glAlphaFragmentOp1ATI;
extern PFNGLALPHAFRAGMENTOP2ATIPROC          glAlphaFragmentOp2ATI;
extern PFNGLALPHAFRAGMENTOP3ATIPROC          glAlphaFragmentOp3ATI;
extern PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI;

extern GLboolean GTGL_ATI_pn_triangles;
extern PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI;
extern PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI;

extern GLboolean GTGL_ATI_vertex_array_object;
extern PFNGLNEWOBJECTBUFFERATIPROC         glNewObjectBufferATI;
extern PFNGLISOBJECTBUFFERATIPROC          glIsObjectBufferATI;
extern PFNGLUPDATEOBJECTBUFFERATIPROC      glUpdateObjectBufferATI;
extern PFNGLGETOBJECTBUFFERFVATIPROC       glGetObjectBufferfvATI;
extern PFNGLGETOBJECTBUFFERIVATIPROC       glGetObjectBufferivATI;
extern PFNGLFREEOBJECTBUFFERATIPROC        glFreeObjectBufferATI;
extern PFNGLARRAYOBJECTATIPROC             glArrayObjectATI;
extern PFNGLGETARRAYOBJECTFVATIPROC        glGetArrayObjectfvATI;
extern PFNGLGETARRAYOBJECTIVATIPROC        glGetArrayObjectivATI;
extern PFNGLVARIANTARRAYOBJECTATIPROC      glVariantArrayObjectATI;
extern PFNGLGETVARIANTARRAYOBJECTFVATIPROC glGetVariantArrayObjectfvATI;
extern PFNGLGETVARIANTARRAYOBJECTIVATIPROC glGetVariantArrayObjectivATI;

extern GLboolean GTGL_ATI_vertex_streams;
extern PFNGLVERTEXSTREAM1SATIPROC           glVertexStream1sATI;
extern PFNGLVERTEXSTREAM1SVATIPROC          glVertexStream1svATI;
extern PFNGLVERTEXSTREAM1IATIPROC           glVertexStream1iATI;
extern PFNGLVERTEXSTREAM1IVATIPROC          glVertexStream1ivATI;
extern PFNGLVERTEXSTREAM1FATIPROC           glVertexStream1fATI;
extern PFNGLVERTEXSTREAM1FVATIPROC          glVertexStream1fvATI;
extern PFNGLVERTEXSTREAM1DATIPROC           glVertexStream1dATI;
extern PFNGLVERTEXSTREAM1DVATIPROC          glVertexStream1dvATI;
extern PFNGLVERTEXSTREAM2SATIPROC           glVertexStream2sATI;
extern PFNGLVERTEXSTREAM2SVATIPROC          glVertexStream2svATI;
extern PFNGLVERTEXSTREAM2IATIPROC           glVertexStream2iATI;
extern PFNGLVERTEXSTREAM2IVATIPROC          glVertexStream2ivATI;
extern PFNGLVERTEXSTREAM2FATIPROC           glVertexStream2fATI;
extern PFNGLVERTEXSTREAM2FVATIPROC          glVertexStream2fvATI;
extern PFNGLVERTEXSTREAM2DATIPROC           glVertexStream2dATI;
extern PFNGLVERTEXSTREAM2DVATIPROC          glVertexStream2dvATI;
extern PFNGLVERTEXSTREAM3SATIPROC           glVertexStream3sATI;
extern PFNGLVERTEXSTREAM3SVATIPROC          glVertexStream3svATI;
extern PFNGLVERTEXSTREAM3IATIPROC           glVertexStream3iATI;
extern PFNGLVERTEXSTREAM3IVATIPROC          glVertexStream3ivATI;
extern PFNGLVERTEXSTREAM3FATIPROC           glVertexStream3fATI;
extern PFNGLVERTEXSTREAM3FVATIPROC          glVertexStream3fvATI;
extern PFNGLVERTEXSTREAM3DATIPROC           glVertexStream3dATI;
extern PFNGLVERTEXSTREAM3DVATIPROC          glVertexStream3dvATI;
extern PFNGLVERTEXSTREAM4SATIPROC           glVertexStream4sATI;
extern PFNGLVERTEXSTREAM4SVATIPROC          glVertexStream4svATI;
extern PFNGLVERTEXSTREAM4IATIPROC           glVertexStream4iATI;
extern PFNGLVERTEXSTREAM4IVATIPROC          glVertexStream4ivATI;
extern PFNGLVERTEXSTREAM4FATIPROC           glVertexStream4fATI;
extern PFNGLVERTEXSTREAM4FVATIPROC          glVertexStream4fvATI;
extern PFNGLVERTEXSTREAM4DATIPROC           glVertexStream4dATI;
extern PFNGLVERTEXSTREAM4DVATIPROC          glVertexStream4dvATI;
extern PFNGLNORMALSTREAM3BATIPROC           glNormalStream3bATI;
extern PFNGLNORMALSTREAM3BVATIPROC          glNormalStream3bvATI;
extern PFNGLNORMALSTREAM3SATIPROC           glNormalStream3sATI;
extern PFNGLNORMALSTREAM3SVATIPROC          glNormalStream3svATI;
extern PFNGLNORMALSTREAM3IATIPROC           glNormalStream3iATI;
extern PFNGLNORMALSTREAM3IVATIPROC          glNormalStream3ivATI;
extern PFNGLNORMALSTREAM3FATIPROC           glNormalStream3fATI;
extern PFNGLNORMALSTREAM3FVATIPROC          glNormalStream3fvATI;
extern PFNGLNORMALSTREAM3DATIPROC           glNormalStream3dATI;
extern PFNGLNORMALSTREAM3DVATIPROC          glNormalStream3dvATI;
extern PFNGLCLIENTACTIVEVERTEXSTREAMATIPROC glClientActiveVertexStreamATI;
extern PFNGLVERTEXBLENDENVIATIPROC          glVertexBlendEnviATI;
extern PFNGLVERTEXBLENDENVFATIPROC          glVertexBlendEnvfATI;

extern GLboolean GTGL_ATI_element_array;
extern PFNGLELEMENTPOINTERATIPROC        glElementPointerATI;
extern PFNGLDRAWELEMENTARRAYATIPROC      glDrawElementArrayATI;
extern PFNGLDRAWRANGEELEMENTARRAYATIPROC glDrawRangeElementArrayATI;

extern GLboolean GTGL_ATI_text_fragment_shader;
extern GLboolean GTGL_ATI_pixel_format_float;
extern GLboolean GTGL_ATI_texture_env_combine3;
extern GLboolean GTGL_ATI_texture_float;

extern GLboolean GTGL_ATI_map_object_buffer;
extern PFNGLMAPOBJECTBUFFERATIPROC   glMapObjectBufferATI;
extern PFNGLUNMAPOBJECTBUFFERATIPROC glUnmapObjectBufferATI;

extern GLboolean GTGL_ATI_separate_stencil;                  /* Core in OpenGL 2.0 */
extern PFNGLSTENCILOPSEPARATEATIPROC   glStencilOpSeparateATI;
extern PFNGLSTENCILFUNCSEPARATEATIPROC glStencilFuncSeparateATI;

extern GLboolean GTGL_ATI_vertex_attrib_array_object;
extern PFNGLVERTEXATTRIBARRAYOBJECTATIPROC      glVertexAttribArrayObjectATI;
extern PFNGLGETVERTEXATTRIBARRAYOBJECTFVATIPROC glGetVertexAttribArrayObjectfvATI;
extern PFNGLGETVERTEXATTRIBARRAYOBJECTIVATIPROC glGetVertexAttribArrayObjectivATI;

extern GLboolean GTGL_ATI_meminfo;

/* AMD Extensions */
extern GLboolean GTGL_AMD_performance_monitor;
extern PFNGLGETPERFMONITORGROUPSAMDPROC        glGetPerfMonitorGroupsAMD;
extern PFNGLGETPERFMONITORCOUNTERSAMDPROC      glGetPerfMonitorCountersAMD;
extern PFNGLGETPERFMONITORGROUPSTRINGAMDPROC   glGetPerfMonitorGroupStringAMD;
extern PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC glGetPerfMonitorCounterStringAMD;
extern PFNGLGETPERFMONITORCOUNTERINFOAMDPROC   glGetPerfMonitorCounterInfoAMD;
extern PFNGLGENPERFMONITORSAMDPROC             glGenPerfMonitorsAMD;
extern PFNGLDELETEPERFMONITORSAMDPROC          glDeletePerfMonitorsAMD;
extern PFNGLSELECTPERFMONITORCOUNTERSAMDPROC   glSelectPerfMonitorCountersAMD;
extern PFNGLBEGINPERFMONITORAMDPROC            glBeginPerfMonitorAMD;
extern PFNGLENDPERFMONITORAMDPROC              glEndPerfMonitorAMD;
extern PFNGLGETPERFMONITORCOUNTERDATAAMDPROC   glGetPerfMonitorCounterDataAMD;

extern GLboolean GTGL_AMD_texture_texture4;

extern GLboolean GTGL_AMD_vertex_shader_tesselator;
extern PFNGLTESSELLATIONFACTORAMDPROC glTessellationFactorAMD;
extern PFNGLTESSELLATIONMODEAMDPROC   glTessellationModeAMD;

extern GLboolean GTGL_AMD_shader_stencil_export;
extern GLboolean GTGL_AMD_seamless_cubemap_per_texture;
extern GLboolean GTGL_AMD_conservative_depth;

extern GLboolean GTGL_AMD_name_gen_delete;
extern PFNGLGENNAMESAMDPROC    glGenNamesAMD;
extern PFNGLDELETENAMESAMDPROC glDeleteNamesAMD;
extern PFNGLISNAMEAMDPROC      glIsNameAMD;

extern GLboolean GTGL_AMD_transform_feedback3_lines_triangles;
extern GLboolean GTGL_AMD_depth_clamp_separate;
extern GLboolean GTGL_AMD_blend_minmax_factor;

extern GLboolean GTGL_AMD_sample_positions;
extern PFNGLSETMULTISAMPLEFVAMDPROC glSetMultisamplefvAMD;

extern GLboolean GTGL_AMD_multi_draw_indirect;
extern PFNGLMULTIDRAWARRAYSINDIRECTAMDPROC   glMultiDrawArraysIndirectAMD;
extern PFNGLMULTIDRAWELEMENTSINDIRECTAMDPROC glMultiDrawElementsIndirectAMD;

extern GLboolean GTGL_AMD_pinned_memory;

extern GLboolean GTGL_AMD_stencil_operation_extended;
extern PFNGLSTENCILOPVALUEAMDPROC glStencilOpValueAMD;

extern GLboolean GTGL_AMD_vertex_shader_viewport_index;
extern GLboolean GTGL_AMD_vertex_shader_layer;


/* HP Extensions */
extern GLboolean GTGL_HP_image_transform;
extern PFNGLIMAGETRANSFORMPARAMETERIHPPROC     glImageTransformParameteriHP;
extern PFNGLIMAGETRANSFORMPARAMETERFHPPROC     glImageTransformParameterfHP;
extern PFNGLIMAGETRANSFORMPARAMETERIVHPPROC    glImageTransformParameterivHP;
extern PFNGLIMAGETRANSFORMPARAMETERFVHPPROC    glImageTransformParameterfvHP;
extern PFNGLGETIMAGETRANSFORMPARAMETERIVHPPROC glGetImageTransformParameterivHP;
extern PFNGLGETIMAGETRANSFORMPARAMETERFVHPPROC glGetImageTransformParameterfvHP;

extern GLboolean GTGL_HP_convolution_border_modes;           /* Core in OpenGL 1.2 */
extern GLboolean GTGL_HP_texture_lighting;
extern GLboolean GTGL_HP_occlusion_test;

/* PGI Extensions */
extern GLboolean GTGL_PGI_vertex_hints;

extern GLboolean GTGL_PGI_misc_hints;
extern PFNGLHINTPGIPROC glHintPGI;

/* IBM Extensions */
extern GLboolean GTGL_IBM_rasterpos_clip;
extern GLboolean GTGL_IBM_cull_vertex;

extern GLboolean GTGL_IBM_multimode_draw_arrays;
extern PFNGLMULTIMODEDRAWARRAYSIBMPROC   glMultiModeDrawArraysIBM;
extern PFNGLMULTIMODEDRAWELEMENTSIBMPROC glMultiModeDrawElementsIBM;

extern GLboolean GTGL_IBM_vertex_array_lists;
extern PFNGLCOLORPOINTERLISTIBMPROC          glColorPointerListIBM;
extern PFNGLSECONDARYCOLORPOINTERLISTIBMPROC glSecondaryColorPointerListIBM;
extern PFNGLEDGEFLAGPOINTERLISTIBMPROC       glEdgeFlagPointerListIBM;
extern PFNGLFOGCOORDPOINTERLISTIBMPROC       glFogCoordPointerListIBM;
extern PFNGLINDEXPOINTERLISTIBMPROC          glIndexPointerListIBM;
extern PFNGLNORMALPOINTERLISTIBMPROC         glNormalPointerListIBM;
extern PFNGLTEXCOORDPOINTERLISTIBMPROC       glTexCoordPointerListIBM;
extern PFNGLVERTEXPOINTERLISTIBMPROC         glVertexPointerListIBM;


/* WIN Extensions */
extern GLboolean GTGL_WIN_phong_shading;
extern GLboolean GTGL_WIN_specular_fog;

/* INTEL Extensions */
extern GLboolean GTGL_INTEL_parallel_arrays;
extern PFNGLVERTEXPOINTERVINTELPROC   glVertexPointervINTEL;
extern PFNGLNORMALPOINTERVINTELPROC   glNormalPointervINTEL;
extern PFNGLCOLORPOINTERVINTELPROC    glColorPointervINTEL;
extern PFNGLTEXCOORDPOINTERVINTELPROC glTexCoordPointervINTEL;

/* REND Extensions */
extern GLboolean GTGL_REND_screen_coordinates;

/* APPLE Extensions */
extern GLboolean GTGL_APPLE_specular_vector;
extern GLboolean GTGL_APPLE_transform_hint;
extern GLboolean GTGL_APPLE_client_storage;

extern GLboolean GTGL_APPLE_element_array;
extern PFNGLELEMENTPOINTERAPPLEPROC             glElementPointerAPPLE;
extern PFNGLDRAWELEMENTARRAYAPPLEPROC           glDrawElementArrayAPPLE;
extern PFNGLDRAWRANGEELEMENTARRAYAPPLEPROC      glDrawRangeElementArrayAPPLE;
extern PFNGLMULTIDRAWELEMENTARRAYAPPLEPROC      glMultiDrawElementArrayAPPLE;
extern PFNGLMULTIDRAWRANGEELEMENTARRAYAPPLEPROC glMultiDrawRangeElementArrayAPPLE;

extern GLboolean GTGL_APPLE_fence;
extern PFNGLGENFENCESAPPLEPROC    glGenFencesAPPLE;
extern PFNGLDELETEFENCESAPPLEPROC glDeleteFencesAPPLE;
extern PFNGLSETFENCEAPPLEPROC     glSetFenceAPPLE;
extern PFNGLISFENCEAPPLEPROC      glIsFenceAPPLE;
extern PFNGLTESTFENCEAPPLEPROC    glTestFenceAPPLE;
extern PFNGLFINISHFENCEAPPLEPROC  glFinishFenceAPPLE;
extern PFNGLTESTOBJECTAPPLEPROC   glTestObjectAPPLE;
extern PFNGLFINISHOBJECTAPPLEPROC glFinishObjectAPPLE;

extern GLboolean GTGL_APPLE_vertex_array_object;             /* Core in OpenGL 3.0 */
extern PFNGLBINDVERTEXARRAYAPPLEPROC    glBindVertexArrayAPPLE;
extern PFNGLDELETEVERTEXARRAYSAPPLEPROC glDeleteVertexArraysAPPLE;
extern PFNGLGENVERTEXARRAYSAPPLEPROC    glGenVertexArraysAPPLE;
extern PFNGLISVERTEXARRAYAPPLEPROC      glIsVertexArrayAPPLE;

extern GLboolean GTGL_APPLE_vertex_array_range;
extern PFNGLVERTEXARRAYRANGEAPPLEPROC      glVertexArrayRangeAPPLE;
extern PFNGLFLUSHVERTEXARRAYRANGEAPPLEPROC glFlushVertexArrayRangeAPPLE;
extern PFNGLVERTEXARRAYPARAMETERIAPPLEPROC glVertexArrayParameteriAPPLE;

extern GLboolean GTGL_APPLE_ycbcr_422;

extern GLboolean GTGL_APPLE_flush_buffer_range;              /* Core in OpenGL 3.0 */
extern PFNGLBUFFERPARAMETERIAPPLEPROC       glBufferParameteriAPPLE;
extern PFNGLFLUSHMAPPEDBUFFERRANGEAPPLEPROC glFlushMappedBufferRangeAPPLE;

extern GLboolean GTGL_APPLE_texture_range;
extern PFNGLTEXTURERANGEAPPLEPROC            glTextureRangeAPPLE;
extern PFNGLGETTEXPARAMETERPOINTERVAPPLEPROC glGetTexParameterPointervAPPLE;

extern GLboolean GTGL_APPLE_float_pixels;

extern GLboolean GTGL_APPLE_vertex_program_evaluators;
extern PFNGLENABLEVERTEXATTRIBAPPLEPROC    glEnableVertexAttribAPPLE;
extern PFNGLDISABLEVERTEXATTRIBAPPLEPROC   glDisableVertexAttribAPPLE;
extern PFNGLISVERTEXATTRIBENABLEDAPPLEPROC glIsVertexAttribEnabledAPPLE;
extern PFNGLMAPVERTEXATTRIB1DAPPLEPROC     glMapVertexAttrib1dAPPLE;
extern PFNGLMAPVERTEXATTRIB1FAPPLEPROC     glMapVertexAttrib1fAPPLE;
extern PFNGLMAPVERTEXATTRIB2DAPPLEPROC     glMapVertexAttrib2dAPPLE;
extern PFNGLMAPVERTEXATTRIB2FAPPLEPROC     glMapVertexAttrib2fAPPLE;

extern GLboolean GTGL_APPLE_aux_depth_stencil;

extern GLboolean GTGL_APPLE_object_purgeable;
extern PFNGLOBJECTPURGEABLEAPPLEPROC      glObjectPurgeableAPPLE;
extern PFNGLOBJECTUNPURGEABLEAPPLEPROC    glObjectUnpurgeableAPPLE;
extern PFNGLGETOBJECTPARAMETERIVAPPLEPROC glGetObjectParameterivAPPLE;

extern GLboolean GTGL_APPLE_row_bytes;
extern GLboolean GTGL_APPLE_rgb_422;

/* SUN Extensions */
extern GLboolean GTGL_SUNX_constant_data;
extern PFNGLFINISHTEXTURESUNXPROC glFinishTextureSUNX;

extern GLboolean GTGL_SUN_global_alpha;
extern PFNGLGLOBALALPHAFACTORBSUNPROC  glGlobalAlphaFactorbSUN;
extern PFNGLGLOBALALPHAFACTORSSUNPROC  glGlobalAlphaFactorsSUN;
extern PFNGLGLOBALALPHAFACTORISUNPROC  glGlobalAlphaFactoriSUN;
extern PFNGLGLOBALALPHAFACTORFSUNPROC  glGlobalAlphaFactorfSUN;
extern PFNGLGLOBALALPHAFACTORDSUNPROC  glGlobalAlphaFactordSUN;
extern PFNGLGLOBALALPHAFACTORUBSUNPROC glGlobalAlphaFactorubSUN;
extern PFNGLGLOBALALPHAFACTORUSSUNPROC glGlobalAlphaFactorusSUN;
extern PFNGLGLOBALALPHAFACTORUISUNPROC glGlobalAlphaFactoruiSUN;

extern GLboolean GTGL_SUN_triangle_list;
extern PFNGLREPLACEMENTCODEUISUNPROC      glReplacementCodeuiSUN;
extern PFNGLREPLACEMENTCODEUSSUNPROC      glReplacementCodeusSUN;
extern PFNGLREPLACEMENTCODEUBSUNPROC      glReplacementCodeubSUN;
extern PFNGLREPLACEMENTCODEUIVSUNPROC     glReplacementCodeuivSUN;
extern PFNGLREPLACEMENTCODEUSVSUNPROC     glReplacementCodeusvSUN;
extern PFNGLREPLACEMENTCODEUBVSUNPROC     glReplacementCodeubvSUN;
extern PFNGLREPLACEMENTCODEPOINTERSUNPROC glReplacementCodePointerSUN;

extern GLboolean GTGL_SUN_vertex;
extern PFNGLCOLOR4UBVERTEX2FSUNPROC                                    glColor4ubVertex2fSUN;
extern PFNGLCOLOR4UBVERTEX2FVSUNPROC                                   glColor4ubVertex2fvSUN;
extern PFNGLCOLOR4UBVERTEX3FSUNPROC                                    glColor4ubVertex3fSUN;
extern PFNGLCOLOR4UBVERTEX3FVSUNPROC                                   glColor4ubVertex3fvSUN;
extern PFNGLCOLOR3FVERTEX3FSUNPROC                                     glColor3fVertex3fSUN;
extern PFNGLCOLOR3FVERTEX3FVSUNPROC                                    glColor3fVertex3fvSUN;
extern PFNGLNORMAL3FVERTEX3FSUNPROC                                    glNormal3fVertex3fSUN;
extern PFNGLNORMAL3FVERTEX3FVSUNPROC                                   glNormal3fVertex3fvSUN;
extern PFNGLCOLOR4FNORMAL3FVERTEX3FSUNPROC                             glColor4fNormal3fVertex3fSUN;
extern PFNGLCOLOR4FNORMAL3FVERTEX3FVSUNPROC                            glColor4fNormal3fVertex3fvSUN;
extern PFNGLTEXCOORD2FVERTEX3FSUNPROC                                  glTexCoord2fVertex3fSUN;
extern PFNGLTEXCOORD2FVERTEX3FVSUNPROC                                 glTexCoord2fVertex3fvSUN;
extern PFNGLTEXCOORD4FVERTEX4FSUNPROC                                  glTexCoord4fVertex4fSUN;
extern PFNGLTEXCOORD4FVERTEX4FVSUNPROC                                 glTexCoord4fVertex4fvSUN;
extern PFNGLTEXCOORD2FCOLOR4UBVERTEX3FSUNPROC                          glTexCoord2fColor4ubVertex3fSUN;
extern PFNGLTEXCOORD2FCOLOR4UBVERTEX3FVSUNPROC                         glTexCoord2fColor4ubVertex3fvSUN;
extern PFNGLTEXCOORD2FCOLOR3FVERTEX3FSUNPROC                           glTexCoord2fColor3fVertex3fSUN;
extern PFNGLTEXCOORD2FCOLOR3FVERTEX3FVSUNPROC                          glTexCoord2fColor3fVertex3fvSUN;
extern PFNGLTEXCOORD2FNORMAL3FVERTEX3FSUNPROC                          glTexCoord2fNormal3fVertex3fSUN;
extern PFNGLTEXCOORD2FNORMAL3FVERTEX3FVSUNPROC                         glTexCoord2fNormal3fVertex3fvSUN;
extern PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC                   glTexCoord2fColor4fNormal3fVertex3fSUN;
extern PFNGLTEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC                  glTexCoord2fColor4fNormal3fVertex3fvSUN;
extern PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FSUNPROC                   glTexCoord4fColor4fNormal3fVertex4fSUN;
extern PFNGLTEXCOORD4FCOLOR4FNORMAL3FVERTEX4FVSUNPROC                  glTexCoord4fColor4fNormal3fVertex4fvSUN;
extern PFNGLREPLACEMENTCODEUIVERTEX3FSUNPROC                           glReplacementCodeuiVertex3fSUN;
extern PFNGLREPLACEMENTCODEUIVERTEX3FVSUNPROC                          glReplacementCodeuiVertex3fvSUN;
extern PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FSUNPROC                   glReplacementCodeuiColor4ubVertex3fSUN;
extern PFNGLREPLACEMENTCODEUICOLOR4UBVERTEX3FVSUNPROC                  glReplacementCodeuiColor4ubVertex3fvSUN;
extern PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FSUNPROC                    glReplacementCodeuiColor3fVertex3fSUN;
extern PFNGLREPLACEMENTCODEUICOLOR3FVERTEX3FVSUNPROC                   glReplacementCodeuiColor3fVertex3fvSUN;
extern PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FSUNPROC                   glReplacementCodeuiNormal3fVertex3fSUN;
extern PFNGLREPLACEMENTCODEUINORMAL3FVERTEX3FVSUNPROC                  glReplacementCodeuiNormal3fVertex3fvSUN;
extern PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FSUNPROC            glReplacementCodeuiColor4fNormal3fVertex3fSUN;
extern PFNGLREPLACEMENTCODEUICOLOR4FNORMAL3FVERTEX3FVSUNPROC           glReplacementCodeuiColor4fNormal3fVertex3fvSUN;
extern PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FSUNPROC                 glReplacementCodeuiTexCoord2fVertex3fSUN;
extern PFNGLREPLACEMENTCODEUITEXCOORD2FVERTEX3FVSUNPROC                glReplacementCodeuiTexCoord2fVertex3fvSUN;
extern PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FSUNPROC         glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN;
extern PFNGLREPLACEMENTCODEUITEXCOORD2FNORMAL3FVERTEX3FVSUNPROC        glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN;
extern PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FSUNPROC  glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN;
extern PFNGLREPLACEMENTCODEUITEXCOORD2FCOLOR4FNORMAL3FVERTEX3FVSUNPROC glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN;

extern GLboolean GTGL_SUN_convolution_border_modes;

extern GLboolean GTGL_SUN_mesh_array;
extern PFNGLDRAWMESHARRAYSSUNPROC glDrawMeshArraysSUN;

extern GLboolean GTGL_SUN_slice_accum;

/* INGR Extensions */
extern GLboolean GTGL_INGR_blend_func_separate;
extern PFNGLBLENDFUNCSEPARATEINGRPROC glBlendFuncSeparateINGR;

extern GLboolean GTGL_INGR_color_clamp;
extern GLboolean GTGL_INGR_interlace_read;

/* MESA Extensions */
extern GLboolean GTGL_MESA_resize_buffers;
extern PFNGLRESIZEBUFFERSMESAPROC glResizeBuffersMESA;

extern GLboolean GTGL_MESA_window_pos;
extern PFNGLWINDOWPOS2DMESAPROC  glWindowPos2dMESA;
extern PFNGLWINDOWPOS2DVMESAPROC glWindowPos2dvMESA;
extern PFNGLWINDOWPOS2FMESAPROC  glWindowPos2fMESA;
extern PFNGLWINDOWPOS2FVMESAPROC glWindowPos2fvMESA;
extern PFNGLWINDOWPOS2IMESAPROC  glWindowPos2iMESA;
extern PFNGLWINDOWPOS2IVMESAPROC glWindowPos2ivMESA;
extern PFNGLWINDOWPOS2SMESAPROC  glWindowPos2sMESA;
extern PFNGLWINDOWPOS2SVMESAPROC glWindowPos2svMESA;
extern PFNGLWINDOWPOS3DMESAPROC  glWindowPos3dMESA;
extern PFNGLWINDOWPOS3DVMESAPROC glWindowPos3dvMESA;
extern PFNGLWINDOWPOS3FMESAPROC  glWindowPos3fMESA;
extern PFNGLWINDOWPOS3FVMESAPROC glWindowPos3fvMESA;
extern PFNGLWINDOWPOS3IMESAPROC  glWindowPos3iMESA;
extern PFNGLWINDOWPOS3IVMESAPROC glWindowPos3ivMESA;
extern PFNGLWINDOWPOS3SMESAPROC  glWindowPos3sMESA;
extern PFNGLWINDOWPOS3SVMESAPROC glWindowPos3svMESA;
extern PFNGLWINDOWPOS4DMESAPROC  glWindowPos4dMESA;
extern PFNGLWINDOWPOS4DVMESAPROC glWindowPos4dvMESA;
extern PFNGLWINDOWPOS4FMESAPROC  glWindowPos4fMESA;
extern PFNGLWINDOWPOS4FVMESAPROC glWindowPos4fvMESA;
extern PFNGLWINDOWPOS4IMESAPROC  glWindowPos4iMESA;
extern PFNGLWINDOWPOS4IVMESAPROC glWindowPos4ivMESA;
extern PFNGLWINDOWPOS4SMESAPROC  glWindowPos4sMESA;
extern PFNGLWINDOWPOS4SVMESAPROC glWindowPos4svMESA;

extern GLboolean GTGL_MESA_pack_invert;
extern GLboolean GTGL_MESA_ycbcr_texture;
extern GLboolean GTGL_MESAX_texture_stack;

/* 3DFX Extensions */
extern GLboolean GTGL_3DFX_texture_compression_FXT1;
extern GLboolean GTGL_3DFX_multisample;

extern GLboolean GTGL_3DFX_tbuffer;
extern PFNGLTBUFFERMASK3DFXPROC glTbufferMask3DFX;

/* OML Extensions */
extern GLboolean GTGL_OML_interlace;
extern GLboolean GTGL_OML_subsample;
extern GLboolean GTGL_OML_resample;

/* OES Extensions */
extern GLboolean GTGL_OES_read_format;

/* GREMEDY Extensions */
extern GLboolean GTGL_GREMEDY_string_marker;
extern PFNGLSTRINGMARKERGREMEDYPROC glStringMarkerGREMEDY;

extern GLboolean GTGL_GREMEDY_frame_terminator;
extern PFNGLFRAMETERMINATORGREMEDYPROC glFrameTerminatorGREMEDY;


/*
Since OpenGL 1.0/1.1 functions are defined in OpenGL32.lib and GL.a, we need to handle them a little differently. Basically, we do
our own implementation which will call either the native or debug versions.
*/
/* OpenGL 1.0 Core, Non-deprecated */
GLAPI void APIENTRY glCullFace (GLenum mode);
GLAPI void APIENTRY glFrontFace (GLenum mode);
GLAPI void APIENTRY glHint (GLenum target, GLenum mode);
GLAPI void APIENTRY glLineWidth (GLfloat width);
GLAPI void APIENTRY glPointSize (GLfloat size);
GLAPI void APIENTRY glPolygonMode (GLenum face, GLenum mode);
GLAPI void APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
GLAPI void APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
GLAPI void APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
GLAPI void APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
GLAPI void APIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLAPI void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLAPI void APIENTRY glDrawBuffer (GLenum mode);
GLAPI void APIENTRY glClear (GLbitfield mask);
GLAPI void APIENTRY glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLAPI void APIENTRY glClearStencil (GLint s);
GLAPI void APIENTRY glClearDepth (GLclampd depth);
GLAPI void APIENTRY glStencilMask (GLuint mask);
GLAPI void APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
GLAPI void APIENTRY glDepthMask (GLboolean flag);
GLAPI void APIENTRY glDisable (GLenum cap);
GLAPI void APIENTRY glEnable (GLenum cap);
GLAPI void APIENTRY glFinish (void);
GLAPI void APIENTRY glFlush (void);
GLAPI void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
GLAPI void APIENTRY glLogicOp (GLenum opcode);
GLAPI void APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
GLAPI void APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
GLAPI void APIENTRY glDepthFunc (GLenum func);
GLAPI void APIENTRY glPixelStoref (GLenum pname, GLfloat param);
GLAPI void APIENTRY glPixelStorei (GLenum pname, GLint param);
GLAPI void APIENTRY glReadBuffer (GLenum mode);
GLAPI void APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
GLAPI void APIENTRY glGetBooleanv (GLenum pname, GLboolean *params);
GLAPI void APIENTRY glGetDoublev (GLenum pname, GLdouble *params);
GLAPI GLenum APIENTRY glGetError (void);
GLAPI void APIENTRY glGetFloatv (GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetIntegerv (GLenum pname, GLint *params);
GLAPI const GLubyte * APIENTRY glGetString (GLenum name);
GLAPI void APIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
GLAPI void APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
GLAPI void APIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
GLAPI GLboolean APIENTRY glIsEnabled (GLenum cap);

/* OpenGL 1.1 Core, Non-deprecated */
GLAPI void APIENTRY glDepthRange (GLclampd near, GLclampd far);
GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count);
GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
GLAPI void APIENTRY glGetPointerv (GLenum pname, GLvoid* *params);
GLAPI void APIENTRY glPolygonOffset (GLfloat factor, GLfloat units);
GLAPI void APIENTRY glCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
GLAPI void APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
GLAPI void APIENTRY glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
GLAPI void APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void APIENTRY glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
GLAPI void APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
GLAPI void APIENTRY glBindTexture (GLenum target, GLuint texture);
GLAPI void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
GLAPI void APIENTRY glGenTextures (GLsizei n, GLuint *textures);
GLAPI GLboolean APIENTRY glIsTexture (GLuint texture);


/* OpenGL 1.2 Core, Non-deprecated */
extern PFNGLBLENDCOLORPROC        glBlendColor;
extern PFNGLBLENDEQUATIONPROC     glBlendEquation;
extern PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
extern PFNGLTEXIMAGE3DPROC        glTexImage3D;
extern PFNGLTEXSUBIMAGE3DPROC     glTexSubImage3D;
extern PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;

/* OpenGL 1.3 Core, Non-deprecated */
extern PFNGLACTIVETEXTUREPROC           glActiveTexture;
extern PFNGLSAMPLECOVERAGEPROC          glSampleCoverage;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC    glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC    glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC    glCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC glCompressedTexSubImage1D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC   glGetCompressedTexImage;

/* OpenGL 1.4 Core, Non-deprecated */
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
extern PFNGLMULTIDRAWARRAYSPROC   glMultiDrawArrays;
extern PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
extern PFNGLPOINTPARAMETERFPROC   glPointParameterf;
extern PFNGLPOINTPARAMETERFVPROC  glPointParameterfv;
extern PFNGLPOINTPARAMETERIPROC   glPointParameteri;
extern PFNGLPOINTPARAMETERIVPROC  glPointParameteriv;

/* OpenGL 1.5 Core, Non-deprecated */
extern PFNGLGENQUERIESPROC           glGenQueries;
extern PFNGLDELETEQUERIESPROC        glDeleteQueries;
extern PFNGLISQUERYPROC              glIsQuery;
extern PFNGLBEGINQUERYPROC           glBeginQuery;
extern PFNGLENDQUERYPROC             glEndQuery;
extern PFNGLGETQUERYIVPROC           glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC     glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC    glGetQueryObjectuiv;
extern PFNGLBINDBUFFERPROC           glBindBuffer;
extern PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
extern PFNGLGENBUFFERSPROC           glGenBuffers;
extern PFNGLISBUFFERPROC             glIsBuffer;
extern PFNGLBUFFERDATAPROC           glBufferData;
extern PFNGLBUFFERSUBDATAPROC        glBufferSubData;
extern PFNGLMAPBUFFERPROC            glMapBuffer;
extern PFNGLUNMAPBUFFERPROC          glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC    glGetBufferPointerv;


/* OpenGL 2.0 Core, Non-deprecated */
extern PFNGLBLENDEQUATIONSEPARATEPROC    glBlendEquationSeparate;
extern PFNGLDRAWBUFFERSPROC              glDrawBuffers;
extern PFNGLSTENCILOPSEPARATEPROC        glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC      glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC      glStencilMaskSeparate;
extern PFNGLATTACHSHADERPROC             glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC            glCompileShader;
extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
extern PFNGLCREATESHADERPROC             glCreateShader;
extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
extern PFNGLDELETESHADERPROC             glDeleteShader;
extern PFNGLDETACHSHADERPROC             glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC          glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC         glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC       glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC              glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC          glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC             glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC             glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC        glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC        glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC        glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC  glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC                glIsProgram;
extern PFNGLISSHADERPROC                 glIsShader;
extern PFNGLLINKPROGRAMPROC              glLinkProgram;
extern PFNGLSHADERSOURCEPROC             glShaderSource;
extern PFNGLUSEPROGRAMPROC               glUseProgram;
extern PFNGLUNIFORM1FPROC                glUniform1f;
extern PFNGLUNIFORM2FPROC                glUniform2f;
extern PFNGLUNIFORM3FPROC                glUniform3f;
extern PFNGLUNIFORM4FPROC                glUniform4f;
extern PFNGLUNIFORM1IPROC                glUniform1i;
extern PFNGLUNIFORM2IPROC                glUniform2i;
extern PFNGLUNIFORM3IPROC                glUniform3i;
extern PFNGLUNIFORM4IPROC                glUniform4i;
extern PFNGLUNIFORM1FVPROC               glUniform1fv;
extern PFNGLUNIFORM2FVPROC               glUniform2fv;
extern PFNGLUNIFORM3FVPROC               glUniform3fv;
extern PFNGLUNIFORM4FVPROC               glUniform4fv;
extern PFNGLUNIFORM1IVPROC               glUniform1iv;
extern PFNGLUNIFORM2IVPROC               glUniform2iv;
extern PFNGLUNIFORM3IVPROC               glUniform3iv;
extern PFNGLUNIFORM4IVPROC               glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC         glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC         glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC           glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC          glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC           glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC          glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC           glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC          glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC           glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC          glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC           glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC          glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC           glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC          glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC           glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC          glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC           glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC          glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC           glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC          glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC         glVertexAttrib4Nbv;
extern PFNGLVERTEXATTRIB4NIVPROC         glVertexAttrib4Niv;
extern PFNGLVERTEXATTRIB4NSVPROC         glVertexAttrib4Nsv;
extern PFNGLVERTEXATTRIB4NUBPROC         glVertexAttrib4Nub;
extern PFNGLVERTEXATTRIB4NUBVPROC        glVertexAttrib4Nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC        glVertexAttrib4Nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC        glVertexAttrib4Nusv;
extern PFNGLVERTEXATTRIB4BVPROC          glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC           glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC          glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC           glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC          glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC          glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC           glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC          glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC         glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC         glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC         glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;

/* OpenGL 2.1 Core, Non-deprecated */
extern PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

/* OpenGL 3.0 Core */
extern PFNGLCOLORMASKIPROC                  glColorMaski;
extern PFNGLGETBOOLEANI_VPROC               glGetBooleani_v;
extern PFNGLGETINTEGERI_VPROC               glGetIntegeri_v;
extern PFNGLENABLEIPROC                     glEnablei;
extern PFNGLDISABLEIPROC                    glDisablei;
extern PFNGLISENABLEDIPROC                  glIsEnabledi;
extern PFNGLBEGINTRANSFORMFEEDBACKPROC      glBeginTransformFeedback;
extern PFNGLENDTRANSFORMFEEDBACKPROC        glEndTransformFeedback;
extern PFNGLBINDBUFFERRANGEPROC             glBindBufferRange;
extern PFNGLBINDBUFFERBASEPROC              glBindBufferBase;
extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC   glTransformFeedbackVaryings;
extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
extern PFNGLCLAMPCOLORPROC                  glClampColor;
extern PFNGLBEGINCONDITIONALRENDERPROC      glBeginConditionalRender;
extern PFNGLENDCONDITIONALRENDERPROC        glEndConditionalRender;
extern PFNGLVERTEXATTRIBIPOINTERPROC        glVertexAttribIPointer;
extern PFNGLGETVERTEXATTRIBIIVPROC          glGetVertexAttribIiv;
extern PFNGLGETVERTEXATTRIBIUIVPROC         glGetVertexAttribIuiv;
extern PFNGLVERTEXATTRIBI1IPROC             glVertexAttribI1i;
extern PFNGLVERTEXATTRIBI2IPROC             glVertexAttribI2i;
extern PFNGLVERTEXATTRIBI3IPROC             glVertexAttribI3i;
extern PFNGLVERTEXATTRIBI4IPROC             glVertexAttribI4i;
extern PFNGLVERTEXATTRIBI1UIPROC            glVertexAttribI1ui;
extern PFNGLVERTEXATTRIBI2UIPROC            glVertexAttribI2ui;
extern PFNGLVERTEXATTRIBI3UIPROC            glVertexAttribI3ui;
extern PFNGLVERTEXATTRIBI4UIPROC            glVertexAttribI4ui;
extern PFNGLVERTEXATTRIBI1IVPROC            glVertexAttribI1iv;
extern PFNGLVERTEXATTRIBI2IVPROC            glVertexAttribI2iv;
extern PFNGLVERTEXATTRIBI3IVPROC            glVertexAttribI3iv;
extern PFNGLVERTEXATTRIBI4IVPROC            glVertexAttribI4iv;
extern PFNGLVERTEXATTRIBI1UIVPROC           glVertexAttribI1uiv;
extern PFNGLVERTEXATTRIBI2UIVPROC           glVertexAttribI2uiv;
extern PFNGLVERTEXATTRIBI3UIVPROC           glVertexAttribI3uiv;
extern PFNGLVERTEXATTRIBI4UIVPROC           glVertexAttribI4uiv;
extern PFNGLVERTEXATTRIBI4BVPROC            glVertexAttribI4bv;
extern PFNGLVERTEXATTRIBI4SVPROC            glVertexAttribI4sv;
extern PFNGLVERTEXATTRIBI4UBVPROC           glVertexAttribI4ubv;
extern PFNGLVERTEXATTRIBI4USVPROC           glVertexAttribI4usv;
extern PFNGLGETUNIFORMUIVPROC               glGetUniformuiv;
extern PFNGLBINDFRAGDATALOCATIONPROC        glBindFragDataLocation;
extern PFNGLGETFRAGDATALOCATIONPROC         glGetFragDataLocation;
extern PFNGLUNIFORM1UIPROC                  glUniform1ui;
extern PFNGLUNIFORM2UIPROC                  glUniform2ui;
extern PFNGLUNIFORM3UIPROC                  glUniform3ui;
extern PFNGLUNIFORM4UIPROC                  glUniform4ui;
extern PFNGLUNIFORM1UIVPROC                 glUniform1uiv;
extern PFNGLUNIFORM2UIVPROC                 glUniform2uiv;
extern PFNGLUNIFORM3UIVPROC                 glUniform3uiv;
extern PFNGLUNIFORM4UIVPROC                 glUniform4uiv;
extern PFNGLTEXPARAMETERIIVPROC             glTexParameterIiv;
extern PFNGLTEXPARAMETERIUIVPROC            glTexParameterIuiv;
extern PFNGLGETTEXPARAMETERIIVPROC          glGetTexParameterIiv;
extern PFNGLGETTEXPARAMETERIUIVPROC         glGetTexParameterIuiv;
extern PFNGLCLEARBUFFERIVPROC               glClearBufferiv;
extern PFNGLCLEARBUFFERUIVPROC              glClearBufferuiv;
extern PFNGLCLEARBUFFERFVPROC               glClearBufferfv;
extern PFNGLCLEARBUFFERFIPROC               glClearBufferfi;
extern PFNGLGETSTRINGIPROC                  glGetStringi;

/* OpenGL 3.1 Core */
extern PFNGLDRAWARRAYSINSTANCEDPROC   glDrawArraysInstanced;
extern PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
extern PFNGLTEXBUFFERPROC             glTexBuffer;
extern PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;

/* OpenGL 3.2 Core */
extern PFNGLGETINTEGER64I_VPROC        glGetInteger64i_v;
extern PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
extern PFNGLFRAMEBUFFERTEXTUREPROC     glFramebufferTexture;

/* OpenGL 3.3 Core */
extern PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;

/* OpenGL 4.0 Core */
extern PFNGLMINSAMPLESHADINGPROC       glMinSampleShading;
extern PFNGLBLENDEQUATIONIPROC         glBlendEquationi;
extern PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
extern PFNGLBLENDFUNCIPROC             glBlendFunci;
extern PFNGLBLENDFUNCSEPARATEIPROC     glBlendFuncSeparatei;

/* OpenGL 4.1 Core */
/* Nothing. */

/* OpenGL 4.2 Core */
/* Nothing. */



/* ARB Extensions */
/* GL_ARB_multitexture */
extern PFNGLACTIVETEXTUREARBPROC       glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
extern PFNGLMULTITEXCOORD1DARBPROC     glMultiTexCoord1dARB;
extern PFNGLMULTITEXCOORD1DVARBPROC    glMultiTexCoord1dvARB;
extern PFNGLMULTITEXCOORD1FARBPROC     glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD1FVARBPROC    glMultiTexCoord1fvARB;
extern PFNGLMULTITEXCOORD1IARBPROC     glMultiTexCoord1iARB;
extern PFNGLMULTITEXCOORD1IVARBPROC    glMultiTexCoord1ivARB;
extern PFNGLMULTITEXCOORD1SARBPROC     glMultiTexCoord1sARB;
extern PFNGLMULTITEXCOORD1SVARBPROC    glMultiTexCoord1svARB;
extern PFNGLMULTITEXCOORD2DARBPROC     glMultiTexCoord2dARB;
extern PFNGLMULTITEXCOORD2DVARBPROC    glMultiTexCoord2dvARB;
extern PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2FVARBPROC    glMultiTexCoord2fvARB;
extern PFNGLMULTITEXCOORD2IARBPROC     glMultiTexCoord2iARB;
extern PFNGLMULTITEXCOORD2IVARBPROC    glMultiTexCoord2ivARB;
extern PFNGLMULTITEXCOORD2SARBPROC     glMultiTexCoord2sARB;
extern PFNGLMULTITEXCOORD2SVARBPROC    glMultiTexCoord2svARB;
extern PFNGLMULTITEXCOORD3DARBPROC     glMultiTexCoord3dARB;
extern PFNGLMULTITEXCOORD3DVARBPROC    glMultiTexCoord3dvARB;
extern PFNGLMULTITEXCOORD3FARBPROC     glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD3FVARBPROC    glMultiTexCoord3fvARB;
extern PFNGLMULTITEXCOORD3IARBPROC     glMultiTexCoord3iARB;
extern PFNGLMULTITEXCOORD3IVARBPROC    glMultiTexCoord3ivARB;
extern PFNGLMULTITEXCOORD3SARBPROC     glMultiTexCoord3sARB;
extern PFNGLMULTITEXCOORD3SVARBPROC    glMultiTexCoord3svARB;
extern PFNGLMULTITEXCOORD4DARBPROC     glMultiTexCoord4dARB;
extern PFNGLMULTITEXCOORD4DVARBPROC    glMultiTexCoord4dvARB;
extern PFNGLMULTITEXCOORD4FARBPROC     glMultiTexCoord4fARB;
extern PFNGLMULTITEXCOORD4FVARBPROC    glMultiTexCoord4fvARB;
extern PFNGLMULTITEXCOORD4IARBPROC     glMultiTexCoord4iARB;
extern PFNGLMULTITEXCOORD4IVARBPROC    glMultiTexCoord4ivARB;
extern PFNGLMULTITEXCOORD4SARBPROC     glMultiTexCoord4sARB;
extern PFNGLMULTITEXCOORD4SVARBPROC    glMultiTexCoord4svARB;

/* GL_ARB_transpose_matrix */
extern PFNGLLOADTRANSPOSEMATRIXFARBPROC glLoadTransposeMatrixfARB;
extern PFNGLLOADTRANSPOSEMATRIXDARBPROC glLoadTransposeMatrixdARB;
extern PFNGLMULTTRANSPOSEMATRIXFARBPROC glMultTransposeMatrixfARB;
extern PFNGLMULTTRANSPOSEMATRIXDARBPROC glMultTransposeMatrixdARB;

/* GL_ARB_multisample */
extern PFNGLSAMPLECOVERAGEARBPROC glSampleCoverageARB;

/* GL_ARB_texture_compression */
extern PFNGLCOMPRESSEDTEXIMAGE3DARBPROC    glCompressedTexImage3DARB;
extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    glCompressedTexImage2DARB;
extern PFNGLCOMPRESSEDTEXIMAGE1DARBPROC    glCompressedTexImage1DARB;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC glCompressedTexSubImage3DARB;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC glCompressedTexSubImage1DARB;
extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   glGetCompressedTexImageARB;

/* GL_ARB_point_parameters */
extern PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB;
extern PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB;

/* GL_ARB_vertex_blend */
extern PFNGLWEIGHTBVARBPROC      glWeightbvARB;
extern PFNGLWEIGHTSVARBPROC      glWeightsvARB;
extern PFNGLWEIGHTIVARBPROC      glWeightivARB;
extern PFNGLWEIGHTFVARBPROC      glWeightfvARB;
extern PFNGLWEIGHTDVARBPROC      glWeightdvARB;
extern PFNGLWEIGHTUBVARBPROC     glWeightubvARB;
extern PFNGLWEIGHTUSVARBPROC     glWeightusvARB;
extern PFNGLWEIGHTUIVARBPROC     glWeightuivARB;
extern PFNGLWEIGHTPOINTERARBPROC glWeightPointerARB;
extern PFNGLVERTEXBLENDARBPROC   glVertexBlendARB;

/* GL_ARB_matrix_palette */
extern PFNGLCURRENTPALETTEMATRIXARBPROC glCurrentPaletteMatrixARB;
extern PFNGLMATRIXINDEXUBVARBPROC       glMatrixIndexubvARB;
extern PFNGLMATRIXINDEXUSVARBPROC       glMatrixIndexusvARB;
extern PFNGLMATRIXINDEXUIVARBPROC       glMatrixIndexuivARB;
extern PFNGLMATRIXINDEXPOINTERARBPROC   glMatrixIndexPointerARB;

/* GL_ARB_window_pos */
extern PFNGLWINDOWPOS2DARBPROC  glWindowPos2dARB;
extern PFNGLWINDOWPOS2DVARBPROC glWindowPos2dvARB;
extern PFNGLWINDOWPOS2FARBPROC  glWindowPos2fARB;
extern PFNGLWINDOWPOS2FVARBPROC glWindowPos2fvARB;
extern PFNGLWINDOWPOS2IARBPROC  glWindowPos2iARB;
extern PFNGLWINDOWPOS2IVARBPROC glWindowPos2ivARB;
extern PFNGLWINDOWPOS2SARBPROC  glWindowPos2sARB;
extern PFNGLWINDOWPOS2SVARBPROC glWindowPos2svARB;
extern PFNGLWINDOWPOS3DARBPROC  glWindowPos3dARB;
extern PFNGLWINDOWPOS3DVARBPROC glWindowPos3dvARB;
extern PFNGLWINDOWPOS3FARBPROC  glWindowPos3fARB;
extern PFNGLWINDOWPOS3FVARBPROC glWindowPos3fvARB;
extern PFNGLWINDOWPOS3IARBPROC  glWindowPos3iARB;
extern PFNGLWINDOWPOS3IVARBPROC glWindowPos3ivARB;
extern PFNGLWINDOWPOS3SARBPROC  glWindowPos3sARB;
extern PFNGLWINDOWPOS3SVARBPROC glWindowPos3svARB;

/* GL_ARB_vertex_program */
extern PFNGLVERTEXATTRIB1DARBPROC             glVertexAttrib1dARB;
extern PFNGLVERTEXATTRIB1DVARBPROC            glVertexAttrib1dvARB;
extern PFNGLVERTEXATTRIB1FARBPROC             glVertexAttrib1fARB;
extern PFNGLVERTEXATTRIB1FVARBPROC            glVertexAttrib1fvARB;
extern PFNGLVERTEXATTRIB1SARBPROC             glVertexAttrib1sARB;
extern PFNGLVERTEXATTRIB1SVARBPROC            glVertexAttrib1svARB;
extern PFNGLVERTEXATTRIB2DARBPROC             glVertexAttrib2dARB;
extern PFNGLVERTEXATTRIB2DVARBPROC            glVertexAttrib2dvARB;
extern PFNGLVERTEXATTRIB2FARBPROC             glVertexAttrib2fARB;
extern PFNGLVERTEXATTRIB2FVARBPROC            glVertexAttrib2fvARB;
extern PFNGLVERTEXATTRIB2SARBPROC             glVertexAttrib2sARB;
extern PFNGLVERTEXATTRIB2SVARBPROC            glVertexAttrib2svARB;
extern PFNGLVERTEXATTRIB3DARBPROC             glVertexAttrib3dARB;
extern PFNGLVERTEXATTRIB3DVARBPROC            glVertexAttrib3dvARB;
extern PFNGLVERTEXATTRIB3FARBPROC             glVertexAttrib3fARB;
extern PFNGLVERTEXATTRIB3FVARBPROC            glVertexAttrib3fvARB;
extern PFNGLVERTEXATTRIB3SARBPROC             glVertexAttrib3sARB;
extern PFNGLVERTEXATTRIB3SVARBPROC            glVertexAttrib3svARB;
extern PFNGLVERTEXATTRIB4NBVARBPROC           glVertexAttrib4NbvARB;
extern PFNGLVERTEXATTRIB4NIVARBPROC           glVertexAttrib4NivARB;
extern PFNGLVERTEXATTRIB4NSVARBPROC           glVertexAttrib4NsvARB;
extern PFNGLVERTEXATTRIB4NUBARBPROC           glVertexAttrib4NubARB;
extern PFNGLVERTEXATTRIB4NUBVARBPROC          glVertexAttrib4NubvARB;
extern PFNGLVERTEXATTRIB4NUIVARBPROC          glVertexAttrib4NuivARB;
extern PFNGLVERTEXATTRIB4NUSVARBPROC          glVertexAttrib4NusvARB;
extern PFNGLVERTEXATTRIB4BVARBPROC            glVertexAttrib4bvARB;
extern PFNGLVERTEXATTRIB4DARBPROC             glVertexAttrib4dARB;
extern PFNGLVERTEXATTRIB4DVARBPROC            glVertexAttrib4dvARB;
extern PFNGLVERTEXATTRIB4FARBPROC             glVertexAttrib4fARB;
extern PFNGLVERTEXATTRIB4FVARBPROC            glVertexAttrib4fvARB;
extern PFNGLVERTEXATTRIB4IVARBPROC            glVertexAttrib4ivARB;
extern PFNGLVERTEXATTRIB4SARBPROC             glVertexAttrib4sARB;
extern PFNGLVERTEXATTRIB4SVARBPROC            glVertexAttrib4svARB;
extern PFNGLVERTEXATTRIB4UBVARBPROC           glVertexAttrib4ubvARB;
extern PFNGLVERTEXATTRIB4UIVARBPROC           glVertexAttrib4uivARB;
extern PFNGLVERTEXATTRIB4USVARBPROC           glVertexAttrib4usvARB;
extern PFNGLVERTEXATTRIBPOINTERARBPROC        glVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC    glEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC   glDisableVertexAttribArrayARB;
extern PFNGLPROGRAMSTRINGARBPROC              glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC                glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC             glDeleteProgramsARB;
extern PFNGLGENPROGRAMSARBPROC                glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4DARBPROC      glProgramEnvParameter4dARB;
extern PFNGLPROGRAMENVPARAMETER4DVARBPROC     glProgramEnvParameter4dvARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC      glProgramEnvParameter4fARB;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC     glProgramEnvParameter4fvARB;
extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC    glProgramLocalParameter4dARB;
extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC   glProgramLocalParameter4dvARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC    glProgramLocalParameter4fARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC   glProgramLocalParameter4fvARB;
extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC   glGetProgramEnvParameterdvARB;
extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC   glGetProgramEnvParameterfvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdvARB;
extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfvARB;
extern PFNGLGETPROGRAMIVARBPROC               glGetProgramivARB;
extern PFNGLGETPROGRAMSTRINGARBPROC           glGetProgramStringARB;
extern PFNGLGETVERTEXATTRIBDVARBPROC          glGetVertexAttribdvARB;
extern PFNGLGETVERTEXATTRIBFVARBPROC          glGetVertexAttribfvARB;
extern PFNGLGETVERTEXATTRIBIVARBPROC          glGetVertexAttribivARB;
extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC    glGetVertexAttribPointervARB;
extern PFNGLISPROGRAMARBPROC                  glIsProgramARB;

/* GL_ARB_vertex_buffer_object */
extern PFNGLBINDBUFFERARBPROC           glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC        glDeleteBuffersARB;
extern PFNGLGENBUFFERSARBPROC           glGenBuffersARB;
extern PFNGLISBUFFERARBPROC             glIsBufferARB;
extern PFNGLBUFFERDATAARBPROC           glBufferDataARB;
extern PFNGLBUFFERSUBDATAARBPROC        glBufferSubDataARB;
extern PFNGLGETBUFFERSUBDATAARBPROC     glGetBufferSubDataARB;
extern PFNGLMAPBUFFERARBPROC            glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC          glUnmapBufferARB;
extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
extern PFNGLGETBUFFERPOINTERVARBPROC    glGetBufferPointervARB;

/* GL_ARB_occlusion_query */
extern PFNGLGENQUERIESARBPROC        glGenQueriesARB;
extern PFNGLDELETEQUERIESARBPROC     glDeleteQueriesARB;
extern PFNGLISQUERYARBPROC           glIsQueryARB;
extern PFNGLBEGINQUERYARBPROC        glBeginQueryARB;
extern PFNGLENDQUERYARBPROC          glEndQueryARB;
extern PFNGLGETQUERYIVARBPROC        glGetQueryivARB;
extern PFNGLGETQUERYOBJECTIVARBPROC  glGetQueryObjectivARB;
extern PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB;

/* GL_ARB_shader_objects */
extern PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB;
extern PFNGLGETHANDLEARBPROC            glGetHandleARB;
extern PFNGLDETACHOBJECTARBPROC         glDetachObjectARB;
extern PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC         glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC        glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC         glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC          glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB;
extern PFNGLVALIDATEPROGRAMARBPROC      glValidateProgramARB;
extern PFNGLUNIFORM1FARBPROC            glUniform1fARB;
extern PFNGLUNIFORM2FARBPROC            glUniform2fARB;
extern PFNGLUNIFORM3FARBPROC            glUniform3fARB;
extern PFNGLUNIFORM4FARBPROC            glUniform4fARB;
extern PFNGLUNIFORM1IARBPROC            glUniform1iARB;
extern PFNGLUNIFORM2IARBPROC            glUniform2iARB;
extern PFNGLUNIFORM3IARBPROC            glUniform3iARB;
extern PFNGLUNIFORM4IARBPROC            glUniform4iARB;
extern PFNGLUNIFORM1FVARBPROC           glUniform1fvARB;
extern PFNGLUNIFORM2FVARBPROC           glUniform2fvARB;
extern PFNGLUNIFORM3FVARBPROC           glUniform3fvARB;
extern PFNGLUNIFORM4FVARBPROC           glUniform4fvARB;
extern PFNGLUNIFORM1IVARBPROC           glUniform1ivARB;
extern PFNGLUNIFORM2IVARBPROC           glUniform2ivARB;
extern PFNGLUNIFORM3IVARBPROC           glUniform3ivARB;
extern PFNGLUNIFORM4IVARBPROC           glUniform4ivARB;
extern PFNGLUNIFORMMATRIX2FVARBPROC     glUniformMatrix2fvARB;
extern PFNGLUNIFORMMATRIX3FVARBPROC     glUniformMatrix3fvARB;
extern PFNGLUNIFORMMATRIX4FVARBPROC     glUniformMatrix4fvARB;
extern PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfvARB;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
extern PFNGLGETINFOLOGARBPROC           glGetInfoLogARB;
extern PFNGLGETATTACHEDOBJECTSARBPROC   glGetAttachedObjectsARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB;
extern PFNGLGETACTIVEUNIFORMARBPROC     glGetActiveUniformARB;
extern PFNGLGETUNIFORMFVARBPROC         glGetUniformfvARB;
extern PFNGLGETUNIFORMIVARBPROC         glGetUniformivARB;
extern PFNGLGETSHADERSOURCEARBPROC      glGetShaderSourceARB;

/* GL_ARB_vertex_shader */
extern PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocationARB;
extern PFNGLGETACTIVEATTRIBARBPROC    glGetActiveAttribARB;
extern PFNGLGETATTRIBLOCATIONARBPROC  glGetAttribLocationARB;

/* GL_ARB_draw_buffers */
extern PFNGLDRAWBUFFERSARBPROC glDrawBuffersARB;

/* GL_ARB_color_buffer_float */
extern PFNGLCLAMPCOLORARBPROC glClampColorARB;

/* GL_ARB_draw_instanced */
extern PFNGLDRAWARRAYSINSTANCEDARBPROC   glDrawArraysInstancedARB;
extern PFNGLDRAWELEMENTSINSTANCEDARBPROC glDrawElementsInstancedARB;

/* GL_ARB_framebuffer_object */
extern PFNGLISRENDERBUFFERPROC                      glIsRenderbuffer;
extern PFNGLBINDRENDERBUFFERPROC                    glBindRenderbuffer;
extern PFNGLDELETERENDERBUFFERSPROC                 glDeleteRenderbuffers;
extern PFNGLGENRENDERBUFFERSPROC                    glGenRenderbuffers;
extern PFNGLRENDERBUFFERSTORAGEPROC                 glRenderbufferStorage;
extern PFNGLGETRENDERBUFFERPARAMETERIVPROC          glGetRenderbufferParameteriv;
extern PFNGLISFRAMEBUFFERPROC                       glIsFramebuffer;
extern PFNGLBINDFRAMEBUFFERPROC                     glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC                  glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC                     glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC              glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTURE1DPROC                glFramebufferTexture1D;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC                glFramebufferTexture2D;
extern PFNGLFRAMEBUFFERTEXTURE3DPROC                glFramebufferTexture3D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC             glFramebufferRenderbuffer;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
extern PFNGLGENERATEMIPMAPPROC                      glGenerateMipmap;
extern PFNGLBLITFRAMEBUFFERPROC                     glBlitFramebuffer;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC      glRenderbufferStorageMultisample;
extern PFNGLFRAMEBUFFERTEXTURELAYERPROC             glFramebufferTextureLayer;

/* GL_ARB_geometry_shader4 */
extern PFNGLPROGRAMPARAMETERIARBPROC       glProgramParameteriARB;
extern PFNGLFRAMEBUFFERTEXTUREARBPROC      glFramebufferTextureARB;
extern PFNGLFRAMEBUFFERTEXTURELAYERARBPROC glFramebufferTextureLayerARB;
extern PFNGLFRAMEBUFFERTEXTUREFACEARBPROC  glFramebufferTextureFaceARB;

/* GL_ARB_instanced_arrays */
extern PFNGLVERTEXATTRIBDIVISORARBPROC glVertexAttribDivisorARB;

/* GL_ARB_map_buffer_range */
extern PFNGLMAPBUFFERRANGEPROC         glMapBufferRange;
extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;

/* GL_ARB_texture_buffer_object */
extern PFNGLTEXBUFFERARBPROC glTexBufferARB;

/* GL_ARB_vertex_array_object */
extern PFNGLBINDVERTEXARRAYPROC    glBindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
extern PFNGLISVERTEXARRAYPROC      glIsVertexArray;

/* GL_ARB_uniform_buffer_object */
extern PFNGLGETUNIFORMINDICESPROC         glGetUniformIndices;
extern PFNGLGETACTIVEUNIFORMSIVPROC       glGetActiveUniformsiv;
extern PFNGLGETACTIVEUNIFORMNAMEPROC      glGetActiveUniformName;
extern PFNGLGETUNIFORMBLOCKINDEXPROC      glGetUniformBlockIndex;
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC   glGetActiveUniformBlockiv;
extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
extern PFNGLUNIFORMBLOCKBINDINGPROC       glUniformBlockBinding;

/* GL_ARB_copy_buffer */
extern PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;

/* GL_ARB_draw_elements_base_vertex */
extern PFNGLDRAWELEMENTSBASEVERTEXPROC          glDrawElementsBaseVertex;
extern PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC     glDrawRangeElementsBaseVertex;
extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
extern PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC     glMultiDrawElementsBaseVertex;

/* GL_ARB_provoking_vertex */
extern PFNGLPROVOKINGVERTEXPROC glProvokingVertex;

/* GL_ARB_sync */
extern PFNGLFENCESYNCPROC      glFenceSync;
extern PFNGLISSYNCPROC         glIsSync;
extern PFNGLDELETESYNCPROC     glDeleteSync;
extern PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
extern PFNGLWAITSYNCPROC       glWaitSync;
extern PFNGLGETINTEGER64VPROC  glGetInteger64v;
extern PFNGLGETSYNCIVPROC      glGetSynciv;

/* GL_ARB_texture_multisample */
extern PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
extern PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
extern PFNGLGETMULTISAMPLEFVPROC      glGetMultisamplefv;
extern PFNGLSAMPLEMASKIPROC           glSampleMaski;

/* GL_ARB_draw_buffers_blend */
extern PFNGLBLENDEQUATIONIARBPROC         glBlendEquationiARB;
extern PFNGLBLENDEQUATIONSEPARATEIARBPROC glBlendEquationSeparateiARB;
extern PFNGLBLENDFUNCIARBPROC             glBlendFunciARB;
extern PFNGLBLENDFUNCSEPARATEIARBPROC     glBlendFuncSeparateiARB;

/* GL_ARB_sample_shading */
extern PFNGLMINSAMPLESHADINGARBPROC glMinSampleShadingARB;

/* GL_ARB_shading_language_include */
extern PFNGLNAMEDSTRINGARBPROC          glNamedStringARB;
extern PFNGLDELETENAMEDSTRINGARBPROC    glDeleteNamedStringARB;
extern PFNGLCOMPILESHADERINCLUDEARBPROC glCompileShaderIncludeARB;
extern PFNGLISNAMEDSTRINGARBPROC        glIsNamedStringARB;
extern PFNGLGETNAMEDSTRINGARBPROC       glGetNamedStringARB;
extern PFNGLGETNAMEDSTRINGIVARBPROC     glGetNamedStringivARB;

/* GL_ARB_blend_func_extended */
extern PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
extern PFNGLGETFRAGDATAINDEXPROC            glGetFragDataIndex;

/* GL_ARB_sampler_objects */
extern PFNGLGENSAMPLERSPROC             glGenSamplers;
extern PFNGLDELETESAMPLERSPROC          glDeleteSamplers;
extern PFNGLISSAMPLERPROC               glIsSampler;
extern PFNGLBINDSAMPLERPROC             glBindSampler;
extern PFNGLSAMPLERPARAMETERIPROC       glSamplerParameteri;
extern PFNGLSAMPLERPARAMETERIVPROC      glSamplerParameteriv;
extern PFNGLSAMPLERPARAMETERFPROC       glSamplerParameterf;
extern PFNGLSAMPLERPARAMETERFVPROC      glSamplerParameterfv;
extern PFNGLSAMPLERPARAMETERIIVPROC     glSamplerParameterIiv;
extern PFNGLSAMPLERPARAMETERIUIVPROC    glSamplerParameterIuiv;
extern PFNGLGETSAMPLERPARAMETERIVPROC   glGetSamplerParameteriv;
extern PFNGLGETSAMPLERPARAMETERIIVPROC  glGetSamplerParameterIiv;
extern PFNGLGETSAMPLERPARAMETERFVPROC   glGetSamplerParameterfv;
extern PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;

/* GL_ARB_timer_query */
extern PFNGLQUERYCOUNTERPROC        glQueryCounter;
extern PFNGLGETQUERYOBJECTI64VPROC  glGetQueryObjecti64v;
extern PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;

/* GL_ARB_vertex_type_2_10_10_10_rev */
extern PFNGLVERTEXP2UIPROC          glVertexP2ui;
extern PFNGLVERTEXP2UIVPROC         glVertexP2uiv;
extern PFNGLVERTEXP3UIPROC          glVertexP3ui;
extern PFNGLVERTEXP3UIVPROC         glVertexP3uiv;
extern PFNGLVERTEXP4UIPROC          glVertexP4ui;
extern PFNGLVERTEXP4UIVPROC         glVertexP4uiv;
extern PFNGLTEXCOORDP1UIPROC        glTexCoordP1ui;
extern PFNGLTEXCOORDP1UIVPROC       glTexCoordP1uiv;
extern PFNGLTEXCOORDP2UIPROC        glTexCoordP2ui;
extern PFNGLTEXCOORDP2UIVPROC       glTexCoordP2uiv;
extern PFNGLTEXCOORDP3UIPROC        glTexCoordP3ui;
extern PFNGLTEXCOORDP3UIVPROC       glTexCoordP3uiv;
extern PFNGLTEXCOORDP4UIPROC        glTexCoordP4ui;
extern PFNGLTEXCOORDP4UIVPROC       glTexCoordP4uiv;
extern PFNGLMULTITEXCOORDP1UIPROC   glMultiTexCoordP1ui;
extern PFNGLMULTITEXCOORDP1UIVPROC  glMultiTexCoordP1uiv;
extern PFNGLMULTITEXCOORDP2UIPROC   glMultiTexCoordP2ui;
extern PFNGLMULTITEXCOORDP2UIVPROC  glMultiTexCoordP2uiv;
extern PFNGLMULTITEXCOORDP3UIPROC   glMultiTexCoordP3ui;
extern PFNGLMULTITEXCOORDP3UIVPROC  glMultiTexCoordP3uiv;
extern PFNGLMULTITEXCOORDP4UIPROC   glMultiTexCoordP4ui;
extern PFNGLMULTITEXCOORDP4UIVPROC  glMultiTexCoordP4uiv;
extern PFNGLNORMALP3UIPROC          glNormalP3ui;
extern PFNGLNORMALP3UIVPROC         glNormalP3uiv;
extern PFNGLCOLORP3UIPROC           glColorP3ui;
extern PFNGLCOLORP3UIVPROC          glColorP3uiv;
extern PFNGLCOLORP4UIPROC           glColorP4ui;
extern PFNGLCOLORP4UIVPROC          glColorP4uiv;
extern PFNGLSECONDARYCOLORP3UIPROC  glSecondaryColorP3ui;
extern PFNGLSECONDARYCOLORP3UIVPROC glSecondaryColorP3uiv;
extern PFNGLVERTEXATTRIBP1UIPROC    glVertexAttribP1ui;
extern PFNGLVERTEXATTRIBP1UIVPROC   glVertexAttribP1uiv;
extern PFNGLVERTEXATTRIBP2UIPROC    glVertexAttribP2ui;
extern PFNGLVERTEXATTRIBP2UIVPROC   glVertexAttribP2uiv;
extern PFNGLVERTEXATTRIBP3UIPROC    glVertexAttribP3ui;
extern PFNGLVERTEXATTRIBP3UIVPROC   glVertexAttribP3uiv;
extern PFNGLVERTEXATTRIBP4UIPROC    glVertexAttribP4ui;
extern PFNGLVERTEXATTRIBP4UIVPROC   glVertexAttribP4uiv;

/* GL_ARB_draw_indirect */
extern PFNGLDRAWARRAYSINDIRECTPROC   glDrawArraysIndirect;
extern PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;

/* GL_ARB_gpu_shader_fp64 */
extern PFNGLUNIFORM1DPROC          glUniform1d;
extern PFNGLUNIFORM2DPROC          glUniform2d;
extern PFNGLUNIFORM3DPROC          glUniform3d;
extern PFNGLUNIFORM4DPROC          glUniform4d;
extern PFNGLUNIFORM1DVPROC         glUniform1dv;
extern PFNGLUNIFORM2DVPROC         glUniform2dv;
extern PFNGLUNIFORM3DVPROC         glUniform3dv;
extern PFNGLUNIFORM4DVPROC         glUniform4dv;
extern PFNGLUNIFORMMATRIX2DVPROC   glUniformMatrix2dv;
extern PFNGLUNIFORMMATRIX3DVPROC   glUniformMatrix3dv;
extern PFNGLUNIFORMMATRIX4DVPROC   glUniformMatrix4dv;
extern PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv;
extern PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv;
extern PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv;
extern PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv;
extern PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv;
extern PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv;
extern PFNGLGETUNIFORMDVPROC       glGetUniformdv;

/* GL_ARB_shader_subroutine */
extern PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC   glGetSubroutineUniformLocation;
extern PFNGLGETSUBROUTINEINDEXPROC             glGetSubroutineIndex;
extern PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC   glGetActiveSubroutineUniformiv;
extern PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
extern PFNGLGETACTIVESUBROUTINENAMEPROC        glGetActiveSubroutineName;
extern PFNGLUNIFORMSUBROUTINESUIVPROC          glUniformSubroutinesuiv;
extern PFNGLGETUNIFORMSUBROUTINEUIVPROC        glGetUniformSubroutineuiv;
extern PFNGLGETPROGRAMSTAGEIVPROC              glGetProgramStageiv;

/* GL_ARB_tessellation_shader */
extern PFNGLPATCHPARAMETERIPROC  glPatchParameteri;
extern PFNGLPATCHPARAMETERFVPROC glPatchParameterfv;

/* GL_ARB_transform_feedback2 */
extern PFNGLBINDTRANSFORMFEEDBACKPROC    glBindTransformFeedback;
extern PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks;
extern PFNGLGENTRANSFORMFEEDBACKSPROC    glGenTransformFeedbacks;
extern PFNGLISTRANSFORMFEEDBACKPROC      glIsTransformFeedback;
extern PFNGLPAUSETRANSFORMFEEDBACKPROC   glPauseTransformFeedback;
extern PFNGLRESUMETRANSFORMFEEDBACKPROC  glResumeTransformFeedback;
extern PFNGLDRAWTRANSFORMFEEDBACKPROC    glDrawTransformFeedback;

/* GL_ARB_transform_feedback3 */
extern PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
extern PFNGLBEGINQUERYINDEXEDPROC           glBeginQueryIndexed;
extern PFNGLENDQUERYINDEXEDPROC             glEndQueryIndexed;
extern PFNGLGETQUERYINDEXEDIVPROC           glGetQueryIndexediv;

/* GL_ARB_ES2_compatibility */
extern PFNGLRELEASESHADERCOMPILERPROC    glReleaseShaderCompiler;
extern PFNGLSHADERBINARYPROC             glShaderBinary;
extern PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat;
extern PFNGLDEPTHRANGEFPROC              glDepthRangef;
extern PFNGLCLEARDEPTHFPROC              glClearDepthf;

/* GL_ARB_get_program_binary */
extern PFNGLGETPROGRAMBINARYPROC  glGetProgramBinary;
extern PFNGLPROGRAMBINARYPROC     glProgramBinary;
extern PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;

/* GL_ARB_separate_shader_objects */
extern PFNGLUSEPROGRAMSTAGESPROC          glUseProgramStages;
extern PFNGLACTIVESHADERPROGRAMPROC       glActiveShaderProgram;
extern PFNGLCREATESHADERPROGRAMVPROC      glCreateShaderProgramv;
extern PFNGLBINDPROGRAMPIPELINEPROC       glBindProgramPipeline;
extern PFNGLDELETEPROGRAMPIPELINESPROC    glDeleteProgramPipelines;
extern PFNGLGENPROGRAMPIPELINESPROC       glGenProgramPipelines;
extern PFNGLISPROGRAMPIPELINEPROC         glIsProgramPipeline;
extern PFNGLGETPROGRAMPIPELINEIVPROC      glGetProgramPipelineiv;
extern PFNGLPROGRAMUNIFORM1IPROC          glProgramUniform1i;
extern PFNGLPROGRAMUNIFORM1IVPROC         glProgramUniform1iv;
extern PFNGLPROGRAMUNIFORM1FPROC          glProgramUniform1f;
extern PFNGLPROGRAMUNIFORM1FVPROC         glProgramUniform1fv;
extern PFNGLPROGRAMUNIFORM1DPROC          glProgramUniform1d;
extern PFNGLPROGRAMUNIFORM1DVPROC         glProgramUniform1dv;
extern PFNGLPROGRAMUNIFORM1UIPROC         glProgramUniform1ui;
extern PFNGLPROGRAMUNIFORM1UIVPROC        glProgramUniform1uiv;
extern PFNGLPROGRAMUNIFORM2IPROC          glProgramUniform2i;
extern PFNGLPROGRAMUNIFORM2IVPROC         glProgramUniform2iv;
extern PFNGLPROGRAMUNIFORM2FPROC          glProgramUniform2f;
extern PFNGLPROGRAMUNIFORM2FVPROC         glProgramUniform2fv;
extern PFNGLPROGRAMUNIFORM2DPROC          glProgramUniform2d;
extern PFNGLPROGRAMUNIFORM2DVPROC         glProgramUniform2dv;
extern PFNGLPROGRAMUNIFORM2UIPROC         glProgramUniform2ui;
extern PFNGLPROGRAMUNIFORM2UIVPROC        glProgramUniform2uiv;
extern PFNGLPROGRAMUNIFORM3IPROC          glProgramUniform3i;
extern PFNGLPROGRAMUNIFORM3IVPROC         glProgramUniform3iv;
extern PFNGLPROGRAMUNIFORM3FPROC          glProgramUniform3f;
extern PFNGLPROGRAMUNIFORM3FVPROC         glProgramUniform3fv;
extern PFNGLPROGRAMUNIFORM3DPROC          glProgramUniform3d;
extern PFNGLPROGRAMUNIFORM3DVPROC         glProgramUniform3dv;
extern PFNGLPROGRAMUNIFORM3UIPROC         glProgramUniform3ui;
extern PFNGLPROGRAMUNIFORM3UIVPROC        glProgramUniform3uiv;
extern PFNGLPROGRAMUNIFORM4IPROC          glProgramUniform4i;
extern PFNGLPROGRAMUNIFORM4IVPROC         glProgramUniform4iv;
extern PFNGLPROGRAMUNIFORM4FPROC          glProgramUniform4f;
extern PFNGLPROGRAMUNIFORM4FVPROC         glProgramUniform4fv;
extern PFNGLPROGRAMUNIFORM4DPROC          glProgramUniform4d;
extern PFNGLPROGRAMUNIFORM4DVPROC         glProgramUniform4dv;
extern PFNGLPROGRAMUNIFORM4UIPROC         glProgramUniform4ui;
extern PFNGLPROGRAMUNIFORM4UIVPROC        glProgramUniform4uiv;
extern PFNGLPROGRAMUNIFORMMATRIX2FVPROC   glProgramUniformMatrix2fv;
extern PFNGLPROGRAMUNIFORMMATRIX3FVPROC   glProgramUniformMatrix3fv;
extern PFNGLPROGRAMUNIFORMMATRIX4FVPROC   glProgramUniformMatrix4fv;
extern PFNGLPROGRAMUNIFORMMATRIX2DVPROC   glProgramUniformMatrix2dv;
extern PFNGLPROGRAMUNIFORMMATRIX3DVPROC   glProgramUniformMatrix3dv;
extern PFNGLPROGRAMUNIFORMMATRIX4DVPROC   glProgramUniformMatrix4dv;
extern PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
extern PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
extern PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
extern PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
extern PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
extern PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
extern PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv;
extern PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv;
extern PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv;
extern PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv;
extern PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv;
extern PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv;
extern PFNGLVALIDATEPROGRAMPIPELINEPROC   glValidateProgramPipeline;
extern PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;

/* GL_ARB_vertex_attrib_64bit */
extern PFNGLVERTEXATTRIBL1DPROC      glVertexAttribL1d;
extern PFNGLVERTEXATTRIBL2DPROC      glVertexAttribL2d;
extern PFNGLVERTEXATTRIBL3DPROC      glVertexAttribL3d;
extern PFNGLVERTEXATTRIBL4DPROC      glVertexAttribL4d;
extern PFNGLVERTEXATTRIBL1DVPROC     glVertexAttribL1dv;
extern PFNGLVERTEXATTRIBL2DVPROC     glVertexAttribL2dv;
extern PFNGLVERTEXATTRIBL3DVPROC     glVertexAttribL3dv;
extern PFNGLVERTEXATTRIBL4DVPROC     glVertexAttribL4dv;
extern PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer;
extern PFNGLGETVERTEXATTRIBLDVPROC   glGetVertexAttribLdv;

/* GL_ARB_viewport_array */
extern PFNGLVIEWPORTARRAYVPROC    glViewportArrayv;
extern PFNGLVIEWPORTINDEXEDFPROC  glViewportIndexedf;
extern PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv;
extern PFNGLSCISSORARRAYVPROC     glScissorArrayv;
extern PFNGLSCISSORINDEXEDPROC    glScissorIndexed;
extern PFNGLSCISSORINDEXEDVPROC   glScissorIndexedv;
extern PFNGLDEPTHRANGEARRAYVPROC  glDepthRangeArrayv;
extern PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed;
extern PFNGLGETFLOATI_VPROC       glGetFloati_v;
extern PFNGLGETDOUBLEI_VPROC      glGetDoublei_v;

/* GL_ARB_cl_event */
extern PFNGLCREATESYNCFROMCLEVENTARBPROC glCreateSyncFromCLeventARB;

/* GL_ARB_debug_output */
extern PFNGLDEBUGMESSAGECONTROLARBPROC  glDebugMessageControlARB;
extern PFNGLDEBUGMESSAGEINSERTARBPROC   glDebugMessageInsertARB;
extern PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB;
extern PFNGLGETDEBUGMESSAGELOGARBPROC   glGetDebugMessageLogARB;

/* GL_ARB_robustness */
extern PFNGLGETGRAPHICSRESETSTATUSARBPROC glGetGraphicsResetStatusARB;
extern PFNGLGETNMAPDVARBPROC              glGetnMapdvARB;
extern PFNGLGETNMAPFVARBPROC              glGetnMapfvARB;
extern PFNGLGETNMAPIVARBPROC              glGetnMapivARB;
extern PFNGLGETNPIXELMAPFVARBPROC         glGetnPixelMapfvARB;
extern PFNGLGETNPIXELMAPUIVARBPROC        glGetnPixelMapuivARB;
extern PFNGLGETNPIXELMAPUSVARBPROC        glGetnPixelMapusvARB;
extern PFNGLGETNPOLYGONSTIPPLEARBPROC     glGetnPolygonStippleARB;
extern PFNGLGETNCOLORTABLEARBPROC         glGetnColorTableARB;
extern PFNGLGETNCONVOLUTIONFILTERARBPROC  glGetnConvolutionFilterARB;
extern PFNGLGETNSEPARABLEFILTERARBPROC    glGetnSeparableFilterARB;
extern PFNGLGETNHISTOGRAMARBPROC          glGetnHistogramARB;
extern PFNGLGETNMINMAXARBPROC             glGetnMinmaxARB;
extern PFNGLGETNTEXIMAGEARBPROC           glGetnTexImageARB;
extern PFNGLREADNPIXELSARBPROC            glReadnPixelsARB;
extern PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC glGetnCompressedTexImageARB;
extern PFNGLGETNUNIFORMFVARBPROC          glGetnUniformfvARB;
extern PFNGLGETNUNIFORMIVARBPROC          glGetnUniformivARB;
extern PFNGLGETNUNIFORMUIVARBPROC         glGetnUniformuivARB;
extern PFNGLGETNUNIFORMDVARBPROC          glGetnUniformdvARB;

/* GL_ARB_base_instance */
extern PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC             glDrawArraysInstancedBaseInstance;
extern PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC           glDrawElementsInstancedBaseInstance;
extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;

/* GL_ARB_transform_feedback_instanced */
extern PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC       glDrawTransformFeedbackInstanced;
extern PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced;

/* GL_ARB_internalformat_query */
extern PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;

/* GL_ARB_shader_atomic_counters */
extern PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv;

/* GL_ARB_shader_image_load_store */
extern PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
extern PFNGLMEMORYBARRIERPROC    glMemoryBarrier;

/* GL_ARB_texture_storage */
extern PFNGLTEXSTORAGE1DPROC        glTexStorage1D;
extern PFNGLTEXSTORAGE2DPROC        glTexStorage2D;
extern PFNGLTEXSTORAGE3DPROC        glTexStorage3D;
extern PFNGLTEXTURESTORAGE1DEXTPROC glTextureStorage1DEXT;
extern PFNGLTEXTURESTORAGE2DEXTPROC glTextureStorage2DEXT;
extern PFNGLTEXTURESTORAGE3DEXTPROC glTextureStorage3DEXT;




/***********************************
* Deprecated OpenGL                *
************************************/
/* OpenGL 1.0/1.1 */
/* TODO: STILL NEED TO REMOVE THE NON_DEPRECATED FROM HERE AS THEY WILL BE INCLUDED IN GL3.H. */
#ifndef GL_VERSION_1_1_DEPRECATED
#define GL_VERSION_1_1_DEPRECATED 1
/* Primitives */
#define GL_POINTS                   0x0000
#define GL_LINES                    0x0001
#define GL_LINE_LOOP                0x0002
#define GL_LINE_STRIP               0x0003
#define GL_TRIANGLES                0x0004
#define GL_TRIANGLE_STRIP           0x0005
#define GL_TRIANGLE_FAN             0x0006
#define GL_QUADS                    0x0007
#define GL_QUAD_STRIP               0x0008
#define GL_POLYGON                  0x0009

/* Vertex Arrays */
#define GL_VERTEX_ARRAY             0x8074
#define GL_NORMAL_ARRAY             0x8075
#define GL_COLOR_ARRAY              0x8076
#define GL_INDEX_ARRAY              0x8077
#define GL_TEXTURE_COORD_ARRAY      0x8078
#define GL_EDGE_FLAG_ARRAY          0x8079
#define GL_VERTEX_ARRAY_SIZE        0x807A
#define GL_VERTEX_ARRAY_TYPE        0x807B
#define GL_VERTEX_ARRAY_STRIDE      0x807C
#define GL_NORMAL_ARRAY_TYPE        0x807E
#define GL_NORMAL_ARRAY_STRIDE      0x807F
#define GL_COLOR_ARRAY_SIZE         0x8081
#define GL_COLOR_ARRAY_TYPE         0x8082
#define GL_COLOR_ARRAY_STRIDE       0x8083
#define GL_INDEX_ARRAY_TYPE         0x8085
#define GL_INDEX_ARRAY_STRIDE       0x8086
#define GL_TEXTURE_COORD_ARRAY_SIZE 0x8088
#define GL_TEXTURE_COORD_ARRAY_TYPE 0x8089
#define GL_TEXTURE_COORD_ARRAY_STRIDE   0x808A
#define GL_EDGE_FLAG_ARRAY_STRIDE   0x808C
#define GL_VERTEX_ARRAY_POINTER     0x808E
#define GL_NORMAL_ARRAY_POINTER     0x808F
#define GL_COLOR_ARRAY_POINTER      0x8090
#define GL_INDEX_ARRAY_POINTER      0x8091
#define GL_TEXTURE_COORD_ARRAY_POINTER  0x8092
#define GL_EDGE_FLAG_ARRAY_POINTER  0x8093
#define GL_V2F                      0x2A20
#define GL_V3F                      0x2A21
#define GL_C4UB_V2F                 0x2A22
#define GL_C4UB_V3F                 0x2A23
#define GL_C3F_V3F                  0x2A24
#define GL_N3F_V3F                  0x2A25
#define GL_C4F_N3F_V3F              0x2A26
#define GL_T2F_V3F                  0x2A27
#define GL_T4F_V4F                  0x2A28
#define GL_T2F_C4UB_V3F             0x2A29
#define GL_T2F_C3F_V3F              0x2A2A
#define GL_T2F_N3F_V3F              0x2A2B
#define GL_T2F_C4F_N3F_V3F          0x2A2C
#define GL_T4F_C4F_N3F_V4F          0x2A2D

/* Matrix Mode */
#define GL_MATRIX_MODE              0x0BA0
#define GL_MODELVIEW                0x1700
#define GL_PROJECTION               0x1701
#define GL_TEXTURE                  0x1702

/* Points */
#define GL_POINT_SMOOTH             0x0B10
#define GL_POINT_SIZE               0x0B11
#define GL_POINT_SIZE_GRANULARITY   0x0B13
#define GL_POINT_SIZE_RANGE         0x0B12

/* Lines */
#define GL_LINE_SMOOTH              0x0B20
#define GL_LINE_STIPPLE             0x0B24
#define GL_LINE_STIPPLE_PATTERN     0x0B25
#define GL_LINE_STIPPLE_REPEAT      0x0B26
#define GL_LINE_WIDTH               0x0B21
#define GL_LINE_WIDTH_GRANULARITY   0x0B23
#define GL_LINE_WIDTH_RANGE         0x0B22

/* Polygons */
#define GL_POINT                    0x1B00
#define GL_LINE                     0x1B01
#define GL_FILL                     0x1B02
#define GL_CW                       0x0900
#define GL_CCW                      0x0901
#define GL_FRONT                    0x0404
#define GL_BACK                     0x0405
#define GL_POLYGON_MODE             0x0B40
#define GL_POLYGON_SMOOTH           0x0B41
#define GL_POLYGON_STIPPLE          0x0B42
#define GL_EDGE_FLAG                0x0B43
#define GL_CULL_FACE                0x0B44
#define GL_CULL_FACE_MODE           0x0B45
#define GL_FRONT_FACE               0x0B46
#define GL_POLYGON_OFFSET_FACTOR    0x8038
#define GL_POLYGON_OFFSET_UNITS     0x2A00
#define GL_POLYGON_OFFSET_POINT     0x2A01
#define GL_POLYGON_OFFSET_LINE      0x2A02
#define GL_POLYGON_OFFSET_FILL      0x8037

/* Display Lists */
#define GL_COMPILE                  0x1300
#define GL_COMPILE_AND_EXECUTE      0x1301
#define GL_LIST_BASE                0x0B32
#define GL_LIST_INDEX               0x0B33
#define GL_LIST_MODE                0x0B30

/* Depth buffer */
#define GL_NEVER                    0x0200
#define GL_LESS                     0x0201
#define GL_EQUAL                    0x0202
#define GL_LEQUAL                   0x0203
#define GL_GREATER                  0x0204
#define GL_NOTEQUAL                 0x0205
#define GL_GEQUAL                   0x0206
#define GL_ALWAYS                   0x0207
#define GL_DEPTH_TEST               0x0B71
#define GL_DEPTH_BITS               0x0D56
#define GL_DEPTH_CLEAR_VALUE        0x0B73
#define GL_DEPTH_FUNC               0x0B74
#define GL_DEPTH_RANGE              0x0B70
#define GL_DEPTH_WRITEMASK          0x0B72
#define GL_DEPTH_COMPONENT          0x1902

/* Lighting */
#define GL_LIGHTING                 0x0B50
#define GL_LIGHT0                   0x4000
#define GL_LIGHT1                   0x4001
#define GL_LIGHT2                   0x4002
#define GL_LIGHT3                   0x4003
#define GL_LIGHT4                   0x4004
#define GL_LIGHT5                   0x4005
#define GL_LIGHT6                   0x4006
#define GL_LIGHT7                   0x4007
#define GL_SPOT_EXPONENT            0x1205
#define GL_SPOT_CUTOFF              0x1206
#define GL_CONSTANT_ATTENUATION     0x1207
#define GL_LINEAR_ATTENUATION       0x1208
#define GL_QUADRATIC_ATTENUATION    0x1209
#define GL_AMBIENT                  0x1200
#define GL_DIFFUSE                  0x1201
#define GL_SPECULAR                 0x1202
#define GL_SHININESS                0x1601
#define GL_EMISSION                 0x1600
#define GL_POSITION                 0x1203
#define GL_SPOT_DIRECTION           0x1204
#define GL_AMBIENT_AND_DIFFUSE      0x1602
#define GL_COLOR_INDEXES            0x1603
#define GL_LIGHT_MODEL_TWO_SIDE     0x0B52
#define GL_LIGHT_MODEL_LOCAL_VIEWER 0x0B51
#define GL_LIGHT_MODEL_AMBIENT      0x0B53
#define GL_FRONT_AND_BACK           0x0408
#define GL_SHADE_MODEL              0x0B54
#define GL_FLAT                     0x1D00
#define GL_SMOOTH                   0x1D01
#define GL_COLOR_MATERIAL           0x0B57
#define GL_COLOR_MATERIAL_FACE      0x0B55
#define GL_COLOR_MATERIAL_PARAMETER 0x0B56
#define GL_NORMALIZE                0x0BA1

/* User clipping planes */
#define GL_CLIP_PLANE0              0x3000
#define GL_CLIP_PLANE1              0x3001
#define GL_CLIP_PLANE2              0x3002
#define GL_CLIP_PLANE3              0x3003
#define GL_CLIP_PLANE4              0x3004
#define GL_CLIP_PLANE5              0x3005

/* Accumulation buffer */
#define GL_ACCUM_RED_BITS           0x0D58
#define GL_ACCUM_GREEN_BITS	        0x0D59
#define GL_ACCUM_BLUE_BITS          0x0D5A
#define GL_ACCUM_ALPHA_BITS         0x0D5B
#define GL_ACCUM_CLEAR_VALUE        0x0B80
#define GL_ACCUM                    0x0100
#define GL_ADD                      0x0104
#define GL_LOAD                     0x0101
#define GL_MULT                     0x0103
#define GL_RETURN                   0x0102

/* Alpha testing */
#define GL_ALPHA_TEST               0x0BC0
#define GL_ALPHA_TEST_REF           0x0BC2
#define GL_ALPHA_TEST_FUNC          0x0BC1

/* Blending */
#define GL_BLEND                    0x0BE2
#define GL_BLEND_SRC                0x0BE1
#define GL_BLEND_DST                0x0BE0
#define GL_SRC_COLOR                0x0300
#define GL_ONE_MINUS_SRC_COLOR      0x0301
#define GL_SRC_ALPHA                0x0302
#define GL_ONE_MINUS_SRC_ALPHA      0x0303
#define GL_DST_ALPHA                0x0304
#define GL_ONE_MINUS_DST_ALPHA      0x0305
#define GL_DST_COLOR                0x0306
#define GL_ONE_MINUS_DST_COLOR      0x0307
#define GL_SRC_ALPHA_SATURATE       0x0308

/* Render Mode */
#define GL_FEEDBACK                 0x1C01
#define GL_RENDER                   0x1C00
#define GL_SELECT                   0x1C02

/* Feedback */
#define GL_2D                       0x0600
#define GL_3D                       0x0601
#define GL_3D_COLOR                 0x0602
#define GL_3D_COLOR_TEXTURE         0x0603
#define GL_4D_COLOR_TEXTURE         0x0604
#define GL_POINT_TOKEN              0x0701
#define GL_LINE_TOKEN               0x0702
#define GL_LINE_RESET_TOKEN         0x0707
#define GL_POLYGON_TOKEN            0x0703
#define GL_BITMAP_TOKEN             0x0704
#define GL_DRAW_PIXEL_TOKEN         0x0705
#define GL_COPY_PIXEL_TOKEN         0x0706
#define GL_PASS_THROUGH_TOKEN       0x0700
#define GL_FEEDBACK_BUFFER_POINTER  0x0DF0
#define GL_FEEDBACK_BUFFER_SIZE     0x0DF1
#define GL_FEEDBACK_BUFFER_TYPE     0x0DF2

/* Selection */
#define GL_SELECTION_BUFFER_POINTER 0x0DF3
#define GL_SELECTION_BUFFER_SIZE    0x0DF4

/* Fog */
#define GL_FOG                      0x0B60
#define GL_FOG_MODE                 0x0B65
#define GL_FOG_DENSITY              0x0B62
#define GL_FOG_COLOR                0x0B66
#define GL_FOG_INDEX                0x0B61
#define GL_FOG_START                0x0B63
#define GL_FOG_END                  0x0B64
#define GL_LINEAR                   0x2601
#define GL_EXP                      0x0800
#define GL_EXP2                     0x0801

/* Logic Ops */
#define GL_LOGIC_OP                 0x0BF1
#define GL_INDEX_LOGIC_OP           0x0BF1
#define GL_COLOR_LOGIC_OP           0x0BF2
#define GL_LOGIC_OP_MODE            0x0BF0
#define GL_CLEAR                    0x1500
#define GL_SET                      0x150F
#define GL_COPY                     0x1503
#define GL_COPY_INVERTED            0x150C
#define GL_NOOP                     0x1505
#define GL_INVERT                   0x150A
#define GL_AND                      0x1501
#define GL_NAND                     0x150E
#define GL_OR                       0x1507
#define GL_NOR                      0x1508
#define GL_XOR                      0x1506
#define GL_EQUIV                    0x1509
#define GL_AND_REVERSE              0x1502
#define GL_AND_INVERTED             0x1504
#define GL_OR_REVERSE               0x150B
#define GL_OR_INVERTED              0x150D

/* Stencil */
#define GL_STENCIL_BITS             0x0D57
#define GL_STENCIL_TEST             0x0B90
#define GL_STENCIL_CLEAR_VALUE      0x0B91
#define GL_STENCIL_FUNC             0x0B92
#define GL_STENCIL_VALUE_MASK       0x0B93
#define GL_STENCIL_FAIL             0x0B94
#define GL_STENCIL_PASS_DEPTH_FAIL  0x0B95
#define GL_STENCIL_PASS_DEPTH_PASS  0x0B96
#define GL_STENCIL_REF              0x0B97
#define GL_STENCIL_WRITEMASK        0x0B98
#define GL_STENCIL_INDEX            0x1901
#define GL_KEEP                     0x1E00
#define GL_REPLACE                  0x1E01
#define GL_INCR                     0x1E02
#define GL_DECR                     0x1E03

/* Buffers, Pixel Drawing/Reading */
#define GL_LEFT                     0x0406
#define GL_RIGHT                    0x0407
#define GL_FRONT_LEFT               0x0400
#define GL_FRONT_RIGHT              0x0401
#define GL_BACK_LEFT                0x0402
#define GL_BACK_RIGHT               0x0403
#define GL_AUX0                     0x0409
#define GL_AUX1                     0x040A
#define GL_AUX2                     0x040B
#define GL_AUX3                     0x040C
#define GL_COLOR_INDEX              0x1900
#define GL_RED                      0x1903
#define GL_GREEN                    0x1904
#define GL_BLUE                     0x1905
#define GL_ALPHA                    0x1906
#define GL_LUMINANCE                0x1909
#define GL_LUMINANCE_ALPHA          0x190A
#define GL_ALPHA_BITS               0x0D55
#define GL_RED_BITS                 0x0D52
#define GL_GREEN_BITS               0x0D53
#define GL_BLUE_BITS                0x0D54
#define GL_INDEX_BITS               0x0D51
#define GL_SUBPIXEL_BITS            0x0D50
#define GL_AUX_BUFFERS              0x0C00
#define GL_READ_BUFFER              0x0C02
#define GL_DRAW_BUFFER              0x0C01
#define GL_DOUBLEBUFFER             0x0C32
#define GL_STEREO                   0x0C33
#define GL_BITMAP                   0x1A00
#define GL_COLOR                    0x1800
#define GL_DEPTH                    0x1801
#define GL_STENCIL                  0x1802
#define GL_DITHER                   0x0BD0
#define GL_RGB                      0x1907
#define GL_RGBA                     0x1908

/* Implementation limits */
#define GL_MAX_LIST_NESTING         0x0B31
#define GL_MAX_EVAL_ORDER           0x0D30
#define GL_MAX_LIGHTS               0x0D31
#define GL_MAX_CLIP_PLANES          0x0D32
#define GL_MAX_TEXTURE_SIZE         0x0D33
#define GL_MAX_PIXEL_MAP_TABLE	    0x0D34
#define GL_MAX_ATTRIB_STACK_DEPTH   0x0D35
#define GL_MAX_MODELVIEW_STACK_DEPTH        0x0D36
#define GL_MAX_NAME_STACK_DEPTH     0x0D37
#define GL_MAX_PROJECTION_STACK_DEPTH       0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH  0x0D39
#define GL_MAX_VIEWPORT_DIMS        0x0D3A
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH    0x0D3B

/* Gets */
#define GL_ATTRIB_STACK_DEPTH       0x0BB0
#define GL_CLIENT_ATTRIB_STACK_DEPTH        0x0BB1
#define GL_COLOR_CLEAR_VALUE        0x0C22
#define GL_COLOR_WRITEMASK          0x0C23
#define GL_CURRENT_INDEX            0x0B01
#define GL_CURRENT_COLOR            0x0B00
#define GL_CURRENT_NORMAL           0x0B02
#define GL_CURRENT_RASTER_COLOR     0x0B04
#define GL_CURRENT_RASTER_DISTANCE  0x0B09
#define GL_CURRENT_RASTER_INDEX     0x0B05
#define GL_CURRENT_RASTER_POSITION  0x0B07
#define GL_CURRENT_RASTER_TEXTURE_COORDS    0x0B06
#define GL_CURRENT_RASTER_POSITION_VALID    0x0B08
#define GL_CURRENT_TEXTURE_COORDS   0x0B03
#define GL_INDEX_CLEAR_VALUE        0x0C20
#define GL_INDEX_MODE               0x0C30
#define GL_INDEX_WRITEMASK          0x0C21
#define GL_MODELVIEW_MATRIX         0x0BA6
#define GL_MODELVIEW_STACK_DEPTH    0x0BA3
#define GL_NAME_STACK_DEPTH         0x0D70
#define GL_PROJECTION_MATRIX        0x0BA7
#define GL_PROJECTION_STACK_DEPTH   0x0BA4
#define GL_RENDER_MODE              0x0C40
#define GL_RGBA_MODE                0x0C31
#define GL_TEXTURE_MATRIX           0x0BA8
#define GL_TEXTURE_STACK_DEPTH      0x0BA5
#define GL_VIEWPORT                 0x0BA2

/* Evaluators */
#define GL_AUTO_NORMAL              0x0D80
#define GL_MAP1_COLOR_4             0x0D90
#define GL_MAP1_INDEX               0x0D91
#define GL_MAP1_NORMAL              0x0D92
#define GL_MAP1_TEXTURE_COORD_1     0x0D93
#define GL_MAP1_TEXTURE_COORD_2     0x0D94
#define GL_MAP1_TEXTURE_COORD_3     0x0D95
#define GL_MAP1_TEXTURE_COORD_4     0x0D96
#define GL_MAP1_VERTEX_3            0x0D97
#define GL_MAP1_VERTEX_4            0x0D98
#define GL_MAP2_COLOR_4             0x0DB0
#define GL_MAP2_INDEX               0x0DB1
#define GL_MAP2_NORMAL              0x0DB2
#define GL_MAP2_TEXTURE_COORD_1     0x0DB3
#define GL_MAP2_TEXTURE_COORD_2     0x0DB4
#define GL_MAP2_TEXTURE_COORD_3     0x0DB5
#define GL_MAP2_TEXTURE_COORD_4     0x0DB6
#define GL_MAP2_VERTEX_3            0x0DB7
#define GL_MAP2_VERTEX_4            0x0DB8
#define GL_MAP1_GRID_DOMAIN         0x0DD0
#define GL_MAP1_GRID_SEGMENTS       0x0DD1
#define GL_MAP2_GRID_DOMAIN         0x0DD2
#define GL_MAP2_GRID_SEGMENTS       0x0DD3
#define GL_COEFF                    0x0A00
#define GL_ORDER                    0x0A01
#define GL_DOMAIN                   0x0A02

/* Hints */
#define GL_PERSPECTIVE_CORRECTION_HINT      0x0C50
#define GL_POINT_SMOOTH_HINT        0x0C51
#define GL_LINE_SMOOTH_HINT         0x0C52
#define GL_POLYGON_SMOOTH_HINT      0x0C53
#define GL_FOG_HINT                 0x0C54
#define GL_DONT_CARE				0x1100
#define GL_FASTEST                  0x1101
#define GL_NICEST                   0x1102

/* Scissor box */
#define GL_SCISSOR_BOX              0x0C10
#define GL_SCISSOR_TEST             0x0C11

/* Pixel Mode / Transfer */
#define GL_MAP_COLOR                0x0D10
#define GL_MAP_STENCIL              0x0D11
#define GL_INDEX_SHIFT              0x0D12
#define GL_INDEX_OFFSET             0x0D13
#define GL_RED_SCALE                0x0D14
#define GL_RED_BIAS                 0x0D15
#define GL_GREEN_SCALE              0x0D18
#define GL_GREEN_BIAS               0x0D19
#define GL_BLUE_SCALE               0x0D1A
#define GL_BLUE_BIAS                0x0D1B
#define GL_ALPHA_SCALE              0x0D1C
#define GL_ALPHA_BIAS               0x0D1D
#define GL_DEPTH_SCALE              0x0D1E
#define GL_DEPTH_BIAS               0x0D1F
#define GL_PIXEL_MAP_S_TO_S_SIZE    0x0CB1
#define GL_PIXEL_MAP_I_TO_I_SIZE    0x0CB0
#define GL_PIXEL_MAP_I_TO_R_SIZE    0x0CB2
#define GL_PIXEL_MAP_I_TO_G_SIZE    0x0CB3
#define GL_PIXEL_MAP_I_TO_B_SIZE    0x0CB4
#define GL_PIXEL_MAP_I_TO_A_SIZE    0x0CB5
#define GL_PIXEL_MAP_R_TO_R_SIZE    0x0CB6
#define GL_PIXEL_MAP_G_TO_G_SIZE    0x0CB7
#define GL_PIXEL_MAP_B_TO_B_SIZE    0x0CB8
#define GL_PIXEL_MAP_A_TO_A_SIZE    0x0CB9
#define GL_PIXEL_MAP_S_TO_S         0x0C71
#define GL_PIXEL_MAP_I_TO_I         0x0C70
#define GL_PIXEL_MAP_I_TO_R         0x0C72
#define GL_PIXEL_MAP_I_TO_G         0x0C73
#define GL_PIXEL_MAP_I_TO_B         0x0C74
#define GL_PIXEL_MAP_I_TO_A         0x0C75
#define GL_PIXEL_MAP_R_TO_R         0x0C76
#define GL_PIXEL_MAP_G_TO_G         0x0C77
#define GL_PIXEL_MAP_B_TO_B         0x0C78
#define GL_PIXEL_MAP_A_TO_A         0x0C79
#define GL_PACK_ALIGNMENT           0x0D05
#define GL_PACK_LSB_FIRST           0x0D01
#define GL_PACK_ROW_LENGTH          0x0D02
#define GL_PACK_SKIP_PIXELS         0x0D04
#define GL_PACK_SKIP_ROWS           0x0D03
#define GL_PACK_SWAP_BYTES          0x0D00
#define GL_UNPACK_ALIGNMENT         0x0CF5
#define GL_UNPACK_LSB_FIRST         0x0CF1
#define GL_UNPACK_ROW_LENGTH        0x0CF2
#define GL_UNPACK_SKIP_PIXELS       0x0CF4
#define GL_UNPACK_SKIP_ROWS         0x0CF3
#define GL_UNPACK_SWAP_BYTES        0x0CF0
#define GL_ZOOM_X                   0x0D16
#define GL_ZOOM_Y                   0x0D17

/* Texture mapping */
#define GL_TEXTURE_ENV              0x2300
#define GL_TEXTURE_ENV_MODE         0x2200
#define GL_TEXTURE_1D               0x0DE0
#define GL_TEXTURE_2D               0x0DE1
#define GL_TEXTURE_WRAP_S           0x2802
#define GL_TEXTURE_WRAP_T           0x2803
#define GL_TEXTURE_MAG_FILTER       0x2800
#define GL_TEXTURE_MIN_FILTER       0x2801
#define GL_TEXTURE_ENV_COLOR        0x2201
#define GL_TEXTURE_GEN_S            0x0C60
#define GL_TEXTURE_GEN_T            0x0C61
#define GL_TEXTURE_GEN_R            0x0C62
#define GL_TEXTURE_GEN_Q            0x0C63
#define GL_TEXTURE_GEN_MODE         0x2500
#define GL_TEXTURE_BORDER_COLOR     0x1004
#define GL_TEXTURE_WIDTH            0x1000
#define GL_TEXTURE_HEIGHT           0x1001
#define GL_TEXTURE_BORDER           0x1005
#define GL_TEXTURE_COMPONENTS       0x1003
#define GL_TEXTURE_RED_SIZE         0x805C
#define GL_TEXTURE_GREEN_SIZE       0x805D
#define GL_TEXTURE_BLUE_SIZE        0x805E
#define GL_TEXTURE_ALPHA_SIZE       0x805F
#define GL_TEXTURE_LUMINANCE_SIZE   0x8060
#define GL_TEXTURE_INTENSITY_SIZE   0x8061
#define GL_NEAREST_MIPMAP_NEAREST   0x2700
#define GL_NEAREST_MIPMAP_LINEAR    0x2702
#define GL_LINEAR_MIPMAP_NEAREST    0x2701
#define GL_LINEAR_MIPMAP_LINEAR     0x2703
#define GL_OBJECT_LINEAR            0x2401
#define GL_OBJECT_PLANE             0x2501
#define GL_EYE_LINEAR               0x2400
#define GL_EYE_PLANE                0x2502
#define GL_SPHERE_MAP               0x2402
#define GL_DECAL                    0x2101
#define GL_MODULATE                 0x2100
#define GL_NEAREST                  0x2600
#define GL_REPEAT                   0x2901
#define GL_CLAMP                    0x2900
#define GL_S                        0x2000
#define GL_T                        0x2001
#define GL_R                        0x2002
#define GL_Q                        0x2003

/* Utility */
#define GL_EXTENSIONS               0x1F03

/* Errors */
#define GL_STACK_OVERFLOW           0x0503
#define GL_STACK_UNDERFLOW          0x0504

/* glPush/PopAttrib bits */
#define GL_CURRENT_BIT              0x00000001
#define GL_POINT_BIT                0x00000002
#define GL_LINE_BIT                 0x00000004
#define GL_POLYGON_BIT              0x00000008
#define GL_POLYGON_STIPPLE_BIT      0x00000010
#define GL_PIXEL_MODE_BIT           0x00000020
#define GL_LIGHTING_BIT             0x00000040
#define GL_FOG_BIT                  0x00000080
#define GL_DEPTH_BUFFER_BIT         0x00000100
#define GL_ACCUM_BUFFER_BIT         0x00000200
#define GL_STENCIL_BUFFER_BIT       0x00000400
#define GL_VIEWPORT_BIT             0x00000800
#define GL_TRANSFORM_BIT            0x00001000
#define GL_ENABLE_BIT               0x00002000
#define GL_COLOR_BUFFER_BIT         0x00004000
#define GL_HINT_BIT                 0x00008000
#define GL_EVAL_BIT                 0x00010000
#define GL_LIST_BIT                 0x00020000
#define GL_TEXTURE_BIT              0x00040000
#define GL_SCISSOR_BIT              0x00080000
#define GL_ALL_ATTRIB_BITS          0x000FFFFF

#define GL_PROXY_TEXTURE_1D         0x8063
#define GL_PROXY_TEXTURE_2D         0x8064
#define GL_TEXTURE_PRIORITY         0x8066
#define GL_TEXTURE_RESIDENT         0x8067
#define GL_TEXTURE_BINDING_1D       0x8068
#define GL_TEXTURE_BINDING_2D       0x8069
#define GL_TEXTURE_INTERNAL_FORMAT  0x1003
#define GL_ALPHA4                   0x803B
#define GL_ALPHA8                   0x803C
#define GL_ALPHA12                  0x803D
#define GL_ALPHA16                  0x803E
#define GL_LUMINANCE4               0x803F
#define GL_LUMINANCE8               0x8040
#define GL_LUMINANCE12              0x8041
#define GL_LUMINANCE16              0x8042
#define GL_LUMINANCE4_ALPHA4        0x8043
#define GL_LUMINANCE6_ALPHA2        0x8044
#define GL_LUMINANCE8_ALPHA8        0x8045
#define GL_LUMINANCE12_ALPHA4       0x8046
#define GL_LUMINANCE12_ALPHA12      0x8047
#define GL_LUMINANCE16_ALPHA16      0x8048
#define GL_INTENSITY                0x8049
#define GL_INTENSITY4               0x804A
#define GL_INTENSITY8               0x804B
#define GL_INTENSITY12              0x804C
#define GL_INTENSITY16              0x804D
#define GL_R3_G3_B2                 0x2A10
#define GL_RGB4                     0x804F
#define GL_RGB5                     0x8050
#define GL_RGB8                     0x8051
#define GL_RGB10                    0x8052
#define GL_RGB12                    0x8053
#define GL_RGB16                    0x8054
#define GL_RGBA2                    0x8055
#define GL_RGBA4                    0x8056
#define GL_RGB5_A1                  0x8057
#define GL_RGBA8                    0x8058
#define GL_RGB10_A2                 0x8059
#define GL_RGBA12                   0x805A
#define GL_RGBA16                   0x805B
#define GL_CLIENT_PIXEL_STORE_BIT   0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT  0x00000002
#define GL_ALL_CLIENT_ATTRIB_BITS   0xFFFFFFFF
#define GL_CLIENT_ALL_ATTRIB_BITS   0xFFFFFFFF

typedef void (APIENTRYP PFNGLCLEARINDEXPROC) (GLfloat c);
typedef void (APIENTRYP PFNGLINDEXMASKPROC) (GLuint mask);
typedef void (APIENTRYP PFNGLALPHAFUNCPROC) (GLenum func, GLclampf ref);
typedef void (APIENTRYP PFNGLLINESTIPPLEPROC) (GLint factor, GLushort pattern);
typedef void (APIENTRYP PFNGLPOLYGONSTIPPLEPROC) (const GLubyte *mask);
typedef void (APIENTRYP PFNGLGETPOLYGONSTIPPLEPROC) (GLubyte *mask);
typedef void (APIENTRYP PFNGLEDGEFLAGPROC) (GLboolean flag);
typedef void (APIENTRYP PFNGLEDGEFLAGVPROC) (const GLboolean *flag);
typedef void (APIENTRYP PFNGLCLIPPLANEPROC) (GLenum plane, const GLdouble *equation);
typedef void (APIENTRYP PFNGLGETCLIPPLANEPROC) (GLenum plane, GLdouble *equation);
typedef void (APIENTRYP PFNGLENABLECLIENTSTATEPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLDISABLECLIENTSTATEPROC) (GLenum cap);
typedef void (APIENTRYP PFNGLPUSHATTRIBPROC) (GLbitfield mask);
typedef void (APIENTRYP PFNGLPOPATTRIBPROC) ();
typedef void (APIENTRYP PFNGLPUSHCLIENTATTRIBPROC) (GLbitfield mask);
typedef void (APIENTRYP PFNGLPOPCLIENTATTRIBPROC) ();
typedef GLint (APIENTRYP PFNGLRENDERMODEPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLCLEARACCUMPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLACCUMPROC) (GLenum op, GLfloat value);
typedef void (APIENTRYP PFNGLMATRIXMODEPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLORTHOPROC) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
typedef void (APIENTRYP PFNGLFRUSTUMPROC) (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
typedef void (APIENTRYP PFNGLPUSHMATRIXPROC) ();
typedef void (APIENTRYP PFNGLPOPMATRIXPROC) ();
typedef void (APIENTRYP PFNGLLOADIDENTITYPROC) ();
typedef void (APIENTRYP PFNGLLOADMATRIXDPROC) (const GLdouble *m);
typedef void (APIENTRYP PFNGLLOADMATRIXFPROC) (const GLfloat *m);
typedef void (APIENTRYP PFNGLMULTMATRIXDPROC) (const GLdouble *m);
typedef void (APIENTRYP PFNGLMULTMATRIXFPROC) (const GLfloat *m);
typedef void (APIENTRYP PFNGLROTATEDPROC) (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLROTATEFPROC) (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLSCALEDPROC) (GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLSCALEFPROC) (GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLTRANSLATEDPROC) (GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLTRANSLATEFPROC) (GLfloat x, GLfloat y, GLfloat z);
typedef GLboolean (APIENTRYP PFNGLISLISTPROC) (GLuint list);
typedef void (APIENTRYP PFNGLDELETELISTSPROC) (GLuint list, GLsizei range);
typedef GLuint (APIENTRYP PFNGLGENLISTSPROC) (GLsizei range);
typedef void (APIENTRYP PFNGLNEWLISTPROC) (GLuint list, GLenum mode);
typedef void (APIENTRYP PFNGLENDLISTPROC) ();
typedef void (APIENTRYP PFNGLCALLLISTPROC) (GLuint list);
typedef void (APIENTRYP PFNGLCALLLISTSPROC) (GLsizei n, GLenum type, const GLvoid *lists);
typedef void (APIENTRYP PFNGLLISTBASEPROC) (GLuint base);
typedef void (APIENTRYP PFNGLBEGINPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLENDPROC) ();
typedef void (APIENTRYP PFNGLVERTEX2DPROC) (GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLVERTEX2FPROC) (GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLVERTEX2IPROC) (GLint x, GLint y);
typedef void (APIENTRYP PFNGLVERTEX2SPROC) (GLshort x, GLshort y);
typedef void (APIENTRYP PFNGLVERTEX3DPROC) (GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLVERTEX3FPROC) (GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLVERTEX3IPROC) (GLint x, GLint y, GLint z);
typedef void (APIENTRYP PFNGLVERTEX3SPROC) (GLshort x, GLshort y, GLshort z);
typedef void (APIENTRYP PFNGLVERTEX4DPROC) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLVERTEX4FPROC) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLVERTEX4IPROC) (GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRYP PFNGLVERTEX4SPROC) (GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRYP PFNGLVERTEX2DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEX2FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEX2IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLVERTEX2SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEX3DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEX3FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEX3IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLVERTEX3SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLVERTEX4DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLVERTEX4FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLVERTEX4IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLVERTEX4SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLNORMAL3BPROC) (GLbyte nx, GLbyte ny, GLbyte nz);
typedef void (APIENTRYP PFNGLNORMAL3DPROC) (GLdouble nx, GLdouble ny, GLdouble nz);
typedef void (APIENTRYP PFNGLNORMAL3FPROC) (GLfloat nx, GLfloat ny, GLfloat nz);
typedef void (APIENTRYP PFNGLNORMAL3IPROC) (GLint nx, GLint ny, GLint nz);
typedef void (APIENTRYP PFNGLNORMAL3SPROC) (GLshort nx, GLshort ny, GLshort nz);
typedef void (APIENTRYP PFNGLNORMAL3BVPROC) (const GLbyte *v);
typedef void (APIENTRYP PFNGLNORMAL3DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLNORMAL3FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLNORMAL3IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLNORMAL3SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLINDEXDPROC) (GLdouble c);
typedef void (APIENTRYP PFNGLINDEXFPROC) (GLfloat c);
typedef void (APIENTRYP PFNGLINDEXIPROC) (GLint c);
typedef void (APIENTRYP PFNGLINDEXSPROC) (GLshort c);
typedef void (APIENTRYP PFNGLINDEXUBPROC) (GLubyte c);
typedef void (APIENTRYP PFNGLINDEXDVPROC) (const GLdouble *c);
typedef void (APIENTRYP PFNGLINDEXFVPROC) (const GLfloat *c);
typedef void (APIENTRYP PFNGLINDEXIVPROC) (const GLint *c);
typedef void (APIENTRYP PFNGLINDEXSVPROC) (const GLshort *c);
typedef void (APIENTRYP PFNGLINDEXUBVPROC) (const GLubyte *c);
typedef void (APIENTRYP PFNGLCOLOR3BPROC) (GLbyte red, GLbyte green, GLbyte blue);
typedef void (APIENTRYP PFNGLCOLOR3DPROC) (GLdouble red, GLdouble green, GLdouble blue);
typedef void (APIENTRYP PFNGLCOLOR3FPROC) (GLfloat red, GLfloat green, GLfloat blue);
typedef void (APIENTRYP PFNGLCOLOR3IPROC) (GLint red, GLint green, GLint blue);
typedef void (APIENTRYP PFNGLCOLOR3SPROC) (GLshort red, GLshort green, GLshort blue);
typedef void (APIENTRYP PFNGLCOLOR3UBPROC) (GLubyte red, GLubyte green, GLubyte blue);
typedef void (APIENTRYP PFNGLCOLOR3UIPROC) (GLuint red, GLuint green, GLuint blue);
typedef void (APIENTRYP PFNGLCOLOR3USPROC) (GLushort red, GLushort green, GLushort blue);
typedef void (APIENTRYP PFNGLCOLOR4BPROC) (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
typedef void (APIENTRYP PFNGLCOLOR4DPROC) (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
typedef void (APIENTRYP PFNGLCOLOR4FPROC) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLCOLOR4IPROC) (GLint red, GLint green, GLint blue, GLint alpha);
typedef void (APIENTRYP PFNGLCOLOR4SPROC) (GLshort red, GLshort green, GLshort blue, GLshort alpha);
typedef void (APIENTRYP PFNGLCOLOR4UBPROC) (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
typedef void (APIENTRYP PFNGLCOLOR4UIPROC) (GLuint red, GLuint green, GLuint blue, GLuint alpha);
typedef void (APIENTRYP PFNGLCOLOR4USPROC) (GLushort red, GLushort green, GLushort blue, GLushort alpha);
typedef void (APIENTRYP PFNGLCOLOR3BVPROC) (const GLbyte *v);
typedef void (APIENTRYP PFNGLCOLOR3DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLCOLOR3FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLCOLOR3IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLCOLOR3SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLCOLOR3UBVPROC) (const GLubyte *v);
typedef void (APIENTRYP PFNGLCOLOR3UIVPROC) (const GLuint *v);
typedef void (APIENTRYP PFNGLCOLOR3USVPROC) (const GLushort *v);
typedef void (APIENTRYP PFNGLCOLOR4BVPROC) (const GLbyte *v);
typedef void (APIENTRYP PFNGLCOLOR4DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLCOLOR4FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLCOLOR4IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLCOLOR4SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLCOLOR4UBVPROC) (const GLubyte *v);
typedef void (APIENTRYP PFNGLCOLOR4UIVPROC) (const GLuint *v);
typedef void (APIENTRYP PFNGLCOLOR4USVPROC) (const GLushort *v);
typedef void (APIENTRYP PFNGLTEXCOORD1DPROC) (GLdouble s);
typedef void (APIENTRYP PFNGLTEXCOORD1FPROC) (GLfloat s);
typedef void (APIENTRYP PFNGLTEXCOORD1IPROC) (GLint s);
typedef void (APIENTRYP PFNGLTEXCOORD1SPROC) (GLshort s);
typedef void (APIENTRYP PFNGLTEXCOORD2DPROC) (GLdouble s, GLdouble t);
typedef void (APIENTRYP PFNGLTEXCOORD2FPROC) (GLfloat s, GLfloat t);
typedef void (APIENTRYP PFNGLTEXCOORD2IPROC) (GLint s, GLint t);
typedef void (APIENTRYP PFNGLTEXCOORD2SPROC) (GLshort s, GLshort t);
typedef void (APIENTRYP PFNGLTEXCOORD3DPROC) (GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRYP PFNGLTEXCOORD3FPROC) (GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRYP PFNGLTEXCOORD3IPROC) (GLint s, GLint t, GLint r);
typedef void (APIENTRYP PFNGLTEXCOORD3SPROC) (GLshort s, GLshort t, GLshort r);
typedef void (APIENTRYP PFNGLTEXCOORD4DPROC) (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRYP PFNGLTEXCOORD4FPROC) (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRYP PFNGLTEXCOORD4IPROC) (GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRYP PFNGLTEXCOORD4SPROC) (GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRYP PFNGLTEXCOORD1DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXCOORD1FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD1IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLTEXCOORD1SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLTEXCOORD2DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXCOORD2FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD2IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLTEXCOORD2SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLTEXCOORD3DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXCOORD3FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD3IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLTEXCOORD3SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLTEXCOORD4DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLTEXCOORD4FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLTEXCOORD4IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLTEXCOORD4SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLRASTERPOS2DPROC) (GLdouble x, GLdouble y);
typedef void (APIENTRYP PFNGLRASTERPOS2FPROC) (GLfloat x, GLfloat y);
typedef void (APIENTRYP PFNGLRASTERPOS2IPROC) (GLint x, GLint y);
typedef void (APIENTRYP PFNGLRASTERPOS2SPROC) (GLshort x, GLshort y);
typedef void (APIENTRYP PFNGLRASTERPOS3DPROC) (GLdouble x, GLdouble y, GLdouble z);
typedef void (APIENTRYP PFNGLRASTERPOS3FPROC) (GLfloat x, GLfloat y, GLfloat z);
typedef void (APIENTRYP PFNGLRASTERPOS3IPROC) (GLint x, GLint y, GLint z);
typedef void (APIENTRYP PFNGLRASTERPOS3SPROC) (GLshort x, GLshort y, GLshort z);
typedef void (APIENTRYP PFNGLRASTERPOS4DPROC) (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (APIENTRYP PFNGLRASTERPOS4FPROC) (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLRASTERPOS4IPROC) (GLint x, GLint y, GLint z, GLint w);
typedef void (APIENTRYP PFNGLRASTERPOS4SPROC) (GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (APIENTRYP PFNGLRASTERPOST2DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLRASTERPOST2FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLRASTERPOST2IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLRASTERPOST2SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLRASTERPOST3DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLRASTERPOST3FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLRASTERPOST3IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLRASTERPOST3SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLRASTERPOST4DVPROC) (const GLdouble *v);
typedef void (APIENTRYP PFNGLRASTERPOST4FVPROC) (const GLfloat *v);
typedef void (APIENTRYP PFNGLRASTERPOST4IVPROC) (const GLint *v);
typedef void (APIENTRYP PFNGLRASTERPOST4SVPROC) (const GLshort *v);
typedef void (APIENTRYP PFNGLRECTDPROC) (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
typedef void (APIENTRYP PFNGLRECTFPROC) (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
typedef void (APIENTRYP PFNGLRECTIPROC) (GLint x1, GLint y1, GLint x2, GLint y2);
typedef void (APIENTRYP PFNGLRECTSPROC) (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
typedef void (APIENTRYP PFNGLRECTDVPROC) (const GLdouble *v1, const GLdouble *v2);
typedef void (APIENTRYP PFNGLRECTFVPROC) (const GLfloat *v1, const GLfloat *v2);
typedef void (APIENTRYP PFNGLRECTIVPROC) (const GLint *v1, const GLint *v2);
typedef void (APIENTRYP PFNGLRECTSVPROC) (const GLshort *v1, const GLshort *v2);
typedef void (APIENTRYP PFNGLVERTEXPOINTERPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
typedef void (APIENTRYP PFNGLNORMALPOINTERPROC) (GLenum type, GLsizei stride, const GLvoid *ptr);
typedef void (APIENTRYP PFNGLCOLORPOINTERPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
typedef void (APIENTRYP PFNGLINDEXPOINTERPROC) (GLenum type, GLsizei stride, const GLvoid *ptr);
typedef void (APIENTRYP PFNGLTEXCOORDPOINTERPROC) (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
typedef void (APIENTRYP PFNGLEDGEFLAGPOINTERPROC) (GLsizei stride, const GLvoid *ptr);
typedef void (APIENTRYP PFNGLARRAYELEMENTPROC) (GLint i);
typedef void (APIENTRYP PFNGLINTERLEAVEDARRAYSPROC) (GLenum format, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRYP PFNGLSHADEMODELPROC) (GLenum mode);
typedef void (APIENTRYP PFNGLLIGHTFPROC) (GLenum light, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLLIGHTIPROC) (GLenum light, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLLIGHTFVPROC) (GLenum light, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLLIGHTIVPROC) (GLenum light, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLGETLIGHTFVPROC) (GLenum light, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETLIGHTIVPROC) (GLenum light, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLLIGHTMODELFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLLIGHTMODELIPROC) (GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLLIGHTMODELFVPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLLIGHTMODELIVPROC) (GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLMATERIALFPROC) (GLenum face, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLMATERIALIPROC) (GLenum face, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLMATERIALFVPROC) (GLenum face, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLMATERIALIVPROC) (GLenum face, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLGETMATERIALFVPROC) (GLenum face, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETMATERIALIVPROC) (GLenum face, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLCOLORMATERIALPROC) (GLenum face, GLenum mode);
typedef void (APIENTRYP PFNGLPIXELZOOMPROC) (GLfloat xfactor, GLfloat yfactor);
typedef void (APIENTRYP PFNGLPIXELTRANSFERFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLPIXELTRANSFERIPROC) (GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLPIXELMAPFVPROC) (GLenum map, GLsizei mapsize, const GLfloat *values);
typedef void (APIENTRYP PFNGLPIXELMAPUIVPROC) (GLenum map, GLsizei mapsize, const GLuint *values);
typedef void (APIENTRYP PFNGLPIXELMAPUSVPROC) (GLenum map, GLsizei mapsize, const GLushort *values);
typedef void (APIENTRYP PFNGLGETPIXELMAPFVPROC) (GLenum map, GLfloat *values);
typedef void (APIENTRYP PFNGLGETPIXELMAPUIVPROC) (GLenum map, GLuint *values);
typedef void (APIENTRYP PFNGLGETPIXELMAPUSPROC) (GLenum map, GLushort *values);
typedef void (APIENTRYP PFNGLBITMAPPROC) (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
typedef void (APIENTRYP PFNGLDRAWPIXELSPROC) (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRYP PFNGLCOPYPIXELSPROC) (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
typedef void (APIENTRYP PFNGLTEXGENDPROC) (GLenum coord, GLenum pname, GLdouble param);
typedef void (APIENTRYP PFNGLTEXGENFPROC) (GLenum coord, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLTEXGENIPROC) (GLenum coord, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXGENDVPROC) (GLenum coord, GLenum pname, const GLdouble *params);
typedef void (APIENTRYP PFNGLTEXGENFVPROC) (GLenum coord, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLTEXGENIVPROC) (GLenum coord, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLGETTEXGENDVPROC) (GLenum coord, GLenum pname, GLdouble *params);
typedef void (APIENTRYP PFNGLGETTEXGENFVPROC) (GLenum coord, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXGENIVPROC) (GLenum coord, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLTEXENVFPROC) (GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLTEXENVIPROC) (GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLTEXENVFVPROC) (GLenum target, GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLTEXENVIVPROC) (GLenum target, GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLGETTEXENVFVPROC) (GLenum target, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETTEXENVIVPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLPRIORITIZETEXTURESPROC) (GLsizei n, const GLuint *textures, const GLclampf *priorities);
typedef GLboolean (APIENTRYP PFNGLARETEXTURESRESIDENTPROC) (GLsizei n, const GLuint *textures, GLboolean *residences);
typedef void (APIENTRYP PFNGLMAP1DPROC) (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
typedef void (APIENTRYP PFNGLMAP1FPROC) (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
typedef void (APIENTRYP PFNGLMAP2DPROC) (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
typedef void (APIENTRYP PFNGLMAP2FPROC) (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
typedef void (APIENTRYP PFNGLGETMAPDVPROC) (GLenum target, GLenum query, GLdouble *v);
typedef void (APIENTRYP PFNGLGETMAPFVPROC) (GLenum target, GLenum query, GLfloat *v);
typedef void (APIENTRYP PFNGLGETMAPIVPROC) (GLenum target, GLenum query, GLint *v);
typedef void (APIENTRYP PFNGLEVALCOORD1DPROC) (GLdouble u);
typedef void (APIENTRYP PFNGLEVALCOORD1FPROC) (GLfloat u);
typedef void (APIENTRYP PFNGLEVALCOORD1DVPROC) (const GLdouble *u);
typedef void (APIENTRYP PFNGLEVALCOORD1FVPROC) (const GLfloat *u);
typedef void (APIENTRYP PFNGLEVALCOORD2DPROC) (GLdouble u, GLdouble v);
typedef void (APIENTRYP PFNGLEVALCOORD2FPROC) (GLfloat u, GLfloat v);
typedef void (APIENTRYP PFNGLEVALCOORD2DVPROC) (const GLdouble *u);
typedef void (APIENTRYP PFNGLEVALCOORD2FVPROC) (const GLfloat *u);
typedef void (APIENTRYP PFNGLMAPGRID1DPROC) (GLint un, GLdouble u1, GLdouble u2);
typedef void (APIENTRYP PFNGLMAPGRID1FPROC) (GLint un, GLfloat u1, GLfloat u2);
typedef void (APIENTRYP PFNGLMAPGRID2DPROC) (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
typedef void (APIENTRYP PFNGLMAPGRID2FPROC) (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLEVALPOINT1PROC) (GLint i);
typedef void (APIENTRYP PFNGLEVALPOINT2PROC) (GLint i, GLint j);
typedef void (APIENTRYP PFNGLEVALMESH1PROC) (GLenum mode, GLint i1, GLint i2);
typedef void (APIENTRYP PFNGLEVALMESH2PROC) (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
typedef void (APIENTRYP PFNGLFOGFPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLFOGIPROC) (GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLFOGFVPROC) (GLenum pname, const GLfloat *params);
typedef void (APIENTRYP PFNGLFOGIVPROC) (GLenum pname, const GLint *params);
typedef void (APIENTRYP PFNGLFEEDBACKBUFFERPROC) (GLsizei size, GLenum type, GLfloat *buffer);
typedef void (APIENTRYP PFNGLPASSTHROUGHPROC) (GLfloat token);
typedef void (APIENTRYP PFNGLSELECTBUFFERPROC) (GLsizei size, GLuint *buffer);
typedef void (APIENTRYP PFNGLINITNAMESPROC) ();
typedef void (APIENTRYP PFNGLLOADNAMEPROC) (GLuint name);
typedef void (APIENTRYP PFNGLPUSHNAMEPROC) (GLuint name);
typedef void (APIENTRYP PFNGLPOPNAMEPROC) ();


GLAPI void APIENTRY glClearIndex(GLfloat c);
GLAPI void APIENTRY glIndexMask(GLuint mask);
GLAPI void APIENTRY glAlphaFunc(GLenum func, GLclampf ref);
GLAPI void APIENTRY glLineStipple(GLint factor, GLushort pattern);
GLAPI void APIENTRY glPolygonStipple(const GLubyte *mask);
GLAPI void APIENTRY glGetPolygonStipple(GLubyte *mask);
GLAPI void APIENTRY glEdgeFlag(GLboolean flag);
GLAPI void APIENTRY glEdgeFlagv(const GLboolean *flag);
GLAPI void APIENTRY glClipPlane(GLenum plane, const GLdouble *equation);
GLAPI void APIENTRY glGetClipPlane(GLenum plane, GLdouble *equation);
GLAPI void APIENTRY glEnableClientState(GLenum cap);  /* 1.1 */
GLAPI void APIENTRY glDisableClientState(GLenum cap);  /* 1.1 */
GLAPI void APIENTRY glPushAttrib(GLbitfield mask);
GLAPI void APIENTRY glPopAttrib(void);
GLAPI void APIENTRY glPushClientAttrib(GLbitfield mask);  /* 1.1 */
GLAPI void APIENTRY glPopClientAttrib(void);  /* 1.1 */
GLAPI GLint APIENTRY glRenderMode(GLenum mode);
GLAPI void APIENTRY glClearAccum(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI void APIENTRY glAccum(GLenum op, GLfloat value);
GLAPI void APIENTRY glMatrixMode(GLenum mode);
GLAPI void APIENTRY glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
GLAPI void APIENTRY glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
GLAPI void APIENTRY glPushMatrix(void);
GLAPI void APIENTRY glPopMatrix(void);
GLAPI void APIENTRY glLoadIdentity(void);
GLAPI void APIENTRY glLoadMatrixd(const GLdouble *m);
GLAPI void APIENTRY glLoadMatrixf(const GLfloat *m);
GLAPI void APIENTRY glMultMatrixd(const GLdouble *m);
GLAPI void APIENTRY glMultMatrixf(const GLfloat *m);
GLAPI void APIENTRY glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
GLAPI void APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glScaled(GLdouble x, GLdouble y, GLdouble z);
GLAPI void APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glTranslated(GLdouble x, GLdouble y, GLdouble z);
GLAPI void APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z);
GLAPI GLboolean APIENTRY glIsList(GLuint list);
GLAPI void APIENTRY glDeleteLists(GLuint list, GLsizei range);
GLAPI GLuint APIENTRY glGenLists(GLsizei range);
GLAPI void APIENTRY glNewList(GLuint list, GLenum mode);
GLAPI void APIENTRY glEndList(void);
GLAPI void APIENTRY glCallList(GLuint list);
GLAPI void APIENTRY glCallLists(GLsizei n, GLenum type, const GLvoid *lists);
GLAPI void APIENTRY glListBase(GLuint base);
GLAPI void APIENTRY glBegin(GLenum mode);
GLAPI void APIENTRY glEnd(void);
GLAPI void APIENTRY glVertex2d(GLdouble x, GLdouble y);
GLAPI void APIENTRY glVertex2f(GLfloat x, GLfloat y);
GLAPI void APIENTRY glVertex2i(GLint x, GLint y);
GLAPI void APIENTRY glVertex2s(GLshort x, GLshort y);
GLAPI void APIENTRY glVertex3d(GLdouble x, GLdouble y, GLdouble z);
GLAPI void APIENTRY glVertex3f(GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glVertex3i(GLint x, GLint y, GLint z);
GLAPI void APIENTRY glVertex3s(GLshort x, GLshort y, GLshort z);
GLAPI void APIENTRY glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLAPI void APIENTRY glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI void APIENTRY glVertex4i(GLint x, GLint y, GLint z, GLint w);
GLAPI void APIENTRY glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w);
GLAPI void APIENTRY glVertex2dv(const GLdouble *v);
GLAPI void APIENTRY glVertex2fv(const GLfloat *v);
GLAPI void APIENTRY glVertex2iv(const GLint *v);
GLAPI void APIENTRY glVertex2sv(const GLshort *v);
GLAPI void APIENTRY glVertex3dv(const GLdouble *v);
GLAPI void APIENTRY glVertex3fv(const GLfloat *v);
GLAPI void APIENTRY glVertex3iv(const GLint *v);
GLAPI void APIENTRY glVertex3sv(const GLshort *v);
GLAPI void APIENTRY glVertex4dv(const GLdouble *v);
GLAPI void APIENTRY glVertex4fv(const GLfloat *v);
GLAPI void APIENTRY glVertex4iv(const GLint *v);
GLAPI void APIENTRY glVertex4sv(const GLshort *v);
GLAPI void APIENTRY glNormal3b(GLbyte nx, GLbyte ny, GLbyte nz);
GLAPI void APIENTRY glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz);
GLAPI void APIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
GLAPI void APIENTRY glNormal3i(GLint nx, GLint ny, GLint nz);
GLAPI void APIENTRY glNormal3s(GLshort nx, GLshort ny, GLshort nz);
GLAPI void APIENTRY glNormal3bv(const GLbyte *v);
GLAPI void APIENTRY glNormal3dv(const GLdouble *v);
GLAPI void APIENTRY glNormal3fv(const GLfloat *v);
GLAPI void APIENTRY glNormal3iv(const GLint *v);
GLAPI void APIENTRY glNormal3sv(const GLshort *v);
GLAPI void APIENTRY glIndexd(GLdouble c);
GLAPI void APIENTRY glIndexf(GLfloat c);
GLAPI void APIENTRY glIndexi(GLint c);
GLAPI void APIENTRY glIndexs(GLshort c);
GLAPI void APIENTRY glIndexub(GLubyte c);  /* 1.1 */
GLAPI void APIENTRY glIndexdv(const GLdouble *c);
GLAPI void APIENTRY glIndexfv(const GLfloat *c);
GLAPI void APIENTRY glIndexiv(const GLint *c);
GLAPI void APIENTRY glIndexsv(const GLshort *c);
GLAPI void APIENTRY glIndexubv(const GLubyte *c);  /* 1.1 */
GLAPI void APIENTRY glColor3b(GLbyte red, GLbyte green, GLbyte blue);
GLAPI void APIENTRY glColor3d(GLdouble red, GLdouble green, GLdouble blue);
GLAPI void APIENTRY glColor3f(GLfloat red, GLfloat green, GLfloat blue);
GLAPI void APIENTRY glColor3i(GLint red, GLint green, GLint blue);
GLAPI void APIENTRY glColor3s(GLshort red, GLshort green, GLshort blue);
GLAPI void APIENTRY glColor3ub(GLubyte red, GLubyte green, GLubyte blue);
GLAPI void APIENTRY glColor3ui(GLuint red, GLuint green, GLuint blue);
GLAPI void APIENTRY glColor3us(GLushort red, GLushort green, GLushort blue);
GLAPI void APIENTRY glColor4b(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
GLAPI void APIENTRY glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
GLAPI void APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI void APIENTRY glColor4i(GLint red, GLint green, GLint blue, GLint alpha);
GLAPI void APIENTRY glColor4s(GLshort red, GLshort green, GLshort blue, GLshort alpha);
GLAPI void APIENTRY glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
GLAPI void APIENTRY glColor4ui(GLuint red, GLuint green, GLuint blue, GLuint alpha);
GLAPI void APIENTRY glColor4us(GLushort red, GLushort green, GLushort blue, GLushort alpha);
GLAPI void APIENTRY glColor3bv(const GLbyte *v);
GLAPI void APIENTRY glColor3dv(const GLdouble *v);
GLAPI void APIENTRY glColor3fv(const GLfloat *v);
GLAPI void APIENTRY glColor3iv(const GLint *v);
GLAPI void APIENTRY glColor3sv(const GLshort *v);
GLAPI void APIENTRY glColor3ubv(const GLubyte *v);
GLAPI void APIENTRY glColor3uiv(const GLuint *v);
GLAPI void APIENTRY glColor3usv(const GLushort *v);
GLAPI void APIENTRY glColor4bv(const GLbyte *v);
GLAPI void APIENTRY glColor4dv(const GLdouble *v);
GLAPI void APIENTRY glColor4fv(const GLfloat *v);
GLAPI void APIENTRY glColor4iv(const GLint *v);
GLAPI void APIENTRY glColor4sv(const GLshort *v);
GLAPI void APIENTRY glColor4ubv(const GLubyte *v);
GLAPI void APIENTRY glColor4uiv(const GLuint *v);
GLAPI void APIENTRY glColor4usv(const GLushort *v);
GLAPI void APIENTRY glTexCoord1d(GLdouble s);
GLAPI void APIENTRY glTexCoord1f(GLfloat s);
GLAPI void APIENTRY glTexCoord1i(GLint s);
GLAPI void APIENTRY glTexCoord1s(GLshort s);
GLAPI void APIENTRY glTexCoord2d(GLdouble s, GLdouble t);
GLAPI void APIENTRY glTexCoord2f(GLfloat s, GLfloat t);
GLAPI void APIENTRY glTexCoord2i(GLint s, GLint t);
GLAPI void APIENTRY glTexCoord2s(GLshort s, GLshort t);
GLAPI void APIENTRY glTexCoord3d(GLdouble s, GLdouble t, GLdouble r);
GLAPI void APIENTRY glTexCoord3f(GLfloat s, GLfloat t, GLfloat r);
GLAPI void APIENTRY glTexCoord3i(GLint s, GLint t, GLint r);
GLAPI void APIENTRY glTexCoord3s(GLshort s, GLshort t, GLshort r);
GLAPI void APIENTRY glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLAPI void APIENTRY glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLAPI void APIENTRY glTexCoord4i(GLint s, GLint t, GLint r, GLint q);
GLAPI void APIENTRY glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q);
GLAPI void APIENTRY glTexCoord1dv(const GLdouble *v);
GLAPI void APIENTRY glTexCoord1fv(const GLfloat *v);
GLAPI void APIENTRY glTexCoord1iv(const GLint *v);
GLAPI void APIENTRY glTexCoord1sv(const GLshort *v);
GLAPI void APIENTRY glTexCoord2dv(const GLdouble *v);
GLAPI void APIENTRY glTexCoord2fv(const GLfloat *v);
GLAPI void APIENTRY glTexCoord2iv(const GLint *v);
GLAPI void APIENTRY glTexCoord2sv(const GLshort *v);
GLAPI void APIENTRY glTexCoord3dv(const GLdouble *v);
GLAPI void APIENTRY glTexCoord3fv(const GLfloat *v);
GLAPI void APIENTRY glTexCoord3iv(const GLint *v);
GLAPI void APIENTRY glTexCoord3sv(const GLshort *v);
GLAPI void APIENTRY glTexCoord4dv(const GLdouble *v);
GLAPI void APIENTRY glTexCoord4fv(const GLfloat *v);
GLAPI void APIENTRY glTexCoord4iv(const GLint *v);
GLAPI void APIENTRY glTexCoord4sv(const GLshort *v);
GLAPI void APIENTRY glRasterPos2d(GLdouble x, GLdouble y);
GLAPI void APIENTRY glRasterPos2f(GLfloat x, GLfloat y);
GLAPI void APIENTRY glRasterPos2i(GLint x, GLint y);
GLAPI void APIENTRY glRasterPos2s(GLshort x, GLshort y);
GLAPI void APIENTRY glRasterPos3d(GLdouble x, GLdouble y, GLdouble z);
GLAPI void APIENTRY glRasterPos3f(GLfloat x, GLfloat y, GLfloat z);
GLAPI void APIENTRY glRasterPos3i(GLint x, GLint y, GLint z);
GLAPI void APIENTRY glRasterPos3s(GLshort x, GLshort y, GLshort z);
GLAPI void APIENTRY glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLAPI void APIENTRY glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLAPI void APIENTRY glRasterPos4i(GLint x, GLint y, GLint z, GLint w);
GLAPI void APIENTRY glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w);
GLAPI void APIENTRY glRasterPos2dv(const GLdouble *v);
GLAPI void APIENTRY glRasterPos2fv(const GLfloat *v);
GLAPI void APIENTRY glRasterPos2iv(const GLint *v);
GLAPI void APIENTRY glRasterPos2sv(const GLshort *v);
GLAPI void APIENTRY glRasterPos3dv(const GLdouble *v);
GLAPI void APIENTRY glRasterPos3fv(const GLfloat *v);
GLAPI void APIENTRY glRasterPos3iv(const GLint *v);
GLAPI void APIENTRY glRasterPos3sv(const GLshort *v);
GLAPI void APIENTRY glRasterPos4dv(const GLdouble *v);
GLAPI void APIENTRY glRasterPos4fv(const GLfloat *v);
GLAPI void APIENTRY glRasterPos4iv(const GLint *v);
GLAPI void APIENTRY glRasterPos4sv(const GLshort *v);
GLAPI void APIENTRY glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
GLAPI void APIENTRY glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLAPI void APIENTRY glRecti(GLint x1, GLint y1, GLint x2, GLint y2);
GLAPI void APIENTRY glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
GLAPI void APIENTRY glRectdv(const GLdouble *v1, const GLdouble *v2);
GLAPI void APIENTRY glRectfv(const GLfloat *v1, const GLfloat *v2);
GLAPI void APIENTRY glRectiv(const GLint *v1, const GLint *v2);
GLAPI void APIENTRY glRectsv(const GLshort *v1, const GLshort *v2);
GLAPI void APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
GLAPI void APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid *ptr);
GLAPI void APIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
GLAPI void APIENTRY glIndexPointer(GLenum type, GLsizei stride, const GLvoid *ptr);
GLAPI void APIENTRY glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
GLAPI void APIENTRY glEdgeFlagPointer(GLsizei stride, const GLvoid *ptr);
GLAPI void APIENTRY glArrayElement(GLint i);
GLAPI void APIENTRY glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid *pointer);
GLAPI void APIENTRY glShadeModel(GLenum mode);
GLAPI void APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param);
GLAPI void APIENTRY glLighti(GLenum light, GLenum pname, GLint param);
GLAPI void APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat *params);
GLAPI void APIENTRY glLightiv(GLenum light, GLenum pname, const GLint *params);
GLAPI void APIENTRY glGetLightfv(GLenum light, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetLightiv(GLenum light, GLenum pname, GLint *params);
GLAPI void APIENTRY glLightModelf(GLenum pname, GLfloat param);
GLAPI void APIENTRY glLightModeli(GLenum pname, GLint param);
GLAPI void APIENTRY glLightModelfv(GLenum pname, const GLfloat *params);
GLAPI void APIENTRY glLightModeliv(GLenum pname, const GLint *params);
GLAPI void APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param);
GLAPI void APIENTRY glMateriali(GLenum face, GLenum pname, GLint param);
GLAPI void APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
GLAPI void APIENTRY glMaterialiv(GLenum face, GLenum pname, const GLint *params);
GLAPI void APIENTRY glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetMaterialiv(GLenum face, GLenum pname, GLint *params);
GLAPI void APIENTRY glColorMaterial(GLenum face, GLenum mode);
GLAPI void APIENTRY glPixelZoom(GLfloat xfactor, GLfloat yfactor);
GLAPI void APIENTRY glPixelTransferf(GLenum pname, GLfloat param);
GLAPI void APIENTRY glPixelTransferi(GLenum pname, GLint param);
GLAPI void APIENTRY glPixelMapfv(GLenum map, GLsizei mapsize, const GLfloat *values);
GLAPI void APIENTRY glPixelMapuiv(GLenum map, GLsizei mapsize, const GLuint *values);
GLAPI void APIENTRY glPixelMapusv(GLenum map, GLsizei mapsize, const GLushort *values);
GLAPI void APIENTRY glGetPixelMapfv(GLenum map, GLfloat *values);
GLAPI void APIENTRY glGetPixelMapuiv(GLenum map, GLuint *values);
GLAPI void APIENTRY glGetPixelMapusv(GLenum map, GLushort *values);
GLAPI void APIENTRY glBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
GLAPI void APIENTRY glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
GLAPI void APIENTRY glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
GLAPI void APIENTRY glTexGend(GLenum coord, GLenum pname, GLdouble param);
GLAPI void APIENTRY glTexGenf(GLenum coord, GLenum pname, GLfloat param);
GLAPI void APIENTRY glTexGeni(GLenum coord, GLenum pname, GLint param);
GLAPI void APIENTRY glTexGendv(GLenum coord, GLenum pname, const GLdouble *params);
GLAPI void APIENTRY glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params);
GLAPI void APIENTRY glTexGeniv(GLenum coord, GLenum pname, const GLint *params);
GLAPI void APIENTRY glGetTexGendv(GLenum coord, GLenum pname, GLdouble *params);
GLAPI void APIENTRY glGetTexGenfv(GLenum coord, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetTexGeniv(GLenum coord, GLenum pname, GLint *params);
GLAPI void APIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param);
GLAPI void APIENTRY glTexEnvi(GLenum target, GLenum pname, GLint param);
GLAPI void APIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params);
GLAPI void APIENTRY glTexEnviv(GLenum target, GLenum pname, const GLint *params);
GLAPI void APIENTRY glGetTexEnvfv(GLenum target, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetTexEnviv(GLenum target, GLenum pname, GLint *params);
GLAPI void APIENTRY glPrioritizeTextures(GLsizei n, const GLuint *textures, const GLclampf *priorities);
GLAPI GLboolean APIENTRY glAreTexturesResident(GLsizei n, const GLuint *textures, GLboolean *residences);
GLAPI void APIENTRY glMap1d(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
GLAPI void APIENTRY glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
GLAPI void APIENTRY glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
GLAPI void APIENTRY glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
GLAPI void APIENTRY glGetMapdv(GLenum target, GLenum query, GLdouble *v);
GLAPI void APIENTRY glGetMapfv(GLenum target, GLenum query, GLfloat *v);
GLAPI void APIENTRY glGetMapiv(GLenum target, GLenum query, GLint *v);
GLAPI void APIENTRY glEvalCoord1d(GLdouble u);
GLAPI void APIENTRY glEvalCoord1f(GLfloat u);
GLAPI void APIENTRY glEvalCoord1dv(const GLdouble *u);
GLAPI void APIENTRY glEvalCoord1fv(const GLfloat *u);
GLAPI void APIENTRY glEvalCoord2d(GLdouble u, GLdouble v);
GLAPI void APIENTRY glEvalCoord2f(GLfloat u, GLfloat v);
GLAPI void APIENTRY glEvalCoord2dv(const GLdouble *u);
GLAPI void APIENTRY glEvalCoord2fv(const GLfloat *u);
GLAPI void APIENTRY glMapGrid1d(GLint un, GLdouble u1, GLdouble u2);
GLAPI void APIENTRY glMapGrid1f(GLint un, GLfloat u1, GLfloat u2);
GLAPI void APIENTRY glMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
GLAPI void APIENTRY glMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
GLAPI void APIENTRY glEvalPoint1(GLint i);
GLAPI void APIENTRY glEvalPoint2(GLint i, GLint j);
GLAPI void APIENTRY glEvalMesh1(GLenum mode, GLint i1, GLint i2);
GLAPI void APIENTRY glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
GLAPI void APIENTRY glFogf(GLenum pname, GLfloat param);
GLAPI void APIENTRY glFogi(GLenum pname, GLint param);
GLAPI void APIENTRY glFogfv(GLenum pname, const GLfloat *params);
GLAPI void APIENTRY glFogiv(GLenum pname, const GLint *params);
GLAPI void APIENTRY glFeedbackBuffer(GLsizei size, GLenum type, GLfloat *buffer);
GLAPI void APIENTRY glPassThrough(GLfloat token);
GLAPI void APIENTRY glSelectBuffer(GLsizei size, GLuint *buffer);
GLAPI void APIENTRY glInitNames(void);
GLAPI void APIENTRY glLoadName(GLuint name);
GLAPI void APIENTRY glPushName(GLuint name);
GLAPI void APIENTRY glPopName(void);
#endif


/* OpenGL 1.2 Deprecated. */
extern  PFNGLCOLORTABLEPROC                glColorTable;
extern  PFNGLCOLORTABLEPARAMETERFVPROC     glColorTableParameterfv;
extern  PFNGLCOLORTABLEPARAMETERIVPROC     glColorTableParameteriv;
extern  PFNGLCOPYCOLORTABLEPROC            glCopyColorTable;
extern  PFNGLGETCOLORTABLEPROC             glGetColorTable;
extern  PFNGLGETCOLORTABLEPARAMETERFVPROC  glGetColorTableParameterfv;
extern  PFNGLGETCOLORTABLEPARAMETERIVPROC  glGetColorTableParameteriv;
extern  PFNGLCOLORSUBTABLEPROC             glColorSubTable;
extern  PFNGLCOPYCOLORSUBTABLEPROC         glCopyColorSubTable;
extern  PFNGLCONVOLUTIONFILTER1DPROC       glConvolutionFilter1D;
extern  PFNGLCONVOLUTIONFILTER2DPROC       glConvolutionFilter2D;
extern  PFNGLCONVOLUTIONPARAMETERFPROC     glConvolutionParameterf;
extern  PFNGLCONVOLUTIONPARAMETERFVPROC    glConvolutionParameterfv;
extern  PFNGLCONVOLUTIONPARAMETERIPROC     glConvolutionParameteri;
extern  PFNGLCONVOLUTIONPARAMETERIVPROC    glConvolutionParameteriv;
extern  PFNGLCOPYCONVOLUTIONFILTER1DPROC   glCopyConvolutionFilter1D;
extern  PFNGLCOPYCONVOLUTIONFILTER2DPROC   glCopyConvolutionFilter2D;
extern  PFNGLGETCONVOLUTIONFILTERPROC      glGetConvolutionFilter;
extern  PFNGLGETCONVOLUTIONPARAMETERFVPROC glGetConvolutionParameterfv;
extern  PFNGLGETCONVOLUTIONPARAMETERIVPROC glGetConvolutionParameteriv;
extern  PFNGLGETSEPARABLEFILTERPROC        glGetSeparableFilter;
extern  PFNGLSEPARABLEFILTER2DPROC         glSeparableFilter2D;
extern  PFNGLGETHISTOGRAMPROC              glGetHistogram;
extern  PFNGLGETHISTOGRAMPARAMETERFVPROC   glGetHistogramParameterfv;
extern  PFNGLGETHISTOGRAMPARAMETERIVPROC   glGetHistogramParameteriv;
extern  PFNGLGETMINMAXPROC                 glGetMinmax;
extern  PFNGLGETMINMAXPARAMETERFVPROC      glGetMinmaxParameterfv;
extern  PFNGLGETMINMAXPARAMETERIVPROC      glGetMinmaxParameteriv;
extern  PFNGLHISTOGRAMPROC                 glHistogram;
extern  PFNGLMINMAXPROC                    glMinmax;
extern  PFNGLRESETHISTOGRAMPROC            glResetHistogram;
extern  PFNGLRESETMINMAXPROC               glResetMinmax;


/* OpenGL 1.3 Deprecated. */
extern PFNGLCLIENTACTIVETEXTUREPROC  glClientActiveTexture;
extern PFNGLMULTITEXCOORD1DPROC      glMultiTexCoord1d;
extern PFNGLMULTITEXCOORD1DVPROC     glMultiTexCoord1dv;
extern PFNGLMULTITEXCOORD1FPROC      glMultiTexCoord1f;
extern PFNGLMULTITEXCOORD1FVPROC     glMultiTexCoord1fv;
extern PFNGLMULTITEXCOORD1IPROC      glMultiTexCoord1i;
extern PFNGLMULTITEXCOORD1IVPROC     glMultiTexCoord1iv;
extern PFNGLMULTITEXCOORD1SPROC      glMultiTexCoord1s;
extern PFNGLMULTITEXCOORD1SVPROC     glMultiTexCoord1sv;
extern PFNGLMULTITEXCOORD2DPROC      glMultiTexCoord2d;
extern PFNGLMULTITEXCOORD2DVPROC     glMultiTexCoord2dv;
extern PFNGLMULTITEXCOORD2FPROC      glMultiTexCoord2f;
extern PFNGLMULTITEXCOORD2FVPROC     glMultiTexCoord2fv;
extern PFNGLMULTITEXCOORD2IPROC      glMultiTexCoord2i;
extern PFNGLMULTITEXCOORD2IVPROC     glMultiTexCoord2iv;
extern PFNGLMULTITEXCOORD2SPROC      glMultiTexCoord2s;
extern PFNGLMULTITEXCOORD2SVPROC     glMultiTexCoord2sv;
extern PFNGLMULTITEXCOORD3DPROC      glMultiTexCoord3d;
extern PFNGLMULTITEXCOORD3DVPROC     glMultiTexCoord3dv;
extern PFNGLMULTITEXCOORD3FPROC      glMultiTexCoord3f;
extern PFNGLMULTITEXCOORD3FVPROC     glMultiTexCoord3fv;
extern PFNGLMULTITEXCOORD3IPROC      glMultiTexCoord3i;
extern PFNGLMULTITEXCOORD3IVPROC     glMultiTexCoord3iv;
extern PFNGLMULTITEXCOORD3SPROC      glMultiTexCoord3s;
extern PFNGLMULTITEXCOORD3SVPROC     glMultiTexCoord3sv;
extern PFNGLMULTITEXCOORD4DPROC      glMultiTexCoord4d;
extern PFNGLMULTITEXCOORD4DVPROC     glMultiTexCoord4dv;
extern PFNGLMULTITEXCOORD4FPROC      glMultiTexCoord4f;
extern PFNGLMULTITEXCOORD4FVPROC     glMultiTexCoord4fv;
extern PFNGLMULTITEXCOORD4IPROC      glMultiTexCoord4i;
extern PFNGLMULTITEXCOORD4IVPROC     glMultiTexCoord4iv;
extern PFNGLMULTITEXCOORD4SPROC      glMultiTexCoord4s;
extern PFNGLMULTITEXCOORD4SVPROC     glMultiTexCoord4sv;
extern PFNGLLOADTRANSPOSEMATRIXFPROC glLoadTransposeMatrixf;
extern PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd;
extern PFNGLMULTTRANSPOSEMATRIXFPROC glMultTransposeMatrixf;
extern PFNGLMULTTRANSPOSEMATRIXDPROC glMultTransposeMatrixd;


/* OpenGL 1.4 Deprecated. */
extern PFNGLFOGCOORDFPROC             glFogCoordf;
extern PFNGLFOGCOORDFVPROC            glFogCoordfv;
extern PFNGLFOGCOORDDPROC             glFogCoordd;
extern PFNGLFOGCOORDDVPROC            glFogCoorddv;
extern PFNGLFOGCOORDPOINTERPROC       glFogCoordPointer;
extern PFNGLSECONDARYCOLOR3BPROC      glSecondaryColor3b;
extern PFNGLSECONDARYCOLOR3BVPROC     glSecondaryColor3bv;
extern PFNGLSECONDARYCOLOR3DPROC      glSecondaryColor3d;
extern PFNGLSECONDARYCOLOR3DVPROC     glSecondaryColor3dv;
extern PFNGLSECONDARYCOLOR3FPROC      glSecondaryColor3f;
extern PFNGLSECONDARYCOLOR3FVPROC     glSecondaryColor3fv;
extern PFNGLSECONDARYCOLOR3IPROC      glSecondaryColor3i;
extern PFNGLSECONDARYCOLOR3IVPROC     glSecondaryColor3iv;
extern PFNGLSECONDARYCOLOR3SPROC      glSecondaryColor3s;
extern PFNGLSECONDARYCOLOR3SVPROC     glSecondaryColor3sv;
extern PFNGLSECONDARYCOLOR3UBPROC     glSecondaryColor3ub;
extern PFNGLSECONDARYCOLOR3UBVPROC    glSecondaryColor3ubv;
extern PFNGLSECONDARYCOLOR3UIPROC     glSecondaryColor3ui;
extern PFNGLSECONDARYCOLOR3UIVPROC    glSecondaryColor3uiv;
extern PFNGLSECONDARYCOLOR3USPROC     glSecondaryColor3us;
extern PFNGLSECONDARYCOLOR3USVPROC    glSecondaryColor3usv;
extern PFNGLSECONDARYCOLORPOINTERPROC glSecondaryColorPointer;
extern PFNGLWINDOWPOS2DPROC           glWindowPos2d;
extern PFNGLWINDOWPOS2DVPROC          glWindowPos2dv;
extern PFNGLWINDOWPOS2FPROC           glWindowPos2f;
extern PFNGLWINDOWPOS2FVPROC          glWindowPos2fv;
extern PFNGLWINDOWPOS2IPROC           glWindowPos2i;
extern PFNGLWINDOWPOS2IVPROC          glWindowPos2iv;
extern PFNGLWINDOWPOS2SPROC           glWindowPos2s;
extern PFNGLWINDOWPOS2SVPROC          glWindowPos2sv;
extern PFNGLWINDOWPOS3DPROC           glWindowPos3d;
extern PFNGLWINDOWPOS3DVPROC          glWindowPos3dv;
extern PFNGLWINDOWPOS3FPROC           glWindowPos3f;
extern PFNGLWINDOWPOS3FVPROC          glWindowPos3fv;
extern PFNGLWINDOWPOS3IPROC           glWindowPos3i;
extern PFNGLWINDOWPOS3IVPROC          glWindowPos3iv;
extern PFNGLWINDOWPOS3SPROC           glWindowPos3s;
extern PFNGLWINDOWPOS3SVPROC          glWindowPos3sv;

/* OpenGL 1.5 Deprecated. */
#define GL_VERTEX_ARRAY_BUFFER_BINDING    0x8896
#define GL_NORMAL_ARRAY_BUFFER_BINDING    0x8897
#define GL_COLOR_ARRAY_BUFFER_BINDING     0x8898
#define GL_INDEX_ARRAY_BUFFER_BINDING     0x8899
#define GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING 0x889A
#define GL_EDGE_FLAG_ARRAY_BUFFER_BINDING 0x889B
#define GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING 0x889C
#define GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING 0x889D
#define GL_WEIGHT_ARRAY_BUFFER_BINDING    0x889E
#define GL_FOG_COORD_SRC                  0x8450
#define GL_FOG_COORD                      0x8451
#define GL_CURRENT_FOG_COORD              0x8453
#define GL_FOG_COORD_ARRAY_TYPE           0x8454
#define GL_FOG_COORD_ARRAY_STRIDE         0x8455
#define GL_FOG_COORD_ARRAY_POINTER        0x8456
#define GL_FOG_COORD_ARRAY                0x8457
#define GL_FOG_COORD_ARRAY_BUFFER_BINDING 0x889D
#define GL_SRC0_RGB                       0x8580
#define GL_SRC1_RGB                       0x8581
#define GL_SRC2_RGB                       0x8582
#define GL_SRC0_ALPHA                     0x8588
#define GL_SRC1_ALPHA                     0x8589
#define GL_SRC2_ALPHA                     0x858A

/* OpenGL 2.0 Deprecated. */
#define GL_VERTEX_PROGRAM_TWO_SIDE        0x8643
#define GL_POINT_SPRITE                   0x8861
#define GL_COORD_REPLACE                  0x8862
#define GL_MAX_TEXTURE_COORDS             0x8871

/* OpenGL 2.1 Deprecated. */
#define GL_CURRENT_RASTER_SECONDARY_COLOR 0x845F
#define GL_SLUMINANCE_ALPHA               0x8C44
#define GL_SLUMINANCE8_ALPHA8             0x8C45
#define GL_SLUMINANCE                     0x8C46
#define GL_SLUMINANCE8                    0x8C47
#define GL_COMPRESSED_SLUMINANCE          0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA    0x8C4B

/* OpenGL 3.0 Deprecated. */
#define GL_CLAMP_VERTEX_COLOR             0x891A
#define GL_CLAMP_FRAGMENT_COLOR           0x891B
#define GL_ALPHA_INTEGER                  0x8D97

#ifdef __cplusplus
}
#endif

#endif
