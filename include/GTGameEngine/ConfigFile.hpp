// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_ConfigFile
#define GT_ConfigFile

#include "FileSystem.hpp"
#include <cstdint>

typedef struct lua_State lua_State;

#undef LoadString

namespace GT
{
    enum class ConfigFileType
    {
        Number,
        Integer
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
        virtual ~ConfigFile();


        /// Initializes the config file, but doesn't load anything.
        ///
        /// @remarks
        ///     Use Load*() to load a file or string.
        ///     @par
        ///     Sub-classes can override this so that it can initialize global variable or whatnot. The way you would do this is as such:
        ///     <code>
        ///     bool MyConfigFile::Initialize() {
        ///         if (ConfigFile::Initialize()) {
        ///             // Register global variables here
        ///             return true;
        ///         }
        ///
        ///         return false;
        ///     }
        ///     </code>
        virtual bool Initialize();


        /// Loads the config file.
        ///
        /// @param path       [in] The path of the config file to load.
        /// @param fileSystem [in] A reference to the file system object to use to load the file.
        ///
        /// @remarks
        ///     This can be called multiple times, in which case variables are just appended or replaced.
        virtual bool LoadFile(const char* path, GT::FileSystem &fileSystem);

        /// Loads a string.
        ///
        /// @param configString [in] The null-terminated string to load.
        virtual bool LoadString(const char* configString);


        float GetFloat(const char* variableName) const;
        int GetInteger(const char* variableName) const;
        bool GetBoolean(const char* variableName) const;
        const char* GetString(const char* variableName) const;      // <-- Don't store the string for later use - make a copy.

        float GetFloatFromArray(const char* arrayName, unsigned int index) const;
        int GetIntegerFromArray(const char* arrayName, unsigned int index) const;
        bool GetBooleanFromArray(const char* arrayName, unsigned int index) const;
        const char* GetStringFromArray(const char* arrayName, unsigned int index) const;          // <-- Don't store the string for later use - make a copy.

        bool IsNumber(const char* variableName);
        bool IsBoolean(const char* variableName);
        bool IsString(const char* variableName);
        bool IsArray(const char* arrayName) const;
        unsigned int GetArrayCount(const char* arrayName) const;


        /// Sets the value of a variable.
        virtual bool SetFloat(const char* variableName, float value);
        virtual bool SetInteger(const char* variableName, int value);
        virtual bool SetBoolean(const char* variableName, bool value);
        virtual bool SetString(const char* variableName, const char* value);



    protected:

        /// A pointer to the Lua state for keeping track of all the variables.
        lua_State* m_pState;
    };
}

#endif
