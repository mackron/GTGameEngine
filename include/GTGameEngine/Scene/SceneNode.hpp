// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNode
#define GT_SceneNode

#include <GTGameEngine/Config.hpp>
#include <GTGameEngine/Math.hpp>
#include <GTGameEngine/AlignedType.hpp>
#include <cstdint>

#include "SceneNodeComponentTypes.hpp"

#define GT_MAX_SCENE_NODE_COMPONENTS        8

namespace GT
{
    class Scene;
    class SceneNodeComponent;

    /// Class representing a scene node.
    class alignas(16) SceneNode : public SIMDAlignedType
    {
    public:

        /// [Internal Use Only] Default constructor.
        ///
        /// @remarks
        ///     This will initialize the ID to 0.
        SceneNode(Scene &scene);

        /// Constructor.
        SceneNode(Scene &scene, uint64_t id);

        /// Destructor.
        ///
        /// @remarks
        ///     This will detach every component to ensure they are aware they are no longer owned by this scene node. It will not delete the components.
        ~SceneNode();


        /// Retrieves a reference to the scene that owns this scene node.
        Scene & GetScene();

        /// Retrieves the scene node's unique ID.
        uint64_t GetID() const;


        /// Retrieves the name of the scene node.
        ///
        /// @return A pointer to a null-terminated string containing the name of the scene node.
        const char* GetName() const;

        /// Sets the name of the scene node.
        ///
        /// @param newName [in] A pointer to the null-terminated string containing the new name.
        ///
        /// @remarks
        ///     The length of the name cannot be more than 31. In this case, the string will be trimmed.
        void SetName(const char* newName);


        /// Retrieves the scene node's tag.
        ///
        /// @return A pointer to a null-terminated string containing the tag of the scene node.
        const char* GetTag() const;

        /// Sets the tag of the scene node.
        ///
        /// @param newTag [in] A pointer to the null terminated string containing the new tag.
        ///
        /// @remarks
        ///     The length of the tag cannot be more than 31. In this case, the string will be trimmed.
        void SetTag(const char* newTag);


        /// Retrieves the number of components that are attached to the scene node.
        ///
        /// @return The number of components currently attached to the scene node.
        ///
        /// @remarks
        ///     A scene node has a maximum of 8 component slots.
        unsigned int GetComponentCount() const;

        /// Retrieves a pointer to the component at the given index.
        ///
        /// @param index [in] The index of the component to retrieve.
        ///
        /// @return A pointer to the component at the given index, or null if the component does not exist at that index.
        SceneNodeComponent* GetComponentByIndex(unsigned int index);

        /// Retrieves a pointer to the component of the given type, if any.
        ///
        /// @param typeID [in] The type of the component to retrieve.
        ///
        /// @return A pointer to the attached component with the given type, or null if a component of the given type is not attached to the scene node.
        SceneNodeComponent* GetComponentByTypeID(SceneNodeComponentTypeID typeID);
        const SceneNodeComponent* GetComponentByTypeID(SceneNodeComponentTypeID typeID) const;




        /// Attaches the given component.
        ///
        /// @param component [in] A reference to the component to add.
        ///
        /// @return True if the component was attached successfully; false otherwise.
        ///
        /// @remarks
        ///     This will fail if a component of the same type is already attached.
        bool AttachComponent(SceneNodeComponent &component);
        bool AttachComponent(SceneNodeComponent* component) { if (component != nullptr) { return this->AttachComponent(*component); } else { return false; }}

        /// Detaches the given component.
        ///
        /// @param component [in] A reference to the component to detach.
        void DetachComponent(SceneNodeComponent &component);
        void DetachComponent(SceneNodeComponent* component) { if (component != nullptr) { this->DetachComponent(*component); }}

        /// Detaches the component of the given type, if any.
        ///
        /// @param typeID [in] The type of the component to detach.
        ///
        /// @remarks
        ///     If a component of the given type is not current attached, this will do nothing.
        void DetachComponentByTypeID(SceneNodeComponentTypeID typeID);

        /// Detaches the component at the given index.
        ///
        /// @param index [in] The index of the component to detach.
        void DetachComponentByIndex(unsigned int index);

