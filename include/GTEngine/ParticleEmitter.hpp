// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleEmitter_hpp_
#define __GTEngine_ParticleEmitter_hpp_

#include "Particle.hpp"
#include "ParticleList.hpp"
#include "ParticleFunction.hpp"
#include "MaterialLibrary.hpp"
#include "Rendering/VertexArray.hpp"
#include <GTCore/Vector.hpp>
#include <GTCore/Random.hpp>

namespace GTEngine
{
    /// Class used for emitting particles.
    ///
    /// The particles emitted by an emitter is basically just a big mesh.
    class ParticleEmitter
    {
    public:

        enum EmissionShapeType
        {
            EmissionShapeType_Cone   = 1,
            EmissionShapeType_Sphere = 2,
            EmissionShapeType_Box    = 3            
        };

        enum ParticleShapeType
        {
            ParticleShapeType_Billboard = 1,
            ParticleShapeType_Model     = 2,
        };



    public:
        
        /// Constructor.
        ParticleEmitter();

        /// Copy constructor.
        ParticleEmitter(const ParticleEmitter &other);


        /// Destructor.
        ~ParticleEmitter();


        /// Starts the emitter.
        ///
        /// @remarks
        ///     If the emitter is in burst mode, the initial burst of particles will occur when Update() is next called.
        void Start();

        /// Stops the emitter.
        ///
        /// @remarks
        ///     Stopping the emitter does not destroy the particles. So long as Update() is called, particles will continue to animate
        ///     until they are themselves or the emitter is destroyed.
        void Stop();


        /// Updates the emitter.
        ///
        /// @param deltaTimeInSeconds [in] The delta time for doing time-based movement.
        ///
        /// @remarks
        ///     This will update the relevant properties of each particle and also spawn/delete particles where necessary.
        ///     @par
        ///     This does not orientate particles towards the camera. That needs to be done at render time.
        void Update(double deltaTimeInSeconds, const glm::vec3 &gravity);


        /// Retrieves the position of the emitter.
        const glm::vec3 & GetPosition() const;

        /// Sets the position of the emitter.
        void SetPosition(const glm::vec3 &position);

        
        /// Retrieves the orientation of the emitter.
        const glm::quat & GetOrientation() const;

        /// Sets the orientation of the emitter.
        void SetOrientation(const glm::quat &orientation);


        /// Retrieves the normalized forward vector of the emitter.
        glm::vec3 GetForwardVector() const;


        /// Enables burst mode.
        void EnableBurstMode();

        /// Disables burst mode.
        void DisableBurstMode();

        /// Determines whether or not burst mode is enabled.
        bool IsBurstModeEnabled() const;


        /// Sets the duration of the emitter in seconds.
        ///
        /// @param durationInSeconds [in] The new duration of the emitter, in seconds.
        ///
        /// @remarks
        ///     If the duration is set to 0, the emitter will loop forever until it is explicitly stopped.
        void SetDurationInSeconds(double durationInSeconds);

        /// Retrieves the duration of the emitter.
        double GetDurationInSeconds() const;


        /// Sets the number of particles to emit per second, or in the case of burst mode the number of particles to emit during the burst.
        void SetEmissionRatePerSecond(double emissionRatePerSecond);

        /// Retrieves the number of particles to emit per second.
        double GetEmissionRatePerSecond() const;


        /// Retrieves the gravity factor.
        double GetGravityFactor() const;

        /// Sets the gravity factor.
        void SetGravityFactor(double newGravityFactor);



        /// Retrieves the type of the emission shape.
        EmissionShapeType GetEmissionShapeType() const;

        /// Sets the emission shape type.
        void SetEmissionShapeType(EmissionShapeType newEmissionShapeType);


        /// Sets the emission shape to a cone.
        void SetConeEmissionShape(float radius, float angle);

        /// Sets the emission shape to a sphere.
        void SetSphereEmissionShape(float radius);

        /// Sets the emission shape to a box.
        void SetBoxEmissionShape(float x, float y, float z);


        /// Retrieves the cone emission shape properties.
        ///
        /// @remarks
        ///     This is only a valid call if the shape is a cone. Results are undefined if this is not the case.
        void GetConeEmissionShape(float &radiusOut, float &angleOut) const;

        /// Retrieves the sphere emission shape properties.
        ///
        /// @remarks
        ///     This is only a valid call if the shape is a sphere. Results are undefined if this is not the case.
        void GetSphereEmissionShape(float &radiusOut) const;

        /// Retrieves the box emission shape properties.
        ///
        /// @remarks
        ///     This is only a valid call if the shape is a box. Results are undefined if this is not the case.
        void GetBoxEmissionShape(float &xOut, float &yOut, float &zOut) const;




