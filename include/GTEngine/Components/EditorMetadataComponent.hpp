// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_EditorMetadataComponent_hpp_
#define __GTEngine_EditorMetadataComponent_hpp_

#include "../Component.hpp"
#include "../Math.hpp"
#include "../Physics.hpp"
#include "../CollisionGroups.hpp"
#include "../SceneRenderer.hpp"
#include "../SceneNodeEventHandler.hpp"

namespace GTEngine
{
    class EditorMetadataComponent : public Component
    {
    public:

        /// Enumerator for constrolling how the custom model should be transformed.
        enum ModelTransformMode
        {
            ModelTransformMode_FromSceneNode = 0,
            ModelTransformMode_FaceCamera    = 1,
            ModelTransformMode_Custom        = 2
        };


        static const uint32_t ChangeFlag_Sprite;
        static const uint32_t ChangeFlag_DirectionArrow;
        static const uint32_t ChangeFlag_WireframeColour;
        static const uint32_t ChangeFlag_PickingCollisionGroup;
        static const uint32_t ChangeFlag_PickingMesh;


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


        /// Determines whether or not the sene node has an ancestor that is marked as selected.
        ///
        /// @remarks
        ///     If the scene node does not have a parent, this will return false.
        bool IsAncestorSelected() const;


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


        /// Clears the picking collision shape.
        void ClearPickingCollisionShape();



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
        bool IsShowingSprite() const { return this->IsUsingSprite(); }

        /// Retrieves a reference to the sprite mesh.
        const SceneRendererMesh & GetSpriteMesh() const { return this->spriteMesh; }

        /// Retrieves a pointer to the picking collision object of the sprite.
        ///
        /// @return
        ///     A pointer to the picking collision object of the sprite, or null if the component is not using a sprite.
        CollisionObject* GetSpritePickingCollisionObject();

        /// Retrieves the path of the texture being used by the sprite.
        const char* GetSpriteTexturePath() const;


        /// Updates the sprites transform based on the given camera.
        void UpdateSpriteTransform(const SceneNode &cameraNode);



        /// Shows the direction arrow model.
        ///
        /// @remarks
        ///     This does not have picking collision info.
        void ShowDirectionArrow();

        /// Hides the direction arrow model.
        void HideDirectionArrow();

        /// Determines whether or not the direction arrow is being shown.
        bool IsShowingDirectionArrow() const;

        /// Retrieves a reference to the direction arrow mesh.
        const SceneRendererMesh & GetDirectionArrowMesh() const { return this->directionArrowMesh; }

        /// Updates the transform of the direction arrow based on the owner scene node.
        void UpdateDirectionArrowTransform();



        /// Show the collision shape mesh.
        ///
        /// @remarks
        ///     This does not have picking collision info.
        void ShowCollisionShapeMesh();

        /// Hides the collision shape mesh.
        void HideCollisionShapeMesh();

        /// Determines whether or not the collision shape mesh is being shown.
        bool IsShowingCollisionShapeMesh() const;

        /// Retrieves a reference to the collision shape mesh.
        const SceneRendererMesh & GetCollisionShapeMesh() const { return this->collisionShapeMesh; }

        /// Updates the collision shape mesh geometry.
        void UpdateCollisionShapeMeshGeometry(const SceneNode &cameraNode, const btCollisionShape &shape);
        void UpdateCollisionShapeMeshGeometry(const SceneNode &cameraNode);

        /// Updates the collision shape transform.
        void UpdateCollisionShapeMeshTransform();

        /// Marks the collision shape mesh as needing to be updated.
        void MarkCollisionShapeMeshAsDirty();


        /// Show the proximity shape mesh.
        ///
        /// @remarks
        ///     This does not have picking collision info.
        void ShowProximityShapeMesh();

        /// Hides the collision shape mesh.
        void HideProximityShapeMesh();

        /// Determines whether or not the collision shape mesh is being shown.
        bool IsShowingProximityShapeMesh() const;

        /// Retrieves a reference to the collision shape mesh.
        const SceneRendererMesh & GetProximityShapeMesh() const { return this->proximityShapeMesh; }

        /// Updates the collision shape mesh geometry.
        void UpdateProximityShapeMeshGeometry(const SceneNode &cameraNode, const btCollisionShape &shape);
        void UpdateProximityShapeMeshGeometry(const SceneNode &cameraNode);

