// Version 0.1 - Public Domain. See "unlicense" statement at the end of this file.

#include "easy_vfs_mtl.h"
#include "../easy_vfs.h"

#include <assert.h>
#include <string.h>

typedef struct
{
    /// The byte offset within the archive 
    easyvfs_int64 offset;

    /// The size of the file, in bytes.
    easyvfs_int64 sizeInBytes;

    /// The name of the material. The specification says this can be any length, but we're going to clamp it to 255 + null terminator which should be fine.
    char name[256];

}easyvfs_file_mtl;

typedef struct
{
    /// The access mode.
    easyvfs_accessmode accessMode;

    /// The buffer containing the list of files.
    easyvfs_file_mtl* pFiles;

    /// The number of files in the archive.
    unsigned int fileCount;

}easyvfs_archive_mtl;

typedef struct
{
    /// The current index of the iterator. When this hits the file count, the iteration is finished.
    unsigned int index;

}easyvfs_iterator_mtl;

typedef struct
{
    /// The offset within the archive file the first byte of the file is located.
    easyvfs_int64 offsetInArchive;

    /// The size of the file in bytes so we can guard against overflowing reads.
    easyvfs_int64 sizeInBytes;

    /// The current position of the file's read pointer.
    easyvfs_int64 readPointer;

}easyvfs_openedfile_mtl;


easyvfs_archive_mtl* easyvfs_mtl_create(easyvfs_accessmode accessMode)
{
    easyvfs_archive_mtl* mtl = easyvfs_malloc(sizeof(easyvfs_archive_mtl));
    if (mtl != NULL)
    {
        mtl->accessMode = accessMode;
        mtl->pFiles     = NULL;
        mtl->fileCount  = 0;
    }

    return mtl;
}

void easyvfs_mtl_delete(easyvfs_archive_mtl* pArchive)
{
    easyvfs_free(pArchive->pFiles);
    easyvfs_free(pArchive);
}

void easyvfs_mtl_addfile(easyvfs_archive_mtl* pArchive, easyvfs_file_mtl* pFile)
{
    if (pArchive != NULL && pFile != NULL)
    {
        easyvfs_file_mtl* pOldBuffer = pArchive->pFiles;
        easyvfs_file_mtl* pNewBuffer = easyvfs_malloc(sizeof(easyvfs_file_mtl) * (pArchive->fileCount + 1));

        if (pNewBuffer != 0)
        {
            for (unsigned int iDst = 0; iDst < pArchive->fileCount; ++iDst)
            {
                pNewBuffer[iDst] = pOldBuffer[iDst];
            }

            pNewBuffer[pArchive->fileCount] = *pFile;


            pArchive->pFiles     = pNewBuffer;
            pArchive->fileCount += 1;
        }
    }
}


int           easyvfs_isvalidarchive_mtl(easyvfs_context* pContext, const char* path);
void*         easyvfs_openarchive_mtl   (easyvfs_file* pFile, easyvfs_accessmode accessMode);
void          easyvfs_closearchive_mtl  (easyvfs_archive* pArchive);
int           easyvfs_getfileinfo_mtl   (easyvfs_archive* pArchive, const char* path, easyvfs_fileinfo *fi);
void*         easyvfs_beginiteration_mtl(easyvfs_archive* pArchive, const char* path);
void          easyvfs_enditeration_mtl  (easyvfs_archive* pArchive, easyvfs_iterator* i);
int           easyvfs_nextiteration_mtl (easyvfs_archive* pArchive, easyvfs_iterator* i, easyvfs_fileinfo* fi);
void*         easyvfs_openfile_mtl      (easyvfs_archive* pArchive, const char* path, easyvfs_accessmode accessMode);
void          easyvfs_closefile_mtl     (easyvfs_file* pFile);
int           easyvfs_readfile_mtl      (easyvfs_file* pFile, void* dst, unsigned int bytesToRead, unsigned int* bytesReadOut);
int           easyvfs_writefile_mtl     (easyvfs_file* pFile, const void* src, unsigned int bytesToWrite, unsigned int* bytesWrittenOut);
easyvfs_bool  easyvfs_seekfile_mtl      (easyvfs_file* pFile, easyvfs_int64 bytesToSeek, easyvfs_seekorigin origin);
easyvfs_int64 easyvfs_tellfile_mtl      (easyvfs_file* pFile);
easyvfs_int64 easyvfs_filesize_mtl      (easyvfs_file* pFile);
int           easyvfs_deletefile_mtl    (easyvfs_archive* pArchive, const char* path);
int           easyvfs_renamefile_mtl    (easyvfs_archive* pArchive, const char* pathOld, const char* pathNew);
int           easyvfs_mkdir_mtl         (easyvfs_archive* pArchive, const char* path);

