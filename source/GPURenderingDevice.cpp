// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GPURenderingDevice.hpp>
#include <GTLib/Strings.hpp>
#include <GTLib/BufferReader.hpp>
#include <GTLib/Math.hpp>

namespace GT
{
    GPURenderingDevice::GPURenderingDevice(const GPURenderingDeviceInfo &info)
        : GraphicsInterface(GraphicsInterfaceType_Unknown), m_info(info)
    {
    }

    GPURenderingDevice::~GPURenderingDevice()
    {
    }


    void GPURenderingDevice::GetInfo(GPURenderingDeviceInfo &infoOut) const
    {
        infoOut = m_info;
    }



    ////////////////////////////////////////////////////
    // Protected

    ResultCode GPURenderingDevice::CreateShaderBinaryData(const char* source, size_t sourceLength, const ShaderDefine* defines, ShaderLanguage language, ShaderType type, const void* binary, size_t binarySizeInBytes, int binaryVersion, GT::BasicBuffer &byteCodeOut)
    {
        // Current Version: 1
        assert(language <= 255);        // Saved as a 1-byte value, so must be less than 255.
        assert(type     <= 255);        // Saved as a 1-byte value, so must be less than 255.

        if (source != nullptr)
        {
            if (sourceLength == 0)
            {
                sourceLength = GTLib::Strings::SizeInBytes(source);
            }


            // The first step is to calculate the size of the buffer so that we can do only a single allocation.
            size_t bufferSizeOut = 0;
            bufferSizeOut += 4;     // Version
            bufferSizeOut += 1;     // Type
            bufferSizeOut += 1;     // Language
            bufferSizeOut += 2;     // Defines count
                
            uint16_t definesCount = 0;
            if (defines != nullptr)
            {
                int iDefine = 0;
                while (defines[iDefine].name != nullptr)
                {
                    definesCount += 1;

                    unsigned int nameSizeInBytes  = static_cast<unsigned int>(GTLib::Strings::SizeInBytes(defines[iDefine].name))  + 1;       // +1 to include room for the null-terminator.
                    unsigned int valueSizeInBytes = static_cast<unsigned int>(GTLib::Strings::SizeInBytes(defines[iDefine].value)) + 1;       // +1 to include room for the null-terminator.

                    unsigned int namePadding  = GTLib::RoundUpToMultipleOf8(nameSizeInBytes) - nameSizeInBytes;
                    unsigned int valuePadding = GTLib::RoundUpToMultipleOf8(valueSizeInBytes) - valueSizeInBytes;

                    assert(namePadding  < 8);
                    assert(valuePadding < 8);


                    bufferSizeOut += nameSizeInBytes + namePadding;
                    bufferSizeOut += valueSizeInBytes + valuePadding;
                }
            }

            bufferSizeOut += sourceLength + 1;          // +1 to include room for the null-terminator.

            unsigned int sourcePadding = GTLib::RoundUpToMultipleOf8(static_cast<unsigned int>(sourceLength) + 1) - (static_cast<unsigned int>(sourceLength) + 1);
            bufferSizeOut += sourcePadding;


            bufferSizeOut += 4;     // Shader binary version.
            bufferSizeOut += 4;     // Shader binary size in bytes.
                
            if (binary != nullptr)
            {
                bufferSizeOut += binarySizeInBytes;
            }



            // The next step is to copy over the relevant data.
            uint8_t* bufferOut = reinterpret_cast<uint8_t*>(byteCodeOut.Allocate(bufferSizeOut));
            if (bufferOut != nullptr)
            {
                // Version.
                *reinterpret_cast<uint32_t*>(bufferOut) = 1;
                bufferOut += 4;
                    
                // Type.
                *bufferOut = static_cast<uint8_t>(type);
                bufferOut += 1;

                // Language.
                *bufferOut = static_cast<uint8_t>(language);
                bufferOut += 1;

                // Defines count.
                *reinterpret_cast<uint16_t*>(bufferOut) = definesCount;
                bufferOut += 2;


                // Defines
                for (uint16_t iDefine = 0; iDefine < definesCount; ++iDefine)
                {
                    unsigned int nameLength  = static_cast<unsigned int>(GTLib::Strings::SizeInBytes(defines[iDefine].name))  + 1;   // +1 for the null terminator.
                    unsigned int valueLength = static_cast<unsigned int>(GTLib::Strings::SizeInBytes(defines[iDefine].value)) + 1;   // +1 for the null terminator.

                    unsigned int namePadding  = GTLib::RoundUpToMultipleOf8(nameLength) - nameLength;
                    unsigned int valuePadding = GTLib::RoundUpToMultipleOf8(valueLength) - valueLength;

                    assert(namePadding  < 8);
                    assert(valuePadding < 8);


                    memcpy(bufferOut, defines[iDefine].name, nameLength);
                    bufferOut += nameLength;

                    memset(bufferOut, 0, namePadding);
                    bufferOut += namePadding;


                    memcpy(bufferOut, defines[iDefine].value, valueLength);
                    bufferOut += valueLength;

                    memset(bufferOut, 0, valuePadding);
                    bufferOut += valuePadding;
                }


                // Source code.
                memcpy(bufferOut, source, sourceLength);
                bufferOut += sourceLength;

                *bufferOut = '\0';      // Null-terminator for source code.
                bufferOut += 1;

                memset(bufferOut, 0, sourcePadding);
                bufferOut += sourcePadding;
                    



                // Binary code version.
                *reinterpret_cast<int32_t*>(bufferOut) = binaryVersion;
                bufferOut += 4;

                // Binary code size in bytes.
                *reinterpret_cast<uint32_t*>(bufferOut) = static_cast<uint32_t>(binarySizeInBytes);
                bufferOut += 4;

                // Binary code.
                if (binary != nullptr && binarySizeInBytes != 0)
                {
                    memcpy(bufferOut, binary, binarySizeInBytes);
                    bufferOut += binarySizeInBytes;
                }


                return 0;   // No error.
            }
            else
            {
                // Error allocating memory. May be out of memory.
                return -2;
            }
        }

        return -1;
    }

