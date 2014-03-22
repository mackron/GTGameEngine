/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include <gtgl/gtgl.h>
#include "gtgl_context.h"
#include "gtgl_utils.h"
#include <stdlib.h>
#include <memory.h>

/* Our master context. Will be initialised properly in win32/gtgl_context_win32.c or x11/gtgl_context_x11.c (in gtglStartup). */
GTGLcontext GTGL_MasterContext = NULL;


/**
*   \brief  Sets the attributes of the context to their defaults.
*/
void GTGLcontext_SetDefaultAttributes(GTGLcontext self);


GTGLcontext GTGLcontext_new(size_t attribs[], GTGLcontext master)
{
    GTGLcontext self = (GTGLcontext)malloc(sizeof(struct _GTGLcontext));
    GTGLcontext_ctor(self, attribs, master);

    return self;
}

void GTGLcontext_delete(GTGLcontext self)
{
    GTGLcontext_dtor(self);
    free(self);
}


void GTGLcontext_ctor_base(GTGLcontext self)
{
    int iSource, iType;

    GTGLcontext_SetAttributes(self, NULL);

    self->master   = NULL;
    self->threadID = 0;

    self->debugMessageCallback = NULL;
    self->openGLCallCallback   = NULL;

    self->ARB_debug_output.emulating = GL_FALSE;
    self->ARB_debug_output.DEBUG_CALLBACK_FUNCTION_ARB          = NULL;
    self->ARB_debug_output.DEBUG_CALLBACK_USER_PARAM_ARB        = NULL;
    self->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB            = 0;
    self->ARB_debug_output.DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB = 0;
    self->ARB_debug_output.DEBUG_OUTPUT_SYNCHRONOUS_ARB         = GL_FALSE;
    self->ARB_debug_output.MAX_DEBUG_MESSAGE_LENGTH_ARB         = 1024;
    self->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB        = 64;

    for (iSource = 0; iSource < 6; ++iSource)
    {
        for (iType = 0; iType < 6; ++iType)
        {
            self->ARB_debug_output.messageStates[iSource][iType][0] = GL_FALSE;
            self->ARB_debug_output.messageStates[iSource][iType][1] = GL_TRUE;
            self->ARB_debug_output.messageStates[iSource][iType][2] = GL_TRUE;
        }
    }

    self->ARB_debug_output.nextMessageIndex = 0;
}

void GTGLcontext_dtor_base(GTGLcontext self)
{
    /* We need to free the strings that were allocated by ARB_debug_output. */
    GLushort iMessage; 
    for (iMessage = 0; iMessage < self->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB; ++iMessage)
    {
        struct _ARB_debug_output_Message* message = self->ARB_debug_output.messageLog + ((self->ARB_debug_output.nextMessageIndex + iMessage) % self->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB);
        free(message->msg);
    }
}

void GTGLcontext_SetAttributes(GTGLcontext self, size_t attribs[])
{
    /* We reset everything to their defaults in order to handle situations where a context attribute isn't specified, which is perfectly acceptable. */
    GTGLcontext_SetDefaultAttributes(self);

    if (attribs != NULL)
    {
        while (!(attribs[0] == 0 && attribs[1] == 0))
        {
            size_t key   = attribs[0];
            size_t value = attribs[1];

            switch (key)
            {
            case GTGL_CONTEXT_MAJOR_VERSION:        self->versionMajor        = (unsigned int)value;  break;
            case GTGL_CONTEXT_MINOR_VERSION:        self->versionMinor        = (unsigned int)value;  break;
            case GTGL_CONTEXT_SINGLE_BUFFER:        self->singleBuffered      = value != 0;           break;
            case GTGL_CONTEXT_DEBUG:                self->debug               = value != 0;           break;
            case GTGL_CONTEXT_FORWARD_COMPATIBLE:   self->forwardCompatible   = value != 0;           break;
            case GTGL_CONTEXT_ES2_COMPATIBLE:       self->es2Compatible       = value != 0;           break;
            case GTGL_CONTEXT_EMULATE_DEBUG_OUTPUT: self->emulateDebugOutput  = (int)value;           break;     /* true, false or GTGL_FORCE. Don't use 'value != 0'. */

            default: break;
            }

            attribs += 2;
        }

        /* ES2 compatibility is only supported when forward compatibility is not set. */
        self->es2Compatible = self->es2Compatible && !self->forwardCompatible;
    }
}

void GTGLcontext_CopyAttributes(GTGLcontext self, GTGLcontext master)
{
    self->versionMajor        = master->versionMajor;
    self->versionMinor        = master->versionMinor;
    self->singleBuffered      = master->singleBuffered;
    self->debug               = master->debug;
    self->forwardCompatible   = master->forwardCompatible;
    self->es2Compatible       = master->es2Compatible;
    self->emulateDebugOutput  = master->emulateDebugOutput;
}

void GTGLcontext_SetDefaultAttributes(GTGLcontext self)
{
    self->versionMajor        = 0;
    self->versionMinor        = 0;
    self->singleBuffered      = GTGL_FALSE;
    self->debug               = GTGL_FALSE;
    self->forwardCompatible   = GTGL_FALSE;
    self->es2Compatible       = GTGL_FALSE;
    self->emulateDebugOutput  = GTGL_FALSE;
}
