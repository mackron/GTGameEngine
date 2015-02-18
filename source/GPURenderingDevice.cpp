// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/GPURenderingDevice.hpp>
#include <GTLib/Strings.hpp>

namespace GT
{
    namespace GE
    {
        GPURenderingDevice::GPURenderingDevice(const GPURenderingDeviceInfo &info)
            : m_info(info)
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

        ResultCode GPURenderingDevice::CreateBufferBinaryData(const char* source, size_t sourceLength, const GPUShaderDefine* defines, GPUShaderTarget target, const void* binary, size_t binarySizeInBytes, int binaryVersion, GT::BasicBuffer &byteCodeOut)
        {
            // Current Version: 1
            assert(target <= 65535);        // Saved as a 2-byte value, so must be less than 65536.

            if (source != nullptr)
            {
                if (sourceLength == 0)
                {
                    sourceLength = GTLib::Strings::SizeInBytes(source);
                }


                // The first step is to calculate the size of the buffer so that we can do only a single allocation.
                size_t bufferSizeOut = 0;
                bufferSizeOut += 1;     // Version
                bufferSizeOut += 2;     // Target
                bufferSizeOut += 2;     // Defines count
                
                uint16_t definesCount = 0;
                if (defines != nullptr)
                {
                    int iDefine = 0;
                    while (defines[iDefine].name != nullptr)
                    {
                        definesCount += 1;
                        bufferSizeOut += GTLib::Strings::SizeInBytes(defines[iDefine].name)  + 1;       // +1 to include room for the null-terminator.
                        bufferSizeOut += GTLib::Strings::SizeInBytes(defines[iDefine].value) + 1;       // +1 to include room for the null-terminator.
                    }
                }

                bufferSizeOut += sourceLength + 1;          // +1 to include room for the null-terminator.

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
                    *bufferOut = 1;
                    bufferOut += 1;
                    
                    // Target.
                    *reinterpret_cast<uint16_t*>(bufferOut) = static_cast<uint16_t>(target);
                    bufferOut += 2;

                    // Defines count.
                    *reinterpret_cast<uint16_t*>(bufferOut) = definesCount;
                    bufferOut += 2;


                    // Defines
                    for (uint16_t iDefine = 0; iDefine < definesCount; ++iDefine)
                    {
                        size_t nameLength  = GTLib::Strings::SizeInBytes(defines[iDefine].name)  + 1;   // +1 for the null terminator.
                        size_t valueLength = GTLib::Strings::SizeInBytes(defines[iDefine].value) + 1;   // +1 for the null terminator.

                        memcpy(bufferOut, defines[iDefine].name, nameLength);
                        bufferOut += nameLength;

                        memcpy(bufferOut, defines[iDefine].value, valueLength);
                        bufferOut += valueLength;
                    }


                    // Source code.
                    memcpy(bufferOut, source, sourceLength);
                    bufferOut += sourceLength;

                    *bufferOut = '\0';      // Null-terminator for source code.
                    bufferOut += 1;



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
    }
}
