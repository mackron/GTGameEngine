// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ModelComponent
#define GT_ModelComponent

#include "../Component.hpp"
#include "../Model.hpp"

namespace GT
{
    class ModelComponent : public Component
    {
    public:

        static const uint32_t ChangeFlag_Flags;
        static const uint32_t ChangeFlag_Model;     // The model itself changed.
        static const uint32_t ChangeFlag_Material;  // A material on the model changed.

    public:

        ModelComponent(SceneNode &node);
        ~ModelComponent();


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
        ///     component no longer uses it (either when it's changed, or when the component is destructed).
        Model* SetModel(const char* fileName, const char* relativeTo = nullptr);

        /// Unsets the current model.
        void UnsetModel();


        /// Retrieves the model currently associated with this component.
              Model* GetModel()       { return this->model; }
        const Model* GetModel() const { return this->model; }


        /// Sets/replaces a material on the model by it's index.
        ///
        /// @param materialIndex        [in] The index of the material being changed.
        /// @param materialRelativePath [in] The relative path of the new material.
        ///
        /// @return True if the material is set successfully; false otherwise. If the material is the same, true will be returned.
        bool SetMaterial(size_t materialIndex, const char* materialRelativePath);


        /// Sets the back/front face culling mode. By default, back faces are culled.
        ///
        /// @param cullFront [in] Specifies whether or not front faces should be drawn.
        /// @param cullBack  [in] Specifies whether or not back faces should be drawn.
        void SetFaceCulling(bool cullFront, bool cullBack);

        /// Determines whether or not front faces should be culled.
        bool IsCullingFrontFaces() const { return (this->flags & CullFrontFaces) != 0; }

        /// Determines whether or not back faces should be culled.
        bool IsCullingBackFaces() const { return (this->flags & CullBackFaces) != 0; }

        
        /// Enables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void EnableShadowCasting();

        /// Disables shadow casting.
        ///
        /// @remarks
        ///     Shadow casting is enabled by default.
        void DisableShadowCasting();

        /// Determines whether or not this model casts shadows.
        bool IsShadowCastingEnabled() const { return (this->flags & CastShadow) != 0; }


        /// Shows the model.
        ///
        /// @remarks
        ///     This is NOT the same as SceneNode::Show(). The scene node's visibility state will not be changed.
        void ShowModel()
        {
            this->flags |= Visible;
            this->OnChanged();
        }

        /// Hides the model.
        ///
        /// @remarks
        ///     This is NOT the same as SceneNode::Hide(). The scene node's visibility state will not be changed, and instead only the model will be hidden.
        void HideModel()
        {
            this->flags &= ~Visible;
            this->OnChanged();
        }

        /// Determines whether or not the model is visible.
        bool IsModelVisible() const { return (this->flags & Visible) != 0; }



        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(Deserializer &deserializer);


    private:

        /// A pointer to the applicable model.
        Model *model;


        enum Flags
        {
            CullFrontFaces = (1 << 0),
            CullBackFaces  = (1 << 1),
            CastShadow     = (1 << 2),
            Visible        = (1 << 3),
            Owner          = (1 << 4),
        };

        /// The flags for this model.
        uint32_t flags;



        GTENGINE_DECL_COMPONENT_ATTRIBS(ModelComponent)
    };
}

#endif
