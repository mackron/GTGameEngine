// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "MaterialAsset_MTL.hpp"

#if defined(GT_BUILD_MTL)
#include <GTGameEngine/FileSystem.hpp>

namespace GT
{
    MaterialAsset_MTL::MaterialAsset_MTL(AssetType assetType)
        : MaterialAsset(assetType)
    {
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
            if (fileSize > 0)
            {
                bool result = true;

                char* fileData = reinterpret_cast<char*>(malloc(fileSize));
                fileSystem.ReadFile(hFile, fileSize, fileData);
                fileSystem.CloseFile(hFile);


                const char* str    = fileData;
                const char* strEnd = str + fileSize;

                // We just go over the file data line by line.
                while (str < strEnd)
                {
                    if (str + 1 < strEnd)
                    {
                        const uint16_t next16 = reinterpret_cast<const uint16_t*>(str)[0];

                        // "newmtl"
                        if (next16 == 'en')
                        {
                            str += 2;
                            if (str + 5 < strEnd)
                            {
                                if (reinterpret_cast<const uint32_t*>(str)[0] == 'ltmw')
                                {
                                }
                            }
                        }
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