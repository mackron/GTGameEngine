/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include "gtgl_extensions.h"
#include "gtgl_utils.h"
#include <gtgl/gtgl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static PFNGLGETSTRINGPROC   __local_glGetString   = NULL;
static PFNGLGETSTRINGIPROC  __local_glGetStringi  = NULL;
static PFNGLGETINTEGERVPROC __local_glGetIntegerv = NULL;


/** Structure representing the result of an GTGLextensionlist search. */
struct _GTGLextensionlist_findresult
{
    /** Specifies whether or not \c result is the exact match. If not, it points to the index that where it would otherwise be inserted. */
    int exact;
            
    /** The index in the buffer of where the item is located. This depends on \c exact. Note that the index may actually fall out-of-bounds if \c exact is false. */
    size_t index;
};
typedef struct _GTGLextensionlist_findresult GTGLextensionlist_findresult;

/** Structure representing an extension list. */
struct _GTGLextensionlist
{
    /** The context that was used when the extension list was generated. */
    GTGLplatformcontext context;

    /** The buffer containing the string pointers. Each string in this list is locally allocated. */
    char** buffer;

    /** The size of the buffer, in char*'s */
    size_t bufferSize;

    /** The number of items in the list. */
    size_t count;
};
typedef struct _GTGLextensionlist* GTGLextensionlist;

void GTGLextensionlist_Clear(GTGLextensionlist self);

GTGLextensionlist GTGLextensionlist_new()
{
    GTGLextensionlist self = (GTGLextensionlist)malloc(sizeof(struct _GTGLextensionlist));
    self->context    = NULL;
    self->buffer     = NULL;
    self->bufferSize = 0;
    self->count      = 0;

    return self;
}

void GTGLextensionlist_delete(GTGLextensionlist self)
{
    GTGLextensionlist_Clear(self);
    free(self);
}

void GTGLextensionlist_Clear(GTGLextensionlist self)
{
    unsigned int i;
    for (i = 0; i < self->count; ++i)
    {
        free(self->buffer[i]);
    }

    free(self->buffer);
    self->context    = NULL;
    self->buffer     = NULL;
    self->bufferSize = 0;
    self->count      = 0;
}

GTGLextensionlist_findresult GTGLextensionlist_Find(const GTGLextensionlist self, const char *extension)
{
    GTGLextensionlist_findresult result;
    result.exact = 0;
    result.index = 0;
            
    if (self->count > 0)
    {
        /* It's just so much easier using signed values here... */
        ptrdiff_t low  = 0;
        ptrdiff_t high = (ptrdiff_t)(self->count - 1);  /* <-- the subtraction here is safe because the count is checked that it's larger than 0 just above. */
        ptrdiff_t mid  = 0;
                
        size_t offset = 0;      /* This will either be 0 or 1 and is added to the index if we don't find an exact value. */
        while (low <= high)
        {
            char * miditem;
            int    compareResult;

            mid = (low + high) / 2;    /* This will be floored. */
                    
            miditem       = self->buffer[mid];
            compareResult = strcmp(extension, miditem);
                    
            if (compareResult < 0)
            {
                high = mid - 1;
                offset = 0;
            }
            else if (compareResult > 0)
            {
                low = mid + 1;
                offset = 1;
            }
            else
            {
                result.exact = 1;
                result.index = (size_t)mid;
                return result;
            }
        }
                
        /* If we have made it here, we weren't exact. */
        result.exact = 0;
        result.index = (size_t)mid + offset;
    }
            
    return result;
}

int GTGLextensionlist_Exists(GTGLextensionlist self, const char *extension)
{
    GTGLextensionlist_findresult result = GTGLextensionlist_Find(self, extension);
    return result.exact;
}

