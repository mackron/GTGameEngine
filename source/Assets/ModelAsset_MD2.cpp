// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "ModelAsset_MD2.hpp"

#if defined(GT_BUILD_MD2)
#include <GTGameEngine/FileSystem.hpp>
#include "../external/easy_path/easy_path.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    struct md2_header
    {
        int32_t ident;
        int32_t version;

        int32_t skinwidth;
        int32_t skinheight;
        int32_t framesize;

        int32_t num_skins;
        int32_t num_xyz;
        int32_t num_st;
        int32_t num_tris;
        int32_t num_glcmds;
        int32_t num_frames;

        int32_t ofs_skins;
        int32_t ofs_st;
        int32_t ofs_tris;
        int32_t ofs_frames;
        int32_t ofs_glcmds;
        int32_t ofs_end;
    };

    struct md2_vertex
    {
        unsigned char v[3];
        unsigned char lightNormalIndex;
    };

    struct md2_texcoord
    {
        short s;
        short t;
    };

    struct md2_triangle_indices
    {
        short positionIndex[3];
        short texcoordIndex[3];
    };

    struct md2_frame_header
    {
        float scale[3];
        float translate[3];
        char  name[16];
    };

    struct md2_index_pair
    {
        uint32_t index;

        md2_index_pair(short position, short texcoord)
            : index(static_cast<uint32_t>(position) << 16 | static_cast<uint32_t>(texcoord))
        {
        }

        bool operator==(const md2_index_pair &other) const
        {
            return this->index == other.index;
        }
    };

    static const float g_Normals[162][3] =
    {
        { -0.525731f,  0.000000f,  0.850651f }, 
        { -0.442863f,  0.238856f,  0.864188f }, 
        { -0.295242f,  0.000000f,  0.955423f }, 
        { -0.309017f,  0.500000f,  0.809017f }, 
        { -0.162460f,  0.262866f,  0.951056f }, 
        {  0.000000f,  0.000000f,  1.000000f }, 
        {  0.000000f,  0.850651f,  0.525731f }, 
        { -0.147621f,  0.716567f,  0.681718f }, 
        {  0.147621f,  0.716567f,  0.681718f }, 
        {  0.000000f,  0.525731f,  0.850651f }, 
        {  0.309017f,  0.500000f,  0.809017f }, 
        {  0.525731f,  0.000000f,  0.850651f }, 
        {  0.295242f,  0.000000f,  0.955423f }, 
        {  0.442863f,  0.238856f,  0.864188f }, 
        {  0.162460f,  0.262866f,  0.951056f }, 
        { -0.681718f,  0.147621f,  0.716567f }, 
        { -0.809017f,  0.309017f,  0.500000f }, 
        { -0.587785f,  0.425325f,  0.688191f }, 
        { -0.850651f,  0.525731f,  0.000000f }, 
        { -0.864188f,  0.442863f,  0.238856f }, 
        { -0.716567f,  0.681718f,  0.147621f }, 
        { -0.688191f,  0.587785f,  0.425325f }, 
        { -0.500000f,  0.809017f,  0.309017f }, 
        { -0.238856f,  0.864188f,  0.442863f }, 
        { -0.425325f,  0.688191f,  0.587785f }, 
        { -0.716567f,  0.681718f, -0.147621f }, 
        { -0.500000f,  0.809017f, -0.309017f }, 
        { -0.525731f,  0.850651f,  0.000000f }, 
        {  0.000000f,  0.850651f, -0.525731f }, 
        { -0.238856f,  0.864188f, -0.442863f }, 
        {  0.000000f,  0.955423f, -0.295242f }, 
        { -0.262866f,  0.951056f, -0.162460f }, 
        {  0.000000f,  1.000000f,  0.000000f }, 
        {  0.000000f,  0.955423f,  0.295242f }, 
        { -0.262866f,  0.951056f,  0.162460f }, 
        {  0.238856f,  0.864188f,  0.442863f }, 
        {  0.262866f,  0.951056f,  0.162460f }, 
        {  0.500000f,  0.809017f,  0.309017f }, 
        {  0.238856f,  0.864188f, -0.442863f }, 
        {  0.262866f,  0.951056f, -0.162460f }, 
        {  0.500000f,  0.809017f, -0.309017f }, 
        {  0.850651f,  0.525731f,  0.000000f }, 
        {  0.716567f,  0.681718f,  0.147621f }, 
        {  0.716567f,  0.681718f, -0.147621f }, 
        {  0.525731f,  0.850651f,  0.000000f }, 
        {  0.425325f,  0.688191f,  0.587785f }, 
        {  0.864188f,  0.442863f,  0.238856f }, 
        {  0.688191f,  0.587785f,  0.425325f }, 
        {  0.809017f,  0.309017f,  0.500000f }, 
        {  0.681718f,  0.147621f,  0.716567f }, 
        {  0.587785f,  0.425325f,  0.688191f }, 
        {  0.955423f,  0.295242f,  0.000000f }, 
        {  1.000000f,  0.000000f,  0.000000f }, 
        {  0.951056f,  0.162460f,  0.262866f }, 
        {  0.850651f, -0.525731f,  0.000000f }, 
        {  0.955423f, -0.295242f,  0.000000f }, 
        {  0.864188f, -0.442863f,  0.238856f }, 
        {  0.951056f, -0.162460f,  0.262866f }, 
        {  0.809017f, -0.309017f,  0.500000f }, 
        {  0.681718f, -0.147621f,  0.716567f }, 
        {  0.850651f,  0.000000f,  0.525731f }, 
        {  0.864188f,  0.442863f, -0.238856f }, 
        {  0.809017f,  0.309017f, -0.500000f }, 
        {  0.951056f,  0.162460f, -0.262866f }, 
        {  0.525731f,  0.000000f, -0.850651f }, 
        {  0.681718f,  0.147621f, -0.716567f }, 
        {  0.681718f, -0.147621f, -0.716567f }, 
        {  0.850651f,  0.000000f, -0.525731f }, 
        {  0.809017f, -0.309017f, -0.500000f }, 
        {  0.864188f, -0.442863f, -0.238856f }, 
        {  0.951056f, -0.162460f, -0.262866f }, 
        {  0.147621f,  0.716567f, -0.681718f }, 
        {  0.309017f,  0.500000f, -0.809017f }, 
        {  0.425325f,  0.688191f, -0.587785f }, 
        {  0.442863f,  0.238856f, -0.864188f }, 
        {  0.587785f,  0.425325f, -0.688191f }, 
        {  0.688191f,  0.587785f, -0.425325f }, 
        { -0.147621f,  0.716567f, -0.681718f }, 
        { -0.309017f,  0.500000f, -0.809017f }, 
        {  0.000000f,  0.525731f, -0.850651f }, 
        { -0.525731f,  0.000000f, -0.850651f }, 
        { -0.442863f,  0.238856f, -0.864188f }, 
        { -0.295242f,  0.000000f, -0.955423f }, 
        { -0.162460f,  0.262866f, -0.951056f }, 
        {  0.000000f,  0.000000f, -1.000000f }, 
        {  0.295242f,  0.000000f, -0.955423f }, 
        {  0.162460f,  0.262866f, -0.951056f }, 
        { -0.442863f, -0.238856f, -0.864188f }, 
        { -0.309017f, -0.500000f, -0.809017f }, 
        { -0.162460f, -0.262866f, -0.951056f }, 
        {  0.000000f, -0.850651f, -0.525731f }, 
        { -0.147621f, -0.716567f, -0.681718f }, 
        {  0.147621f, -0.716567f, -0.681718f }, 
        {  0.000000f, -0.525731f, -0.850651f }, 
        {  0.309017f, -0.500000f, -0.809017f }, 
        {  0.442863f, -0.238856f, -0.864188f }, 
        {  0.162460f, -0.262866f, -0.951056f }, 
        {  0.238856f, -0.864188f, -0.442863f }, 
        {  0.500000f, -0.809017f, -0.309017f }, 
        {  0.425325f, -0.688191f, -0.587785f }, 
        {  0.716567f, -0.681718f, -0.147621f }, 
        {  0.688191f, -0.587785f, -0.425325f }, 
        {  0.587785f, -0.425325f, -0.688191f }, 
        {  0.000000f, -0.955423f, -0.295242f }, 
        {  0.000000f, -1.000000f,  0.000000f }, 
        {  0.262866f, -0.951056f, -0.162460f }, 
        {  0.000000f, -0.850651f,  0.525731f }, 
        {  0.000000f, -0.955423f,  0.295242f }, 
        {  0.238856f, -0.864188f,  0.442863f }, 
        {  0.262866f, -0.951056f,  0.162460f }, 
        {  0.500000f, -0.809017f,  0.309017f }, 
        {  0.716567f, -0.681718f,  0.147621f }, 
        {  0.525731f, -0.850651f,  0.000000f }, 
        { -0.238856f, -0.864188f, -0.442863f }, 
        { -0.500000f, -0.809017f, -0.309017f }, 
        { -0.262866f, -0.951056f, -0.162460f }, 
        { -0.850651f, -0.525731f,  0.000000f }, 
        { -0.716567f, -0.681718f, -0.147621f }, 
        { -0.716567f, -0.681718f,  0.147621f }, 
        { -0.525731f, -0.850651f,  0.000000f }, 
        { -0.500000f, -0.809017f,  0.309017f }, 
        { -0.238856f, -0.864188f,  0.442863f }, 
        { -0.262866f, -0.951056f,  0.162460f }, 
        { -0.864188f, -0.442863f,  0.238856f }, 
        { -0.809017f, -0.309017f,  0.500000f }, 
        { -0.688191f, -0.587785f,  0.425325f }, 
        { -0.681718f, -0.147621f,  0.716567f }, 
        { -0.442863f, -0.238856f,  0.864188f }, 
        { -0.587785f, -0.425325f,  0.688191f }, 
        { -0.309017f, -0.500000f,  0.809017f }, 
        { -0.147621f, -0.716567f,  0.681718f }, 
        { -0.425325f, -0.688191f,  0.587785f }, 
        { -0.162460f, -0.262866f,  0.951056f }, 
        {  0.442863f, -0.238856f,  0.864188f }, 
        {  0.162460f, -0.262866f,  0.951056f }, 
        {  0.309017f, -0.500000f,  0.809017f }, 
        {  0.147621f, -0.716567f,  0.681718f }, 
        {  0.000000f, -0.525731f,  0.850651f }, 
        {  0.425325f, -0.688191f,  0.587785f }, 
        {  0.587785f, -0.425325f,  0.688191f }, 
        {  0.688191f, -0.587785f,  0.425325f }, 
        { -0.955423f,  0.295242f,  0.000000f }, 
        { -0.951056f,  0.162460f,  0.262866f }, 
        { -1.000000f,  0.000000f,  0.000000f }, 
        { -0.850651f,  0.000000f,  0.525731f }, 
        { -0.955423f, -0.295242f,  0.000000f }, 
        { -0.951056f, -0.162460f,  0.262866f }, 
        { -0.864188f,  0.442863f, -0.238856f }, 
        { -0.951056f,  0.162460f, -0.262866f }, 
        { -0.809017f,  0.309017f, -0.500000f }, 
        { -0.864188f, -0.442863f, -0.238856f }, 
        { -0.951056f, -0.162460f, -0.262866f }, 
        { -0.809017f, -0.309017f, -0.500000f }, 
        { -0.681718f,  0.147621f, -0.716567f }, 
        { -0.681718f, -0.147621f, -0.716567f }, 
        { -0.850651f,  0.000000f, -0.525731f }, 
        { -0.688191f,  0.587785f, -0.425325f }, 
        { -0.587785f,  0.425325f, -0.688191f }, 
        { -0.425325f,  0.688191f, -0.587785f }, 
        { -0.425325f, -0.688191f, -0.587785f }, 
        { -0.587785f, -0.425325f, -0.688191f }, 
        { -0.688191f, -0.587785f, -0.425325f }
    };




    ModelAsset_MD2::ModelAsset_MD2(const char* absolutePathOrIdentifier, AssetType assetType)
        : ModelAsset(absolutePathOrIdentifier, assetType),
          m_vertexData(nullptr), m_vertexCount(0), m_vertexCountPerFrame(0), m_vertexLayout(),
          m_indexData(nullptr), m_indexCount(0),
          m_materialOffsetCountPair(),
          m_keyframeCount(0)
    {
        m_vertexLayout[0] = {VertexAttribFormat_Float, 3, sizeof(float) * 0, VertexAttribSemantic_Position};
        m_vertexLayout[1] = {VertexAttribFormat_Float, 2, sizeof(float) * 3, VertexAttribSemantic_TexCoord};
        m_vertexLayout[2] = {VertexAttribFormat_Float, 3, sizeof(float) * 5, VertexAttribSemantic_Normal  };

        m_materialOffsetCountPair[0] = 0;
        m_materialOffsetCountPair[1] = 0;
    }

    ModelAsset_MD2::~ModelAsset_MD2()
    {
        delete [] m_vertexData;
        delete [] m_indexData;
    }


    bool ModelAsset_MD2::Load(const char* absolutePath, GT::FileSystem &fileSystem)
    {
        HFile hFile = fileSystem.OpenFile(absolutePath, FileAccessMode::Read);
        if (hFile != 0)
        {
            unsigned int fileSize = static_cast<unsigned int>(fileSystem.GetFileSize(hFile));
            if (fileSize > sizeof(md2_header))
            {
                bool result = true;

                uint8_t* fileData = reinterpret_cast<uint8_t*>(malloc(fileSize));
                fileSystem.ReadFile(hFile, fileSize, fileData);
                fileSystem.CloseFile(hFile);

                md2_header* header = reinterpret_cast<md2_header*>(fileData);
                if (header->ident == '2PDI' && header->version == 8)
                {
                    if (fileSize > static_cast<unsigned int>(header->ofs_glcmds))
                    {
                        auto md2TexCoords = reinterpret_cast<md2_texcoord*        >(fileData + header->ofs_st);
                        auto md2Indices   = reinterpret_cast<md2_triangle_indices*>(fileData + header->ofs_tris);

                        // MD2 separates the position/texcoord/normal. We want them to be interlaced. To do this, we need to convert the position/texcoord index pair
                        // from MD2 into just a singular index. We do this by simply counting the number of unique combinations of index pairs.
                        GTLib::Vector<md2_index_pair> uniqueIndexPairs(header->num_tris);
                        GTLib::Vector<uint32_t> actualIndices(header->num_tris);

                        for (int32_t iIndex = 0; iIndex < header->num_tris; ++iIndex)
                        {
                            for (int iCorner = 0; iCorner < 3; ++iCorner)
                            {
                                md2_index_pair pair(md2Indices[iIndex].positionIndex[iCorner], md2Indices[iIndex].texcoordIndex[iCorner]);

                                size_t existingIndexLocation;
                                if (uniqueIndexPairs.FindFirstIndexOf(pair, existingIndexLocation))
                                {
                                    actualIndices.PushBack(static_cast<uint32_t>(existingIndexLocation));
                                }
                                else
                                {
                                    actualIndices.PushBack(static_cast<uint32_t>(uniqueIndexPairs.GetCount()));
                                    uniqueIndexPairs.PushBack(pair);
                                }
                            }
                        }

                        m_vertexCountPerFrame = static_cast<unsigned int>(uniqueIndexPairs.GetCount());
                        m_vertexCount         = static_cast<unsigned int>(m_vertexCountPerFrame * header->num_frames);
                        m_vertexData          = new MD2MeshVertex[m_vertexCount];

                        m_indexCount = static_cast<unsigned int>(actualIndices.GetCount());
                        m_indexData  = new uint32_t[m_indexCount];
                        memcpy(m_indexData, actualIndices.buffer, m_indexCount * sizeof(uint32_t));

                        m_materialOffsetCountPair[0] = 0;
                        m_materialOffsetCountPair[1] = static_cast<uint32_t>(m_indexCount);


                        // Frames
                        for (int32_t iTriangle = 0; iTriangle < header->num_tris; ++iTriangle)
                        {
                            for (int iCorner = 0; iCorner < 3; ++iCorner)
                            {
                                short positionIndexMD2 = md2Indices[iTriangle].positionIndex[iCorner];
                                short texcoordIndexMD2 = md2Indices[iTriangle].texcoordIndex[iCorner];

                                size_t actualIndexLocation;
                                if (uniqueIndexPairs.FindFirstIndexOf(md2_index_pair(positionIndexMD2, texcoordIndexMD2), actualIndexLocation))
                                {
                                    for (int32_t iFrame = 0; iFrame < header->num_frames; ++iFrame)
                                    {
                                        auto frameHeader = reinterpret_cast<md2_frame_header*>((fileData + header->ofs_frames) + (iFrame * header->framesize));
                                        auto md2Vertices = reinterpret_cast<md2_vertex*      >((fileData + header->ofs_frames) + (iFrame * header->framesize) + sizeof(md2_frame_header));

                                        float position[3];
                                        position[0] = (md2Vertices[positionIndexMD2].v[0] * frameHeader->scale[0]) + frameHeader->translate[0];
                                        position[1] = (md2Vertices[positionIndexMD2].v[1] * frameHeader->scale[1]) + frameHeader->translate[1];
                                        position[2] = (md2Vertices[positionIndexMD2].v[2] * frameHeader->scale[2]) + frameHeader->translate[2];

                                        float texcoord[2];
                                        texcoord[0] = static_cast<float>(md2TexCoords[texcoordIndexMD2].s) / header->skinwidth;
                                        texcoord[1] = static_cast<float>(md2TexCoords[texcoordIndexMD2].t) / header->skinheight;

                                        float normal[3];
                                        normal[0] = g_Normals[md2Vertices[positionIndexMD2].lightNormalIndex][0];
                                        normal[1] = g_Normals[md2Vertices[positionIndexMD2].lightNormalIndex][1];
                                        normal[2] = g_Normals[md2Vertices[positionIndexMD2].lightNormalIndex][2];


                                        size_t vertexDataLocation = (iFrame * m_vertexCountPerFrame) + actualIndexLocation;

                                        m_vertexData[vertexDataLocation].position[0] = position[0];
                                        m_vertexData[vertexDataLocation].position[1] = position[1];
                                        m_vertexData[vertexDataLocation].position[2] = position[2];

                                        m_vertexData[vertexDataLocation].texcoord[0] = texcoord[0];
                                        m_vertexData[vertexDataLocation].texcoord[1] = texcoord[1];

                                        m_vertexData[vertexDataLocation].normal[0] = normal[0];
                                        m_vertexData[vertexDataLocation].normal[1] = normal[1];
                                        m_vertexData[vertexDataLocation].normal[2] = normal[2];
                                    }
                                }
                                else
                                {
                                    // Should never hit this.
                                    assert(false);
                                }
                            }
                        }


                        m_keyframeCount = static_cast<unsigned int>(header->num_frames);
                        result = true;
                    }
                    else
                    {
                        // File is too small.
                        result = false;
                    }
                }
                else
                {
                    // Magic number or version is incorrect.
                    result = false;
                }
                

                free(fileData);
                return result;
            }
            else
            {
                // The file size is incorrect.
                fileSystem.CloseFile(hFile);
                return false;
            }
        }
        else
        {
            return false;
        }
    }


    unsigned int ModelAsset_MD2::GetMeshCount() const
    {
        return 1;
    }


    ////////////////////////////////////////
    // Vertex Data

    const void* ModelAsset_MD2::GetMeshVertexData(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_vertexData;
        }
    }

    size_t ModelAsset_MD2::GetMeshVertexDataSize(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_vertexCount * sizeof(MD2MeshVertex);
        }
    }

    size_t ModelAsset_MD2::GetMeshVertexStride(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return sizeof(MD2MeshVertex);
        }
    }

    const VertexAttribLayout* ModelAsset_MD2::GetMeshVertexLayout(unsigned int meshIndex, unsigned int &attribCountOut) const
    {
        assert(meshIndex == 0);
        {
            attribCountOut = 3;
            return m_vertexLayout;
        }
    }


    ////////////////////////////////////////
    // Index Data

    const void* ModelAsset_MD2::GetMeshIndexData(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_indexData;
        }
    }

    size_t ModelAsset_MD2::GetMeshIndexDataSize(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_indexCount * sizeof(uint32_t);
        }
    }

    unsigned int ModelAsset_MD2::GetMeshIndexCount(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_indexCount;
        }
    }

    IndexFormat ModelAsset_MD2::GetMeshIndexFormat(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return IndexFormat::IndexFormat_UInt32;
        }
    }


    ////////////////////////////////////////
    // Material Data

    unsigned int ModelAsset_MD2::GetMeshMaterialCount(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return 1;
        }
    }

    const uint32_t* ModelAsset_MD2::GetMeshMaterialIndexOffsetCountPairs(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_materialOffsetCountPair;
        }
    }

    void ModelAsset_MD2::GetMeshMaterialName(unsigned int meshIndex, unsigned int materialIndex, char* materialNameOut, unsigned int materialNameSizeInBytes) const
    {
        assert(meshIndex == 0);
        assert(materialIndex == 0);
        
        if (materialNameOut != nullptr && materialNameSizeInBytes > 0)
        {
            easypath_strcpy(materialNameOut, materialNameSizeInBytes, "default");
        }
    }


    ////////////////////////////////////////
    // Animation Data

    MeshAnimationType ModelAsset_MD2::GetMeshAnimationType(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            if (m_keyframeCount > 1)
            {
                return MeshAnimationType::Keyframe;
            }
            else
            {
                return MeshAnimationType::None;
            }
        }
    }

    unsigned int ModelAsset_MD2::GetMeshVertexCountPerKeyframe(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_vertexCountPerFrame;
        }
    }

    unsigned int ModelAsset_MD2::GetMeshTotalKeyframeCount(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_keyframeCount;
        }
    }

    unsigned int ModelAsset_MD2::GetMeshAnimationSegmentCount(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return 0;
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

#endif
