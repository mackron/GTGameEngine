
#ifndef __GTEngine_TransformGizmo_hpp_
#define __GTEngine_TransformGizmo_hpp_

#include "../SceneNode.hpp"

namespace GTEngine
{
    /// Class representing the positioning gizmo in an editor.
    class TransformGizmo
    {
    public:

        /// Constructor.
        TransformGizmo();

        /// Destructor.
        ~TransformGizmo();


        /// Initialises the gizmo.
        void Initialise();



        /// Sets the position of the gizmo.
        void SetPosition(const glm::vec3 &position);

        /// Retrieves the position of the gizmo.
        const glm::vec3 & GetPosition() const;


        /// Sets the rotation of the gizmo.
        ///
        /// @param cameraNode [in] The position of the camera.
        ///
        /// @remarks
        ///     This is only used for rotating the circles.
        void SetRotation(const glm::quat &rotation, const SceneNode &cameraNode);


        /// Sets the scale of the gizmo.
        ///
        /// @remarks
        ///     This will also update the picking shape.
        void SetScale(const glm::vec3 &scale);
        
        /// Retrieves the scale of the gizmo.
        const glm::vec3 & GetScale() const;



        /// Retrieves a reference the main scene node.
              SceneNode & GetSceneNode()       { return this->sceneNode; }
        const SceneNode & GetSceneNode() const { return this->sceneNode; }

        /// Retrieves a reference to the x axis arrow scene node.
              SceneNode & GetXArrowSceneNode()       { return this->xArrowSceneNode; }
        const SceneNode & GetXArrowSceneNode() const { return this->xArrowSceneNode; }

        /// Retrieves a reference to the y axis arrow scene node.
              SceneNode & GetYArrowSceneNode()       { return this->yArrowSceneNode; }
        const SceneNode & GetYArrowSceneNode() const { return this->yArrowSceneNode; }

        /// Retrieves a reference to the z axis arrow scene node.
              SceneNode & GetZArrowSceneNode()       { return this->zArrowSceneNode; }
        const SceneNode & GetZArrowSceneNode() const { return this->zArrowSceneNode; }


        /// Retrieves a reference to the y axis circle scene node.
              SceneNode & GetXCircleSceneNode()       { return this->xCircleSceneNode; }
        const SceneNode & GetXCircleSceneNode() const { return this->xCircleSceneNode; }

        /// Retrieves a reference to the y axis circle scene node.
              SceneNode & GetYCircleSceneNode()       { return this->yCircleSceneNode; }
        const SceneNode & GetYCircleSceneNode() const { return this->yCircleSceneNode; }

        /// Retrieves a reference to the z axis circle scene node.
              SceneNode & GetZCircleSceneNode()       { return this->zCircleSceneNode; }
        const SceneNode & GetZCircleSceneNode() const { return this->zCircleSceneNode; }

        /// Retrieves a reference to the y axis circle scene node.
              SceneNode & GetCameraFacingCircleSceneNode()       { return this->cameraFacingCircleSceneNode; }
        const SceneNode & GetCameraFacingCircleSceneNode() const { return this->cameraFacingCircleSceneNode; }


        /// Retrieves a reference to the x axis arrow scene node.
              SceneNode & GetXScaleSceneNode()       { return this->xScaleSceneNode; }
        const SceneNode & GetXScaleSceneNode() const { return this->xScaleSceneNode; }

        /// Retrieves a reference to the y axis arrow scene node.
              SceneNode & GetYScaleSceneNode()       { return this->yScaleSceneNode; }
        const SceneNode & GetYScaleSceneNode() const { return this->yScaleSceneNode; }

        /// Retrieves a reference to the z axis arrow scene node.
              SceneNode & GetZScaleSceneNode()       { return this->zScaleSceneNode; }
        const SceneNode & GetZScaleSceneNode() const { return this->zScaleSceneNode; }




        /// Shows the gizmo.
        void Show();

        /// Hides the gizmo.
        void Hide();


        /// Restores the colours of the gizmo axes to their defaults.
        void RestoreColours();

        /// Changes the colour of the given scene node.
        ///
        /// @param axisSceneNode [in] A reference to the scene node of the axis whose colour is being changed.
        void ChangeAxisColour(SceneNode &axisSceneNode, float r, float g, float b);


    private:

        /// Updates the picking volumes of the arrows.
        void UpdatePickingVolumes();

        /// Updates the vertex arrays of the given circles so that only the front facing lines are shown.
        ///
        /// @remarks
        ///     This does not change the vertices, only the indices.
        void UpdateCircleVertexArray(VertexArray* vertexArray, const SceneNode &circleNode, const SceneNode &cameraNode);



    private:

        /// The main scene node.
        SceneNode sceneNode;

        /// The scene node for the x axis arrow.
        SceneNode xArrowSceneNode;

        /// The scene node for the y axis arrow.
        SceneNode yArrowSceneNode;

        /// The scene node for the z axis arrow.
        SceneNode zArrowSceneNode;

        /// The scene node for the x axis circle.
        SceneNode xCircleSceneNode;

        /// The scene node for the y axis circle.
        SceneNode yCircleSceneNode;

        /// The scene node for the z axis circle.
        SceneNode zCircleSceneNode;

        /// The scene node for the camera-facing circle.
        SceneNode cameraFacingCircleSceneNode;

        /// The scene node for the x axis scale handle.
        SceneNode xScaleSceneNode;

        /// The scene node for the y axis scale handle.
        SceneNode yScaleSceneNode;

        /// The scene node for the z axis scale handle.
        SceneNode zScaleSceneNode;


        /// The model to use for the x axis arrow.
        Model xArrowModel;

        /// The model to use for the y axis arrow.
        Model yArrowModel;

        /// The model to use for the z axis arrow.
        Model zArrowModel;

        /// The model to use for the x axis circle.
        Model xCircleModel;

        /// The model to use for the y axis circle.
        Model yCircleModel;

        /// The model to use for the z axis circle.
        Model zCircleModel;

        /// The model to use for the camera-facing circle.
        Model cameraFacingCircleModel;

        /// The model to use for the x axis scale handle.
        Model xScaleModel;

        /// The model to use for the y axis scale handle.
        Model yScaleModel;

        /// The model to use for the z axis scale handle.
        Model zScaleModel;


        /// The geometry of the line part of the arrow models' mesh.
        VertexArray* arrowLineVA;

        /// The geometry of the head part of the arrow models' mesh.
        VertexArray* arrowHeadVA;


        /// The geometry of the x axis circle mesh.
        VertexArray* xCircleVA;

        /// The geometry of the y axis circle mesh.
        VertexArray* yCircleVA;

        /// The geometry of the z axis circle mesh.
        VertexArray* zCircleVA;

        /// The geometry of the camera-facing circle mesh.
        VertexArray* cameraFacingCircleVA;


        /// The geometry of the line part of the scale handle.
        VertexArray* scaleLineVA;

        /// the geometry of the head part of the scale handle.
        VertexArray* scaleHeadVA;
    };
}

#endif