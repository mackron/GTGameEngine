// Version 0.1 - Public Domain. See "unlicense" statement at the end of this file.

#include "easy_mtl.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


// Platform detection
#if   !defined(EASYMTL_PLATFORM_WINDOWS) && (defined(__WIN32__) || defined(_WIN32) || defined(_WIN64))
#define EASYMTL_PLATFORM_WINDOWS
#elif !defined(EASFMTL_PLATFORM_LINUX)   &&  defined(__linux__)
#define EASYMTL_PLATFORM_LINUX
#elif !defined(EASYMTL_PLATFORM_MAC)     && (defined(__APPLE__) && defined(__MACH__))
#define	EASYMTL_PLATFORM_MAC
#endif


// When constructing the material's raw data, memory allocated in blocks of this amount. This must be at least 256.
#define EASYMTL_CHUNK_SIZE              4096

#define EASYMTL_STAGE_IDS               0
#define EASYMTL_STAGE_PRIVATE_INPUTS    1
#define EASYMTL_STAGE_PUBLIC_INPUTS     2
#define EASYMTL_STAGE_CHANNELS          3
#define EASYMTL_STAGE_PROPERTIES        4
#define EASYMTL_STAGE_COMPLETE          UINT_MAX


/// Inflates the materials data buffer by EASYMTL_CHUNK_SIZE.
easymtl_bool _easymtl_inflate(easymtl_material* pMaterial);


easymtl_bool easymtl_init(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        assert(EASYMTL_CHUNK_SIZE >= 256);

        pMaterial->pRawData = malloc(EASYMTL_CHUNK_SIZE);
        if (pMaterial->pRawData != NULL)
        {
            pMaterial->sizeInBytes          = sizeof(easymtl_header);
            pMaterial->bufferSizeInBytes    = EASYMTL_CHUNK_SIZE;
            pMaterial->currentStage         = EASYMTL_STAGE_IDS;
            pMaterial->currentChannelOffset = 0;
            pMaterial->ownsRawData          = 1;

            easymtl_header* pHeader = easymtl_getheader(pMaterial);
            assert(pHeader != NULL);

            pHeader->magic                    = EASYMTL_MAGIC_NUMBER;
            pHeader->version                  = EASYMTL_CURRENT_VERSION;
            pHeader->identifierSizeInBytes    = sizeof(easymtl_identifier);
            pHeader->inputSizeInBytes         = sizeof(easymtl_input);
            pHeader->channelHeaderSizeInBytes = sizeof(easymtl_channel_header);
            pHeader->instructionSizeInBytes   = sizeof(easymtl_instruction);
            pHeader->propertySizeInBytes      = sizeof(easymtl_property);
            pHeader->identifierCount          = 0;
            pHeader->privateInputCount        = 0;
            pHeader->publicInputCount         = 0;
            pHeader->channelCount             = 0;
            pHeader->propertyCount            = 0;
            pHeader->identifiersOffset        = pMaterial->sizeInBytes;
            pHeader->inputsOffset             = pMaterial->sizeInBytes;
            pHeader->channelsOffset           = pMaterial->sizeInBytes;
            pHeader->propertiesOffset         = pMaterial->sizeInBytes;

            return 1;
        }
    }

    return 0;
}

easymtl_bool easymtl_initfromexisting(easymtl_material* pMaterial, const void* pRawData, unsigned int dataSizeInBytes)
{
    if (pMaterial != NULL)
    {
        if (pRawData != NULL && dataSizeInBytes >= sizeof(easymtl_header))
        {
            if (((easymtl_header*)pMaterial->pRawData)->magic == EASYMTL_MAGIC_NUMBER)
            {
                pMaterial->pRawData = malloc(EASYMTL_CHUNK_SIZE);
                if (pMaterial->pRawData != NULL)
                {
                    memcpy(pMaterial->pRawData, pRawData, dataSizeInBytes);
                    pMaterial->sizeInBytes          = dataSizeInBytes;
                    pMaterial->bufferSizeInBytes    = dataSizeInBytes;
                    pMaterial->currentStage         = EASYMTL_STAGE_COMPLETE;
                    pMaterial->currentChannelOffset = 0;
                    pMaterial->ownsRawData          = 1;

                    return 1;
                }
            }
        }
    }

    return 0;
}

easymtl_bool easymtl_initfromexisting_nocopy(easymtl_material* pMaterial, const void* pRawData, unsigned int dataSizeInBytes)
{
    if (pMaterial != NULL)
    {
        if (pRawData != NULL && dataSizeInBytes >= sizeof(easymtl_header))
        {
            if (((easymtl_header*)pRawData)->magic == EASYMTL_MAGIC_NUMBER)
            {
                pMaterial->pRawData             = (void*)pRawData;
                pMaterial->sizeInBytes          = dataSizeInBytes;
                pMaterial->bufferSizeInBytes    = dataSizeInBytes;
                pMaterial->currentStage         = EASYMTL_STAGE_COMPLETE;
                pMaterial->currentChannelOffset = 0;
                pMaterial->ownsRawData          = 0;

                return 1;
            }
        }
    }

    return 0;
}

void easymtl_uninit(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        if (pMaterial->ownsRawData)
        {
            free(pMaterial->pRawData);
        }

        pMaterial->pRawData = NULL;
    }
}


easymtl_header* easymtl_getheader(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        return (easymtl_header*)pMaterial->pRawData;
    }

    return NULL;
}


