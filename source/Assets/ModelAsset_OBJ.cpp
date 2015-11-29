// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "ModelAsset_OBJ.hpp"

#if defined(GT_BUILD_OBJ)
#include <GTEngine/Math.hpp>
#include <GTLib/Vector.hpp>
#include <easy_util/easy_util.h>

// TODO: Correctly handle "usemtl" to properly handle multiple materials.

namespace GT
{
    // The indices in this list are 1 based. A value of 0 means it was unspecified.
    struct OBJFaceVertex
    {
        int positionIndex;
        int texcoordIndex;
        int normalIndex;

        bool operator==(const OBJFaceVertex &other) const
        {
            return positionIndex == other.positionIndex && texcoordIndex == other.texcoordIndex && normalIndex == other.normalIndex;
        }
    };

    struct OBJFace
    {
        OBJFaceVertex v[3];
    };


    inline bool OBJ_IsWhitespace(char c)
    {
        return c == ' ' || c == '\t';
    }

    inline bool OBJ_IsValidDigit(char c)
    {
        return c >= '0' && c <= '9';
    }


    // Special implementation of atof() for converting a string to a float.
    float OBJ_atof(const char* str, const char* strEnd, const char** strEndOut)
    {
        // Skip leading whitespace.
        while (str < strEnd && OBJ_IsWhitespace(*str))
        {
            str += 1;
        }


        // Check that we have a string after moving the whitespace.
        if (str < strEnd)
        {
            float sign  = 1.0f;
            float value = 0.0f;

            // Sign.
            if (*str == '-')
            {
                sign = -1.0f;
                str += 1;
            }
            else if (*str == '+')
            {
                sign = 1.0f;
                str += 1;
            }


            // Digits before the decimal point.
            while (str < strEnd && OBJ_IsValidDigit(*str))
            {
                value = value * 10.0f + (*str - '0');

                str += 1;
            }

            // Digits after the decimal point.
            if (*str == '.')
            {
                float pow10 = 10.0f;

                str += 1;
                while (str < strEnd && OBJ_IsValidDigit(*str))
                {
                    value += (*str - '0') / pow10;
                    pow10 *= 10.0f;

                    str += 1;
                }
            }

            
            if (strEndOut != nullptr)
            {
                *strEndOut = str;
            }

            return sign * value;
        }
        else
        {
            // Empty string.
            return 0.0f;
        }
    }


    /// Parses a single index in a faces p/t/n string.
    ///
    /// @remarks
    ///     This function assumes the whitespace has already been trimmed.
    int OBJ_ParseFaceVertexIndex(const char* str, const char* strEnd, const char** strEndOut)
    {
        int sign  = 1;
        int value = 0;

        if (str < strEnd)
        {
            // Sign.
            if (*str == '-')
            {
                sign = -1;
                str += 1;
            }
            else if (*str == '+')
            {
                sign = 1;
                str += 1;
            }


            // Value.
            while (str < strEnd && OBJ_IsValidDigit(*str))
            {
                value = value * 10 + (*str - '0');
                str += 1;
            }
        }


        if (*strEndOut != nullptr)
        {
            *strEndOut = str;
        }
        
        return sign * value;
    }


    /// Parses a face vertex index string.
    OBJFaceVertex OBJ_ParseFaceVertex(const char* str, const char* strEnd, const char** strEndOut)
    {
        OBJFaceVertex result;
        result.positionIndex = 0;
        result.texcoordIndex = 0;
        result.normalIndex   = 0;


        // Skip leading whitespace.
        while (str < strEnd && OBJ_IsWhitespace(*str))
        {
            str += 1;
        }


        // Check that we have a string after moving the whitespace.
        if (str < strEnd)
        {
            result.positionIndex = OBJ_ParseFaceVertexIndex(str, strEnd, &str);

            str += 1;   // Move past the slash.
            result.texcoordIndex = OBJ_ParseFaceVertexIndex(str, strEnd, &str);

            str += 1;   // Move past the slash.
            result.normalIndex   = OBJ_ParseFaceVertexIndex(str, strEnd, &str);
        }
        

        if (*strEndOut != nullptr)
        {
            *strEndOut = str;
        }

        return result;
    }