void easyvfs_registerarchivecallbacks_mtl(easyvfs_context* pContext)
{
    easyvfs_archive_callbacks callbacks;
    callbacks.isvalidarchive = easyvfs_isvalidarchive_mtl;
    callbacks.openarchive    = easyvfs_openarchive_mtl;
    callbacks.closearchive   = easyvfs_closearchive_mtl;
    callbacks.getfileinfo    = easyvfs_getfileinfo_mtl;
    callbacks.beginiteration = easyvfs_beginiteration_mtl;
    callbacks.enditeration   = easyvfs_enditeration_mtl;
    callbacks.nextiteration  = easyvfs_nextiteration_mtl;
    callbacks.openfile       = easyvfs_openfile_mtl;
    callbacks.closefile      = easyvfs_closefile_mtl;
    callbacks.readfile       = easyvfs_readfile_mtl;
    callbacks.writefile      = easyvfs_writefile_mtl;
    callbacks.seekfile       = easyvfs_seekfile_mtl;
    callbacks.tellfile       = easyvfs_tellfile_mtl;
    callbacks.filesize       = easyvfs_filesize_mtl;
    callbacks.deletefile     = easyvfs_deletefile_mtl;
    callbacks.renamefile     = easyvfs_renamefile_mtl;
    callbacks.mkdir          = easyvfs_mkdir_mtl;
    easyvfs_registerarchivecallbacks(pContext, callbacks);
}


typedef struct
{
    easyvfs_file* pFile;
    easyvfs_int64 archiveSizeInBytes;
    easyvfs_int64 bytesRemaining;
    char          chunk[4096];
    unsigned int  chunkSize;
    char*         chunkPointer;
    char*         chunkEnd;

}easyvfs_openarchive_mtl_state;

int easyvfs_mtl_loadnextchunk(easyvfs_openarchive_mtl_state* pState)
{
    assert(pState != NULL);

    if (pState->bytesRemaining > 0)
    {
        pState->chunkSize = (pState->bytesRemaining > 4096) ? 4096 : (unsigned int)pState->bytesRemaining;
        assert(pState->chunkSize);

        if (easyvfs_readfile(pState->pFile, pState->chunk, pState->chunkSize, NULL))
        {
            pState->bytesRemaining -= pState->chunkSize;
            pState->chunkPointer = pState->chunk;
            pState->chunkEnd     = pState->chunk + pState->chunkSize;

            return 1;
        }
        else
        {
            // An error occured while reading. Just reset everything to make it look like an error occured.
            pState->bytesRemaining = 0;
            pState->chunkSize      = 0;
            pState->chunkPointer   = pState->chunk;
            pState->chunkEnd       = pState->chunkPointer;
        }
    }

    return 0;
}

int easyvfs_mtl_loadnewmtl(easyvfs_openarchive_mtl_state* pState)
{
    assert(pState != NULL);

    const char newmtl[7] = "newmtl";
    for (unsigned int i = 0; i < 6; ++i)
    {
        // Check if we need a new chunk.
        if (pState->chunkPointer >= pState->chunkEnd)
        {
            if (!easyvfs_mtl_loadnextchunk(pState))
            {
                return 0;
            }
        }


        if (pState->chunkPointer[0] != newmtl[i])
        {
            return 0;
        }

        pState->chunkPointer += 1;
    }

    // At this point the first 6 characters equal "newmtl".
    return 1;
}

int easyvfs_mtl_skipline(easyvfs_openarchive_mtl_state* pState)
{
    assert(pState != NULL);

    // Keep looping until we find a new line character.
    while (pState->chunkPointer < pState->chunkEnd)
    {
        if (pState->chunkPointer[0] == '\n')
        {
            // Found the new line. Now move forward by one to get past the new line character.
            pState->chunkPointer += 1;
            if (pState->chunkPointer >= pState->chunkEnd)
            {
                return easyvfs_mtl_loadnextchunk(pState);
            }

            return 1;
        }

        pState->chunkPointer += 1;
    }

    // If we get here it means we got past the end of the chunk. We just read the next chunk and call this recursively.
    if (easyvfs_mtl_loadnextchunk(pState))
    {
        return easyvfs_mtl_skipline(pState);
    }

    return 0;
}