easymtl_bool easymtl_appendidentifier(easymtl_material* pMaterial, easymtl_identifier identifier, unsigned int* indexOut)
{
    if (pMaterial != NULL)
    {
        if (pMaterial->currentStage <= EASYMTL_STAGE_IDS)
        {
            easymtl_header* pHeader = easymtl_getheader(pMaterial);
            if (pHeader != NULL)
            {
                if (pMaterial->sizeInBytes + pHeader->identifierSizeInBytes > pMaterial->bufferSizeInBytes)
                {
                    if (!_easymtl_inflate(pMaterial))
                    {
                        // An error occured when trying to inflate the buffer. Might be out of memory.
                        return 0;
                    }
                    
                    pHeader = easymtl_getheader(pMaterial);
                    assert(pMaterial->sizeInBytes + pHeader->identifierSizeInBytes <= pMaterial->bufferSizeInBytes);
                }
                

                memcpy(pMaterial->pRawData + pHeader->inputsOffset, &identifier, pHeader->identifierSizeInBytes);
                pMaterial->sizeInBytes += pHeader->identifierSizeInBytes;
                
                pHeader->identifierCount  += 1;
                pHeader->inputsOffset     += pHeader->identifierSizeInBytes;
                pHeader->channelsOffset   += pHeader->identifierSizeInBytes;
                pHeader->propertiesOffset += pHeader->identifierSizeInBytes;


                if (indexOut != NULL)
                {
                    *indexOut = pHeader->identifierCount - 1;
                }

                return 1;
            }
        }
    }

    return 0;
}

easymtl_bool easymtl_appendprivateinput(easymtl_material* pMaterial, easymtl_input input)
{
    if (pMaterial != NULL)
    {
        if (pMaterial->currentStage <= EASYMTL_STAGE_PRIVATE_INPUTS)
        {
            easymtl_header* pHeader = easymtl_getheader(pMaterial);
            if (pHeader != NULL)
            {
                if (pMaterial->sizeInBytes + pHeader->inputSizeInBytes > pMaterial->bufferSizeInBytes)
                {
                    if (!_easymtl_inflate(pMaterial))
                    {
                        // An error occured when trying to inflate the buffer. Might be out of memory.
                        return 0;
                    }
                    
                    pHeader = easymtl_getheader(pMaterial);
                    assert(pMaterial->sizeInBytes + pHeader->inputSizeInBytes <= pMaterial->bufferSizeInBytes);
                }


                memcpy(pMaterial->pRawData + pHeader->channelsOffset, &input, pHeader->inputSizeInBytes);
                pMaterial->sizeInBytes += pHeader->inputSizeInBytes;

                pHeader->privateInputCount += 1;
                pHeader->channelsOffset    += pHeader->inputSizeInBytes;
                pHeader->propertiesOffset  += pHeader->inputSizeInBytes;


                pMaterial->currentStage = EASYMTL_STAGE_PRIVATE_INPUTS;
                return 1;
            }
        }
    }

    return 0;
}

easymtl_bool easymtl_appendpublicinput(easymtl_material* pMaterial, easymtl_input input)
{
    if (pMaterial != NULL)
    {
        if (pMaterial->currentStage <= EASYMTL_STAGE_PUBLIC_INPUTS)
        {
            easymtl_header* pHeader = easymtl_getheader(pMaterial);
            if (pHeader != NULL)
            {
                if (pMaterial->sizeInBytes + pHeader->inputSizeInBytes > pMaterial->bufferSizeInBytes)
                {
                    if (!_easymtl_inflate(pMaterial))
                    {
                        // An error occured when trying to inflate the buffer. Might be out of memory.
                        return 0;
                    }
                    
                    pHeader = easymtl_getheader(pMaterial);
                    assert(pMaterial->sizeInBytes + pHeader->inputSizeInBytes <= pMaterial->bufferSizeInBytes);
                }


                memcpy(pMaterial->pRawData + pHeader->channelsOffset, &input, pHeader->inputSizeInBytes);
                pMaterial->sizeInBytes += pHeader->inputSizeInBytes;

                pHeader->publicInputCount  += 1;
                pHeader->channelsOffset    += pHeader->inputSizeInBytes;
                pHeader->propertiesOffset  += pHeader->inputSizeInBytes;


                pMaterial->currentStage = EASYMTL_STAGE_PUBLIC_INPUTS;
                return 1;
            }
        }
    }

    return 0;
}

easymtl_bool easymtl_appendchannel(easymtl_material* pMaterial, easymtl_channel channel)
{
    if (pMaterial != NULL)
    {
        if (pMaterial->currentStage <= EASYMTL_STAGE_CHANNELS)
        {
            easymtl_header* pHeader = easymtl_getheader(pMaterial);
            if (pHeader != NULL)
            {
                easymtl_channel_header channelHeader;
                channelHeader.channel         = channel;
                channelHeader.instructionCount = 0;

                if (pMaterial->sizeInBytes + pHeader->channelHeaderSizeInBytes > pMaterial->bufferSizeInBytes)
                {
                    if (!_easymtl_inflate(pMaterial))
                    {
                        // An error occured when trying to inflate the buffer. Might be out of memory.
                        return 0;
                    }
                    
                    pHeader = easymtl_getheader(pMaterial);
                    assert(pMaterial->sizeInBytes + pHeader->channelHeaderSizeInBytes <= pMaterial->bufferSizeInBytes);
                }


                memcpy(pMaterial->pRawData + pHeader->propertiesOffset, &channelHeader, pHeader->channelHeaderSizeInBytes);
                pMaterial->currentChannelOffset = pMaterial->sizeInBytes;
                pMaterial->sizeInBytes += pHeader->channelHeaderSizeInBytes;

                pHeader->channelCount     += 1;
                pHeader->propertiesOffset += pHeader->channelHeaderSizeInBytes;


                pMaterial->currentStage = EASYMTL_STAGE_CHANNELS;
                return 1;
            }
        }
    }

    return 0;
}

