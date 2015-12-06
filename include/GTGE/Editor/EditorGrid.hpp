// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Editor_Grid
#define GT_Editor_Grid

#include "../SceneRenderer.hpp"

namespace GT
{
    /// Class representing a grid in a 3D editing tool.
    ///
    /// The grid is always centered at the origin, on the X/Z plane.
    ///
    /// Be sure to remove the grid from any scene renderer before destructing.
    class EditorGrid
    {
    public:

        /// Constructor.
        EditorGrid(Context &context, float spacing, unsigned int subdivisions, unsigned int lineCount);

        /// Destructor.
        ~EditorGrid();


        /// Shows the grid.
        void Show(SceneRenderer &renderer);

        /// Hides the grid.
        void Hide(SceneRenderer &renderer);


        /// Resizes the grid.
        void Resize(float spacing, unsigned int subdivisions, unsigned int lineCount);


    private:

        /// The mesh for the inner spacing.
        SceneRendererMesh innerSpacingMesh;

        /// The mesh to use for the subdivision lines (darker lines). This includes the border lines.
        SceneRendererMesh subdivisionMesh;


        /// The material definition.
        MaterialDefinition materialDefinition;
    };
}

#endif