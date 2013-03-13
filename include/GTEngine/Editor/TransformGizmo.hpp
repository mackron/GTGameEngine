// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

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
        void SetPosition(const glm::vec3 &position, const SceneNode &cameraNode);

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
        void SetScale(const glm::vec3 &scale, const SceneNode &cameraNode);

        /// Retrieves the scale of the gizmo.
        const glm::vec3 & GetScale() const;


        /// Updates the transform of every gizmo.
        void UpdateHandleTransforms(const SceneNode &cameraNode);



        /// Retrieves a reference the main scene node.
              SceneNode & GetSceneNode()       { return this->sceneNode; }
        const SceneNode & GetSceneNode() const { return this->sceneNode; }

        /// Retrieves a reference to the x axis arrow scene node.
        //      SceneNode & GetXArrowSceneNode()       { return this->xArrowSceneNode; }
        //const SceneNode & GetXArrowSceneNode() const { return this->xArrowSceneNode; }

        /// Retrieves a reference to the y axis arrow scene node.
        //      SceneNode & GetYArrowSceneNode()       { return this->yArrowSceneNode; }
        //const SceneNode & GetYArrowSceneNode() const { return this->yArrowSceneNode; }

        /// Retrieves a reference to the z axis arrow scene node.
        //      SceneNode & GetZArrowSceneNode()       { return this->zArrowSceneNode; }
        //const SceneNode & GetZArrowSceneNode() const { return this->zArrowSceneNode; }


        /*
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
        */

        /*
        /// Retrieves a reference to the x axis arrow scene node.
              SceneNode & GetXScaleSceneNode()       { return this->xScaleSceneNode; }
        const SceneNode & GetXScaleSceneNode() const { return this->xScaleSceneNode; }

        /// Retrieves a reference to the y axis arrow scene node.
              SceneNode & GetYScaleSceneNode()       { return this->yScaleSceneNode; }
        const SceneNode & GetYScaleSceneNode() const { return this->yScaleSceneNode; }

        /// Retrieves a reference to the z axis arrow scene node.
              SceneNode & GetZScaleSceneNode()       { return this->zScaleSceneNode; }
        const SceneNode & GetZScaleSceneNode() const { return this->zScaleSceneNode; }
        */



        /// Shows the gizmo.
        void Show(SceneRenderer &renderer, CollisionWorld &pickingWorld);

        /// Hides the gizmo.
        void Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld);


        /// Shows the translation handles and hides the others.
        void ShowTranslationHandles(SceneRenderer &renderer, CollisionWorld &pickingWorld);

        /// Shows the rotation handles and hides the others.
        void ShowRotationHandles(SceneRenderer &renderer, CollisionWorld &pickingWorld);

        /// Shows the scale handles and hides the others.
        void ShowScaleHandles(SceneRenderer &renderer, CollisionWorld &pickingWorld);



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
        //void UpdateCircleVertexArray(VertexArray* vertexArray, const SceneNode &circleNode, const SceneNode &cameraNode);




    public:

        /// Enumerator for the different types of handles (translation, rotation, scale).
        enum HandleType
        {
            HandleType_Translate,
            HandleType_Rotate,
            HandleType_Scale
        };

        /// Enumerator for the different axes a handle operates on. This can be used in a bit field to allow a
        /// handle to operate on multiple axes.
        enum HandleAxis
        {
            HandleAxis_X           = (1 << 1),
            HandleAxis_Y           = (1 << 2),
            HandleAxis_Z           = (1 << 3),
            HandleAxis_FrontFacing = (1 << 4)
        };


        /// Base structure reprsenting the mesh of a handle.
        struct Handle
        {
            /// The type of the handle. Needed for casting.
            HandleType type;

            /// The axes the handle operates on.
            uint32_t axis;

            /// The material to use with the handle.
            Material* material;

            /// The base colour of the handle (usually red, green or blue). Used to restore the colour after it's changed.
            glm::vec3 baseColour;

            /// The picking object. The shape will be defined and set by sub-classes.
            CollisionObject pickingObject;

            
            


            /// Constructor.
            Handle(HandleType type, uint32_t axis);

            /// Destructor.
            virtual ~Handle();


            /// Sets the colour of the handle.
            ///
            /// @remarks
            ///     Restore the colour with RestoreColour().
            void SetColour(const glm::vec3 &colour);

            /// Restores the colour to the base colour.
            void RestoreBaseColour();

            /// Virtual method for retrieving the forward vector.
            virtual glm::vec3 GetForwardVector() const { return glm::vec3(0.0f, 0.0f, 0.0f); }
        };


        /// Structure representing a translation arrow.
        struct TranslateHandle : public Handle
        {
            /// The local orientation of the handle.
            glm::mat3 localOrientation;

            /// The mesh for the arrow head.
            SceneRendererMesh headMesh;

            /// The mesh for the arrow line.
            SceneRendererMesh lineMesh;

            /// The collision shape of the handle. An offset is applied, so we need to use a compound shape.
            btCompoundShape pickingShape;

            /// The forward vector.
            glm::vec3 forwardVector;



            /// Constructor.
            TranslateHandle(HandleAxis axis);

            /// Destructor.
            ~TranslateHandle();


            /// Handle::GetForwardVector().
            glm::vec3 GetForwardVector() const;


            /// Updates the transform of the mesh.
            void UpdateTransform(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale);


            /// Shows the handle.
            void Show(SceneRenderer &renderer, CollisionWorld &pickingWorld);

            /// Hides the handle.
            void Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld);
        };


        /// Structure representing a rotation handle.
        struct RotateHandle : public Handle
        {
            /// The local orientation of the handle.
            glm::mat3 localOrientation;

            /// The circle mesh.
            SceneRendererMesh mesh;

            /// The collision shape of the handle. An offset is applied, so we need to use a compound shape.
            btCompoundShape pickingShape;

            /// The capsule shape to use for each segment of the picking shape. The picking shape is a torus made up of these shapes in a compound shape.
            btCapsuleShape pickingShapeSegment;

            /// The forward vector.
            glm::vec3 forwardVector;


            /// Constructor.
            RotateHandle(HandleAxis axis);

            /// Destructor.
            ~RotateHandle();


            /// Handle::GetForwardVector().
            glm::vec3 GetForwardVector() const;


            /// Updates the transform of the mesh.
            void UpdateTransform(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale, const glm::mat4 &viewMatrix);


            /// Shows the handle.
            void Show(SceneRenderer &renderer, CollisionWorld &pickingWorld);

            /// Hides the handle.
            void Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld);
        };


        /// Structure representing a scale handle
        struct ScaleHandle : public Handle
        {
            /// The local orientation of the handle.
            glm::mat3 localOrientation;

            /// The mesh for the arrow head.
            SceneRendererMesh headMesh;

            /// The mesh for the arrow line.
            SceneRendererMesh lineMesh;

            /// The collision shape of the handle. An offset is applied, so we need to use a compound shape.
            btCompoundShape pickingShape;

            /// The forward vector.
            glm::vec3 forwardVector;



            /// Constructor.
            ScaleHandle(HandleAxis axis);

            /// Destructor.
            ~ScaleHandle();


            /// Handle::GetForwardVector().
            glm::vec3 GetForwardVector() const;


            /// Updates the transform of the mesh.
            void UpdateTransform(const glm::vec3 &position, const glm::quat &orientation, const glm::vec3 &scale);


            /// Shows the handle.
            void Show(SceneRenderer &renderer, CollisionWorld &pickingWorld);

            /// Hides the handle.
            void Hide(SceneRenderer &renderer, CollisionWorld &pickingWorld);
        };




    private:

        /// The position of the gizmo.
        glm::vec3 position;

        /// The orientation of the gizmo.
        glm::quat orientation;

        /// The scale of the gizmo.
        glm::vec3 scale;


        /// Keeps track of whether or not the translation handles are showing.
        bool showingTranslationHandles;

        /// Keeps track of whether or not the rotation handles are showing.
        bool showingRotationHandles;

        /// Keeps track of whether or not the scale handles are showing.
        bool showingScaleHandles;



        /// The main scene node.
        SceneNode sceneNode;

        /// The scene node for the x axis arrow.
        //SceneNode xArrowSceneNode;

        /// The scene node for the y axis arrow.
        //SceneNode yArrowSceneNode;

        /// The scene node for the z axis arrow.
        //SceneNode zArrowSceneNode;

        /// The scene node for the x axis circle.
        //SceneNode xCircleSceneNode;

        /// The scene node for the y axis circle.
        //SceneNode yCircleSceneNode;

        /// The scene node for the z axis circle.
        //SceneNode zCircleSceneNode;

        /// The scene node for the camera-facing circle.
        //SceneNode cameraFacingCircleSceneNode;

        /// The scene node for the x axis scale handle.
        //SceneNode xScaleSceneNode;

        /// The scene node for the y axis scale handle.
        //SceneNode yScaleSceneNode;

        /// The scene node for the z axis scale handle.
        //SceneNode zScaleSceneNode;


        /// The model to use for the x axis arrow.
        //Model xArrowModel;

        /// The model to use for the y axis arrow.
        //Model yArrowModel;

        /// The model to use for the z axis arrow.
        //Model zArrowModel;

        /// The model to use for the x axis circle.
        //Model xCircleModel;

        /// The model to use for the y axis circle.
        //Model yCircleModel;

        /// The model to use for the z axis circle.
        //Model zCircleModel;

        /// The model to use for the camera-facing circle.
        //Model cameraFacingCircleModel;

        /// The model to use for the x axis scale handle.
        //Model xScaleModel;

        /// The model to use for the y axis scale handle.
        //Model yScaleModel;

        /// The model to use for the z axis scale handle.
        //Model zScaleModel;



        


        /// The x translation handle.
        TranslateHandle xTranslateHandle;

        /// The y translation handle.
        TranslateHandle yTranslateHandle;

        /// The z translation handle.
        TranslateHandle zTranslateHandle;


        /// The x rotation handle.
        RotateHandle xRotateHandle;

        /// The y rotation handle.
        RotateHandle yRotateHandle;

        /// The z rotation handle.
        RotateHandle zRotateHandle;

        /// The front-facing rotation handle.
        RotateHandle cameraFacingRotateHandle;


        /// The x scale handle.
        ScaleHandle xScaleHandle;

        /// The y scale handle.
        ScaleHandle yScaleHandle;

        /// The z scale handle.
        ScaleHandle zScaleHandle;




        /// The geometry of the line part of the arrow models' mesh.
        //VertexArray* arrowLineVA;

        /// The geometry of the head part of the arrow models' mesh.
        //VertexArray* arrowHeadVA;


        /// The geometry of the x axis circle mesh.
        //VertexArray* xCircleVA;

        /// The geometry of the y axis circle mesh.
        //VertexArray* yCircleVA;

        /// The geometry of the z axis circle mesh.
        //VertexArray* zCircleVA;

        /// The geometry of the camera-facing circle mesh.
        //VertexArray* cameraFacingCircleVA;


        /// The geometry of the line part of the scale handle.
        //VertexArray* scaleLineVA;

        /// the geometry of the head part of the scale handle.
        //VertexArray* scaleHeadVA;


    private:    // No copying.
        TransformGizmo(const TransformGizmo &);
        TransformGizmo & operator=(const TransformGizmo &);
    };
}

#endif
