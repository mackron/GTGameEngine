// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Editor_Grid_hpp_
#define __GTEngine_Editor_Grid_hpp_

#include "../SceneRenderer.hpp"

namespace GTEngine
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
        EditorGrid(float spacing, unsigned int subdivisions, unsigned int lineCount);

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