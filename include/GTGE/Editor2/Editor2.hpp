// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_Editor2
#define GT_Editor2

#include <easy_appkit/ak_application.h>

namespace GT
{
    class Context;

    /// Main class representing the editor.
    class Editor2
    {
    public:

        /// Constructor.
        Editor2(Context &context);

        /// Destructor.
        ~Editor2();

        
        /// Starts up and runs the editor.
        ///
        /// @return The result code.
        ///
        /// @remarks
        ///     A return value of 0 indicates success. Errors will be indicated by a negative value.
        int StartupAndRun();

        /// Shuts down the editor.
        void Shutdown();




        ///////////////////////////////////////////////////////////
        //
        // The methods below should only be used internally.
        //
        ///////////////////////////////////////////////////////////

        /// Retrieves the default config script.
        const char* GetDefaultConfig();



    private:

        /// A reference to the context that owns this editor instance.
        Context &m_context;

        /// A pointer to the easy_appkit application object.
        ak_application* m_pApplication;
    };
}

#endif
