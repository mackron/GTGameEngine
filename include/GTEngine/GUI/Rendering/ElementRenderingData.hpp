// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef GT_GUI_ElementRenderingData
#define GT_GUI_ElementRenderingData

#include "Mesh.hpp"
#include "../GarbageCollector.hpp"
#include <GTEngine/Core/Vector.hpp>

namespace GTGUI
{
    class Element;

    /// Class containing the rendering data for an element.
    ///
    /// This is mainly just a bunch of Mesh objects for the different parts of an element.
    ///
    /// The garbage collector is used for deleting internal meshes. This is passed to the constructor.
    class ElementRenderingData
    {
    public:

        /// Constructor.
        ElementRenderingData(GarbageCollector &gc);

        /// Destructor.
        ~ElementRenderingData();


        /// Retrieves a reference to the list of meshes used to render the text.
              GT::Vector<Mesh*> & GetTextMeshes()       { return this->textMeshes; }
        const GT::Vector<Mesh*> & GetTextMeshes() const { return this->textMeshes; }

        /// Retrieves a pointer to the shadow mesh.
              Mesh* GetShadowMesh()       { return this->shadowMesh; }
        const Mesh* GetShadowMesh() const { return this->shadowMesh; }

        /// Retrieves a pointer to the border mesh.
              Mesh* GetBorderMesh()       { return this->borderMesh; }
        const Mesh* GetBorderMesh() const { return this->borderMesh; }

        /// Retrieves a pointer to the background mesh.
              Mesh* GetBackgroundMesh()       { return this->backgroundMesh; }
        const Mesh* GetBackgroundMesh() const { return this->backgroundMesh; }

        /// Retrieves a pointer to the background image mesh.
              Mesh* GetBackgroundImageMesh()       { return this->backgroundImageMesh; }
        const Mesh* GetBackgroundImageMesh() const { return this->backgroundImageMesh; }
        


        /// Updates the text meshes based on the text of the given element.
        ///
        /// @param element [in] The element whose text rendering data is being updated.
        void UpdateTextMeshes(const Element &element);

        /// Update the colour of every text mesh.
        ///
        /// @param colour [in] The new text colour.
        void UpdateTextMeshesColour(const GT::Colour &colour);


        /// Updates the shadow mesh for the given element.
        ///
        /// @param element [in] The element whose shadow is being updated.
        void UpdateShadowMesh(const Element &element);


        /// Updates the border mesh for the given element.
        ///
        /// @param element [in] The element whose shadow is being updated.
        void UpdateBorderMesh(const Element &element);


        /// Updates the background meshes.
        void UpdateBackgroundMeshes(const Element &element);


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
        GarbageCollector &gc;

        /// The list of meshes used for rendering the text of the element.
        GT::Vector<Mesh*> textMeshes;

        /// The mesh for the shadow volume. This will be null if there is no shadow.
        Mesh* shadowMesh;

        /// The mesh for the borders.
        Mesh* borderMesh;

        /// The mesh for the background (not the background image; that is separate)
        Mesh* backgroundMesh;

        /// The mesh for the background image.
        Mesh* backgroundImageMesh;

        /// The absolute opacity.
        float absoluteOpacity;


    private:    // No copying.
        ElementRenderingData(const ElementRenderingData &);
        ElementRenderingData & operator=(const ElementRenderingData &);
    };
}

#endif