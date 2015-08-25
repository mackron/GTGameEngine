// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#include <GTLib/CommandLine.hpp>
#include "../external/easy_path/easy_path.h"
#include <cassert>

#if defined(GT_PLATFORM_WINDOWS)
#include <GTLib/windows.hpp>
#endif

#if defined(GT_PLATFORM_LINUX)
#include <unistd.h>
#endif


namespace GTLib
{
    // TODO: Add support for quoted string segments. For example: -f "C:\Program Files\My Text File.txt"

    CommandLine::CommandLine()
        : m_applicationDirectory(), m_executablePath(), m_arguments()
    {
        
    }

    CommandLine::~CommandLine()
    {
        this->Clear();
    }


    void CommandLine::Parse(int argc, char** argv)
    {
        this->Clear();

        // Start by grabbing the executable path.
        char executablePath[GT_CMDLINE_MAX_PATH];
#if defined(GT_PLATFORM_WINDOWS)
        GetModuleFileNameA(NULL, executablePath, GT_CMDLINE_MAX_PATH);
#elif defined(GT_PLATFORM_LINUX)
        readlink("/proc/self/etc", executablePath, GT_CMDLINE_MAX_PATH);
#else
        if (easypath_isrelative(argv[0]))
        {
            getcwd(executablePath, GT_CMDLINE_MAX_PATH);
            easypath_append(argv[0]);
        }
        else
        {
            easypath_strcpy(executablePath, GT_CMDLINE_MAX_PATH, argv[0]);
        }
#endif

        // Make sure the executable path is cleaned at this point.
        easypath_clean(executablePath, m_executablePath, GT_CMDLINE_MAX_PATH);

        // We need the directory of the executable which is just the base path.
        easypath_copybasepath(m_executablePath, m_applicationDirectory, GT_CMDLINE_MAX_PATH);


        // The key for anonymous arguments.
        const char* anonKey = "";
        const char* anonKeyStart = anonKey;
        const char* anonKeyEnd   = anonKey + 1;

        // The key and value of the current argument. We start with the anonymous key.
        const char* currentKeyStart = anonKeyStart;
        const char* currentKeyEnd   = anonKeyEnd;    // <-- 1 character past the end. End - Start will equal the length.

        // Here we need to parse the command line.
        for (int i = 1; i < argc; ++i)
        {
            const char* arg = argv[i];

            // Arguments beginning with a single dash means that the key is a single character. It also means that
            // characters can be chained together, such as -caef which means 4 arguments: c, a, e, and f.
            if (*arg == '-')
            {
                ++arg;

                if (*arg != '\0')
                {
                    if (*arg == '-')
                    {
                        ++arg;

                        // We're starting a multi-character keyed argument. We loop until the end of the argument to find
                        // the key.
                        currentKeyStart = arg;
                        currentKeyEnd   = arg;

                        while (*currentKeyEnd != '\0')
                        {
                            ++currentKeyEnd;
                        }

                        this->AppendArg(currentKeyStart, currentKeyEnd);
                    }
                    else
                    {
                        // Each character is a separate argument.
                        while (*arg != '\0')
                        {
                            currentKeyStart = arg++;
                            currentKeyEnd   = arg;

                            this->AppendArg(currentKeyStart, currentKeyEnd);
                        }
                    }
                }
            }
            else
            {
                this->AppendArg(currentKeyStart, currentKeyEnd, arg);
            }
        }



        // At this point we have every argument done. Remember that each argument has a list of values (for things like -f file0 -f file1). We
        // are going to null-terminate these arrays.
        for (size_t i = 0; i < m_arguments.count; ++i)
        {
            auto values = m_arguments.buffer[i]->value;
            values->PushBack(nullptr);
        }
    }