easymtl_bool easymtl_appendinstruction(easymtl_material* pMaterial, easymtl_instruction instruction)
{
    if (pMaterial != NULL)
    {
        if (pMaterial->currentStage == EASYMTL_STAGE_CHANNELS)
        {
            easymtl_header* pHeader = easymtl_getheader(pMaterial);
            if (pHeader != NULL)
            {
                if (pMaterial->sizeInBytes + pHeader->instructionSizeInBytes > pMaterial->bufferSizeInBytes)
                {
                    if (!_easymtl_inflate(pMaterial))
                    {
                        // An error occured when trying to inflate the buffer. Might be out of memory.
                        return 0;
                    }
                    
                    pHeader = easymtl_getheader(pMaterial);
                    assert(pMaterial->sizeInBytes + pHeader->instructionSizeInBytes <= pMaterial->bufferSizeInBytes);
                }


                memcpy(pMaterial->pRawData + pHeader->propertiesOffset, &instruction, pHeader->instructionSizeInBytes);
                pMaterial->sizeInBytes += pHeader->instructionSizeInBytes;

                easymtl_channel_header* pChannelHeader = (easymtl_channel_header*)(pMaterial->pRawData + pMaterial->currentChannelOffset);
                if (pChannelHeader != NULL)
                {
                    pChannelHeader->instructionCount += 1;
                }
                
                pHeader->propertiesOffset += pHeader->instructionSizeInBytes;


                return 1;
            }
        }
    }

    return 0;
}

easymtl_bool easymtl_appendproperty(easymtl_material* pMaterial, easymtl_property prop)
{
    if (pMaterial != NULL)
    {
        if (pMaterial->currentStage <= EASYMTL_STAGE_PRIVATE_INPUTS)
        {
            easymtl_header* pHeader = easymtl_getheader(pMaterial);
            if (pHeader != NULL)
            {
                if (pMaterial->sizeInBytes + pHeader->propertySizeInBytes > pMaterial->bufferSizeInBytes)
                {
                    if (!_easymtl_inflate(pMaterial))
                    {
                        // An error occured when trying to inflate the buffer. Might be out of memory.
                        return 0;
                    }
                    
                    pHeader = easymtl_getheader(pMaterial);
                    assert(pMaterial->sizeInBytes + pHeader->propertySizeInBytes <= pMaterial->bufferSizeInBytes);
                }


                memcpy(pMaterial->pRawData + pMaterial->sizeInBytes, &prop, pHeader->propertySizeInBytes);
                pMaterial->sizeInBytes += pHeader->propertySizeInBytes;

                pHeader->propertyCount += 1;


                pMaterial->currentStage = EASYMTL_STAGE_PROPERTIES;
                return 1;
            }
        }
    }

    return 0;
}


easymtl_channel_header* easymtl_getchannelheaderbyindex(easymtl_material* pMaterial, unsigned int channelIndex)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        if (channelIndex < pHeader->channelCount)
        {
            easymtl_uint8* pChannelHeader = pMaterial->pRawData + pHeader->channelsOffset;
            for (unsigned int iChannel = 0; iChannel < channelIndex; ++iChannel)
            {
                pChannelHeader += sizeof(easymtl_channel_header) + (sizeof(easymtl_instruction) * ((easymtl_channel_header*)pChannelHeader)->instructionCount);
            }

            return (easymtl_channel_header*)pChannelHeader;
        }
    }
    
    return NULL;
}

easymtl_channel_header* easymtl_getchannelheaderbyname(easymtl_material* pMaterial, const char* channelName)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        easymtl_uint8* pChannelHeader = pMaterial->pRawData + pHeader->channelsOffset;
        for (unsigned int iChannel = 0; iChannel < pHeader->channelCount; ++iChannel)
        {
            if (strcmp(((easymtl_channel_header*)pChannelHeader)->channel.name, channelName) == 0)
            {
                return (easymtl_channel_header*)pChannelHeader;
            }

            pChannelHeader += sizeof(easymtl_channel_header) + (sizeof(easymtl_instruction) * ((easymtl_channel_header*)pChannelHeader)->instructionCount);
        }
    }
    
    return NULL;
}

easymtl_identifier* easymtl_getidentifiers(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        return (easymtl_identifier*)(pMaterial->pRawData + pHeader->identifiersOffset);
    }

    return NULL;
}

easymtl_identifier* easymtl_getidentifier(easymtl_material* pMaterial, unsigned int index)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        if (index < pHeader->identifierCount)
        {
            easymtl_identifier* firstIdentifier = (easymtl_identifier*)(pMaterial->pRawData + pHeader->identifiersOffset);
            return firstIdentifier + index;
        }
    }

    return NULL;
}

