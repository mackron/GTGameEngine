/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#ifndef __gtgl_utils_h_
#define __gtgl_utils_h_

#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

typedef void(*voidproc)();

/**
*   \brief  Retrieves the current time in milliseconds. This is used for timing function calls.
*/
double gtglGetTime();

/**
*   \brief  Retrieves the ID of the calling thread.
*/
int gtglGetCurrentThreadID();


/* These functions are defined in order to avoid compiler warnings. */
#ifdef _MSC_VER
int     gtgl_vsnprintf(char *buffer, size_t sizeOfBuffer, const char* format, va_list argList);
int     gtgl_snprintf(char *buffer, size_t sizeOfBuffer, const char* format, ...);
#define gtgl_strdup _strdup
#else
#define gtgl_vsnprintf vsnprintf
#define gtgl_snprintf  snprintf
#define gtgl_strdup    strdup
#endif

char* gtgl_strcpy(char *dest, const char *src);


/** Converts a void* to a function pointer. */
voidproc voidp_to_function(void* input);

/** Converts a function pointer to a void* */
void* function_to_voidp(voidproc input);



/** Mutex */
typedef struct _GTGLmutex* GTGLmutex;

/** Creates a new GTGLmutex on the heap. Unlocked by default. */
GTGLmutex GTGLmutex_new();

/** Deletes an GTGLmutex that was previously created with GTGLmutex_new(). */
void GTGLmutex_delete(GTGLmutex self);

/** Waits for the mutex to become signaled. This function will automatically put the mutex back into a non-signaled state. */
void GTGLmutex_Lock(GTGLmutex self);

/** Releases the mutex, putting it into a signaled state. */
void GTGLmutex_Unlock(GTGLmutex self);



/** Dictionary */
typedef struct _GTGLdictionary* GTGLdictionary;

/** Creates a new GTGLdictionary on the heap. */
GTGLdictionary GTGLdictionary_new();

/** Deletes a GTGLdictionary that was previously created with GTGLdictionary_new(). */
void GTGLdictionary_delete(GTGLdictionary self);

/** Adds an item to the dictionary. */
void GTGLdictionary_Add(GTGLdictionary self, const char* key, void* value);

/** Removes an item from the dictionary. */
void GTGLdictionary_Remove(GTGLdictionary self, const char* key);

/** Finds the value of the given key. */
void* GTGLdictionary_Find(GTGLdictionary self, const char* key);


#endif
