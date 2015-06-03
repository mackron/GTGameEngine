// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_GUITextLayout_hpp_
#define __GT_GUITextLayout_hpp_

#include "GUIFontManager.hpp"
#include "../../../GTLib/Colour.hpp"

namespace GT
{
    /// Structure containing information about a text run in a text layout.
    struct GUITextRunDesc
    {
        /// The text.
        GTLib::String text;

        /// A handle to the font the text should be drawn with.
        HGUIFont hFont;

        /// The position of the text run on the x axis.
        int xPos;

        /// The position of the text run on the y axis.
        int yPos;

        /// The rotation of the text run in tenths of degrees.
        int rotationInDegrees;

        /// The color of the text.
        GTLib::Colour color;
    };

    /// Base class for laying out text.
    ///
    /// This class is used to layout the text of an element. It has been built in such a way that it doesn't actually depend
    /// on a GUI context, elements or surfaces. Thus, it can be used in places outside of the normal GUI stuff.
    ///
    /// The text is always 
    class GUITextLayout
    {
    protected:

        /// Constructor.
        GUITextLayout(GUIFontManager &fontManager);


    public:

        /// Destructor.
        virtual ~GUITextLayout();


        /// Retrieves a reference to the font manager.
        ///
        /// @return A reference to the font manager.
              GUIFontManager & GetFontManager();
        const GUIFontManager & GetFontManager() const;


        /// Sets the text of the text manager.
        ///
        /// @param text [in] The text.
        virtual void SetText(const char* text) = 0;

        /// Retrieves a pointer to the text.
        ///
        /// @return A null-terminated string containing the text of the text layout.
        virtual const char* GetText() const = 0;

    
        /// Sets the size of the bounding rectangle of the text.
        ///
        /// @param width  [in] The width of the bounding rectangle.
        /// @param height [in] The height of the bounding rectangle.
        virtual void SetBounds(unsigned int width, unsigned int height) = 0;

        /// Retrieves the bounding rectangle.
        ///
        /// @param widthOut  [out] A reference to the variable that will receive the width.
        /// @param heightOut [out] A reference to the variable that will receive the height.
        virtual void GetBounds(unsigned int &width, unsigned int &height) const = 0;
        

        /// Sets the offset of the text.
        ///
        /// @param offsetX [in] The offset on the x axis.
        /// @param offsetY [in] The offset on the y axis.
        virtual void SetOffset(int offsetX, int offsetY) = 0;

        /// Retrieves the offset of the text.
        ///
        /// @param offsetXOut [out] A reference to the variable that will receive the offset on the x axis.
        /// @param offsetYOut [out] A reference to the variable that will receive the offset on the y axis.
        virtual void GetOffset(int &offsetXOut, int &offsetYOut) const = 0;


        /// Iterates over every text run contained withing the current bounds.
        ///
        /// @param func [in] The function to call for each text run.
        virtual void IterateVisibleTextRuns(std::function<void (const GUITextRunDesc &textRunDesc)> func) const = 0;


        /// Sets the size of a tab in spaces.
        ///
        /// @param tabSizeInSpaces [in] The size of a tab in spaces.
        virtual void SetTabSizeInSpaces(int tabSizeInSpaces) = 0;
        
        /// Retrieves the size of a tab in spaces.
        ///
        /// @return The size of a tab character in spaces.
        virtual int GetTabSizeInSpaces() const = 0;


        /// Retrieves the width of the text.
        ///
        /// @return The width of the text.
        ///
        /// @remarks
        ///     This is not the same as the main boundary that's retrieved with GetBounds(), but rather the width of the text itself.
        virtual int GetTextWidth() const = 0;

        /// Retrieves the height of the text.
        ///
        /// @return The height of the text.
        ///
        /// @remarks
        ///     This is not the same as the main boundary that's retrieved with GetBounds(), but rather the height of the text itself.
        virtual int GetTextHeight() const = 0;



    private:

        /// A reference to the font manager for working with the font.
        GUIFontManager &m_fontManager;



    private:    // No copying.
        GUITextLayout(const GUITextLayout &);
        GUITextLayout & operator=(const GUITextLayout &);
    };
}

#endif // !__GT_GUITextLayout_hpp_