int easyvfs_mtl_skipwhitespace(easyvfs_openarchive_mtl_state* pState)
{
    assert(pState != NULL);

    while (pState->chunkPointer < pState->chunkEnd)
    {
        const char c = pState->chunkPointer[0];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
        {
            return 1;
        }

        pState->chunkPointer += 1;
    }

    if (easyvfs_mtl_loadnextchunk(pState))
    {
        return easyvfs_mtl_skipwhitespace(pState);
    }

    return 0;
}

int easyvfs_mtl_loadmtlname(easyvfs_openarchive_mtl_state* pState, void* dst, unsigned int dstSizeInBytes)
{
    assert(pState != NULL);

    // We loop over character by character until we find a whitespace, "#" character or the end of the file.
    char* dst8 = dst;
    while (dstSizeInBytes > 0 && pState->chunkPointer < pState->chunkEnd)
    {
        const char c = pState->chunkPointer[0];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '#')
        {
            // We've found the end of the name. Null terminate and return.
            *dst8 = '\0';
            return 1;
        }
        else
        {
            *dst8++ = c;
            dstSizeInBytes -= 1;
            pState->chunkPointer += 1;
        }
    }

    // At this point we either ran out of space in the destination buffer or the chunk.
    if (dstSizeInBytes > 0)
    {
        // We got to the end of the chunk, so we need to load the next chunk and call this recursively.
        assert(pState->chunkPointer == pState->chunkEnd);

        if (easyvfs_mtl_loadnextchunk(pState))
        {
            return easyvfs_mtl_loadmtlname(pState, dst8, dstSizeInBytes);
        }
        else
        {
            // We reached the end of the file, but the name may be valid.
            return 1;
        }
    }
    else
    {
        // We ran out of room in the buffer.
        return 0;
    }
}


int easyvfs_isvalidarchive_mtl(easyvfs_context* pContext, const char* path)
{
    (void)pContext;

    if (easyvfs_extensionequal(path, "mtl"))
    {
        return 1;
    }

    return 0;
}

void* easyvfs_openarchive_mtl(easyvfs_file* pFile, easyvfs_accessmode accessMode)
{
    assert(pFile != NULL);
    assert(easyvfs_tellfile(pFile) == 0);

    easyvfs_archive_mtl* mtl = easyvfs_mtl_create(accessMode);
    if (mtl != NULL)
    {
        mtl->accessMode = accessMode;


        // We create a state object that is used to help us with chunk management.
        easyvfs_openarchive_mtl_state state;
        state.pFile              = pFile;
        state.archiveSizeInBytes = easyvfs_filesize(pFile);
        state.bytesRemaining     = state.archiveSizeInBytes;
        state.chunkSize          = 0;
        state.chunkPointer       = state.chunk;
        state.chunkEnd           = state.chunkEnd;
        if (easyvfs_mtl_loadnextchunk(&state))
        {
            while (state.bytesRemaining > 0 || state.chunkPointer < state.chunkEnd)
            {
                if (easyvfs_mtl_loadnewmtl(&state))
                {
                    if (state.chunkPointer[0] == ' ' || state.chunkPointer[0] == '\t')
                    {
                        // We found a new material. We need to iterate until we hit the first whitespace, "#", or the end of the file.
                        if (easyvfs_mtl_skipwhitespace(&state))
                        {
                            easyvfs_file_mtl file;
                            if (easyvfs_mtl_loadmtlname(&state, file.name, 256))
                            {
                                // Everything worked out. We now need to create the file and add it to our list. At this point we won't know the size. We determine
                                // the size in a post-processing step later.
                                unsigned int bytesRemainingInChunk = state.chunkEnd - state.chunkPointer;
                                file.offset = state.archiveSizeInBytes - state.bytesRemaining - bytesRemainingInChunk;

                                easyvfs_mtl_addfile(mtl, &file);
                            }
                        }
                    }
                }

                // Move to the next line.
                easyvfs_mtl_skipline(&state);
            }


            // The files will have been read at this point, but we need to do a post-processing step to retrieve the size of each file.
            for (unsigned int iFile = 0; iFile < mtl->fileCount; ++iFile)
            {
                if (iFile < mtl->fileCount - 1)
                {
                    // It's not the last item. The size of this item is the offset of the next file minus the offset of this file.
                    mtl->pFiles[iFile].sizeInBytes = mtl->pFiles[iFile + 1].offset - mtl->pFiles[iFile].offset;
                }
                else
                {
                    // It's the last item. The size of this item is the size of the archive file minus the file's offset.
                    mtl->pFiles[iFile].sizeInBytes = state.archiveSizeInBytes - mtl->pFiles[iFile].offset;
                }
            }
        }
        else
        {
            easyvfs_mtl_delete(mtl);
            mtl = NULL;
        }
    }

    return mtl;
}

