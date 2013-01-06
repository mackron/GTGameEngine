
# Serialized Scene Format

This document gives a very rough, high level outline of the format of a serialized
scene. This is mainly intended as a reference for future development.


## General Structure

The data is organized into chunks. Each chunk is a logical group of data. Each chunk
has two general sections - a header and a body:

Header:

* 4 bytes - Chunk ID
* 4 bytes - Chunk Version
* 4 bytes - Chunk Body Size (in bytes, not including the chunk header)

Body

* General data whose size MUST be the same as defined in the chunk header.


The idea behind the chunk system is to make it easy to have different things save
additional data that it might need. The best example is editting tools, where
things like camera positions and settings might need to be saved. This, of course,
would be useless for most other things, so the chunk system allows that section to
be skipped in that case.

To read a specific chunk, you simply skip over all chunks until you find the one
you're look for, or until you reach the end of the file/buffer.

    ChunkHeader header;
    while (deserializer.Read(header) == sizeof(ChunkHeader))
    {
        if (header.id == MyChunkID)
        {
            ReadMyChunk(deserializer, header);
            break;
        }
        else
        {
            deserializer.Seek(header.sizeInBytes);
        }
    }

There is not global header or footer - just chunks. There is no global versioning.
Instead, each chunk has it's own localised versioning.


## Engine-Defined Chunks

### Scene Nodes - Chunk ID 1

A flat list containing the serialized data of each scene node. This does not contain
hierarchy information (parent, children).


### Scene Nodes Hierarcy - Chunk ID 2

A flat list of indices used to link scene nodes with their parent, if any. For each
scene node with a parent, this has two indices - the index of the node in question
and the index of it's parent. Nodes without parents are not included in this list. 
The indices are index into the list retrieved from the "Scene Nodes" chunk.


### Editor Metadata - Chunk ID 100

Metadata for the scene editor. Contains things like the camera transformation at the
time of serialization.