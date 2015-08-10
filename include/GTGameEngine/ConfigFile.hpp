// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_ConfigFile_hpp_
#define __GT_ConfigFile_hpp_

#include <cstdint>

typedef struct lua_State lua_State;

namespace GT
{
    enum class ConfigFileType
    {
        Number,
        Integer,

    };

    /// Class representing a config file.
    ///
    /// Config files are implemented as Lua scripts, and thus are implemented using the Lua library.
    class ConfigFile
    {
    public:

        /// Constructor.
        ConfigFile();

        /// Destructor.
        ~ConfigFile();


        /// Loads the config file.
        ///
        /// @param absolutePath [in] The absolute path of the config file to load.
        ///
        /// @remarks
        ///     This can be called multiple times, in which case variables are just appended or replaced.
        bool Load(const char* absolutePath);


        float GetFloat(const char* variableName) const;
        int GetInteger(const char* variableName) const;
        bool GetBoolean(const char* variableName) const;
        const char* GetString(const char* variableName) const;      //< Don't store the string for later use - make a copy.

        float GetFloatFromArray(const char* arrayName, unsigned int index) const;
        int GetIntegerFromArray(const char* arrayName, unsigned int index) const;
        bool GetBooleanFromArray(const char* arrayName, unsigned int index) const;
        const char* GetStringFromArray(const char* arrayName, unsigned int index) const;          //< Don't store the string for later use - make a copy.

        bool IsArray(const char* arrayName) const;
        unsigned int GetArrayCount(const char* arrayName) const;


    private:

        /// A pointer to the Lua state for keeping track of all the variables.
        lua_State* m_pState;
    };
}

#endif
