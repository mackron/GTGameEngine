/* Copyright (C) 2011 - 2013 David Reid. See included LICENCE file. */

#include "gtgl_arb_debug_output.h"
#include "../gtgl_context.h"
#include "../gtgl_utils.h"
#include <stdlib.h>

void APIENTRY gtglDebugMessageControlARB(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled)
{
    GTGLcontext context = gtglGetCurrentContext();
    if (context != NULL)
    {
        /* We run the setters in a loop. We need to grab the ranges that we'll be looping over in each array. */
        GLenum sourceStart,   sourceEnd;
        GLenum typeStart,     typeEnd;
        GLenum severityStart, severityEnd;

        GLenum iSource, iType, iSeverity;

        if (source == GL_DONT_CARE)
        {
            sourceStart = 0;
            sourceEnd   = 5;
        }
        else
        {
            sourceStart = sourceEnd = (GL_DEBUG_SOURCE_OTHER_ARB - source);
        }

        if (type == GL_DONT_CARE)
        {
            typeStart = 0;
            typeEnd   = 5;
        }
        else
        {
            typeStart = typeEnd = (GL_DEBUG_TYPE_OTHER_ARB - type);
        }

        if (severity == GL_DONT_CARE)
        {
            severityStart = 0;
            severityEnd   = 2;
        }
        else
        {
            severityStart = severityEnd = (GL_DEBUG_SEVERITY_LOW_ARB - severity);
        }


        /* Now that we have the loop indices, we iterate over the arrays and set the appropriate values. */
        for (iSource = sourceStart; iSource <= sourceEnd; ++iSource)
        {
            for (iType = typeStart; iType <= typeEnd; ++iType)
            {
                for (iSeverity = severityStart; iSeverity <= severityEnd; ++iSeverity)
                {
                    context->ARB_debug_output.messageStates[iSource][iType][iSeverity] = enabled;
                }
            }
        }
    }


    (void)count;
    (void)ids;
}

void APIENTRY gtglDebugMessageInsertARB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* buf)
{
    /*
    A conformant implementation of this function will emit an error if <source> is not GL_DEBUG_SOURCE_THIRD_PARTY_ARB or
    GL_DEBUG_SOURCE_APPLICATION_ARB. However, GTGL needs to use GL_DEBUG_SOURCE_API_ARB when it detects an error. We're
    going to allow GL_DEBUG_SOURCE_API_ARB for now.

    Note here how we don't make use of DEBUG_OUTPUT_SYNCHRONOUS_ARB. This is because none of our internal calls are ever
    multi-threaded. Thus, it's not needed.
    */

    GTGLcontext context = gtglGetCurrentContext();
    if (context != NULL)
    {
        GLenum iSource   = (GL_DEBUG_SOURCE_OTHER_ARB - source);
        GLenum iType     = (GL_DEBUG_TYPE_OTHER_ARB - type);
        GLenum iSeverity = (GL_DEBUG_SEVERITY_LOW_ARB - severity);

        if (context->ARB_debug_output.messageStates[iSource][iType][iSeverity] == GL_TRUE)
        {
            if (context->ARB_debug_output.DEBUG_CALLBACK_FUNCTION_ARB != NULL)
            {
                context->ARB_debug_output.DEBUG_CALLBACK_FUNCTION_ARB(source, type, id, severity, length, buf, (GLvoid*)context->ARB_debug_output.DEBUG_CALLBACK_USER_PARAM_ARB);
            }
            else
            {
                /* We log instead. Discard when full. */
                if (context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB < context->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB)  /* Do we have room? We discard if not. */
                {
                    struct _ARB_debug_output_Message newMessage;
                    newMessage.source   = source;
                    newMessage.type     = type;
                    newMessage.severity = severity;
                    newMessage.id       = id;
                    newMessage.length   = length;
                    newMessage.msg      = gtgl_strdup(buf);

                    context->ARB_debug_output.messageLog[(context->ARB_debug_output.nextMessageIndex + context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB) % context->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB] = newMessage;

                    /* If this was the first message, we need to set the DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB appropriately. */
                    if (context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB == 0)
                    {
                        context->ARB_debug_output.DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB = (GLushort)length;
                    }

                    ++context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB;
                }
            }
        }
    }
}

void APIENTRY gtglDebugMessageCallbackARB(GLDEBUGPROCARB callback, const GLvoid* userParam)
{
    GTGLcontext context = gtglGetCurrentContext();
    if (context != NULL)
    {
        context->ARB_debug_output.DEBUG_CALLBACK_FUNCTION_ARB   = callback;
        context->ARB_debug_output.DEBUG_CALLBACK_USER_PARAM_ARB = userParam;
    }
}

GLuint APIENTRY gtglGetDebugMessageLogARB(GLuint count, GLsizei bufsize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog)
{
    GLuint retValue = 0;

    GTGLcontext context = gtglGetCurrentContext();
    if (context != NULL)
    {
        GLuint iMessage;

        /* We'll begin by making sure our count is clamped. This will simplify things a bit. */
        if (count > context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB)
        {
            count = context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB;
        }

        for (iMessage = 0; iMessage < count; ++iMessage)
        {
            struct _ARB_debug_output_Message* nextMessage = context->ARB_debug_output.messageLog + context->ARB_debug_output.nextMessageIndex;

            /* If we don't have enough room in the buffer we need to break from this loop. */
            if (bufsize > nextMessage->length)
            {
                if (sources != NULL)    { *sources++    = nextMessage->source; }
                if (types != NULL)      { *types++      = nextMessage->type; }
                if (ids != NULL)        { *ids++        = nextMessage->id; }
                if (severities != NULL) { *severities++ = nextMessage->severity; }
                if (lengths != NULL)    { *lengths++    = nextMessage->length; }

                if (messageLog != NULL)
                {
                    gtgl_strcpy(messageLog, nextMessage->msg);
                    messageLog += nextMessage->length + 1;      /* +1 to skip past the null terminator. */
                }

                /* We need to free the memory allocated from the previous message. */
                free(nextMessage->msg);
                nextMessage->msg = NULL;


                /* We need to move to the next message. */
                context->ARB_debug_output.nextMessageIndex = (context->ARB_debug_output.nextMessageIndex + 1) % context->ARB_debug_output.MAX_DEBUG_LOGGED_MESSAGES_ARB;


                /* Some state variables need to be updated... */
                --context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB;

                if (context->ARB_debug_output.DEBUG_LOGGED_MESSAGES_ARB > 0)
                {
                    context->ARB_debug_output.DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB = (GLushort)context->ARB_debug_output.messageLog[context->ARB_debug_output.nextMessageIndex].length;
                }
                else
                {
                    context->ARB_debug_output.DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB = 0;
                }


                /* We need to increment our return value, which is the number of messages we have retrieved. */
                ++retValue;
            }
            else
            {
                break;  /* Not enough room in the buffer. */
            }
        }
    }

    return retValue;
}
