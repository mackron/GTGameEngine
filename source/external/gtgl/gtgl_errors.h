/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_errors_h_
#define __gtgl_errors_h_

/* If you are looking for error codes, they are in gtgl.h */

/**
*   \brief                 Posts a GTGL error. This error can be retrieved with gtglGetLastError().
*   \param  function  [in] The name of the function posting the error.
*   \param  errorcode [in] The error code.
*   \param  format    [in] The printf style message string.
*
*   \remarks
*       The last error can be retrieved with gtglGetLastError(). Note that the previous error, if any, will be removed.
*       \par
*       If the current context has an error callback attached to it, the error will also be posted to that.
*/
void gtglPostError(const char *function, int code, const char *format, ...);

/**
*   \brief  Shuts down error stuff.
*/
void gtglShutdownErrors();


#endif