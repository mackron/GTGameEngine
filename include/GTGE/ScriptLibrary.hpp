// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ScriptLibrary
#define GT_ScriptLibrary

namespace GT
{
    class Context;

    /// Static class for managing the loading and unloading of scripts.
    ///
    /// Although this loads script that will eventually find their way into higher-level script objects, the library is not responsible
    /// for doing any of that kind of stuff.
    class ScriptLibrary
    {
    public:

        /// Constructor.
        ScriptLibrary(Context &context);

        /// Destructor.
        ~ScriptLibrary();



        /////////////////////////////////////////////////
        // Startup/Shutdown

        /// Starts up the script library.
        bool Startup();

        /// Shuts down the script library.
        void Shutdown();



        /////////////////////////////////////////////////
        // Acquire/Unacquire

        /// Acquires a script definition.
        ///
        /// @param fileName       [in] The file name of the script being loaded, relative to the data directory.
        /// @param makeRelativeTo [in] If 'fileName' is absolute, this will be used to turn it into a relative path.
        ///
        /// @return A pointer to the script definition, or null if an error occurs.
        ///
        /// @remarks
        ///     Each call to this function must match up to an Unacquire().
        ///     @par
        ///     All resources must have a relative path somewhere. If it doesn't, there will be errors with serialization. Thus,
        ///     this will return null if 'fileName' is absolute and 'makeRelativeTo' is null.
        ScriptDefinition* Acquire(const char* fileName, const char* makeRelativeTo = nullptr, bool silenceMissingFileWarning = false);

        /// Unacquires a script definition that was first acquired by Acquire().
        ///
        /// @param scriptDefinitionToUnacquire [in] A pointer to the script definition to unacquire.
        void Unacquire(const ScriptDefinition* scriptDefinitionToUnacquire);

        /// Determines whether or not the given definition is currently loaded (currently acquired at least once).
        ///
        /// @param fileName [in] The file name of the script being loaded, relative to the data directory.
        bool IsLoaded(const char* fileName);

        /// Reloads the given file.
        ///
        /// @param fileName [in] The file name of the script being reloaded, relative to the data directory.
        ///
        /// @remarks
        ///     If the file has not yet been loaded it will NOT be loaded and false will be returned.
        bool Reload(const char* fileName);


        /////////////////////////////////////////////////
        // Iteration

        /// Retrieves the number of the loaded definitions. This is used for iteration in conjunction with GetLoadedDefinitionByIndex().
        size_t GetLoadedDefinitionCount();

        /// Retrieves a pointer to the loaded definition at the given index.
        ///
        /// @param index [in] The index of the definition to retrieve.
        const ScriptDefinition* GetLoadedDefinitionByIndex(size_t index);



        /////////////////////////////////////////////////
        // Misc.

        /// Retrieves a reference to a script object that can be used for doing various work like script checking and loading.
        ///
        /// @remarks
        ///     This script does not contain any high-level script functionality.
        GT::Script & GetWorkingScript();




    private:

        /// A reference to the context that owns this script library.
        Context &m_context;

        /// The working script. This acts like a workspace for doing script compilations outside of the context's main script.
        GT::Script m_workingScript;


        typedef std::pair<ScriptDefinition*, size_t> ScriptDefinitionReference;

        /// The list of loaded classes, indexed by the absolute path.
        Dictionary<ScriptDefinitionReference> m_loadedDefinitions;
    };
}

#endif