// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_Editor_AxisArrows
#define GT_Editor_AxisArrows

#include "../SceneRenderer.hpp"
#include "../SceneNode.hpp"

namespace GT
{
    /// Class representing the arrows that show up in the top right corner (by default) to show the direction of the axes.
    class EditorAxisArrows
    {
    public:

        /// Constructor.
        EditorAxisArrows(Context &context);

        /// Destructor.
        ~EditorAxisArrows();



        /// Shows the axis arrows.
        void Show(SceneRenderer &renderer);

        /// Hides the axis arrows.
        void Hide(SceneRenderer &renderer);


        /// Updates the transformation of the arrows based on the given camera.
        ///
        /// @remarks
        ///     The arrows will be placed at the top right of the camera.
        void UpdateTransform(const SceneNode &camera);


        /// Sets the local position of the arrows.
        void SetLocalPosition(const glm::vec3 &newLocalPosition);

        /// Sets the local scale of the arrows.
        void SetLocalScale(const glm::vec3 &newLocalScale);



    private:

        /// Creates an arrow mesh of the given local transformation.
        ///
        /// @param localTransform [in]  The local transformation of the arrow.
        /// @param lineOut        [out] A pointer to the vertex array that will receive the arrow mesh.
        /// @param headOut        [out] A pointer to the vertex array that will receive the arrow mesh.
        void CreateArrowVertexArray(const glm::mat4 &localTransform, VertexArray* lineOut, VertexArray* headOut);



    private:

        /// The mesh representing the x axis arrow line.
        SceneRendererMesh xArrowLine;

        /// The mesh representing the y axis arrow line.
        SceneRendererMesh yArrowLine;

        /// The mesh representing the z axis arrow line.
        SceneRendererMesh zArrowLine;


        /// The mesh representing the x axis arrow head.
        SceneRendererMesh xArrowHead;

        /// The mesh representing the y axis arrow head.
        SceneRendererMesh yArrowHead;

        /// The mesh representing the z axis arrow head.
        SceneRendererMesh zArrowHead;


        /// The definition of the material to use with the arrows.
        MaterialDefinition materialDefinition;



        /// The local position of the arrows.
        glm::vec3 localPosition;

        /// The local scale of the arrows.
        glm::vec3 localScale;
    };
}

#endif