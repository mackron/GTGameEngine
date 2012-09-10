
#ifndef __GTEngine_ModelComponent_hpp_
#define __GTEngine_ModelComponent_hpp_

#include "../Component.hpp"
#include "../Model.hpp"

namespace GTEngine
{
    GTENGINE_DECL_COMPONENT_START(ModelComponent)
    public:

        ModelComponent(SceneNode &node);
        ~ModelComponent();


        /// Sets the model.
        void SetModel(Model* model, bool takeOwnership = false);

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


        /// Retrieves the model currently associated with this component.
              Model* GetModel()       { return this->model; }
        const Model* GetModel() const { return this->model; }


        /// Sets the back/front face culling mode. By default, back faces are culled.
        ///
        /// @param cullFront [in] Specifies whether or not front faces should be drawn.
        /// @param cullBack  [in] Specifies whether or not back faces should be drawn.
        void SetFaceCulling(bool cullFront, bool cullBack);

        /// Determines whether or not front faces should be culled.
        bool CullFrontFaces() const { return this->cullFrontFaces; }

        /// Determines whether or not back faces should be culled.
        bool CullBackFaces() const { return this->cullBackFaces; }

        
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
        bool IsShadowCastingEnabled() const { return this->castShadow; }



    private:

        /// A pointer to the applicable model.
        Model *model;

        /// Keeps track of whether or not to draw front faces.
        bool cullFrontFaces;

        /// Keeps track of whether or not back faces should be drawn.
        bool cullBackFaces;
        

        /// Keeps track of whether or not this model casts a shadow.
        bool castShadow;


        /// Keeps track of whether or not the component owns the model. If this is 'true', the component will destruct
        /// the model when it is destructed.
        bool isOwner;

    GTENGINE_DECL_COMPONENT_END()   
}

#endif
