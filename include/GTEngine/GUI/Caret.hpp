// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUICaret
#define GT_GUICaret

#include <GTEngine/Core/Colour.hpp>
#include "Element.hpp"
#include "Rendering/GUIMesh.hpp"


namespace GTGUI
{
    /**
    *   \brief  Class representing a caret.
    *
    *   There is one caret per server.
    *   
    *   The caret is the cursor for text input. Only a single element can "own" the caret at a time. When an element gains focus, it should show
    *   the caret. When it loses focus, it should hide it.
    */
    class GUICaret
    {
    public:

        /**
        *   \brief  Constructor.
        */
        GUICaret();

        /// Destructor.
        ~GUICaret();
    
        /**
        *   \brief  Shows the caret for the given element.
        *
        *   \remarks
        *       If an element needs the caret, it should obtain it with GUICaret::Show() in the OnReceiveFocus() event. When the element receives
        *       the OnLoseFocus() event it should call GUICaret::Hide().
        */
        void Show(GUIElement *owner);
        
        /**
        *   \brief  Hides the caret.
        */
        void Hide();
        
        /**
        *   \brief  Retrieves the position of the caret.
        */
        void GetPosition(int &x, int &y);

        /**
        *   \brief  Sets the position of the caret relative to the top left of the element that currently owns it.
        */
        void SetPosition(int x, int y);
        

        /**
        *   \brief  Retrieves the size of the caret.
        */
        void GetSize(unsigned int &width, unsigned int &height);

        /**
        *   \brief  Sets the size of the caret in pixels.
        */
        void SetSize(unsigned int width, unsigned int height);

        /**
        *   \brief  Sets the blink interval in seconds. Set this to 0 for a non-blinking caret. 
        */
        void SetBlinkInterval(double blinkIntervalSeconds);
    
        /**
        *   \brief  Determines if the caret is visible. The caret must be shown with Show() and also "on".
        */
        bool IsVisible() const;
        
    
        /**
        *   \brief  Retrieves a pointer to the control that owns the caret, if any.
        *
        *   \remarks
        *       If the carent is hidden (not owned by anything) the function will return null.
        */
        GUIElement* GetOwner() const { return this->owner; }


        /// Validates the rendering data. This is called just before the caret's RC command is posted.
        void ValidateRenderingData();

        /// Invalidates the rendering data.
        void InvalidateRenderingData();

        /// Retrieves the mesh representing the visual representation of the caret.
              GUIMesh* GetMesh()       { return this->mesh; }
        const GUIMesh* GetMesh() const { return this->mesh; }



    public:
    
        /**
        *   \brief  Updates the caret. This is used to control the blinking of the caret.
        */
        void Update(double dt);


    private:

        /// The control that currently owns the caret.
        GUIElement *owner;
    
        /// The current position of the caret.
        int xPos;
        int yPos;

        /// The width and height of the caret.
        unsigned int width;
        unsigned int height;
    
        /// The blink interval of the caret in seconds.
        double blinkInterval;

        /// Keeps track of how long the caret has been in it's current blink state. It is used to keep track of whether or
        /// not to toggle the on/off state of the caret.
        double blinkTime;
    
        /// Keeps track of whether or not the caret is "On" or "Off".
        bool isOn;

        /// Keeps track of whether or not the caret needs to have it's rendering information updated.
        bool isRenderingDataValid;

        /// The caret's mesh.
        GUIMesh* mesh;


    private:    // No copying.
        GUICaret(const GUICaret &);
        GUICaret & operator=(const GUICaret &);
    };
}


#endif
