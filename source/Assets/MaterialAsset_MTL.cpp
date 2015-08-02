// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "MaterialAsset_MTL.hpp"

#if defined(GT_BUILD_MTL)
#include <GTGameEngine/FileSystem.hpp>

namespace GT
{
    const char* mtl_move_to_non_whitespace(const char* str)
    {
        while (str[0] != '\0' && str[0] != '\n' && (str[0] == ' ' || str[0] == '\t'))
        {
            str += 1;
        }

        return str;
    }

    const char* mtl_move_to_whitespace_or_newline_or_comment(const char* str)
    {
        while (str[0] != '\0' && str[0] != '\n' && str[0] != ' ' && str[0] != '\t' && str[0] != '#')
        {
            str += 1;
        }

        return str;
    }

    const char* mtl_move_to_end_of_line(const char* str)
    {
        while (str[0] != '\0' && str[0] != '\n')
        {
            str += 1;
        }

        return str;
    }

    const char* mtl_move_to_next_line(const char* str)
    {
        str = mtl_move_to_end_of_line(str);

        // At this point we are either sitting on the null terminator or the new-line character. We need to move past the new-line character
        // if that's what we're sitting on.
        if (str[0] == '\n')
        {
            str += 1;
        }

        return str;
    }


    inline bool mtl_is_whitespace(char c)
    {
        return c == ' ' || c == '\t';
    }

    inline bool mtl_is_valid_digit(char c)
    {
        return c >= '0' && c <= '9';
    }

