// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include "../Component.hpp"
#include "../ParticleSystem.hpp"

namespace GTEngine
{
    class ParticleSystemComponent : public Component
    {
    public:

        /// Constructor.
        ParticleSystemComponent(SceneNode &sceneNode);

        /// Destructor.
        ~ParticleSystemComponent();



        /// Sets the particle system.
        void SetParticleSystem(ParticleSystem* newParticleSystem, bool takeOwnership = false);
        void SetParticleSystem(ParticleSystem &newParticleSystem, bool takeOwnership = false) { this->SetParticleSystem(&newParticleSystem, takeOwnership); }

        /// Sets the particle system from a file.
        ///
        /// @param fileName   [in] The name of the particle system file to load.
        /// @param relativeTo [in] If 'fileName' is absolute, it will be made relative to this path.
        ///
        /// @return A pointer to the particle system that was loaded by the component.
        ///
        /// @remarks
        ///     This will load the particle system via the particle system library and will take ownership, which means it will be deleted whenever the
        ///     component no longer uses it (either when it's changed, or when the component is destructed).
        ParticleSystem* SetParticleSystem(const char* fileName, const char* relativeTo = nullptr);

        /// Unsets the particle system.
        void UnsetParticleSystem();


        /// Retrieves a pointer to the particle system.
              ParticleSystem* GetParticleSystem()       { return this->particleSystem; }
        const ParticleSystem* GetParticleSystem() const { return this->particleSystem; }





        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTCore::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTCore::Deserializer &deserializer);



    private:

        /// A pointer to the particle system.
        ParticleSystem* particleSystem;

        /// Keeps track of whether or not the component has taken ownership of the particle system.
        bool isOwner;



        GTENGINE_DECL_COMPONENT_ATTRIBS(ParticleSystemComponent)
    };
}