/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include "gtgl_utils.h"
#include <stdlib.h>
#include <stdint.h>


#if defined(_MSC_VER) || defined(__CYGWIN32__) || defined(__MINGW32__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static BOOL     GTGL_TimingInitialised = FALSE;
static LONGLONG GTGL_HighResFrequency  = 1;

double gtglGetTime()
{
    LARGE_INTEGER li;

    /* Timing must be initialised if it hasn't already. */
    if (!GTGL_TimingInitialised)
    {
        QueryPerformanceFrequency(&li);

        GTGL_HighResFrequency  = li.QuadPart;
        GTGL_TimingInitialised = TRUE;
    }

    QueryPerformanceCounter(&li);

    return (double)li.QuadPart / (double)GTGL_HighResFrequency * 1000.0;
}

int gtglGetCurrentThreadID()
{
    return (int)GetCurrentThreadId();
}


/**** GTGLmutex ****/
struct _GTGLmutex
{
    HANDLE internalMutex;
};

GTGLmutex GTGLmutex_new()
{
    GTGLmutex self = (GTGLmutex)malloc(sizeof(struct _GTGLmutex));
    self->internalMutex = CreateEventA(NULL, FALSE, TRUE, NULL);

    return self;
}

void GTGLmutex_delete(GTGLmutex self)
{
    if (self != NULL)
    {
        CloseHandle(self->internalMutex);
    }
}

void GTGLmutex_Lock(GTGLmutex self)
{
    WaitForSingleObject(self->internalMutex, INFINITE);
}

void GTGLmutex_Unlock(GTGLmutex self)
{
    SetEvent(self->internalMutex);
}

#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>

double gtglGetTime()
{
    struct timeval t;
    gettimeofday(&t, NULL);

    return (t.tv_sec * 1000.0) + (t.tv_usec * 0.001);
}

/* FIXME: This will break on Mac. Needs a Mac-specific implementation. */
int gtglGetCurrentThreadID()
{
    return (int)syscall(SYS_gettid);
}


/**** GTGLmutex ****/
struct _GTGLmutex
{
    pthread_mutex_t internalMutex;
};

GTGLmutex GTGLmutex_new()
{
    GTGLmutex self = (GTGLmutex)malloc(sizeof(struct _GTGLmutex));
    pthread_mutex_init(&self->internalMutex, NULL);

    return self;
}

void GTGLmutex_delete(GTGLmutex self)
{
    if (self != NULL)
    {
        pthread_mutex_destroy(&self->internalMutex);
    }
}

void GTGLmutex_Lock(GTGLmutex self)
{
    pthread_mutex_lock(&self->internalMutex);
}

void GTGLmutex_Unlock(GTGLmutex self)
{
    pthread_mutex_unlock(&self->internalMutex);
}


#endif

/* These functions are defined in order to avoid MSVC deprecation warnings. */
#ifdef _MSC_VER
#include <stdio.h>
#include <stdlib.h>


int gtgl_vsnprintf(char *buffer, size_t sizeOfBuffer, const char *format, va_list argList)
{
    if (buffer == NULL && sizeOfBuffer == 0)
    {
        return _vscprintf(format, argList);
    }

    return vsnprintf_s(buffer, sizeOfBuffer, _TRUNCATE, format, argList);
}
int gtgl_snprintf(char *buffer, size_t sizeOfBuffer, const char *format, ...)
{
    int result;

    va_list argList;
    va_start(argList, format);
        result = gtgl_vsnprintf(buffer, sizeOfBuffer, format, argList);
    va_end(argList);

    return result;
}
#endif

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable:4706)   /* assignment within conditional expression */
#endif

char * gtgl_strcpy(char *dest, const char *src)
{
    while((*dest++ = *src++));

    return dest;
}

#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)   /* conditional expression is constant */
#endif

voidproc voidp_to_function(void* input)
{
    if (sizeof(input) == sizeof(voidproc))
    {
        return *(voidproc*)(&input);
    }
    else
    {
        voidproc result = 0;
            
        if (sizeof(input) < sizeof(result))
        {
            memcpy(&result, &input, sizeof(input));
        }
        else
        {
            memcpy(&result, &input, sizeof(input));
        }
        
        return result;
    }
}