    ResultCode GPURenderingDevice::ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const char* &sourceOut, size_t &sourceLengthOut, GTLib::Vector<ShaderDefine> &definesOut, ShaderLanguage &languageOut, ShaderType &typeOut, const void* &binaryOut, size_t &binarySizeOut, int &binaryVersionOut)
    {
        if (shaderData != nullptr)
        {
            GT::BufferReader reader(shaderData, shaderDataSize);

            uint64_t version;
            if (reader.Read(version) != sizeof(uint64_t))
            {
                // Invalid data.
                return -2;
            }


            if ((version & 0x00000000FFFFFFFF) == 1)
            {
                languageOut = static_cast<ShaderLanguage>((version & 0x0000FF0000000000) >> 40);
                typeOut     = static_cast<ShaderType    >((version & 0x000000FF00000000) >> 32);

                uint16_t definesCount = static_cast<uint16_t>((version & 0xFFFF000000000000) >> 48);
                for (uint16_t iDefine = 0; iDefine < definesCount; ++iDefine)
                {
                    ShaderDefine define;


                    // Name.
                    size_t m_nameStart = reader.Tell();
                    size_t m_nameEnd;
                    if (reader.FindNextByte('\0', m_nameEnd))
                    {
                        define.name = reinterpret_cast<const char*>(shaderData) + m_nameStart;

                        // Seek past the name, including the padding.
                        reader.Seek(static_cast<int64_t>(GTLib::RoundUpToMultipleOf8(static_cast<unsigned int>(m_nameEnd - m_nameStart) + 1)));
                    }
                    else
                    {
                        // Could not find the null-terminator.
                        return -4;
                    }


                    // Value.
                    size_t m_valueStart = reader.Tell();
                    size_t m_valueEnd;
                    if (reader.FindNextByte('\0', m_valueEnd))
                    {
                        define.value = reinterpret_cast<const char*>(shaderData) + m_valueStart;

                        // Seek past the value, including the padding.
                        reader.Seek(static_cast<int64_t>(GTLib::RoundUpToMultipleOf8(static_cast<unsigned int>(m_nameEnd - m_nameStart) + 1)));
                    }
                    else
                    {
                        // Could not find the null-terminator.
                        return -4;
                    }


                    definesOut.PushBack(define);
                }

                ShaderDefine defineNullTerminator = { nullptr, nullptr };
                definesOut.PushBack(defineNullTerminator);



                size_t m_sourceStart = reader.Tell();
                size_t m_sourceEnd;
                if (reader.FindNextByte('\0', m_sourceEnd))
                {
                    sourceOut       = reinterpret_cast<const char*>(shaderData) + m_sourceStart;
                    sourceLengthOut = m_sourceEnd - m_sourceStart;

                    // Seek past the source, including the padding.
                    reader.Seek(static_cast<int64_t>(GTLib::RoundUpToMultipleOf8(static_cast<unsigned int>(sourceLengthOut) + 1)));
                }
                else
                {
                    // Could not find the null-terminator.
                    return -4;
                }



                // Binary code version and size in bytes.
                uint64_t binaryVersionAndSize;
                if (reader.Read(binaryVersionAndSize) == sizeof(uint64_t))
                {
                    binaryVersionOut = (binaryVersionAndSize & 0x00000000FFFFFFFF);
                    binarySizeOut    = (binaryVersionAndSize >> 32);

                    binaryOut = reinterpret_cast<const char*>(shaderData) + reader.Tell();

                    if (reader.Seek(binarySizeOut) == static_cast<int64_t>(binarySizeOut))
                    {
                        return 0;
                    }
                    else
                    {
                        // Buffer is too short.
                        return -4;
                    }
                }
                else
                {
                    // Buffer is too short.
                    return -4;
                }
            }
            else
            {
                // Unsupported version.
                return -3;
            }
        }
            

        // Invalid input.
        return -1;
    }

    ResultCode GPURenderingDevice::ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const char* &sourceOut, size_t &sourceLengthOut, GTLib::Vector<ShaderDefine> &definesOut, ShaderLanguage &languageOut, ShaderType &typeOut)
    {
        const void* binary;
        size_t binarySize;
        int binaryVersion;
        return ExtractShaderBinaryData(shaderData, shaderDataSize, sourceOut, sourceLengthOut, definesOut, languageOut, typeOut, binary, binarySize, binaryVersion);
    }

    ResultCode GPURenderingDevice::ExtractShaderBinaryData(const void* shaderData, size_t shaderDataSize, const void* &binaryOut, size_t &binarySizeOut)
    {
        const char* sourceOut;
        size_t sourceLengthOut;
        GTLib::Vector<ShaderDefine> definesOut;
        ShaderLanguage languageOut;
        ShaderType typeOut;
        int binaryVersionOut;
        return ExtractShaderBinaryData(shaderData, shaderDataSize, sourceOut, sourceLengthOut, definesOut, languageOut, typeOut, binaryOut, binarySizeOut, binaryVersionOut);
    }

    bool GPURenderingDevice::IsInputLayoutValid(const VSInputAttribFormat* attribDesc, size_t attribDescCount)
    {
        for (size_t iAttrib = 0; iAttrib < attribDescCount; ++iAttrib)
        {
            auto &attrib = attribDesc[iAttrib];

            if (attrib.attributeComponentCount == 3)
            {
                if (attrib.attributeComponentType == VertexAttribFormat_Int16 || attrib.attributeComponentType == VertexAttribFormat_Int16 ||
                    attrib.attributeComponentType == VertexAttribFormat_Int8  || attrib.attributeComponentType == VertexAttribFormat_Int8)
                {
                    // The vertex attribute is a 3-component, 16- or 8-bit format which is not allowed.
                    return false;
                }
            }
        }

        return true;
    }
}
