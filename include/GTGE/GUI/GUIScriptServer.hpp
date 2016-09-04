// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIScriptServer
#define GT_GUIScriptServer

namespace GT
{
    class GUIServer;
    class GUIElement;

    /**
    *   \brief  Class for managing the scripts in the server.
    *
    *   The script server handles everything to do with scripting.
    */
    class GUIScriptServer
    {
    public:
    
        /**
        *   \brief              Constructor.
        *   \param  script [in] A pointer to the script object to use for scripting. Can be null, in which case GTGUI will create it's own script object.
        */
        GUIScriptServer(GUIServer &server, GT::Script* script);
        
        /**
        *   \brief  Destructor.
        */
        ~GUIScriptServer();
        
        /**
        *   \brief              Loads a script string into the server.
        *   \param  script [in] The string containing the script to load. This is \b not a file name.
        *   \return             True if the script was loaded into the server successfully; false otherwise.
        */
        bool Execute(const char* script, int returnValueCount = -1);
        
        /**
        *   \brief                Loads a script from a file.
        *   \param  filename [in] The file to load.
        *
        *   \remarks
        *       The file should be in plain text.
        */
        bool ExecuteFromFile(const char* filename, int returnValueCount = -1);

        /// Retrieves a reference to the main server.
              GUIServer & GetServer()       { return this->server; }
        const GUIServer & GetServer() const { return this->server; }


        /**
        *   \brief  Retrieves a reference to the internal script.
        *
        *   \remarks
        *       Use this function to manually build your application's foreign function interface.
        */
        GT::Script & GetScript();


        /// Removes the given element from the scripting environment.
        ///
        /// @param element [in] The element to remove from the scripting environment.
        void RemoveElement(GUIElement &element);

        /// Detaches an element from it's parent, but leaves the element in the scripting environment.
        ///
        /// @param parent [in] The parent element.
        /// @param child  [in] The child element.
        void RemoveChild(GUIElement &parent, GUIElement &child);

        /// Detaches every child from the given element.
        ///
        /// @param parent [in] The element whose children are being deleted.
        void RemoveAllChildren(GUIElement &parent);


    // Event posting.
    public:

        /**
        *   \brief               Posts the OnMouseEnter event to the given element.
        *   \param  element [in] A pointer to the element that is receiving the event.
        */
        void PostEvent_OnMouseEnter(GUIElement &element);

        /**
        *   \brief               Posts the OnMouseLeave event to the given element.
        *   \param  element [in] A pointer to the element that is receiving the event.
        */
        void PostEvent_OnMouseLeave(GUIElement &element);

        void PostEvent_OnPush(GUIElement &element);
        void PostEvent_OnRelease(GUIElement &element);
        void PostEvent_OnPressed(GUIElement &element);

        /// Posts the OnSize event to the given element.
        void PostEvent_OnSize(GUIElement &element);

        /// Posts the OnMove event to the given element.
        void PostEvent_OnMove(GUIElement &element);


        /**
        *   \brief               Posts the OnKeyPressed() event to the given receiver.
        *   \param  element [in] A pointer to the element that is receiving the event.
        *   \param  key     [in] The key that was just pressed.
        *
        *   \remarks
        *       This event is different to OnKeyDown in that it will not send repeating events.
        *       \par
        *       The initial key press will call both this event and OnKeyDown.
        */
        void PostEvent_OnKeyPressed(GUIElement &element, char32_t key);

        /**
        *   \brief               Posts the OnKeyDown() event to the given receiver.
        *   \param  element [in] A pointer to the element that is receiving the event.
        *   \param  key     [in] The key that was just pressed.
        *
        *   \remarks
        *       If this function returns true, the character will be inserted into the elements text if that element is being editted. If it
        *       returns false, the key press will be discarded.
        *       \par
        *       The initial key press will call both this event and OnKeyPressed.
        */
        bool PostEvent_OnKeyDown(GUIElement &element, char32_t key);

        /**
        *   \brief               Posts the OnKeyUp() event to the given receiver.
        *   \param  element [in] The element that will be receiving the event.
        *   \param  key     [in] The key that was just released.
        */
        void PostEvent_OnKeyUp(GUIElement &element, char32_t key);


        /// Posts the OnMouseButtonDown event to all MouseButtonDown watchers.
        void PostEvent_OnMouseButtonDown(GUIElement &receiver, int button);
        
        /// Posts the OnMouseButtonUp event to all MouseButtonUp watchers.
        void PostEvent_OnMouseButtonUp(GUIElement &receiver, int button);
        
        /// Posts the OnMouseButtonDoubleClick event to all MouseButtonDoubleClick watchers.
        void PostEvent_OnMouseButtonDoubleClick(GUIElement &receiver, int button);

        /// Posts the OnMouseMove event to the given receiver.
        void PostEvent_OnMouseMove(GUIElement &element, int relativePosX, int relativePosY);

        /// Posts the OnMouseWheel event to the given receiver.
        void PostEvent_OnMouseWheel(GUIElement &element, int delta, int relativePosX, int relativePosY);


        /// Posts the OnFocus event to the given receiver.
        void PostEvent_OnFocus(GUIElement &receiver);

        /// Posts the OnBlur event to the given receiver.
        void PostEvent_OnBlur(GUIElement &receiver);


        /// Posts the OnTear event to the given receiver.
        void PostEvent_OnTear(GUIElement &receiver);

        /// Posts the OnDrop event to the given receiver.
        void PostEvent_OnDrop(GUIElement &receiver, GUIElement &droppedElement);

        /// Posts the OnDragAndDropEnter event to the given receiver.
        void PostEvent_OnDragAndDropEnter(GUIElement &receiver, GUIElement &dragAndDropElement);