        /// Detaches every component.
        void DetachAllComponents();


        /// Sets the scene node to static.
        ///
        /// @remarks
        ///     When this is changed, the scene node will need to be removed from the scene and re-inserted before it will take effect.
        ///     @par
        ///     This is just an optimization, and does not strictly need to be set.
        void SetStatic(bool isStatic);

        /// Determines whether or not the scene node is static.
        bool IsStatic() const;


        /// Retrieves the position of the scene node.
        vec4 GetPosition() const;
        vec2 GetPosition2D() const;

        /// Retrieves the rotation of the scene node.
        quat GetRotation() const;

        /// Retrieves the scane of the scene node.
        vec4 GetScale() const;
        vec2 GetScale2D() const;


        /// Sets the position of the scene node.
        void SetPosition(const vec4 &position);
        void SetPosition2D(const vec2 &position);

        /// Sets the rotation of the scene node.
        void SetRotation(const quat &rotation);

        /// Sets the scale of the scene node.
        void SetScale(const vec4 &scale);
        void SetScale2D(const vec2 &scale);



        /// Retrieves a vector pointing in the forward direction.
        vec4 GetForward() const;
        vec2 GetForward2D() const;

        /// Retrieves a vector pointing in the right direction.
        vec4 GetRight() const;
        vec2 GetRight2D() const;

        /// Retrieves a vector pointing in the left direction.
        vec4 GetUp() const;
        vec2 GetUp2D() const;


        ///////////////////////////////////
        // Helpers

        template <typename T>
        T* GetComponent()
        {
            return reinterpret_cast<T*>(this->GetComponentByTypeID(T::GetTypeID()));
        }

        template <typename T>
        const T* GetComponent() const
        {
            return reinterpret_cast<const T*>(this->GetComponentByTypeID(T::GetTypeID()));
        }




        ///////////////////////////////////
        // Internal Use Only

        /// [Internal Use Only] Retrieves the scene node's flags.
        uint16_t _GetFlags() const;

        /// [Internal Use Only] Sets the scene node's flags.
        void _SetFlags(uint16_t flags);



    private:

        /// The unique ID of the scene node.
        uint64_t m_id;

        /// The name of the scene node. Does not need to be unique. Maximum of 31 characters, with the last character always padded with a null terminator. When
        /// the string is less than 31 characters, the buffer is padded with 0.
        char m_name[32];

        /// The scene nodes tag. This can be used to categorize scene nodes.
        char m_tag[32];

        /// The list of pointers to the components attached to the scene node.
        SceneNodeComponent* m_components[GT_MAX_SCENE_NODE_COMPONENTS];

        /// The number of components attached to the scene node.
        uint16_t m_componentCount;

        /// The various flags used by the scene node.
        uint16_t m_flags;


        /// Padding. Not used.
        uint32_t m_padding0;


        /// The position of the scene node.
        vec4 m_position;

        /// The rotation of the scene node.
        quat m_rotation;

        /// The scale of the scene node.
        vec4 m_scale;



        /// A pointer to the scene that owns this scene node.
        Scene &m_scene;


    public:

        static const uint16_t _PositionChanged  = (1 << 0);      //< [Internal Use Only] Set when the position has changed.
        static const uint16_t _RotationChanged  = (1 << 1);      //< [Internal Use Only] Set when the rotation has changed.
        static const uint16_t _ScaleChanged     = (1 << 2);      //< [Internal Use Only] Set when the scale has changed.
        static const uint16_t _IsDeleted        = (1 << 3);      //< [Internal Use Only] Set when the scene node is marked as deleted.
        static const uint16_t _IsDisabled       = (1 << 4);      //< [Internal Use Only] Set when the scene node is marked as disabled. A disabled scene node is not stepped nor does it have events posted for it.
        static const uint16_t _IsInScene        = (1 << 5);      //< [Internal Use Only] Set when the scene node is currently within it's scene.

        static const uint16_t _UpdatingDisabled = (1 << 14);     //< Set when the updates are disable on this scene node.
        static const uint16_t _IsStatic         = (1 << 15);     //< Set when the scene node is marked as static.
    };
}

#endif
