// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_TransformGizmo
#define GT_TransformGizmo

#include "../SceneNode.hpp"

namespace GT
{
    /// Class representing the positioning gizmo in an editor.
    class TransformGizmo
    {
    public:

        /// Constructor.
        TransformGizmo(MaterialLibrary &materialLibrary);

        /// Destructor.
        ~TransformGizmo();



        /// Sets the position of the gizmo.
        void SetPosition(const glm::vec3 &position, const SceneNode &cameraNode, bool onlyVisibleHandles);

        /// Retrieves the position of the gizmo.
        const glm::vec3 & GetPosition() const;


        /// Sets the rotation of the gizmo.
        ///
        /// @param cameraNode [in] The position of the camera.
        ///
        /// @remarks
        ///     This is only used for rotating the circles.
        void SetRotation(const glm::quat &rotation, const SceneNode &cameraNode, bool onlyVisibleHandles);


        /// Sets the scale of the gizmo.
        ///
        /// @remarks
        ///     This will also update the picking shape.
        void SetScale(const glm::vec3 &scale, const SceneNode &cameraNode, bool onlyVisibleHandles);

        /// Retrieves the scale of the gizmo.
        const glm::vec3 & GetScale() const;


        /// Sets the position, rotation and scale of the gizmo in one go.
        void SetTransform(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale, const SceneNode &cameraNode, bool updateOnlyVisibleHandles);


        /// Updates the transform of every gizmo.
        void UpdateHandleTransforms(const SceneNode &cameraNode, bool onlyVisibleHandles);



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


        /// Determines if the transform gizmo is visible.
        bool IsVisible() const { return this->isVisible; }



        /// Restores the colours of the gizmo axes to their defaults.
        void RestoreColours();


    private:


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
            /// A reference to the material library for loading the material to use when drawing the handle.
            MaterialLibrary &materialLibrary;

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
            Handle(MaterialLibrary &materialLibrary, HandleType type, uint32_t axis);

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


        private:    // No copying.
            Handle(const Handle &);
            Handle & operator=(const Handle &);
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

            /// The box collision shape to as the main shape. This is attached to the compound shape so that an offset can be applied.
            btBoxShape pickingShapeBox;

            /// The forward vector.
            glm::vec3 forwardVector;



            /// Constructor.
            TranslateHandle(MaterialLibrary &materialLibrary, HandleAxis axis);

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
            RotateHandle(MaterialLibrary &materialLibrary, HandleAxis axis);

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

            /// The box collision shape to as the main shape. This is attached to the compound shape so that an offset can be applied.
            btBoxShape pickingShapeBox;

            /// The forward vector.
            glm::vec3 forwardVector;



            /// Constructor.
            ScaleHandle(MaterialLibrary &materialLibrary, HandleAxis axis);

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
        glm::vec3 m_position;

        /// The orientation of the gizmo.
        glm::quat m_orientation;

        /// The scale of the gizmo.
        glm::vec3 m_scale;


        /// Keeps track of whether or not the gizmo itself is visible.
        bool isVisible;

        /// Keeps track of whether or not the translation handles are showing.
        bool showingTranslationHandles;

        /// Keeps track of whether or not the rotation handles are showing.
        bool showingRotationHandles;

        /// Keeps track of whether or not the scale handles are showing.
        bool showingScaleHandles;


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



    private:    // No copying.
        TransformGizmo(const TransformGizmo &);
        TransformGizmo & operator=(const TransformGizmo &);
    };
}

#endif
