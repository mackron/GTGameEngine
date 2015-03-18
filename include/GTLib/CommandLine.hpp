// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTLib_CommandLine_hpp_
#define __GTLib_CommandLine_hpp_

#include "Config.hpp"
#include "Vector.hpp"
#include "Dictionary.hpp"
#include "String.hpp"
#include <cstddef>

namespace GTLib
{
#if defined(GT_PLATFORM_WINDOWS)
    /**
    *   \brief                Converts the command line provided by WinMain into main() format.
    *   \param  cmdLine [in]  The command line from WinMain.
    *   \param  argc    [out] A reference to the variable that will receive the argument count.
    *   \param  argv    [out] A reference to the list of strings that will receive the argument strings.
    *
    *   \remarks
    *       Delete argv with delete when finished with the command line.
    */
    void ParseWinMainCommandLine(char *cmdLine, int &argc, char **&argv);
#endif
}

namespace GTLib
{
    /**
    *   \brief  Class for working with GNU style command lines.
    *
    *   \remarks
    *       The command line is treated as key/value pairs. It is possible to have a single value without a key. This particular value is called the
    *       anonymous value. The anon. value is usually a file name.
    *       \par
    *       Arguments without an arguments (-c, for example) are considered booleans and will be given a value of "true". You can use IsSet() to
    *       determine if a boolean argument was specified, also.
    */
    class CommandLine
    {
    public:

        /// Constructor.
        CommandLine();

        /// Destructor.
        ~CommandLine();


        /// Parses a main()-style command line.
        ///
        /// @param argc [in] The argument count.
        /// @param arbv [in] The argument array.
        ///
        /// @remarks
        ///     This will clear any existing command line data.
        void Parse(int argc, char** argv);

        /// Parses a WinMain()-style command line.
        ///
        /// @param cmdLine [in] The command line.
        ///
        /// @remarks
        ///     This will clear any existing command line data.
        void Parse(const char* cmdLine);


        /// Retrieves the directory of the application executable.
        ///
        /// @return The absolute path of the directory containing the executable.
        ///
        /// @remarks
        ///     The return value does not include the name of the executable file.
        const char* GetApplicationDirectory() const;

        /// Retrieves the full absolute path of the executable.
        ///
        /// @return The absolute path of the executable file.
        const char* GetExecutablePath() const;


        /// Retrieves a null-terminated array of strings representing the values of an argument.
        ///
        /// @param  key [in] The name of the argument whose values are being retrieved.
        ///
        /// @remarks
        ///     This will return null if the given argument is not found.
        const char** GetArgument(const char* key) const;


        /// Checks whether or not an argument is set. This is useful for boolean arguments.
        bool IsSet(const char* key) const;


        /// Overload for GetArgument().
        const char** operator[](const char* key) const { return this->GetArgument(key); }


    private:

        /// Clears the command line data.
        void Clear();

        /// Helper for appending an argument.
        void AppendArg(const char* keyStart, const char* keyEnd, const char* valueStart, const char* valueEnd);
        void AppendArg(const char* keyStart, const char* keyEnd, const char* value);
        void AppendArg(const char* keyStart, const char* keyEnd);


    private:

        /// The application directory.
        GTLib::String m_applicationDirectory;

        /// The absolute path of the executable file.
        GTLib::String m_executablePath;

        /// The list of key/value arguments.
        GTLib::Dictionary<GTLib::Vector<const char*>*> m_arguments;


    private:    // No copying.
        CommandLine(const CommandLine &);
        CommandLine & operator=(const CommandLine &);
    };
}

#endif