    void CommandLine::Parse(const char* cmdLine)
    {
        // This code is taken from here: http://www.flipcode.com/archives/WinMain_Command_Line_Parser.shtml
        size_t cmdLineLength = strlen(cmdLine);

        char* arg = reinterpret_cast<char*>(malloc(cmdLineLength + 1));
        if (arg != nullptr)
        {
            memcpy(arg, cmdLine, cmdLineLength);
            arg[cmdLineLength] = '\0';

        
            // First we count the arguments.
            int argc = 1;

            int i = 0;
            while (arg[i] != '\0')
            {
                while (arg[i] != '\0' && arg[i] == ' ')
                {
                    i += 1;
                }
            
                if (arg[i] != '\0')
                {
                    ++argc;
                
                    while (arg[i] != '\0' && arg[i] == ' ')
                    {
                        i += 1;
                    }
                }
            }
        
            // Now we tokenise the arguments.
            char** argv = reinterpret_cast<char**>(malloc(sizeof(char*) * argc));
            if (argv != nullptr)
            {
                int iArgv = 1;
        
                while (arg[0] != '\0')
                {
                    while (arg[0] != '\0' && arg[0] == ' ')
                    {
                        ++arg;
                    }
            
                    if (arg[0] != '\0')
                    {
                        argv[iArgv] = arg;
                        iArgv += 1;
                
                        while (arg[0] != '\0' && arg[0] == ' ')
                        {
                            ++arg;
                        }
                
                        if (arg[0] != '\0')
                        {
                            arg[0] = '\0';
                            ++arg;
                        }
                    }
                }
        
        
                // Now just parse as a main()-style command line.
                this->Parse(argc, argv);


                free(argv);                
            }

            free(arg);
        }
    }


    const char* CommandLine::GetApplicationDirectory() const
    {
        return m_applicationDirectory;
    }

    const char* CommandLine::GetExecutablePath() const
    {
        return m_executablePath;
    }

    const char** CommandLine::GetArgument(const char* key) const
    {
        auto item = m_arguments.Find(key);
        if (item != nullptr)
        {
            return item->value->buffer;
        }

        return nullptr;
    }

    bool CommandLine::IsSet(const char* key) const
    {
        return m_arguments.Find(key) != nullptr;
    }



    ////////////////////////////////////
    // Private

    void CommandLine::Clear()
    {
        // We need to delete the arguments.
        for (size_t i = 0; i < m_arguments.count; ++i)
        {
            auto values = m_arguments.buffer[i]->value;

            // Before deleting the vector object, we need to delete the argument strings.
            for (size_t iArgValue = 0; iArgValue < values->count; ++iArgValue)
            {
                Strings::Delete(const_cast<char*>(values->buffer[iArgValue]));
            }

            delete values;
        }

        m_applicationDirectory[0] = '\0';
        m_executablePath[0] = '\0';
        m_arguments.Clear();
    }

    void CommandLine::AppendArg(const char* keyStart, const char* keyEnd, const char* valueStart, const char* valueEnd)
    {
        assert(keyEnd > keyStart);

        Vector<const char*>* argValues = nullptr;

        // Step 1: Create the vector that will store our array of strings.
        auto dictItem  = m_arguments.Find(keyStart, keyEnd - keyStart);
        if (dictItem == nullptr)
        {
            argValues = new GTLib::Vector<const char*>(2);
            m_arguments.Add(keyStart, keyEnd - keyStart, argValues);
        }
        else
        {
            argValues = dictItem->value;
        }

        // Step 2: Add the argument to the string array.
        if (valueStart != nullptr)
        {
            assert(valueEnd > valueStart);
            argValues->PushBack(GTLib::Strings::Create(valueStart, valueEnd - valueStart));
        }
    }

    void CommandLine::AppendArg(const char* keyStart, const char* keyEnd, const char* value)
    {
        this->AppendArg(keyStart, keyEnd, value, value + GTLib::Strings::SizeInTs(value));
    }

    void CommandLine::AppendArg(const char* keyStart, const char* keyEnd)
    {
        this->AppendArg(keyStart, keyEnd, nullptr, nullptr);
    }
}