void* function_to_voidp(voidproc input)
{
    if (sizeof(input) == sizeof(voidproc))
    {
        return *(void**)(&input);
    }
    else
    {
        void* result = 0;
        
        if (sizeof(input) < sizeof(result))
        {
            memcpy(&result, &input, sizeof(input));
        }
        else
        {
            memcpy(&result, &input, sizeof(result));
        }
        
        return result;
    }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif



/**** GTGLdictionary ****/
struct _GTGLdictionary_findresult
{
    /** Specifies whether or not \c result is the exact match. If not, it points to the index that where it would otherwise be inserted. */
    int exact;

    /** The index in the buffer of where the item is located. This depends on \c exact. Note that the index may actually fall out-of-bounds if \c exact is false. */
    size_t index;
};
typedef struct _GTGLdictionary_findresult GTGLdictionary_findresult;

struct _GTGLdictionary_item
{
    /** The key of the item. */
    char* key;

    /** The value of the item. */
    void* value;
};
typedef struct _GTGLdictionary_item GTGLdictionary_item;

struct _GTGLdictionary
{
    /** The buffer containing the dictionary items. */
    GTGLdictionary_item** buffer;

    /** The size of the buffer, in char*'s */
    size_t bufferSize;

    /** The number of items in the list. */
    size_t count;
};

void GTGLdictionary_Clear(GTGLdictionary self);
GTGLdictionary_findresult GTGLdictionary_GenericFind(GTGLdictionary self, const char* key);

GTGLdictionary GTGLdictionary_new()
{
    GTGLdictionary self = (GTGLdictionary)malloc(sizeof(struct _GTGLdictionary));
    self->buffer     = NULL;
    self->bufferSize = 0;
    self->count      = 0;

    return self;
}

void GTGLdictionary_delete(GTGLdictionary self)
{
    GTGLdictionary_Clear(self);
    free(self);
}

void GTGLdictionary_Add(GTGLdictionary self, const char* key, void* value)
{
    /* We first need to find where to insert the item. */
    GTGLdictionary_findresult findResult = GTGLdictionary_GenericFind(self, key);
    if (!findResult.exact)
    {
        size_t keyLength = strlen(key);

        /* The new item. */
        GTGLdictionary_item* newItem = (GTGLdictionary_item*)malloc(sizeof(GTGLdictionary_item));
        newItem->value = value;

        /* Here we allocate and set the key. */
        newItem->key = (char*)malloc(keyLength + 1);
        memcpy(newItem->key, key, keyLength); newItem->key[keyLength] = '\0';


        /* If we don't have enough room to fit the item, we need a new buffer. Otherwise, we move everything after the insertion index down a position and just set the value directly. */
        ++self->count;
        if (self->count > self->bufferSize)
        {
            GTGLdictionary_item** newbuffer;
            size_t i;

            /* Need a new buffer... */
            self->bufferSize = self->bufferSize * 2 + 1;
            newbuffer = (GTGLdictionary_item**)malloc(sizeof(GTGLdictionary_item*) * self->bufferSize);

            /* Copying over is done in three parts... */
            /* Part 1: The part before the insertion point. */
            i = 0;
            for ( ; i < findResult.index; ++i)
            {
                newbuffer[i] = self->buffer[i];
            }

            /* Part 2: The new item. */
            newbuffer[i] = newItem;
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
            self->buffer[findResult.index] = newItem;
        }
    }
}

void GTGLdictionary_Remove(GTGLdictionary self, const char* key)
{
    /* We first find the item. */
    GTGLdictionary_findresult result = GTGLdictionary_GenericFind(self, key);
    if (result.exact)
    {
        size_t i;

        /* We will delete the item before moving everything after it down. */
        free(self->buffer[result.index]->key);
        free(self->buffer[result.index]);

        for (i = result.index; i < self->count - 1; ++i)
        {
            self->buffer[i] = self->buffer[i + 1];
        }

        --self->count;
    }
}

void* GTGLdictionary_Find(GTGLdictionary self, const char* key)
{
    GTGLdictionary_findresult result = GTGLdictionary_GenericFind(self, key);
    if (result.exact)
    {
        return self->buffer[result.index]->value;
    }

    return NULL;
}


void GTGLdictionary_Clear(GTGLdictionary self)
{
    unsigned int i;
    for (i = 0; i < self->count; ++i)
    {
        free(self->buffer[i]->key);
        free(self->buffer[i]);
    }

    free(self->buffer);
    self->buffer     = NULL;
    self->bufferSize = 0;
    self->count      = 0;
}

GTGLdictionary_findresult GTGLdictionary_GenericFind(GTGLdictionary self, const char* key)
{
    GTGLdictionary_findresult result;
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
            GTGLdictionary_item* miditem;
            int compareResult;

            mid = (low + high) / 2;    /* This will be floored. */

            miditem       = self->buffer[mid];
            compareResult = strcmp(key, miditem->key);

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