    float mtl_atof(const char* str, const char* strEnd, const char** strEndOut)
    {
        // Skip leading whitespace.
        while (str < strEnd && mtl_is_whitespace(*str))
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
            while (str < strEnd && mtl_is_valid_digit(*str))
            {
                value = value * 10.0f + (*str - '0');

                str += 1;
            }

            // Digits after the decimal point.
            if (*str == '.')
            {
                float pow10 = 10.0f;

                str += 1;
                while (str < strEnd && mtl_is_valid_digit(*str))
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

    void mtl_atof_3(const char* str, const char* strEnd, const char** strEndOut, float valueOut[3])
    {
        valueOut[0] = mtl_atof(str, strEnd, strEndOut);
        valueOut[1] = mtl_atof(str, strEnd, strEndOut);
        valueOut[2] = mtl_atof(str, strEnd, strEndOut);
    }



    MaterialAsset_MTL::MaterialAsset_MTL(AssetType assetType)
        : MaterialAsset(assetType),
          m_ambientColor(), m_diffuseColor(),  m_specularColor(),
          m_specularExponent(10),
          m_opacity(1),
          m_ambientMap(),
          m_diffuseMap(),
          m_specularMap(),
          m_specularExponentMap()
    {
        m_ambientColor[0]  = 1; m_ambientColor[1]  = 1; m_ambientColor[2]  = 1;
        m_diffuseColor[0]  = 1; m_diffuseColor[1]  = 1; m_diffuseColor[2]  = 1;
        m_specularColor[0] = 1; m_specularColor[1] = 1; m_specularColor[2] = 1;
    }

    MaterialAsset_MTL::~MaterialAsset_MTL()
    {
    }


    bool MaterialAsset_MTL::Load(const char* absolutePath, FileSystem &fileSystem)
    {
        HFile hFile = fileSystem.OpenFile(absolutePath, FileAccessMode::Read);
        if (hFile != 0)
        {
            size_t fileSize = static_cast<size_t>(fileSystem.GetFileSize(hFile));
            if (fileSize > 6)
            {
                bool result = true;

                char* fileData = reinterpret_cast<char*>(malloc(fileSize + 1));
                fileSystem.ReadFile(hFile, fileSize, fileData);
                fileSystem.CloseFile(hFile);

                fileData[fileSize] = '\0';


                const char* fileDataCur = fileData;
                const char* fileDataEnd = fileData + fileSize;

                // The virtual file system will load these materials such that the first 6 bytes should be "newmtl". If it's not, assume an invalid file.
                if (fileDataCur[0] == 'n' && fileDataCur[1] == 'e' && fileDataCur[2] == 'w' && fileDataCur[3] == 'm' && fileDataCur[4] == 't' && fileDataCur[5] == 'l')
                {
                    // The first 6 bytes is "newmtl", but the next character must be whitespace.
                    if (fileDataCur[6] == ' ' || fileDataCur[6] == '\t')
                    {
                        // It appears to be a valid file, so now we need to move to the first non-whitespace character which is the beginning of the
                        // material name. Note that we could also retrieve the file name from the file path, however that would be slower because we
                        // need to seek past this section of the file anyway.
                        fileDataCur += 7;

                        if ((fileDataEnd - fileDataCur) > 0)
                        {
                            fileDataCur = mtl_move_to_non_whitespace(fileDataCur);
                            if (fileDataCur[0] != '\0' && fileDataCur[0] != '\n')
                            {
                                fileDataCur = mtl_move_to_whitespace_or_newline_or_comment(fileDataCur);
                                
                                // At this point we should be past the name, but we are probably sitting on a new line. We now just need to loop line by line and read it based
                                // on the identifiers at the beginning of the line.
                                while (fileDataCur < fileDataEnd)
                                {
                                    fileDataCur = mtl_move_to_non_whitespace(fileDataCur);
                                    const char* endOfLine = mtl_move_to_end_of_line(fileDataCur);
                                    if ((endOfLine - fileDataCur) > 2)
                                    {
                                        if (fileDataCur[0] == 'K')
                                        {
                                            if (fileDataCur[1] == 'a')          // Ambient colour.
                                            {
                                                fileDataCur += 2;
                                                if ((endOfLine - fileDataCur) > 0)
                                                {
                                                    mtl_atof_3(fileDataCur, endOfLine, &fileDataCur, m_ambientColor);
                                                }
                                            }
                                            else if (fileDataCur[1] == 'd')     // Diffuse colour.
                                            {
                                                fileDataCur += 2;
                                                if ((endOfLine - fileDataCur) > 0)
                                                {
                                                    mtl_atof_3(fileDataCur, endOfLine, &fileDataCur, m_diffuseColor);
                                                }
                                            }
                                            else if (fileDataCur[1] == 's')     // Specular colour.
                                            {
                                                fileDataCur += 2;
                                                if ((endOfLine - fileDataCur) > 0)
                                                {
                                                    mtl_atof_3(fileDataCur, endOfLine, &fileDataCur, m_specularColor);
                                                }
                                            }
                                        }
                                        else if (fileDataCur[0] == 'N')
                                        {
                                            if (fileDataCur[1] == 's')          // Specular exponent.
                                            {
                                                fileDataCur += 2;
                                                if ((endOfLine - fileDataCur) > 0)
                                                {
                                                    m_specularExponent = mtl_atof(fileDataCur, endOfLine, &fileDataCur);
                                                }
                                            }
                                            else if (fileDataCur[1] == 'i')     // Optical density. Not used at the moment.
                                            {
                                            }
                                        }
                                        else if (fileDataCur[0] == 'T')
                                        {
                                            if (fileDataCur[1] == 'f')          // Transmission filter. Unused at the moment.
                                            {
                                            }
                                        }
                                        else if (fileDataCur[0] == 'd')         // "dissolve" (transparency). A value of 1 is opaque, 0 is fully transparent.
                                        {
                                            fileDataCur += 1;
                                            if ((endOfLine - fileDataCur) > 0)
                                            {
                                                m_opacity = mtl_atof(fileDataCur, endOfLine, &fileDataCur);
                                            }
                                        }
                                        else if (fileDataCur[0] == 'm')
                                        {
                                            if ((endOfLine - fileDataCur) > 4)
                                            {
                                                if (fileDataCur[1] == 'a' && fileDataCur[2] == 'p' && fileDataCur[3] == '_')
                                                {
                                                    fileDataCur += 4;

                                                    if (fileDataCur[0] == 'K')
                                                    {
                                                        if (fileDataCur[1] == 'a')          // Ambient map.
                                                        {
                                                            fileDataCur += 2;

                                                            const char* pathStart = mtl_move_to_non_whitespace(fileDataCur);
                                                            m_ambientMap.Assign(pathStart, endOfLine - pathStart);
                                                        }
                                                        else if (fileDataCur[1] == 'd')     // Diffuse map.
                                                        {
                                                            fileDataCur += 2;

                                                            const char* pathStart = mtl_move_to_non_whitespace(fileDataCur);
                                                            m_diffuseMap.Assign(pathStart, endOfLine - pathStart);
                                                        }
                                                        else if (fileDataCur[1] == 's')     // Specular map.
                                                        {
                                                            fileDataCur += 2;

                                                            const char* pathStart = mtl_move_to_non_whitespace(fileDataCur);
                                                            m_specularMap.Assign(pathStart, endOfLine - pathStart);
                                                        }
                                                    }
                                                    else if (fileDataCur[0] == 'N')
                                                    {
                                                        if (fileDataCur[1] == 's')          // Specular exponent map.
                                                        {
                                                            fileDataCur += 2;

                                                            const char* pathStart = mtl_move_to_non_whitespace(fileDataCur);
                                                            m_specularExponentMap.Assign(pathStart, endOfLine - pathStart);
                                                        }
                                                    }
                                                    else if (fileDataCur[0] == 'd')         // Transparency map. Not used at the moment.
                                                    {
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    fileDataCur = mtl_move_to_next_line(fileDataCur);
                                }
                            }
                        }
                    }
                }

                


                // At this point the file has been parsed, so free the file data...
                free(fileData);

                return result;
            }
            else
            {
                fileSystem.CloseFile(hFile);
            }
        }

        return false;
    }
}

#endif