void GTGLextensionlist_Add(GTGLextensionlist self, const char *extensionIn, size_t extensionSizeInBytes)
{
    /* We first need to find where to insert the extension... */
    GTGLextensionlist_findresult findResult = GTGLextensionlist_Find(self, extensionIn);
    if (!findResult.exact)
    {
        char* extension;

        /* If the input extension is null-terminated, we'll need the length. */
        if (extensionSizeInBytes == (size_t)-1)
        {
            extensionSizeInBytes = strlen(extensionIn);
        }

        /* Unfortunately we need to copy the extension because of glGetStringi(). */
        extension = (char *)malloc(extensionSizeInBytes + 1);
        memcpy(extension, extensionIn, extensionSizeInBytes);
        extension[extensionSizeInBytes] = '\0';

        /* If we don't have enough room to fit the item, we need a new buffer. Otherwise, we move everything after the insertion index down a position and just set the value directly. */
        ++self->count;
        if (self->count > self->bufferSize)
        {
            char** newbuffer;
            size_t i;

            /* Need a new buffer... */
            self->bufferSize = self->bufferSize * 2 + 1;
            newbuffer = (char **)malloc(sizeof(char *) * self->bufferSize);

            /* Copying over is done in three parts... */
            /* Part 1: The part before the insertion point. */
            i = 0;
            for ( ; i < findResult.index; ++i)
            {
                newbuffer[i] = self->buffer[i];
            }

            /* Part 2: The new item. */
            newbuffer[i] = extension;
            ++i;

            /* Part 3: The part after the insertion point. */
            for ( ; i < self->count; ++i)
            {
                newbuffer[i] = self->buffer[i - 1];
            }

            free(self->buffer);
            self->buffer = newbuffer;
        }
        else
        {
            /* Don't need a new buffer... */
            size_t i = self->count - 1;

            /* Inserting is done in two parts... */
            /* Part 1: Move up a position starting at the insertion point. */
            for ( ; i > findResult.index; --i)
            {
                self->buffer[i] = self->buffer[i - 1];
            }

            /* Part 2: Apply the new item. */
            self->buffer[findResult.index] = extension;
        }
    }
}

void GTGLextensionlist_AddExtensionsString(GTGLextensionlist self, const char* extensions)
{
    if (extensions != NULL)
    {
        const char* nextExtStart = extensions;

        while (*extensions != '\0')
        {
            ++extensions;

            if (*extensions == ' ')
            {
                /* We've found the end of the next extension. It needs to be added. */
                GTGLextensionlist_Add(self, nextExtStart, (size_t)(extensions - nextExtStart));

                /* Skip past the space and set the start of the next extension. */
                ++extensions;
                nextExtStart = extensions;
            }
        }

        /* When we get here, there will be one more extension to add. */
        GTGLextensionlist_Add(self, nextExtStart, (size_t)(extensions - nextExtStart));
    }
}


/** The global extension list. */
static GTGLextensionlist ExtensionList = NULL;

void gtglRebuildExtensionList(GTGLplatformcontext newContext)
{
    if (__local_glGetString == NULL)
    {
        __local_glGetString = (PFNGLGETSTRINGPROC)gtglGetProcAddress("glGetString");
    }

    if (__local_glGetStringi == NULL)
    {
        __local_glGetStringi = (PFNGLGETSTRINGIPROC)gtglGetProcAddress("glGetStringi");
    }

    if (__local_glGetIntegerv == NULL)
    {
        __local_glGetIntegerv = (PFNGLGETINTEGERVPROC)gtglGetProcAddress("glGetIntegerv");
    }


    assert(ExtensionList != NULL);

    /* The old stuff needs to be cleared first. */
    GTGLextensionlist_Clear(ExtensionList);
    ExtensionList->context = newContext;

    /* We need to check base OpenGL extensions and also platform-specific extensions (WGL/GLX). */
    if (__local_glGetStringi != NULL && __local_glGetIntegerv != NULL)
    {
        GLint i;

        GLint extCount = 0;
        __local_glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
        
        /* If the extension count is 0, it probably means we have a 3.1 or below context. We'll fallback to the old way in this case. */
        if (extCount > 0)
        {
            for (i = 0; i < extCount; ++i)
            {
                GTGLextensionlist_Add(ExtensionList, (const char *)__local_glGetStringi(GL_EXTENSIONS, (GLuint)i), (size_t)-1);
            }
        }
        else
        {
            GTGLextensionlist_AddExtensionsString(ExtensionList, (const char *)__local_glGetString(GL_EXTENSIONS));
        }
    }
    else
    {
        GTGLextensionlist_AddExtensionsString(ExtensionList, (const char *)__local_glGetString(GL_EXTENSIONS));
    }

#ifdef GTGL_WGL
    GTGLextensionlist_AddExtensionsString(ExtensionList, gtwglGetExtensionsString());
#else
    GTGLextensionlist_AddExtensionsString(ExtensionList, gtglXGetExtensionsString());
#endif
}

void gtglRefreshExtensionList()
{
    GTGLplatformcontext currentContext;

    /* We need an extension list... */
    if (ExtensionList == NULL)
    {
        ExtensionList = GTGLextensionlist_new();
    }

    /* Only refresh if we have a different context. */
    currentContext = gtglGetCurrentPlatformContext();
    if (currentContext != ExtensionList->context)
    {
        gtglRebuildExtensionList(currentContext);
    }
}


int gtglIsExtensionSupported(const char * extension)
{
    /* First we refresh our extension list. The will return immediately if nothing needs to be done. */
    gtglRefreshExtensionList();

    /* Now we just check the list... */
    return GTGLextensionlist_Exists(ExtensionList, extension);
}


void gtglShutdownExtensions()
{
    GTGLextensionlist_delete(ExtensionList);
    ExtensionList = NULL;
}