        /// Posts the OnDragAndDropLeave event to the given receiver.
        void PostEvent_OnDragAndDropLeave(GUIElement &receiver, GUIElement &dragAndDropElement);

        /// Posts the OnDragAndDropProxyRemoved event to the given receiver.
        void PostEvent_OnDragAndDropProxyRemoved(GUIElement &receiver);


        /// Posts the OnTextChanged event to the given receiver.
        void PostEvent_OnTextChanged(GUIElement &receiver);


        /// Posts the OnInnerXOffsetChanged event to the given receiver.
        void PostEvent_OnInnerXOffsetChanged(GUIElement &receiver);

        /// Posts the OnInnerYOffsetChanged event to the given receiver.
        void PostEvent_OnInnerYOffsetChanged(GUIElement &receiver);

    
        /// Posts the OnShow event to the given receiver.
        void PostEvent_OnShow(GUIElement &receiver);

        /// Posts the OnHide event to the given receiver.
        void PostEvent_OnHide(GUIElement &receiver);


        /// Posts the OnShowTooltip event to the given receiver.
        void PostEvent_OnShowTooltip(GUIElement &receiver);

        /// Posts the OnHideTooltip event to the given receiver.
        void PostEvent_OnHideTooltip(GUIElement &receiver);


    private:
    
        /**
        *   \brief  Loads defaults into the script server.
        */
        bool LoadDefaults();
        
    
    private:

        /// A reference to the server that owns this script server.
        GUIServer &server;
    
        /// The internal script object. This object basically IS the script server. We use a pointer here because applications have the option
        /// of using their own script object for scripting.
        GT::Script* script;

        /// Keeps track of whether or not to delete the script object.
        bool deleteScript;

        
    private: // FFI.
    
        struct FFI
        {
            static GUIServer & GetServer(GT::Script &script);


            // System.
            static int _CreateNewElement(GT::Script &script);
            static int _DeleteElement(GT::Script &script);
            
            static int _GetElementID(GT::Script &script);

            static int _AppendChild(GT::Script &script);
            static int _PrependChild(GT::Script &script);
            static int _RemoveChild(GT::Script &script);
            static int _RemoveAllChildren(GT::Script &script);
            static int _DeleteAllChildren(GT::Script &script);

            static int _IsAncestor(GT::Script &script);
            static int _IsChild(GT::Script &script);
            static int _AttachStyleClass(GT::Script &script);
            static int _DetachStyleClass(GT::Script &script);
            static int _GetStyle(GT::Script &script);
            static int _SetStyle(GT::Script &script);

            static int _LoadStyleScriptFromFile(GT::Script &script);

            ///////////////////////////////////
            // Text
            static int _GetText(GT::Script &script);
            static int _SetText(GT::Script &script);
            static int _GetTextWidth(GT::Script &script);
            static int _GetTextHeight(GT::Script &script);
            static int _GetTextLineHeight(GT::Script &script);
            static int _GetTextLineCount(GT::Script &script);
            static int _GetTextPageLineCount(GT::Script &script);
            static int _SelectAllText(GT::Script &script);
            static int _GoToLine(GT::Script &script);

            
            
            static int _GetWidth(GT::Script &script);
            static int _GetHeight(GT::Script &script);
            static int _GetInnerWidth(GT::Script &script);
            static int _GetInnerHeight(GT::Script &script);
            static int _GetXPosition(GT::Script &script);
            static int _GetYPosition(GT::Script &script);
            static int _GetInnerXPosition(GT::Script &script);
            static int _GetInnerYPosition(GT::Script &script);
            static int _GetInnerXOffset(GT::Script &script);
            static int _GetInnerYOffset(GT::Script &script);
            static int _GetAbsolutePosition(GT::Script &script);


            static int _GetLeftPadding(GT::Script &script);
            static int _GetRightPadding(GT::Script &script);
            static int _GetTopPadding(GT::Script &script);
            static int _GetBottomPadding(GT::Script &script);
            static int _GetHorizontalPadding(GT::Script &script);
            static int _GetVerticalPadding(GT::Script &script);

            static int _Show(GT::Script &script);
            static int _Hide(GT::Script &script);
            static int _IsVisible(GT::Script &script);
            static int _LockStyleRefreshing(GT::Script &script);
            static int _UnlockStyleRefreshing(GT::Script &script);
            static int _RefreshStyle(GT::Script &script);

            static int _GetNextSiblingID(GT::Script &script);
            static int _GetPrevSiblingID(GT::Script &script);

            static int _SetDragAndDropProxyElement(GT::Script &script);
            static int _GetDragAndDropProxyElementID(GT::Script &script);
            static int _RemoveCurrentDragAndDropProxyElement(GT::Script &script);
            static int _SetDragAndDropProxyElementOffset(GT::Script &script);

            static int _FocusElement(GT::Script &script);
            static int _GetFocusedElementID(GT::Script &script);

            static int _BringToTop(GT::Script &script);
            static int _PushToBack(GT::Script &script);

            static int _GetViewportWidth(GT::Script &script);
            static int _GetViewportHeight(GT::Script &script);

            
            // GUIServer.
            static int Log(GT::Script &script);
            static int New(GT::Script &script);
            static int BlurFocusedElement(GT::Script &script);
            static int IsCTRLKeyDown(GT::Script &script);
            static int IsShiftKeyDown(GT::Script &script);
            static int GetMousePosition(GT::Script &script);

            static int IsLMBDown(GT::Script &script);
            static int IsRMBDown(GT::Script &script);
            static int IsMMBDown(GT::Script &script);

            static int DoesFocusedElementHaveEditableText(GT::Script &script);
        };
        
        
    private:    // No copying.
        GUIScriptServer(const GUIScriptServer &);
        GUIScriptServer & operator=(const GUIScriptServer &);
    };
}

#endif
