// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIRenderer
#define GT_GUIRenderer

namespace GT
{
    class GUIServer;
    class GUIElement;
    class GUIMesh;

    /// Virtual class acting as the base class for GUI renderers.
    ///
    /// A renderer is attached to a GUI server and virtual methods will be called when the server is stepped. The derived class can
    /// decide for itself how it wants to perform the rendering.
    ///
    /// The default offset should be 0,0.
    /// The default texture should be null.
    /// Blending should be disabled by default.
    class GUIRenderer
    {
    public:

        /// Constructor.
        GUIRenderer();

        /// Destructor.
        virtual ~GUIRenderer();



        ////////////////////////////////////////////////////////////////////////
        // Virtual Methods
        //
        // These methods need to be implemented by a derived class. These are the only methods that need to be implemented in order
        // for rendering to work properly.
        //
        // Look at the document of each method to know what to do.

        /// Called at the beginning of a step render.
        virtual void Begin(const GUIServer &server);

        /// Called at the end of a step render.
        virtual void End();

        /// Called just before the OnDraw events are called.
        ///
        /// @param element [in] A reference to the element whose OnDraw events are about to be called.
        virtual void BeginElementOnDrawEvent(GUIElement &element);

        /// Called just after the OnDraw events have been called.
        ///
        /// @param element [in] A reference to the element whose OnDraw events have just been called.
        ///
        /// @remarks
        ///     Because OnDraw event handlers can change rendering state, derived classes may need to restore certain state afterwards. They
        ///     can do this within this method.
        virtual void EndElementOnDrawEvent(GUIElement &element);

        /// Called when the scissor rectangle needs to be set for clipping.
        ///
        /// @param x      [in] The x position of the scissor rectangle.
        /// @param y      [in] The y position of the scissor rectangle, from the top.
        /// @param width  [in] The width of the scissor rectangle.
        /// @param height [in] The height of the rectangle.
        virtual void SetScissor(int x, int y, unsigned int width, unsigned int height);

        /// Sets the offset that needs to be applied to the geometry of all future rendering operations.
        ///
        /// @param offsetX [in] The x offset to apply to geometry of all future rendering operations.
        /// @param offsetY [in] The y offset to apply to geometry of all future rendering operations.
        virtual void SetOffset(float offsetX, float offsetY);

        /// Sets the texture to apply to the geometry of all future rendering operations.
        ///
        /// @param texture [in] A handle to the image to apply to all future geometry draws.
        ///
        /// @remarks
        ///     'texture' can be 0, in which case no texturing should be used.
        virtual void SetTexture(GUIImageHandle texture);

        /// Enables blending.
        ///
        /// @remarks
        ///     The blending should be equivalent to simple alpha blending.
        virtual void EnableBlending();

        /// Disables blending.
        virtual void DisableBlending();

        /// Called when a mesh needs to be rendered.
        ///
        /// @param vertices    [in] A pointer to the vertex data.
        /// @param vertexCount [in] The number of vertices defined in 'vertices'.
        /// @param indices     [in] A pointer to the indices.
        /// @param indexCount  [in] The number of indices defined in 'indices'.
        ///
        /// @remarks
        ///     The format of the geometry is always 2xPosition, 2xTexCoord and 4xColour. That is, 8 floating point numbers for each vertex, with the first two being
        ///     the position, the next two being the texture coordinates and the last four being the colour. If <image> is null, and thus no textures are being used,
        ///     the texture coordintes will still be present, only they will be set to 0,0. The opacity will be defined by the a/w component of the colour.
        ///     @par
        ///     The verted and index data must be copied by the implementation so that it can be freed at any time by GTGUI.
        virtual void Draw(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount);



        ////////////////////////////////////////////////////////////////////////
        // Non-Virtual Methods
        //
        // These methods should not be touched by derived classes.

        /// Recursively renders the given element.
        ///
        /// @param server    [in] A reference to the server.
        /// @param element   [in] A reference to the element to render.
        ///
        /// @remarks
        ///     This will render the element regardless of whether or not it is visible. Visibility checks should be done at a higher level. Child elements
        ///     will not be drawn if they are invisible.
        void RenderElement(GUIServer &server, GUIElement &element);



    private:

        /// Private implementation for setting the scissor rectangle.
        void _SetScissor(int x, int y, unsigned int width, unsigned int height, bool isSetToWholeViewport = false);

        /// Private implementation for setting the geometry offset.
        void _SetOffset(float offsetX, float offsetY);

        /// Private implementation for setting the texture.
        void _SetTexture(GUIImageHandle texture);

        /// Private implementation for enabling blending.
        void _EnableBlending();

        /// Private implementation for disabling blending.
        void _DisableBlending();

        /// Draws a mesh.
        ///
        /// @remarks
        ///     This just a helper to keep things simpler.
        void _Draw(GUIMesh* mesh);
        

        /// The current texture.
        GUIImageHandle currentTexture;
        

        /// Whether or not the scissor rectangle is set to the whole viewport.
        bool isScissorSetToViewport;

        /// Keeps track of whether or not blending is enabled.
        bool isBlendingEnabled;



    private:    // No copying.
        GUIRenderer(const GUIRenderer &);
        GUIRenderer & operator=(const GUIRenderer &);
    };
}

#endif