unsigned int easymtl_getidentifiercount(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        return pHeader->identifierCount;
    }

    return 0;
}


unsigned int easymtl_getinputcount(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        return pHeader->privateInputCount + pHeader->publicInputCount;
    }

    return 0;
}

easymtl_input* easymtl_getinputbyindex(easymtl_material* pMaterial, unsigned int index)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        if (index < (pHeader->privateInputCount + pHeader->publicInputCount))
        {
            easymtl_input* firstInput = (easymtl_input*)(pMaterial->pRawData + pHeader->inputsOffset);
            return firstInput + index;
        }
    }

    return NULL;
}

unsigned int easymtl_getprivateinputcount(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        return pHeader->privateInputCount;
    }
    
    return 0;
}

easymtl_input* easymtl_getprivateinputbyindex(easymtl_material* pMaterial, unsigned int index)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        if (index < pHeader->privateInputCount)
        {
            easymtl_input* firstInput = (easymtl_input*)(pMaterial->pRawData + pHeader->inputsOffset);
            return firstInput + index;
        }
    }

    return NULL;
}

unsigned int easymtl_getpublicinputcount(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        return pHeader->publicInputCount;
    }
    
    return 0;
}

easymtl_input* easymtl_getpublicinputbyindex(easymtl_material* pMaterial, unsigned int index)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        if (index < pHeader->publicInputCount)
        {
            easymtl_input* firstInput = (easymtl_input*)(pMaterial->pRawData + pHeader->inputsOffset);
            return firstInput + pHeader->privateInputCount + index;
        }
    }

    return NULL;
}


unsigned int easymtl_getpropertycount(easymtl_material* pMaterial)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        return pHeader->propertyCount;
    }

    return 0;
}

easymtl_property* easymtl_getpropertybyindex(easymtl_material* pMaterial, unsigned int index)
{
    if (pMaterial != NULL)
    {
        easymtl_header* pHeader = easymtl_getheader(pMaterial);
        assert(pHeader != NULL);

        if (index < pHeader->propertyCount)
        {
            easymtl_property* firstProperty = (easymtl_property*)(pMaterial->pRawData + pHeader->propertiesOffset);
            return firstProperty + index;
        }
    }

    return NULL;
}


//////////////////////////////////
// Private low-level API.

easymtl_bool _easymtl_inflate(easymtl_material* pMaterial)
{
    assert(pMaterial != NULL);

    easymtl_uint8* pOldBuffer = pMaterial->pRawData;
    easymtl_uint8* pNewBuffer = malloc(pMaterial->bufferSizeInBytes + EASYMTL_CHUNK_SIZE);
    if (pNewBuffer != NULL)
    {
        memcpy(pNewBuffer, pOldBuffer, pMaterial->sizeInBytes);
        pMaterial->pRawData = pNewBuffer;
        pMaterial->bufferSizeInBytes += EASYMTL_CHUNK_SIZE;

        free(pOldBuffer);
        return 1;
    }

    return 0;
}



////////////////////////////////////////////////////////
// Mid-Level APIs

easymtl_identifier easymtl_identifier_float(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_float;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}

easymtl_identifier easymtl_identifier_float2(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_float2;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}

easymtl_identifier easymtl_identifier_float3(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_float3;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}

easymtl_identifier easymtl_identifier_float4(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_float4;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}

easymtl_identifier easymtl_identifier_int(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_int;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}

easymtl_identifier easymtl_identifier_int2(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_int2;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}

easymtl_identifier easymtl_identifier_int3(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_int3;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}

easymtl_identifier easymtl_identifier_int4(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_int4;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}

easymtl_identifier easymtl_identifier_tex2d(const char* name)
{
    easymtl_identifier identifier;
    identifier.type = easymtl_type_tex2d;
    easymtl_strcpy(identifier.name, EASYMTL_MAX_IDENTIFIER_NAME, name);

    return identifier;
}


easymtl_input easymtl_input_float(unsigned int identifierIndex, float x)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    input.f1.x = x;

    return input;
}

easymtl_input easymtl_input_float2(unsigned int identifierIndex, float x, float y)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    input.f2.x = x;
    input.f2.y = y;

    return input;
}

easymtl_input easymtl_input_float3(unsigned int identifierIndex, float x, float y, float z)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    input.f3.x = x;
    input.f3.y = y;
    input.f3.z = z;

    return input;
}

easymtl_input easymtl_input_float4(unsigned int identifierIndex, float x, float y, float z, float w)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    input.f4.x = x;
    input.f4.y = y;
    input.f4.z = z;
    input.f4.w = w;

    return input;
}

easymtl_input easymtl_input_int(unsigned int identifierIndex, int x)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    input.i1.x = x;

    return input;
}

easymtl_input easymtl_input_int2(unsigned int identifierIndex, int x, int y)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    input.i2.x = x;
    input.i2.y = y;

    return input;
}

easymtl_input easymtl_input_int3(unsigned int identifierIndex, int x, int y, int z)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    input.i3.x = x;
    input.i3.y = y;
    input.i3.z = z;

    return input;
}

easymtl_input easymtl_input_int4(unsigned int identifierIndex, int x, int y, int z, int w)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    input.i4.x = x;
    input.i4.y = y;
    input.i4.z = z;
    input.i4.w = w;

    return input;
}