    /// Parses a face index string.
    ///
    /// @remarks
    ///     The string is assumed to be in the format p0/t0/n0 p1/t1/n1 p2/t2/n2.
    void OBJ_ParseFace(const char* str, const char* strEnd, OBJFace &faceOut, const char** strEndOut)
    {
        // OBJ is counter-clockwise by default, but we want clockwise. Thus we read the indices in 2, 1, 0 order rather than 0, 1, 2.

        faceOut.v[2] = OBJ_ParseFaceVertex(str, strEnd, &str);
        faceOut.v[1] = OBJ_ParseFaceVertex(str, strEnd, &str);
        faceOut.v[0] = OBJ_ParseFaceVertex(str, strEnd, &str);

        if (*strEndOut != nullptr)
        {
            *strEndOut = str;
        }
    }



    ModelAsset_OBJ::ModelAsset_OBJ(const char* absolutePathOrIdentifier, AssetType assetType)
        : ModelAsset(absolutePathOrIdentifier, assetType),
         m_vertexData(nullptr),
         m_indexData(nullptr)
    {
        m_vertexLayout[0] = {VertexAttribFormat_Float, 3, sizeof(float) * 0, VertexAttribSemantic_Position};
        m_vertexLayout[1] = {VertexAttribFormat_Float, 2, sizeof(float) * 3, VertexAttribSemantic_TexCoord};
        m_vertexLayout[2] = {VertexAttribFormat_Float, 3, sizeof(float) * 5, VertexAttribSemantic_Normal  };

        m_materialOffsetCountPair[0] = 0;
        m_materialOffsetCountPair[1] = 0;
    }

    ModelAsset_OBJ::~ModelAsset_OBJ()
    {
        delete [] m_vertexData;
        delete [] m_indexData;
    }


