// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/ConfigFile.hpp>
#include "external/lua-5.3.1/src/lua.hpp"
#include <assert.h>

namespace GT
{
    ConfigFile::ConfigFile()
        : m_pState(nullptr)
    {
    }

    ConfigFile::~ConfigFile()
    {
        if (m_pState != nullptr)
        {
            lua_close(m_pState);
        }
    }


    bool ConfigFile::Initialize()
    {
        if (m_pState == nullptr)
        {
            m_pState = luaL_newstate();
            if (m_pState != nullptr)
            {
                luaL_openlibs(m_pState);

                return true;
            }

            return false;
        }

        return true;
    }


    bool ConfigFile::LoadFile(const char* path, GT::FileSystem &fileSystem)
    {
        if (m_pState != nullptr)
        {
            bool result = false;

            GT::HFile hFile = fileSystem.OpenFile(path, GT::FileAccessMode::Read);
            if (hFile != 0)
            {
                unsigned int fileSize = static_cast<unsigned int>(fileSystem.GetFileSize(hFile));
                if (fileSize > 0)
                {
                    char* fileData = reinterpret_cast<char*>(malloc(fileSize + 1));
                    fileSystem.ReadFile(hFile, fileSize, fileData);
                    fileData[fileSize] = '\0';

                    result = this->LoadString(fileData);

                    free(fileData);
                }
                
                fileSystem.CloseFile(hFile);
            }

            return result;
        }

        return false;
    }

    bool ConfigFile::LoadString(const char* configString)
    {
        if (m_pState != nullptr)
        {
            if (luaL_loadstring(m_pState, configString) == 0)
            {
                if (lua_pcall(m_pState, 0, 0, 0) == 0)
                {
                    return true;
                }
            }
        }

        return false;
    }


    // TODO: Add support for complex variables such as a.b.c.d

    float ConfigFile::GetFloat(const char* variableName) const
    {
        float result = 0;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, variableName);
            if (lua_isnumber(m_pState, -1))
            {
                result = static_cast<float>(lua_tonumber(m_pState, -1));
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    int ConfigFile::GetInteger(const char* variableName) const
    {
        int result = 0;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, variableName);
            if (lua_isinteger(m_pState, -1))
            {
                result = static_cast<int>(lua_tointeger(m_pState, -1));
            }
            else
            {
                if (lua_isnumber(m_pState, -1))
                {
                    result = static_cast<int>(lua_tonumber(m_pState, -1));
                }
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    bool ConfigFile::GetBoolean(const char* variableName) const
    {
        bool result = false;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, variableName);
            if (lua_isboolean(m_pState, -1))
            {
                result = lua_toboolean(m_pState, -1) != 0;
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    bool ConfigFile::GetString(const char* variableName, char* strOut, unsigned int strOutSizeInBytes)
    {
        bool result = false;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, variableName);
            if (lua_isstring(m_pState, -1))
            {
                result = strcpy_s(strOut, strOutSizeInBytes, lua_tostring(m_pState, -1)) == 0;  // strcpy_s() returns 0 on success. TODO: This won't work with GCC/Clang - replace with a different implementation.
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    const char* ConfigFile::GetString(const char* variableName) const
    {
        const char* result = nullptr;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, variableName);
            if (lua_isstring(m_pState, -1))
            {
                result = lua_tostring(m_pState, -1);
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }


    float ConfigFile::GetFloatFromArray(const char* arrayName, unsigned int index) const
    {
        float result = 0;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, arrayName);
            if (lua_istable(m_pState, -1))
            {
                lua_pushnumber(m_pState, index + 1);
                lua_gettable(m_pState, -2);
                if (lua_isnumber(m_pState, -1))
                {
                    result = static_cast<float>(lua_tonumber(m_pState, -1));
                }
                lua_pop(m_pState, 1);
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    int ConfigFile::GetIntegerFromArray(const char* arrayName, unsigned int index) const
    {
        int result = 0;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, arrayName);
            if (lua_istable(m_pState, -1))
            {
                lua_pushnumber(m_pState, index + 1);
                lua_gettable(m_pState, -2);
                if (lua_isinteger(m_pState, -1))
                {
                    result = static_cast<int>(lua_tointeger(m_pState, -1));
                }
                else
                {
                    if (lua_isnumber(m_pState, -1))
                    {
                        result = static_cast<int>(lua_tonumber(m_pState, -1));
                    }
                }
                lua_pop(m_pState, 1);
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    bool ConfigFile::GetBooleanFromArray(const char* arrayName, unsigned int index) const
    {
        bool result = 0;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, arrayName);
            if (lua_istable(m_pState, -1))
            {
                lua_pushnumber(m_pState, index + 1);
                lua_gettable(m_pState, -2);
                if (lua_isboolean(m_pState, -1))
                {
                    result = lua_toboolean(m_pState, -1) != 0;
                }
                lua_pop(m_pState, 1);
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    const char* ConfigFile::GetStringFromArray(const char* arrayName, unsigned int index) const
    {
        const char* result = 0;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, arrayName);
            if (lua_istable(m_pState, -1))
            {
                lua_pushnumber(m_pState, index + 1);
                lua_gettable(m_pState, -2);
                if (lua_isstring(m_pState, -1))
                {
                    result = lua_tostring(m_pState, -1);
                }
                lua_pop(m_pState, 1);
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }


    bool ConfigFile::IsNumber(const char* variableName)
    {
        bool result = false;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, variableName);
            {
                result = lua_isnumber(m_pState, -1) != 0;
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    bool ConfigFile::IsBoolean(const char* variableName)
    {
        bool result = false;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, variableName);
            {
                result = lua_isboolean(m_pState, -1) != 0;
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    bool ConfigFile::IsString(const char* variableName)
    {
        bool result = false;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, variableName);
            {
                result = lua_isstring(m_pState, -1) != 0;
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    bool ConfigFile::IsArray(const char* arrayName) const
    {
        bool result = false;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, arrayName);
            if (lua_istable(m_pState, -1))
            {
                result = true;
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }

    unsigned int ConfigFile::GetArrayCount(const char* arrayName) const
    {
        unsigned int result = 0;
        if (m_pState != nullptr)
        {
            lua_getglobal(m_pState, arrayName);
            if (lua_istable(m_pState, -1))
            {
                lua_len(m_pState, -1);
                {
                    result = static_cast<unsigned int>(lua_tointeger(m_pState, -1));
                }
                lua_pop(m_pState, 1);
            }
            lua_pop(m_pState, 1);
        }

        return result;
    }


    bool ConfigFile::SetFloat(const char* variableName, float value)
    {
        if (m_pState != nullptr)
        {
            lua_pushnumber(m_pState, value);
            lua_setglobal(m_pState, variableName);

            return true;
        }

        return false;
    }

    bool ConfigFile::SetInteger(const char* variableName, int value)
    {
        if (m_pState != nullptr)
        {
            lua_pushinteger(m_pState, value);
            lua_setglobal(m_pState, variableName);

            return true;
        }

        return false;
    }

    bool ConfigFile::SetBoolean(const char* variableName, bool value)
    {
        if (m_pState != nullptr)
        {
            lua_pushboolean(m_pState, value);
            lua_setglobal(m_pState, variableName);

            return true;
        }

        return false;
    }

    bool ConfigFile::SetString(const char* variableName, const char* value)
    {
        if (m_pState != nullptr)
        {
            lua_pushstring(m_pState, value);
            lua_setglobal(m_pState, variableName);

            return true;
        }

        return false;
    }
}