easymtl_input easymtl_input_tex(unsigned int identifierIndex, const char* path)
{
    easymtl_input input;
    input.identifierIndex = identifierIndex;
    easymtl_strcpy(input.path.value, EASYMTL_MAX_INPUT_PATH, path);

    return input;
}


easymtl_channel easymtl_channel_float(const char* name)
{
    easymtl_channel channel;
    channel.type = easymtl_type_float;
    easymtl_strcpy(channel.name, EASYMTL_MAX_CHANNEL_NAME, name);

    return channel;
}

easymtl_channel easymtl_channel_float2(const char* name)
{
    easymtl_channel channel;
    channel.type = easymtl_type_float2;
    easymtl_strcpy(channel.name, EASYMTL_MAX_CHANNEL_NAME, name);

    return channel;
}

easymtl_channel easymtl_channel_float3(const char* name)
{
    easymtl_channel channel;
    channel.type = easymtl_type_float3;
    easymtl_strcpy(channel.name, EASYMTL_MAX_CHANNEL_NAME, name);

    return channel;
}

easymtl_channel easymtl_channel_float4(const char* name)
{
    easymtl_channel channel;
    channel.type = easymtl_type_float4;
    easymtl_strcpy(channel.name, EASYMTL_MAX_CHANNEL_NAME, name);

    return channel;
}

easymtl_channel easymtl_channel_int(const char* name)
{
    easymtl_channel channel;
    channel.type = easymtl_type_int;
    easymtl_strcpy(channel.name, EASYMTL_MAX_CHANNEL_NAME, name);

    return channel;
}

easymtl_channel easymtl_channel_int2(const char* name)
{
    easymtl_channel channel;
    channel.type = easymtl_type_int2;
    easymtl_strcpy(channel.name, EASYMTL_MAX_CHANNEL_NAME, name);

    return channel;
}

easymtl_channel easymtl_channel_int3(const char* name)
{
    easymtl_channel channel;
    channel.type = easymtl_type_int3;
    easymtl_strcpy(channel.name, EASYMTL_MAX_CHANNEL_NAME, name);

    return channel;
}

easymtl_channel easymtl_channel_int4(const char* name)
{
    easymtl_channel channel;
    channel.type = easymtl_type_int4;
    easymtl_strcpy(channel.name, EASYMTL_MAX_CHANNEL_NAME, name);

    return channel;
}