    bool ModelAsset_OBJ::Load(const char* absolutePath, easyvfs_context* pVFS)
    {
        size_t fileSize;
        char* pFileData = easyvfs_open_and_read_text_file(pVFS, absolutePath, &fileSize);
        if (pFileData != 0 && fileSize > 0)
        {
            GTLib::Vector<glm::vec4>    positions;
            GTLib::Vector<glm::vec4>    texcoords;
            GTLib::Vector<glm::vec4>    normals;
            GTLib::Vector<OBJFace> faces;

            GTLib::Vector<OBJFaceVertex> uniqueIndices;
            GTLib::Vector<uint32_t>      actualIndices;

            const char* str    = pFileData;
            const char* strEnd = str + fileSize;

            // We just go over the file data line by line.
            while (str < strEnd)
            {
                if (str + 1 < strEnd)
                {
                    const uint16_t next16 = reinterpret_cast<const uint16_t*>(str)[0];

                    if (next16 == ' v' || next16 == '\tv')
                    {
                        // Position. Only supporting X, Y and Z. W and colours are ignored.
                        str += 2;

                        float value[3];
                        value[0] = OBJ_atof(str, strEnd, &str);
                        value[1] = OBJ_atof(str, strEnd, &str);
                        value[2] = OBJ_atof(str, strEnd, &str);

                        positions.PushBack(glm::vec4(value[0], value[1], value[2], 1.0f));
                    }
                    else if (next16 == 'tv')
                    {
                        // Texture Coordinate.
                        str += 3;

                        float value[2];
                        value[0] = OBJ_atof(str, strEnd, &str);
                        value[1] = OBJ_atof(str, strEnd, &str);

                        texcoords.PushBack(glm::vec4(value[0], value[1], 0.0f, 0.0f));
                    }
                    else if (next16 == 'nv')
                    {
                        // Normal.
                        str += 3;

                        float value[3];
                        value[0] = OBJ_atof(str, strEnd, &str);
                        value[1] = OBJ_atof(str, strEnd, &str);
                        value[2] = OBJ_atof(str, strEnd, &str);

                        normals.PushBack(glm::vec4(value[0], value[1], value[2], 0.0f));
                    }
                    else if (next16 == ' f' || next16 == '\tf')
                    {
                        // Face. Only supporting triangles.
                        str += 2;

                        OBJFace face;
                        OBJ_ParseFace(str, strEnd, face, &str);

                        faces.PushBack(face);

                        for (size_t iVertex = 0; iVertex < 3; ++iVertex)
                        {
                            size_t existingIndexLocation;
                            if (uniqueIndices.FindFirstIndexOf(face.v[iVertex], existingIndexLocation))
                            {
                                actualIndices.PushBack(static_cast<uint32_t>(existingIndexLocation));
                            }
                            else
                            {
                                actualIndices.PushBack(static_cast<uint32_t>(uniqueIndices.GetCount()));
                                uniqueIndices.PushBack(face.v[iVertex]);
                            }
                        }
                    }
                    else if (next16 == 'su')
                    {
                        // "usemtl"
                        str += 2;
                        if (str + 5 < strEnd)
                        {
                            if (reinterpret_cast<const uint32_t*>(str)[0] == 'ltme')
                            {

                            }
                        }
                    }
                    else
                    {
                        // We don't know what the token is, so skip over the line.
                    }
                }
                else
                {
                    // Empty line. Skip it.
                }


                // Move to the end of the line
                while (str < strEnd && *str != '\n')
                {
                    str += 1;
                }

                // Move past the new-line character which will take us to the beginning of the next line.
                str += 1;
            }

            // At this point the file has been parsed, so free the file data...
            easyvfs_free(pFileData);

            // ... and start converting the data to our own format. OBJ separates positions, texture coordinates and normals, but we want them
            // to be interlaced.
            //
            // If there is no position, texture coordinate or normal we will add a default one to the list.
            if (positions.GetCount() == 0)
            {
                positions.PushBack(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            }
            if (texcoords.GetCount() == 0)
            {
                positions.PushBack(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
            }
            if (normals.GetCount() == 0)
            {
                normals.PushBack(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
            }



            m_vertexCount = static_cast<unsigned int>(uniqueIndices.GetCount());
            m_vertexData  = new OBJMeshVertex[m_vertexCount];

            m_indexCount = static_cast<unsigned int>(actualIndices.GetCount());
            m_indexData  = new uint32_t[m_indexCount];
            memcpy(m_indexData, actualIndices.buffer, m_indexCount * sizeof(uint32_t));

            m_materialOffsetCountPair[0] = 0;
            m_materialOffsetCountPair[1] = static_cast<uint32_t>(m_indexCount);


            for (size_t iFace = 0; iFace < faces.GetCount(); ++iFace)
            {
                for (size_t iVertex = 0; iVertex < 3; ++iVertex)
                {
                    size_t actualIndexLocation;
                    if (uniqueIndices.FindFirstIndexOf(faces[iFace].v[iVertex], actualIndexLocation))
                    {
                        int positionIndexOBJ = faces[iFace].v[iVertex].positionIndex;
                        int texcoordIndexOBJ = faces[iFace].v[iVertex].texcoordIndex;
                        int normalIndexOBJ   = faces[iFace].v[iVertex].normalIndex;

                        // The indices need to be normalized. An OBJ file can have positive and negative indices. Positive indices are 1-based indices that
                        // one would typically expect. Negative indices are "relative" where all we need to do is add the vertex count to it.
                        if (positionIndexOBJ > 0) {
                            positionIndexOBJ -= 1;
                        } else {
                            positionIndexOBJ += static_cast<int>(positions.GetCount());
                        }

                        if (texcoordIndexOBJ > 0) {
                            texcoordIndexOBJ -= 1;
                        } else {
                            texcoordIndexOBJ += static_cast<int>(texcoords.GetCount());
                        }

                        if (normalIndexOBJ > 0) {
                            normalIndexOBJ -= 1;
                        } else {
                            normalIndexOBJ += static_cast<int>(normals.GetCount());
                        }


                        m_vertexData[actualIndexLocation].position[0] = positions[positionIndexOBJ][0];
                        m_vertexData[actualIndexLocation].position[1] = positions[positionIndexOBJ][1];
                        m_vertexData[actualIndexLocation].position[2] = positions[positionIndexOBJ][2];

                        m_vertexData[actualIndexLocation].texcoord[0] = texcoords[texcoordIndexOBJ][0];
                        m_vertexData[actualIndexLocation].texcoord[1] = texcoords[texcoordIndexOBJ][1];

                        m_vertexData[actualIndexLocation].normal[0]   = normals[normalIndexOBJ][0];
                        m_vertexData[actualIndexLocation].normal[1]   = normals[normalIndexOBJ][1];
                        m_vertexData[actualIndexLocation].normal[2]   = normals[normalIndexOBJ][2];
                    }
                    else
                    {
                        // Should never hit this
                        assert(false);
                    }
                }
            }

            return true;
        }
        else
        {
            return false;
        }
    }


    unsigned int ModelAsset_OBJ::GetMeshCount() const
    {
        return 1;
    }


    ////////////////////////////////////////
    // Vertex Data

    const void* ModelAsset_OBJ::GetMeshVertexData(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_vertexData;
        }
    }

    size_t ModelAsset_OBJ::GetMeshVertexDataSize(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_vertexCount * sizeof(OBJMeshVertex);
        }
    }

    size_t ModelAsset_OBJ::GetMeshVertexStride(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return sizeof(OBJMeshVertex);
        }
    }

