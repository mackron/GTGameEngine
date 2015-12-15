// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_Editor2
#define GT_Editor2

#include <easy_appkit/ak_application.h>
#include <easy_gui/easy_gui.h>

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


        /// Retrieves a reference to the context that owns the editor.
        Context & GetContext() { return m_context; }

        /// Retrieves a pointer to the internal GUI context.
        easygui_context* GetGUI();

        /// Retrieves a pointer to the context's easyvfs_context object. 
        easyvfs_context* GetVFS();

        /// Retrieves a pointer to the internal easy_appkit application.
        ak_application* GetAKApplication();

        /// Retrieves a pointer to the internal theme.
        ak_theme* GetAKTheme();

        /// Retrieves a pointer to the font to use for drawing symbols.
        easygui_font* GetSymbolFont() { return m_pSymbolFont; }
        
        /// Retrieves the metrics of the symbol font.
        easygui_font_metrics GetSymbolFontMetrics() { return m_symbolFontMetrics; }



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

        /// The font to use for symbols.
        easygui_font* m_pSymbolFont;

        /// The metrics of the symbol font.
        easygui_font_metrics m_symbolFontMetrics;
    };
}

#endif
