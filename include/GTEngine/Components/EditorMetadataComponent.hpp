
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


        /// The picking group.
        short pickingCollisionGroup;


    GTENGINE_DECL_COMPONENT_END()
}

#endif