easymtl_instruction easymtl_movf1_v1(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_movf1;
    inst.mov.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.mov.inputX.id   = inputIdentifierIndex;
    inst.mov.output      = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_movf1_c1(unsigned int outputIdentifierIndex, float x)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_movf1;
    inst.mov.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputX.valuef = x;
    inst.mov.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_movf2_v2(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_movf2;
    inst.mov.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.mov.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.mov.inputX.id   = inputIdentifierIndex;
    inst.mov.inputY.id   = inputIdentifierIndex;
    inst.mov.output      = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_movf2_c2(unsigned int outputIdentifierIndex, float x, float y)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_movf2;
    inst.mov.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputX.valuef = x;
    inst.mov.inputY.valuef = y;
    inst.mov.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_movf3_v3(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_movf3;
    inst.mov.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.mov.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.mov.inputDesc.z = EASYMTL_INPUT_DESC_VARZ;
    inst.mov.inputX.id   = inputIdentifierIndex;
    inst.mov.inputY.id   = inputIdentifierIndex;
    inst.mov.inputZ.id   = inputIdentifierIndex;
    inst.mov.output      = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_movf3_c3(unsigned int outputIdentifierIndex, float x, float y, float z)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_movf3;
    inst.mov.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputX.valuef = x;
    inst.mov.inputY.valuef = y;
    inst.mov.inputZ.valuef = z;
    inst.mov.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_movf4_v4(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_movf4;
    inst.mov.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.mov.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.mov.inputDesc.z = EASYMTL_INPUT_DESC_VARZ;
    inst.mov.inputDesc.w = EASYMTL_INPUT_DESC_VARW;
    inst.mov.inputX.id   = inputIdentifierIndex;
    inst.mov.inputY.id   = inputIdentifierIndex;
    inst.mov.inputZ.id   = inputIdentifierIndex;
    inst.mov.inputW.id   = inputIdentifierIndex;
    inst.mov.output      = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_movf4_c4(unsigned int outputIdentifierIndex, float x, float y, float z, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_movf4;
    inst.mov.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputDesc.w   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mov.inputX.valuef = x;
    inst.mov.inputY.valuef = y;
    inst.mov.inputZ.valuef = z;
    inst.mov.inputW.valuef = w;
    inst.mov.output        = outputIdentifierIndex;

    return inst;
}


easymtl_instruction easymtl_addf1_v1(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_addf1;
    inst.add.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.add.inputX.id     = inputIdentifierIndex;
    inst.add.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_addf1_c1(unsigned int outputIdentifierIndex, float x)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_addf1;
    inst.add.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputX.valuef = x;
    inst.add.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_addf2_v2(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_addf2;
    inst.add.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.add.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.add.inputX.id     = inputIdentifierIndex;
    inst.add.inputY.id     = inputIdentifierIndex;
    inst.add.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_addf2_c2(unsigned int outputIdentifierIndex, float x, float y)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_addf2;
    inst.add.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputX.valuef = x;
    inst.add.inputY.valuef = y;
    inst.add.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_addf3_v3(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_addf3;
    inst.add.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.add.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.add.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.add.inputX.id     = inputIdentifierIndex;
    inst.add.inputY.id     = inputIdentifierIndex;
    inst.add.inputZ.id     = inputIdentifierIndex;
    inst.add.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_addf3_c3(unsigned int outputIdentifierIndex, float x, float y, float z)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_addf3;
    inst.add.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputX.valuef = x;
    inst.add.inputY.valuef = y;
    inst.add.inputZ.valuef = z;
    inst.add.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_addf4_v4(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_addf4;
    inst.add.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.add.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.add.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.add.inputDesc.w   = EASYMTL_INPUT_DESC_VARW;
    inst.add.inputX.id     = inputIdentifierIndex;
    inst.add.inputY.id     = inputIdentifierIndex;
    inst.add.inputZ.id     = inputIdentifierIndex;
    inst.add.inputW.id     = inputIdentifierIndex;
    inst.add.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_addf4_c4(unsigned int outputIdentifierIndex, float x, float y, float z, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_addf4;
    inst.add.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputDesc.w   = EASYMTL_INPUT_DESC_CONSTF;
    inst.add.inputX.valuef = x;
    inst.add.inputY.valuef = y;
    inst.add.inputZ.valuef = z;
    inst.add.inputW.valuef = w;
    inst.add.output        = outputIdentifierIndex;

    return inst;
}


easymtl_instruction easymtl_subf1_v1(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_subf1;
    inst.sub.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.sub.inputX.id     = inputIdentifierIndex;
    inst.sub.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_subf1_c1(unsigned int outputIdentifierIndex, float x)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_subf1;
    inst.sub.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputX.valuef = x;
    inst.sub.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_subf2_v2(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_subf2;
    inst.sub.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.sub.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.sub.inputX.id     = inputIdentifierIndex;
    inst.sub.inputY.id     = inputIdentifierIndex;
    inst.sub.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_subf2_c2(unsigned int outputIdentifierIndex, float x, float y)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_subf2;
    inst.sub.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputX.valuef = x;
    inst.sub.inputY.valuef = y;
    inst.sub.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_subf3_v3(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_subf3;
    inst.sub.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.sub.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.sub.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.sub.inputX.id     = inputIdentifierIndex;
    inst.sub.inputY.id     = inputIdentifierIndex;
    inst.sub.inputZ.id     = inputIdentifierIndex;
    inst.sub.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_subf3_c3(unsigned int outputIdentifierIndex, float x, float y, float z)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_subf3;
    inst.sub.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputX.valuef = x;
    inst.sub.inputY.valuef = y;
    inst.sub.inputZ.valuef = z;
    inst.sub.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_subf4_v4(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_subf4;
    inst.sub.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.sub.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.sub.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.sub.inputDesc.w   = EASYMTL_INPUT_DESC_VARW;
    inst.sub.inputX.id     = inputIdentifierIndex;
    inst.sub.inputY.id     = inputIdentifierIndex;
    inst.sub.inputZ.id     = inputIdentifierIndex;
    inst.sub.inputW.id     = inputIdentifierIndex;
    inst.sub.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_subf4_c4(unsigned int outputIdentifierIndex, float x, float y, float z, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_subf4;
    inst.sub.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputDesc.w   = EASYMTL_INPUT_DESC_CONSTF;
    inst.sub.inputX.valuef = x;
    inst.sub.inputY.valuef = y;
    inst.sub.inputZ.valuef = z;
    inst.sub.inputW.valuef = w;
    inst.sub.output        = outputIdentifierIndex;

    return inst;
}


easymtl_instruction easymtl_mulf1_v1(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf1;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputX.id     = inputIdentifierIndex;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf1_c1(unsigned int outputIdentifierIndex, float x)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf1;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputX.valuef = x;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf2_v2(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf2;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.mul.inputX.id     = inputIdentifierIndex;
    inst.mul.inputY.id     = inputIdentifierIndex;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf2_c2(unsigned int outputIdentifierIndex, float x, float y)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf2;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputX.valuef = x;
    inst.mul.inputY.valuef = y;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf3_v3(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf3;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.mul.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.mul.inputX.id     = inputIdentifierIndex;
    inst.mul.inputY.id     = inputIdentifierIndex;
    inst.mul.inputZ.id     = inputIdentifierIndex;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf3_c3(unsigned int outputIdentifierIndex, float x, float y, float z)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf3;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputX.valuef = x;
    inst.mul.inputY.valuef = y;
    inst.mul.inputZ.valuef = z;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf4_v4(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf4;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.mul.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.mul.inputDesc.w   = EASYMTL_INPUT_DESC_VARW;
    inst.mul.inputX.id     = inputIdentifierIndex;
    inst.mul.inputY.id     = inputIdentifierIndex;
    inst.mul.inputZ.id     = inputIdentifierIndex;
    inst.mul.inputW.id     = inputIdentifierIndex;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf4_c4(unsigned int outputIdentifierIndex, float x, float y, float z, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf4;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.w   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputX.valuef = x;
    inst.mul.inputY.valuef = y;
    inst.mul.inputZ.valuef = z;
    inst.mul.inputW.valuef = w;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf4_v3c1(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf4;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.mul.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.mul.inputDesc.w   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputX.id     = inputIdentifierIndex;
    inst.mul.inputY.id     = inputIdentifierIndex;
    inst.mul.inputZ.id     = inputIdentifierIndex;
    inst.mul.inputW.valuef = w;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf4_v2c2(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex, float z, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf4;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.mul.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.w   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputX.id     = inputIdentifierIndex;
    inst.mul.inputY.id     = inputIdentifierIndex;
    inst.mul.inputZ.valuef = z;
    inst.mul.inputW.valuef = w;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf4_v1c3(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex, float y, float z, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf4;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputDesc.w   = EASYMTL_INPUT_DESC_CONSTF;
    inst.mul.inputX.id     = inputIdentifierIndex;
    inst.mul.inputY.valuef = y;
    inst.mul.inputZ.valuef = z;
    inst.mul.inputW.valuef = w;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_mulf4_v1v1v1v1(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndexX, unsigned int inputIdentifierIndexY, unsigned int inputIdentifierIndexZ, unsigned int inputIdentifierIndexW)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_mulf4;
    inst.mul.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.y   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.z   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputDesc.w   = EASYMTL_INPUT_DESC_VARX;
    inst.mul.inputX.id     = inputIdentifierIndexX;
    inst.mul.inputY.id     = inputIdentifierIndexY;
    inst.mul.inputZ.id     = inputIdentifierIndexZ;
    inst.mul.inputW.id     = inputIdentifierIndexW;
    inst.mul.output        = outputIdentifierIndex;

    return inst;
}


easymtl_instruction easymtl_divf1_v1(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_divf1;
    inst.div.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.div.inputX.id     = inputIdentifierIndex;
    inst.div.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_divf1_c1(unsigned int outputIdentifierIndex, float x)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_divf1;
    inst.div.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputX.valuef = x;
    inst.div.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_divf2_v2(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_divf2;
    inst.div.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.div.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.div.inputX.id     = inputIdentifierIndex;
    inst.div.inputY.id     = inputIdentifierIndex;
    inst.div.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_divf2_c2(unsigned int outputIdentifierIndex, float x, float y)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_divf2;
    inst.div.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputX.valuef = x;
    inst.div.inputY.valuef = y;
    inst.div.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_divf3_v3(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_divf3;
    inst.div.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.div.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.div.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.div.inputX.id     = inputIdentifierIndex;
    inst.div.inputY.id     = inputIdentifierIndex;
    inst.div.inputZ.id     = inputIdentifierIndex;
    inst.div.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_divf3_c3(unsigned int outputIdentifierIndex, float x, float y, float z)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_divf3;
    inst.div.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputX.valuef = x;
    inst.div.inputY.valuef = y;
    inst.div.inputZ.valuef = z;
    inst.div.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_divf4_v4(unsigned int outputIdentifierIndex, unsigned int inputIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_divf4;
    inst.div.inputDesc.x   = EASYMTL_INPUT_DESC_VARX;
    inst.div.inputDesc.y   = EASYMTL_INPUT_DESC_VARY;
    inst.div.inputDesc.z   = EASYMTL_INPUT_DESC_VARZ;
    inst.div.inputDesc.w   = EASYMTL_INPUT_DESC_VARW;
    inst.div.inputX.id     = inputIdentifierIndex;
    inst.div.inputY.id     = inputIdentifierIndex;
    inst.div.inputZ.id     = inputIdentifierIndex;
    inst.div.inputW.id     = inputIdentifierIndex;
    inst.div.output        = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_divf4_c4(unsigned int outputIdentifierIndex, float x, float y, float z, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_divf4;
    inst.div.inputDesc.x   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputDesc.y   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputDesc.z   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputDesc.w   = EASYMTL_INPUT_DESC_CONSTF;
    inst.div.inputX.valuef = x;
    inst.div.inputY.valuef = y;
    inst.div.inputZ.valuef = z;
    inst.div.inputW.valuef = w;
    inst.div.output        = outputIdentifierIndex;

    return inst;
}


easymtl_instruction easymtl_tex2(unsigned int outputIdentifierIndex, unsigned int textureIdentifierIndex, unsigned int texcoordIdentifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_tex2;
    inst.tex.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.tex.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.tex.inputX.id   = texcoordIdentifierIndex;
    inst.tex.inputY.id   = texcoordIdentifierIndex;
    inst.tex.texture     = textureIdentifierIndex;
    inst.tex.output      = outputIdentifierIndex;

    return inst;
}

easymtl_instruction easymtl_var(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_var;
    inst.var.identifierIndex = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_retf1(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_retf1;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.ret.inputX.id = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_retf2(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_retf2;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.ret.inputX.id = identifierIndex;
    inst.ret.inputY.id = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_retf3(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_retf3;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.ret.inputDesc.z = EASYMTL_INPUT_DESC_VARZ;
    inst.ret.inputX.id = identifierIndex;
    inst.ret.inputY.id = identifierIndex;
    inst.ret.inputZ.id = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_retf4(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_retf4;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.ret.inputDesc.z = EASYMTL_INPUT_DESC_VARZ;
    inst.ret.inputDesc.w = EASYMTL_INPUT_DESC_VARW;
    inst.ret.inputX.id = identifierIndex;
    inst.ret.inputY.id = identifierIndex;
    inst.ret.inputZ.id = identifierIndex;
    inst.ret.inputW.id = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_retf1_c1(float x)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_retf1;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputX.valuef = x;

    return inst;
}

easymtl_instruction easymtl_retf2_c2(float x, float y)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_retf2;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputX.valuef = x;
    inst.ret.inputY.valuef = y;

    return inst;
}

easymtl_instruction easymtl_retf3_c3(float x, float y, float z)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_retf3;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputDesc.z = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputX.valuef = x;
    inst.ret.inputY.valuef = y;
    inst.ret.inputZ.valuef = z;

    return inst;
}

easymtl_instruction easymtl_retf4_c4(float x, float y, float z, float w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_retf4;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputDesc.z = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputDesc.w = EASYMTL_INPUT_DESC_CONSTF;
    inst.ret.inputX.valuef = x;
    inst.ret.inputY.valuef = y;
    inst.ret.inputZ.valuef = z;
    inst.ret.inputW.valuef = w;

    return inst;
}

easymtl_instruction easymtl_reti1(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_reti1;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.ret.inputX.id = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_reti2(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_reti2;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.ret.inputX.id = identifierIndex;
    inst.ret.inputY.id = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_reti3(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_reti3;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.ret.inputDesc.z = EASYMTL_INPUT_DESC_VARZ;
    inst.ret.inputX.id = identifierIndex;
    inst.ret.inputY.id = identifierIndex;
    inst.ret.inputZ.id = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_reti4(unsigned int identifierIndex)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_reti4;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_VARX;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_VARY;
    inst.ret.inputDesc.z = EASYMTL_INPUT_DESC_VARZ;
    inst.ret.inputDesc.w = EASYMTL_INPUT_DESC_VARW;
    inst.ret.inputX.id = identifierIndex;
    inst.ret.inputY.id = identifierIndex;
    inst.ret.inputZ.id = identifierIndex;
    inst.ret.inputW.id = identifierIndex;

    return inst;
}

easymtl_instruction easymtl_reti1_c1(int x)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_reti1;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputX.valuei = x;

    return inst;
}

easymtl_instruction easymtl_reti2_c2(int x, int y)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_reti2;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputX.valuei = x;
    inst.ret.inputY.valuei = y;

    return inst;
}

easymtl_instruction easymtl_reti3_c3(int x, int y, int z)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_reti3;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputDesc.z = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputX.valuei = x;
    inst.ret.inputY.valuei = y;
    inst.ret.inputZ.valuei = z;

    return inst;
}

easymtl_instruction easymtl_reti4_c4(int x, int y, int z, int w)
{
    easymtl_instruction inst;
    inst.opcode = easymtl_opcode_reti4;
    inst.ret.inputDesc.x = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputDesc.y = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputDesc.z = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputDesc.w = EASYMTL_INPUT_DESC_CONSTI;
    inst.ret.inputX.valuei = x;
    inst.ret.inputY.valuei = y;
    inst.ret.inputZ.valuei = z;
    inst.ret.inputW.valuei = w;

    return inst;
}



easymtl_property easymtl_property_float(const char* name, float x)
{
    easymtl_property prop;
    prop.type = easymtl_type_float;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.f1.x = x;

    return prop;
}

easymtl_property easymtl_property_float2(const char* name, float x, float y)
{
    easymtl_property prop;
    prop.type = easymtl_type_float2;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.f2.x = x;
    prop.f2.y = y;

    return prop;
}

easymtl_property easymtl_property_float3(const char* name, float x, float y, float z)
{
    easymtl_property prop;
    prop.type = easymtl_type_float3;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.f3.x = x;
    prop.f3.y = y;
    prop.f3.z = z;

    return prop;
}

easymtl_property easymtl_property_float4(const char* name, float x, float y, float z, float w)
{
    easymtl_property prop;
    prop.type = easymtl_type_float4;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.f4.x = x;
    prop.f4.y = y;
    prop.f4.z = z;
    prop.f4.w = w;

    return prop;
}

easymtl_property easymtl_property_int(const char* name, int x)
{
    easymtl_property prop;
    prop.type = easymtl_type_int;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.i1.x = x;

    return prop;
}

easymtl_property easymtl_property_int2(const char* name, int x, int y)
{
    easymtl_property prop;
    prop.type = easymtl_type_int2;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.i2.x = x;
    prop.i2.y = y;

    return prop;
}

easymtl_property easymtl_property_int3(const char* name, int x, int y, int z)
{
    easymtl_property prop;
    prop.type = easymtl_type_int3;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.i3.x = x;
    prop.i3.y = y;
    prop.i3.z = z;

    return prop;
}

easymtl_property easymtl_property_int4(const char* name, int x, int y, int z, int w)
{
    easymtl_property prop;
    prop.type = easymtl_type_int4;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.i4.x = x;
    prop.i4.y = y;
    prop.i4.z = z;
    prop.i4.w = w;

    return prop;
}

easymtl_property easymtl_property_bool(const char* name, easymtl_bool value)
{
    easymtl_property prop;
    prop.type = easymtl_type_bool;
    easymtl_strcpy(prop.name, EASYMTL_MAX_PROPERTY_NAME, name);
    prop.b1.x = value;

    return prop;
}






////////////////////////////////////////////////////////
// Utilities

// strcpy()
void easymtl_strcpy(char* dst, size_t dstSizeInBytes, const char* src)
{
#if defined(EASYMTL_PLATFORM_WINDOWS)
    strcpy_s(dst, dstSizeInBytes, src);
#else
    while (dstSizeInBytes > 0 && src[0] != '\0')
    {
        dst[0] = src[0];

        dst += 1;
        src += 1;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes > 0)
    {
        dst[0] = '\0';
    }
#endif
}



/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/