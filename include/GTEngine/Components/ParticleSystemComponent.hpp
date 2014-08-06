// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_ParticleSystemComponent_hpp_
#define __GTEngine_ParticleSystemComponent_hpp_

#include "../Component.hpp"
#include "../ParticleSystem.hpp"
#include "../SceneNodeEventHandler.hpp"

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



        /// Updates the transformation of the particle system based on the scene node.
        void UpdateTransform();



        /// Marks the particle system as playing.
        void Play();

        /// Marks the particle system as paused (default).
        void Pause();

        /// Determines whether or not the particle system is playing.
        bool IsPlaying() const;


        /// Sets whether or not to play the particle system on startup.
        void PlayOnStartup(bool playOnStartup);

        /// Determines whether or not the particle system should be played on startup.
        bool IsPlayingOnStartup() const;


        /// Resets the particle system.
        ///
        /// @remarks
        ///     This does not change anything to do with the playback state. All this does is resets the internal particle system.
        void Reset();


        /// Retrieves the lifetime of the particle system.
        ///
        /// @param emitterIndex   [in]  The index of the emitter whose lifetime is being retrieved.
        /// @param lifetimeMinOut [out] A reference to the variable that will receive the min lifetime.
        /// @param lifetimeMaxOut [out] A reference to the variable that will receive the max lifetime.
        void GetEmitterLifetime(size_t emitterIndex, double &lifetimeMinOut, double &lifetimeMaxOut) const;


        ///////////////////////////////////////////////////////
        // Serialization/Deserialization.

        /// Component::Serialize()
        void Serialize(GTLib::Serializer &serializer) const;

        /// Component::Deserialize()
        void Deserialize(GTLib::Deserializer &deserializer);



    private:

        /// A pointer to the particle system.
        ParticleSystem* particleSystem;

        /// Keeps track of whether or not the component has taken ownership of the particle system.
        bool isOwner;

        /// Keeps track of whether or not the particle system is currently playing.
        bool isPlaying;

        /// Keeps track of whether or not to start playing the particle system on startup.
        bool playOnStartup;


        /// The event handler to attach to the scene node. This is used only to detect when it has had it's transformation changed
        /// so that we can re-orientation the particle system.
        struct EventHandler : public SceneNodeEventHandler
        {
            /// SceneNodeEventHandler::OnTransform().
            void OnTransform(SceneNode &node);

        }sceneNodeEventHandler;


        GTENGINE_DECL_COMPONENT_ATTRIBS(ParticleSystemComponent)
    };
}

#endif