        /// Updates the collision shape transform.
        void UpdateProximityShapeMeshTransform();

        /// Marks the proximity shape mesh as needing to be updated.
        void MarkProximityShapeMeshAsDirty();



        /// Retrieves the relative path of the prefab the scene node is linked to.
        const char* GetPrefabRelativePath() const;

        /// Sets the relative path of the prefab the scene node id linked to.
        void SetPrefabRelativePath(const char* newPrefabRelativePath);

        /// Retrieves the ID of the prefab scene node the scene node of this component refers to.
        uint64_t GetPrefabID() const;

        /// Sets the ID of the prefab scene node the scene node of this component refers to.
        void SetPrefabID(uint64_t newPrefabID);

        /// Determines whether or not the scene node is linked to a prefab.
        bool IsLinkedToPrefab() const;

        /// Links the component to a prefab.
        void LinkToPrefab(const char* prefabRelativePath, uint64_t id);

        /// Unlinkes the component from the prefab.
        void UnlinkFromPrefab();



        /// Retrieves the flags that have been set for the call to OnChanged().
        uint32_t GetOnChangeFlags() const { return this->changeFlags; }



        ////////////////////////////////////////////////////////
        // Events.

        /// Called when the scene node has been transformed (not scaled).
        void OnSceneNodeTransform();

        /// Called when the scene node has been scaled.
        void OnSceneNodeScale();



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTLib::Serializer &serializer, unsigned int flags = 0U) const;

        /// Component::Deserialize()
        void Deserialize(GTLib::Deserializer &deserializer);



    private:

        /// Deletes the collision shape and sets the pointer to null.
        void DeleteCollisionShape();



    private:

        /// Keeps track of whether or not the scene node should be rendered on top of everything else. Depth testing will always be disabled when
        /// this is set. This is mainly intended for things like gizmos.
        bool alwaysShowOnTop;

        /// Keeps track of whether or not the picking collision shape should be that of the model.
        bool useModelForPickingShape;


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



        /// The collision object to use for the sprite. We use a pointer here because most objects won't actually be using sprites, thus we can save a bit of memory with them.
        CollisionObject* spritePickingCollisionObject;

        /// The collision shape to use for picking the sprite. This is a box shape.
        btBoxShape* spritePickingCollisionShape;

        /// The path of the texture to use for the sprite.
        GTLib::String spriteTexturePath;

        /// A pointer to the texture being used with the sprite.
        Texture2D* spriteTexture;

        /// The sprite scene renderer mesh object.
        SceneRendererMesh spriteMesh;


        /// The direction arrow mesh object that'll be added to the scene.
        SceneRendererMesh directionArrowMesh;


        /// The collision volume mesh.
        SceneRendererMesh collisionShapeMesh;

        /// Keeps track of whether or not the collision shape mesh is being shown.
        bool isShowingCollisionShapeMesh;

        /// Keeps track of whether or not the collision shape mesh needs updating.
        bool isCollisionShapeMeshDirty;

        /// Keeps track of whether or not the collision shape has a child shape whose visual representation depends on the 


        /// The proximity shape mesh.
        SceneRendererMesh proximityShapeMesh;

        /// Keeps track of whether or not the proximity shape mesh is being shown.
        bool isShowingProximityShapeMesh;

        /// Keeps track of whether or not the proximity shape mesh needs updating.
        bool isProximityShapeMeshDirty;


        /// The name of the prefab the scene node is part of. If this is empty, it means it is not linked to a prefab.
        GTLib::String prefabRelativePath;

        /// The index of the node in the prefab. If this is 0, it is the root.
        uint64_t prefabID;



        /// Flags that specify what has changed when OnChanged() is called. This is retrieved with GetOnChangeFlags().
        uint32_t changeFlags;



        /// The event handler to attach to the scene node. Needed to intercept transformations so the transform of external meshes can be updated.
        class SceneNodeEventHandler : public GTEngine::SceneNodeEventHandler
        {
        public:

            /// Called when the scene node is translated or rotated.
            void OnTransform(SceneNode &node);

            /// Called when the scene node is scaled.
            void OnScale(SceneNode &node);


        }sceneNodeEventHandler;


        GTENGINE_DECL_COMPONENT_ATTRIBS(EditorMetadataComponent)
    };
}

#endif