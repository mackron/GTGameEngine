
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



        /// Sets the model.
        void SetModel(Model* model, bool takeOwnership = false);
        void SetModel(Model &model, bool takeOwnership = false);

        /// Sets the model from a file.
        ///
        /// @param fileName [in] The name of the model file to load.
        ///
        /// @return A pointer to the model that was loaded by the component.
        ///
        /// @remarks
        ///     This will load the model via the model library and will take ownership, which means it will be deleted whenever the
        ///     component no longer uses it (either when it's change, or when the component is destructed).
        Model* SetModel(const char* fileName);

        /// Unsets the current model.
        void UnsetModel();


        /// Retrieves the model currently associated with this component.
              Model* GetModel()       { return this->model; }
        const Model* GetModel() const { return this->model; }


        /// Specifies whether or not to use a custom transform for the model.
        void UseCustomModelTransform(bool useCustomTransform, const glm::mat4 &customTransform = glm::mat4());

        /// Determines whether or not we are using a custom transform for the model.
        bool IsUsingCustomModelTransform() const { return this->useCustomModelTransform; }

        /// Retrieves the custom model transform.
        const glm:: mat4 & GetCustomModelTransform() const { return this->customModelTransform; }



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


        /// The picking group.
        short pickingCollisionGroup;


        /// Keeps track of whether or not we own the model. We use this for determining whether or not we need to memory manage it ourselves.
        bool ownsModel;

        /// Keeps track of whether or not to use a custom transformation for rendering the model.
        bool useCustomModelTransform;

        /// The model to drawn in addition to the model contained within the ModelComponent. We need this to draw things like sprites, but don't want
        /// them to be defined in ModelComponent.
        Model* model;

        /// The custom transformation to use with the model.
        glm::mat4 customModelTransform;

        


    GTENGINE_DECL_COMPONENT_END()
}

#endif