void easyvfs_closearchive_mtl(easyvfs_archive* pArchive)
{
    assert(pArchive != 0);

    easyvfs_archive_mtl* pUserData = pArchive->pUserData;
    if (pUserData != NULL)
    {
        easyvfs_mtl_delete(pUserData);
    }
}

int easyvfs_getfileinfo_mtl(easyvfs_archive* pArchive, const char* path, easyvfs_fileinfo *fi)
{
    assert(pArchive != 0);

    easyvfs_archive_mtl* mtl = pArchive->pUserData;
    if (mtl != NULL)
    {
        for (unsigned int iFile = 0; iFile < mtl->fileCount; ++iFile)
        {
            if (strcmp(path, mtl->pFiles[iFile].name) == 0)
            {
                // We found the file.
                if (fi != NULL)
                {
                    easyvfs_copyandappendpath(fi->absolutePath, EASYVFS_MAX_PATH, pArchive->absolutePath, path);
                    fi->sizeInBytes      = mtl->pFiles[iFile].sizeInBytes;
                    fi->lastModifiedTime = 0;
                    fi->attributes       = EASYVFS_FILE_ATTRIBUTE_READONLY;
                }

                return 1;
            }
        }
    }

    return 0;
}

void* easyvfs_beginiteration_mtl(easyvfs_archive* pArchive, const char* path)
{
    assert(pArchive != 0);
    assert(path != NULL);

    easyvfs_archive_mtl* mtl = pArchive->pUserData;
    if (mtl != NULL)
    {
        if (mtl->fileCount > 0)
        {
            if (path[0] == '\0' || (path[0] == '/' && path[1] == '\0'))     // This is a flat archive, so no sub-folders.
            {
                easyvfs_iterator_mtl* i = easyvfs_malloc(sizeof(easyvfs_iterator_mtl));
                if (i != NULL)
                {
                    i->index = 0;
                    return i;
                }
            }
        }
    }

    return NULL;
}

void easyvfs_enditeration_mtl(easyvfs_archive* pArchive, easyvfs_iterator* i)
{
    assert(pArchive != 0);
    assert(i != NULL);

    easyvfs_free(i->pUserData);
}

int easyvfs_nextiteration_mtl(easyvfs_archive* pArchive, easyvfs_iterator* i, easyvfs_fileinfo* fi)
{
    assert(pArchive != 0);
    assert(i != NULL);

    easyvfs_archive_mtl* mtl = pArchive->pUserData;
    if (mtl != NULL)
    {
        easyvfs_iterator_mtl* imtl = i->pUserData;
        if (imtl != NULL)
        {
            if (imtl->index < mtl->fileCount)
            {
                if (fi != NULL)
                {
                    easyvfs_strcpy(fi->absolutePath, EASYVFS_MAX_PATH, mtl->pFiles[imtl->index].name);
                    fi->sizeInBytes      = mtl->pFiles[imtl->index].sizeInBytes;
                    fi->lastModifiedTime = 0;
                    fi->attributes       = EASYVFS_FILE_ATTRIBUTE_READONLY;
                }

                imtl->index += 1;
                return 1;
            }
        }
    }
    

    return 0;
}

void* easyvfs_openfile_mtl(easyvfs_archive* pArchive, const char* path, easyvfs_accessmode accessMode)
{
    assert(pArchive != 0);
    assert(path != NULL);

    // Only supporting read-only for now.
    if (accessMode == easyvfs_write || accessMode == easyvfs_readwrite)
    {
        return NULL;
    }

    easyvfs_archive_mtl* mtl = pArchive->pUserData;
    if (mtl != NULL)
    {
        for (unsigned int iFile = 0; iFile < mtl->fileCount; ++iFile)
        {
            if (strcmp(path, mtl->pFiles[iFile].name) == 0)
            {
                // We found the file.
                easyvfs_openedfile_mtl* pOpenedFile = easyvfs_malloc(sizeof(easyvfs_openedfile_mtl));
                if (pOpenedFile != NULL)
                {
                    pOpenedFile->offsetInArchive = mtl->pFiles[iFile].offset;
                    pOpenedFile->sizeInBytes     = mtl->pFiles[iFile].sizeInBytes;
                    pOpenedFile->readPointer     = 0;

                    return pOpenedFile;
                }
            }
        }
    }

    return NULL;
}

