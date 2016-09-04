// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUIGUIElementRenderingData
#define GT_GUIGUIElementRenderingData

namespace GT
{
    class GUIElement;

    /// Class containing the rendering data for an element.
    ///
    /// This is mainly just a bunch of Mesh objects for the different parts of an element.
    ///
    /// The garbage collector is used for deleting internal meshes. This is passed to the constructor.
    class GUIElementRenderingData
    {
    public:

        /// Constructor.
        GUIElementRenderingData(GUIGarbageCollector &gc);

        /// Destructor.
        ~GUIElementRenderingData();


        /// Retrieves a reference to the list of meshes used to render the text.
              GT::Vector<GUIMesh*> & GetTextMeshes()       { return this->textMeshes; }
        const GT::Vector<GUIMesh*> & GetTextMeshes() const { return this->textMeshes; }

        /// Retrieves a pointer to the shadow mesh.
              GUIMesh* GetShadowMesh()       { return this->shadowMesh; }
        const GUIMesh* GetShadowMesh() const { return this->shadowMesh; }

        /// Retrieves a pointer to the border mesh.
              GUIMesh* GetBorderMesh()       { return this->borderMesh; }
        const GUIMesh* GetBorderMesh() const { return this->borderMesh; }

        /// Retrieves a pointer to the background mesh.
              GUIMesh* GetBackgroundMesh()       { return this->backgroundMesh; }
        const GUIMesh* GetBackgroundMesh() const { return this->backgroundMesh; }

        /// Retrieves a pointer to the background image mesh.
              GUIMesh* GetBackgroundImageMesh()       { return this->backgroundImageMesh; }
        const GUIMesh* GetBackgroundImageMesh() const { return this->backgroundImageMesh; }
        


        /// Updates the text meshes based on the text of the given element.
        ///
        /// @param element [in] The element whose text rendering data is being updated.
        void UpdateTextMeshes(const GUIElement &element);

        /// Update the colour of every text mesh.
        ///
        /// @param colour [in] The new text colour.
        void UpdateTextMeshesColour(const GT::ColorF &colour);


        /// Updates the shadow mesh for the given element.
        ///
        /// @param element [in] The element whose shadow is being updated.
        void UpdateShadowMesh(const GUIElement &element);


        /// Updates the border mesh for the given element.
        ///
        /// @param element [in] The element whose shadow is being updated.
        void UpdateBorderMesh(const GUIElement &element);


        /// Updates the background meshes.
        void UpdateBackgroundMeshes(const GUIElement &element);


        /// Sets the absolute opacity.
        ///
        /// @remarks
        ///     This is a simple setter. It does not update any meshes.
        void SetAbsoluteOpacity(float newAbsoluteOpacity);

        /// Retrieves the absolute opacity.
        float GetAbsoluteOpacity() const;


    private:
        
        /// Deletes the text meshes. This will delete them via the garbage collector.
        void DeleteTextMeshes();

        /// Deletes the shadow mesh.
        void DeleteShadowMesh();

        /// Deletes the border mesh.
        void DeleteBorderMesh();

        /// Deletes the background meshes.
        void DeleteBackgroundMeshes();

        /// Updates the absoluate opacity.
        void UpdateAbsoluteOpacity();


    private:

        /// The garbage collector to use for deleting meshes.
        GUIGarbageCollector &gc;

        /// The list of meshes used for rendering the text of the element.
        GT::Vector<GUIMesh*> textMeshes;

        /// The mesh for the shadow volume. This will be null if there is no shadow.
        GUIMesh* shadowMesh;

        /// The mesh for the borders.
        GUIMesh* borderMesh;

        /// The mesh for the background (not the background image; that is separate)
        GUIMesh* backgroundMesh;

        /// The mesh for the background image.
        GUIMesh* backgroundImageMesh;

        /// The absolute opacity.
        float absoluteOpacity;


    private:    // No copying.
        GUIElementRenderingData(const GUIElementRenderingData &);
        GUIElementRenderingData & operator=(const GUIElementRenderingData &);
    };
}

#endif