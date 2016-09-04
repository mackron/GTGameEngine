/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_arb_debug_output_h_
#define __gtgl_arb_debug_output_h_

#include <gtgl/gtgl.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** glDebugMessageControlARB */
    void APIENTRY gtglDebugMessageControlARB(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);

    /** glDebugMessageInsertARB */
    void APIENTRY gtglDebugMessageInsertARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf);

    /** glDebugMessageCallbackARB */
    void APIENTRY gtglDebugMessageCallbackARB(GLDEBUGPROCARB callback, const GLvoid* userParam);

    /** glGetDebugMessageLogARB */
    GLuint APIENTRY gtglGetDebugMessageLogARB(GLuint count, GLsizei bufsize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog);



#ifdef __cplusplus
}
#endif

#endif