void easyvfs_closefile_mtl(easyvfs_file* pFile)
{
    assert(pFile != 0);

    easyvfs_openedfile_mtl* pOpenedFile = pFile->pUserData;
    if (pOpenedFile != NULL)
    {
        easyvfs_free(pOpenedFile);
    }
}

int easyvfs_readfile_mtl(easyvfs_file* pFile, void* dst, unsigned int bytesToRead, unsigned int* bytesReadOut)
{
    assert(pFile != 0);
    assert(dst != NULL);
    assert(bytesToRead > 0);

    easyvfs_openedfile_mtl* pOpenedFile = pFile->pUserData;
    if (pOpenedFile != NULL)
    {
        easyvfs_file* pArchiveFile = pFile->pArchive->pFile;
        assert(pArchiveFile != NULL);

        if (pOpenedFile->readPointer + bytesToRead < pOpenedFile->sizeInBytes)
        {
            easyvfs_seekfile(pArchiveFile, pOpenedFile->offsetInArchive + pOpenedFile->readPointer, easyvfs_start);
            int result = easyvfs_readfile(pArchiveFile, dst, bytesToRead, bytesReadOut);
            if (result != 0)
            {
                pOpenedFile->readPointer += bytesToRead;
            }

            return result;
        }
        else
        {
            // Trying to read too much.
            return 0;
        }
    }

    return 0;
}

int easyvfs_writefile_mtl(easyvfs_file* pFile, const void* src, unsigned int bytesToWrite, unsigned int* bytesWrittenOut)
{
    assert(pFile != 0);
    assert(src != NULL);
    assert(bytesToWrite > 0);

    // All files are read-only for now.
    if (bytesWrittenOut != NULL)
    {
        *bytesWrittenOut = 0;
    }

    return 0;
}

easyvfs_bool easyvfs_seekfile_mtl(easyvfs_file* pFile, easyvfs_int64 bytesToSeek, easyvfs_seekorigin origin)
{
    assert(pFile != 0);

    easyvfs_openedfile_mtl* pOpenedFile = pFile->pUserData;
    if (pOpenedFile != NULL)
    {
        easyvfs_int64 newPos = pOpenedFile->readPointer;
        if (origin == easyvfs_current)
        {
            newPos += bytesToSeek;
        }
        else if (origin == easyvfs_start)
        {
            newPos = bytesToSeek;
        }
        else if (origin == easyvfs_end)
        {
            newPos = pOpenedFile->sizeInBytes - bytesToSeek;
        }
        else
        {
            // Should never get here.
            return 0;
        }


        if (newPos < 0 || newPos > pOpenedFile->sizeInBytes)
        {
            return 0;
        }

        pOpenedFile->readPointer = newPos;
        return 1;
    }

    return 0;
}

easyvfs_int64 easyvfs_tellfile_mtl(easyvfs_file* pFile)
{
    assert(pFile != 0);

    easyvfs_openedfile_mtl* pOpenedFile = pFile->pUserData;
    if (pOpenedFile != NULL)
    {
        return pOpenedFile->readPointer;
    }

    return 0;
}

easyvfs_int64 easyvfs_filesize_mtl(easyvfs_file* pFile)
{
    assert(pFile != 0);

    easyvfs_openedfile_mtl* pOpenedFile = pFile->pUserData;
    if (pOpenedFile != NULL)
    {
        return pOpenedFile->sizeInBytes;
    }

    return 0;
}

int easyvfs_deletefile_mtl(easyvfs_archive* pArchive, const char* path)
{
    assert(pArchive != 0);
    assert(path     != 0);

    // No support for this at the moment because it's read-only for now.
    return 0;
}

int easyvfs_renamefile_mtl(easyvfs_archive* pArchive, const char* pathOld, const char* pathNew)
{
    assert(pArchive != 0);
    assert(pathOld  != 0);
    assert(pathNew  != 0);

    // No support for this at the moment because it's read-only for now.
    return 0;
}

int easyvfs_mkdir_mtl(easyvfs_archive* pArchive, const char* path)
{
    assert(pArchive != 0);
    assert(path     != 0);

    // MTL archives do not have the notion of folders.
    return 0;
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