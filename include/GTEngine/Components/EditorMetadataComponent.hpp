
#ifndef __GTEngine_EditorMetadataComponent_hpp_
#define __GTEngine_EditorMetadataComponent_hpp_

#include "../Component.hpp"
#include "../Math.hpp"
#include "../Physics.hpp"
#include "../CollisionGroups.hpp"

namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(EditorMetadataComponent)
    public:

        /// Enumerator for constrolling how the custom model should be transformed.
        enum ModelTransformMode
        {
            ModelTransformMode_FromSceneNode = 0,
            ModelTransformMode_FaceCamera    = 1,
            ModelTransformMode_Custom        = 2
        };


    public:

        /// Constructor.
        EditorMetadataComponent(SceneNode &node);

        /// Destructor.
        ~EditorMetadataComponent();


        /// Determines whether or not the scene node is marked as selected.
        bool IsSelected() const;

        /// Marks the scene node as selected.
        void Select();

        /// Marks the scene node as deselected.
        void Deselect();


        /// Retrieves the selection wireframe colour.
        const glm::vec3 & GetSelectionWireframeColour() const;

        /// Sets the selection wireframe colour.
        void SetSelectionWireframeColour(const glm::vec3 &newColour);


        /// Retrieves a reference to the collision object to use for picking.
              CollisionObject & GetPickingCollisionObject()       { return this->pickingCollisionObject; }
        const CollisionObject & GetPickingCollisionObject() const { return this->pickingCollisionObject; }

        /// Retrieves a pointer to the collision shape to use for picking. Returns null if there is no collision shape.
              btCollisionShape * GetPickingCollisionShape()       { return this->pickingCollisionShape; }
        const btCollisionShape * GetPickingCollisionShape() const { return this->pickingCollisionShape; }


        /// Sets the picking collision shape to that of the model geometry.
        void SetPickingCollisionShapeToModel();

        /// Sets the picking shape to a box with a given offset.
        void SetPickingCollisionShapeToBox(const glm::vec3 &halfExtents, const glm::vec3 &offset);

        /// Sets the picking shape to a torus.
        ///
        /// @param outerRadius  [in] The outer radius of the torus, minus the inner radius.
        /// @param innerRadius  [in] The radius of the geometry making up the ring shape.
        /// @param subdivisions [in] The number of segments to use for the ring.
        ///
        /// @remarks
        ///     Currently, this is just a hacky compound shape made up of a series of cylinders.
        ///     \par
        ///     This will be created on the XY plane, looking down -Z.
        void SetPickingCollisionShapeToTorus(float outerRadius, float innerRadius, unsigned int subdivisions);


        /// Sets the picking collision group.
        void SetPickingCollisionGroup(short group);

        /// Retrieves the picking collision group.
        short GetPickingCollisionGroup() const;


        /// Determines whether or not the scene node should be rendered on top of everything else.
        bool IsAlwaysShownOnTop() const;

        /// Sets whether or not the scene node should be rendered on top of everything else.
        void SetAlwaysShowOnTop(bool alwaysOnTop);


        /// Determines whether or not the model is being used for the picking shape.
        bool UseModelForPickingShape() const { return this->useModelForPickingShape; }

        /// Sets whether or not the model should be used for the picking shape.
        void UseModelForPickingShape(bool useModelForPickingShapeIn) { this->useModelForPickingShape = useModelForPickingShapeIn; }


        /// Determines whether or not the scene node should be deleted when the editor is closed.
        bool DeleteOnClose() const { return this->deleteOnClose; }

        /// Sets whether or not the scene node should be deleted when the editor is closed.
        void DeleteOnClose(bool deleteOnCloseIn) { this->deleteOnClose = deleteOnCloseIn; }




        /// Shows a sprite on the object.
        void ShowSprite(const char* texturePath, const glm::vec3 &colour = glm::vec3(1.0f, 1.0f, 1.0f));
        void ShowSprite(const char* texturePath, float colourR, float colourG, float colourB);

        /// Hides the sprite.
        ///
        /// @remarks
        ///     This will actually deallocate everything also.
        void HideSprite();

        /// Applies the scene node transformation to the sprite picking collision shape.
        void ApplyTransformToSprite();

        /// Applies the scene node scaling to the sprite picking collision shape.
        void ApplyScaleToSprite();

        /// Determines whether or not a sprite is being used.
        ///
        /// @return
        ///     True if the sprite is being shown.
        bool IsUsingSprite() const;

        /// Retrieves a pointer to model of the sprite.
        ///
        /// @return
        ///     A pointer to the model that should be used to draw the sprite, or null if the component is not using a sprite.
        const Model* GetSpriteModel() const;

        /// Retrieves a pointer to the picking collision object of the sprite.
        ///
        /// @return
        ///     A pointer to the picking collision object of the sprite, or null if the component is not using a sprite.
        CollisionObject* GetSpritePickingCollisionObject();

        /// Retrieves the path of the texture being used by the sprite.
        const char* GetSpriteTexturePath() const;


        /// TEMP: Sets the transformation of the sprite.
        void SetSpriteTransform(const glm::mat4 &spriteTransformIn) { this->spriteTransform = spriteTransformIn; }

        /// TEMP: Retrieves the transformation of the sprite.
        const glm::mat4 & GetSpriteTransform() const { return this->spriteTransform; }




        /// Shows the direction arrow model.
        ///
        /// @remarks
        ///     This does not have picking collision info.
        void ShowDirectionArrow();

        /// Hides the direction arrow model.
        void HideDirectionArrow();

        /// Determines whether or not the direction arrow is being shown.
        bool IsShowingDirectionArrow() const;

        /// Retrieves a pointer to the model of the direction arrow.
        ///
        /// @remarks
        ///     A pointer to the model of the direction arrow, or null if it isn't being shown.
        const Model* GetDirectionArrowModel() const;



    private:

        /// Deletes the collision shape and sets the pointer to null.
        void DeleteCollisionShape();



    private:

        /// Keeps track of whether or not the scene node should be rendered on top of everything else. Depth testing will always be disabled when
        /// this is set. This is mainly intended for things like gizmos.
        bool alwaysShowOnTop;

        /// Keeps track of whether or not the picking collision shape should be that of the model.
        bool useModelForPickingShape;

        /// Keeps track of whether or not the scene node should be deleted when the editor is closed.
        bool deleteOnClose;


        /// Keeps track of whether or not the scene node is selected in the editor.
        bool isSelected;

        /// The colour of the wireframe to use when an object is selected.
        glm::vec3 selectionWireframeColour;

        /// The collision object to use for picking.
        CollisionObject pickingCollisionObject;

        /// The collision shape to use for picking. Need to use a pointer here because we might not always use the same kind of shape for everything.
        btCollisionShape* pickingCollisionShape;


        /// Enumerator for the type of collision shape we are using. We need this in determining how to handle certain things.
        enum PickingCollisionShapeType
        {
            PickingCollisionShapeType_None,
            PickingCollisionShapeType_Model,
            PickingCollisionShapeType_Box,
            PickingCollisionShapeType_Torus,

        }pickingCollisionShapeType;


        /// The picking group.
        short pickingCollisionGroup;


        /// The model to use for the sprite. This is always memory managed by the component itself.
        Model* spriteModel;

        /// The collision object to use for the sprite. We use a pointer here because most objects won't actually be using sprites, thus we can save a bit of memory with them.
        CollisionObject* spritePickingCollisionObject;

        /// The collision shape to use for picking the sprite. This is a box shape.
        btBoxShape* spritePickingCollisionShape;

        /// The path of the texture to use for the sprite.
        GTCore::String spriteTexturePath;

        /// TEMP: The sprite transformation.
        glm::mat4 spriteTransform;


        /// The model to use for the direction arrow.
        Model* directionArrowModel;

        /// The vertex array to use for the direction arrow.
        VertexArray* directionArrowVA;




    GTENGINE_DECL_COMPONENT_END()
}

#endif