        /// Retrieves the start speed of each particle.
        void GetStartSpeed(double &startSpeedMinOut, double &startSpeedMaxOut) const;

        /// Sets the start speed of each particle.
        void SetStartSpeed(double newStartSpeedMin, double newStartSpeedMax);
        void SetStartSpeed(double newStartSpeed) { this->SetStartSpeed(newStartSpeed, newStartSpeed); }
        


        /// Retrieves the lifetime of a particle.
        void GetLifetime(double &ifetimeMinOut, double &lifetimeMaxOut) const;

        /// Sets the lifetime of particles.
        void SetLifetime(double newLifetimeMin, double newLifetimeMax);
        void SetLifetime(double newLifetime) { this->SetLifetime(newLifetime, newLifetime); }


        




        /// Sets the material to use with each particle.
        bool SetMaterial(const char* relativePath);

        /// Retrieves a pointer to the material.
        Material* GetMaterial() const { return this->material; }



        /// Retrieves the number of functions attached to the emitter.
        size_t GetFunctionCount() const;

        /// Retrieves a reference to the function at the given index.
              ParticleFunction & GetFunction(size_t index);
        const ParticleFunction & GetFunction(size_t index) const;

        /// Removes a function by it's index.
        void RemoveFunctionByIndex(size_t index);

        /// Adds a new instance of the function of the given type.
        ///
        /// @param type The type of the function to instantiate.
        ///
        /// @return A reference to the new function.
        ParticleFunction & AddFunction(ParticleFunctionType type);





        /// Retrieves the number of alive particles.
        size_t GetParticleCount() const { return this->particles.GetCount(); }

        /// Retrieves a reference to the particle at the given index.
              Particle & GetParticle(size_t index)       { return this->particles.GetParticle(index); }
        const Particle & GetParticle(size_t index) const { return this->particles.GetParticle(index); }


        /// Retrieves a reference to the internal vertex array.
        VertexArray & GetVertexArray() const { return *this->vertexArray; }



    private:

        /// Determines whether or not an emission has taken place.
        bool HasDoneFirstEmission() const;

        /// Sets whether or not the first emission has taken place.
        void HasDoneFirstEmission(bool doneFirstEmission);


    private:

        struct ConeEmissionShape
        {
            ConeEmissionShape()
                : radius(1.0f), angle(25.0f)
            {
            }

            float radius;
            float angle;
        };

        struct SphereEmissionShape
        {
            SphereEmissionShape()
                : radius(1.0f)
            {
            }

            float radius;
        };

        struct BoxEmissionShape
        {
            BoxEmissionShape()
                : x(1.0f), y(1.0f), z(1.0f)
            {
            }

            float x;
            float y;
            float z;
        };

        enum Flags
        {
            BurstEnabled      = (1 << 1),
            DoneFirstEmission = (1 << 2)
        };


        /// The position of the emitter.
        glm::vec3 position;

        /// The orientation of the emitter.
        glm::quat orientation;


        /// Various boolean flags. Defaults to 0 (all false).
        uint32_t flags;

        /// Keeps track of the amount of time to emit particles in seconds. When this is set to 0, the emitter will emit particles infinately. If particles
        /// are being bursted, this will control the amount of time between bursts.
        double durationInSeconds;

        /// The number of particles to spawn per second. If burst mode is enabled, this will control the number of particles to burst.
        double emissionRatePerSecond;

        /// The factor to apply to gravity.
        double gravityFactor;


        /// The emission shape type.
        EmissionShapeType emissionShapeType;

        /// The emission shape.
        ConeEmissionShape   emissionShapeCone;
        SphereEmissionShape emissionShapeSphere;
        BoxEmissionShape    emissionShapeBox;



        
        /// The starting speed of the particles.
        double startSpeedMin;
        double startSpeedMax;

        /// The starting rotation of the particles.
        glm::vec3 startRotationMin;
        glm::vec3 startRotationMax;

        /// The lifetime of a particle.
        double lifetimeMin;
        double lifetimeMax;



        /// The material that will be cloned for each particle.
        Material* material;


        /// The time since the last time a particle was emitted.
        double timeSinceLastEmission;

        /// The random number generator for spawning particles randomly.
        GTCore::Random random;


        // TODO: Use a more efficient data structure here. Look at the same type of structure as the event messaging one.
        /// The flat list of alive particles.
        //GTCore::Vector<Particle> particles;
        ParticleList particles;


        /// The list of functions to apply to each particle as time goes on.
        GTCore::Vector<ParticleFunction*> functions;
        

        /// The vertex array for rendering. This will only be used by the renderer.
        mutable VertexArray* vertexArray;
        
        
    private:    // No assignment operator.
        ParticleEmitter & operator=(const ParticleEmitter &);
    };
}

#endif