    const VertexAttribLayout* ModelAsset_OBJ::GetMeshVertexLayout(unsigned int meshIndex, unsigned int &attribCountOut) const
    {
        assert(meshIndex == 0);
        {
            attribCountOut = 3;
            return m_vertexLayout;
        }
    }


    ////////////////////////////////////////
    // Index Data

    const void* ModelAsset_OBJ::GetMeshIndexData(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_indexData;
        }
    }

    size_t ModelAsset_OBJ::GetMeshIndexDataSize(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_indexCount * sizeof(uint32_t);
        }
    }

    unsigned int ModelAsset_OBJ::GetMeshIndexCount(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_indexCount;
        }
    }

    IndexFormat ModelAsset_OBJ::GetMeshIndexFormat(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return IndexFormat::IndexFormat_UInt32;
        }
    }


    ////////////////////////////////////////
    // Material Data

    unsigned int ModelAsset_OBJ::GetMeshMaterialCount(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return 1;
        }
    }

    const uint32_t* ModelAsset_OBJ::GetMeshMaterialIndexOffsetCountPairs(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_materialOffsetCountPair;
        }
    }

    void ModelAsset_OBJ::GetMeshMaterialName(unsigned int meshIndex, unsigned int materialIndex, char* materialNameOut, unsigned int materialNameSizeInBytes) const
    {
        (void)materialIndex;

        assert(meshIndex == 0);

        if (materialNameOut != nullptr && materialNameSizeInBytes > 0)
        {
            // TODO: Implement this properly.
            strcpy_s(materialNameOut, materialNameSizeInBytes, "default");
        }
    }


    ////////////////////////////////////////
    // Animation Data

    MeshAnimationType ModelAsset_OBJ::GetMeshAnimationType(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return MeshAnimationType::None;
        }
    }

    unsigned int ModelAsset_OBJ::GetMeshVertexCountPerKeyframe(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return m_vertexCount;
        }
    }

    unsigned int ModelAsset_OBJ::GetMeshTotalKeyframeCount(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return 1;
        }
    }

    unsigned int ModelAsset_OBJ::GetMeshAnimationSegmentCount(unsigned int meshIndex) const
    {
        assert(meshIndex == 0);
        {
            return 0;
        }
    }
}

#endif
