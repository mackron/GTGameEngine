/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

/*
This header file is for common functions that need overrides in order to correctly implement emulated functionality. Every function here will become
the native versions.
*/
#ifndef __gtgl_emulation_common_h_
#define __gtgl_emulation_common_h_

#include <gtgl/gtgl.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** glEnable */
    void APIENTRY gtglEnable(GLenum cap);

    /** glDisable */
    void APIENTRY gtglDisable(GLenum cap);

    /** glIsEnabled */
    GLboolean APIENTRY gtglIsEnabled(GLenum cap);


    /** glGetBooleanv */
    void APIENTRY gtglGetBooleanv(GLenum pname, GLboolean* params);

    /** glGetDoublev */
    void APIENTRY gtglGetDoublev(GLenum pname, GLdouble* params);

    /** glGetFloatv */
    void APIENTRY gtglGetFloatv(GLenum pname, GLfloat* params);

    /** glGetIntegerv */
    void APIENTRY gtglGetIntegerv(GLenum pname, GLint* params);



    /** glGetPointerv */
    void APIENTRY gtglGetPointerv(GLenum pname, GLvoid** params);


#ifdef __cplusplus
}
#endif

#endif