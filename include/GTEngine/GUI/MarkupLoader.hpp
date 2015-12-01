// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_MarkupLoader
#define GT_GUI_MarkupLoader

#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/Dictionary.hpp>
#include <GTEngine/Core/Vector.hpp>

#include <GTEngine/GUI/ServerXMLParser.hpp>

namespace GTGUI
{
    class GUIServer;
    class GUIElement;
    
    /// Class for loading GUI markup files and strings.
    ///
    /// This class keeps track of files that have previously been loaded. This ensures that files do not get reloaded unnecessarily. When
    /// LoadFromFile() is called, it will NOT load a file that has already been loaded. It will instead return true immediately.
    class GUIMarkupLoader
    {
    public:
    
        /// Constructor.
        GUIMarkupLoader(GUIServer &server);
        
        /// Destructor.
        virtual ~GUIMarkupLoader();
        
        
        
        /// Loads a markup string.
        ///
        /// @param markup            [in]  The markup string.
        /// @param absoluteDirectory [in]  The absolute directory to load include files from.
        /// @param topElementID      [out] A reference to the variable that will receive the ID of the top-most element.
        virtual bool Load(const char* markup, const char* absoluteDirectory, GT::String &topElementID);
        
        /// Loads a markup string from a file.
        ///
        /// @param filePath [in] The path of the file to load.
        ///
        /// @return True if the file is loaded successfully or it has been successfully loaded beforehand; false otherwise.
        ///
        /// @remarks
        ///     This will not reload the file if it has previously been loaded. Instead it will return true immediately.
        ///     @par
        ///     The file name will be converted to an absolute path before attempting to load.
        virtual bool LoadFile(const char* filePath);
        
        /// Marks a file as unloaded.
        ///
        /// @param filePath [in] The path fo the file to unload.
        ///
        /// @remarks
        ///     It's important to note that this will not actually delete any element. It simply marks the file as unloaded
        ///     so it can subsequently be reloaded with LoadFile().
        virtual void UnloadFile(const char* filePath);
    
        
        /// Determines whether or not a file has already been loaded.
        ///
        /// @param absolutePath [in] The absolute path of the file to check.
        ///
        /// @return True if the file is loaded; false otherwise.
        virtual bool IsFileLoaded(const char* absolutePath) const;
            
            
            
    private:
        
        /// Main implementation for loading a markup script string.
        ///
        /// @param markup            [in]  The markup string.
        /// @param markupSizeInBytes [in]  The size in bytes of the markup string, or -1 if it is null terminated.
        /// @param absoluteDirectory [in]  The absolute directory to load include files from.
        /// @param loadedElementsOut [out] A reference to the list that will keep track of every element that is loaded.
        ///
        /// @remarks
        ///     This function will get called recursively in order to handle include files. We need to keep track of loaded elements
        ///     so we can perform a post-process step.
        bool Load(const char* markup, size_t markupSizeInBytes, const char* absoluteDirectory, GT::Vector<GUIElement*> &loadedElementsOut);
        
        /// Main implementation for loading a file.
        ///
        /// @param filePath          [in]  The path fo the file to load.
        /// @param loadedElementsOut [out] A reference to the list that will keep track of every element that is loaded.
        ///
        /// @remarks
        ///     This function will get called recursively in order to handle include files. We need to keep track of loaded elements
        ///     so we can perform a post-process step.
        bool LoadFile(const char* filePath, GT::Vector<GUIElement*> &loadedElementsOut);
        
        /// Performs a post-processing step on the given elements.
        ///
        /// @param loadedElements [in] A reference to the list of elements that have been loaded by Load() or LoadFile().
        ///
        /// @remarks
        ///     The main purpose of this step is to post events like OnShow, etc.
        void PostLoad(const GT::Vector<GUIElement*> &loadedElements);
            
    
    private:
    
        /// A reference to the GUI server.
        GUIServer &m_server;
        
        /// A container for keeping track of files that have already been loaded.
        GT::Dictionary<bool> m_loadedFiles;



    private:    // No copying.
        GUIMarkupLoader(const GUIMarkupLoader &);
        GUIMarkupLoader & operator=(const GUIMarkupLoader &);
    };
}

#endif // GT_GUI_MarkupLoader
