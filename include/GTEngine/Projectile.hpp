
#ifndef __GTEngine_Projectile_hpp_
#define __GTEngine_Projectile_hpp_

#include "Math.hpp"

namespace GTEngine
{
    /// Base class representing a modelled projectile.
    ///
    /// This class can be used for (mostly) realistic projectiles. This is not 100% physically accuracte, but accurate enough
    /// for most games.
    ///
    /// Any projectile can be represented by this class, but some considerations must be handled. First, the projectile is
    /// represented as an infinitely small point. So, for things will large-ish volumes, such as rockets, collision detection
    /// will not be fully accurate.
    ///
    /// This class is designed to be inherited. An inherited class can customize the projectile to their liking, such as for
    /// attaching a scene node for visual representation or whatnot.
    ///
    /// A note of probable importance is that this class is NOT implemented as a SceneNode or GameObject. If this functionality
    /// is desired, a sub-class should be created with the appropriate virtual method implementations. The base Projectile class
    /// works completely independantly. This allows the greatest flexibility.
    ///
    /// Since gravity is mostly constant for each projectile, there is not much point in storing a vector for gravity. Instead,
    /// the gravity is controlled with a virtual getter method called GetGravity(). This returns (0.0, -9.81, 0.0) by default.
    /// For a different value, an application will need to sub-class.
    ///
    /// Note that since this class has no relationship with scenes that it does not do collision detection. This needs to be
    /// handled by sub-classes.
    ///
    /// Velocities are always in units per second. The direction of the projectile's movement can be determined by looking at
    /// the velocity.
    ///
    /// The DefaultProjectile class is an easier-to-use Projectile implementation that performs collision detection a scene, life
    /// times (with death events) and whatever other common functionality.
    class Projectile
    {
    public:

        /// Default constructor.
        Projectile();

        /// Destructor.
        virtual ~Projectile();


        /// Launches the projectile.
        ///
        /// @param position [in] The initial position of the projectile.
        /// @param velocity [in] The initial velocity of the projectile.
        ///
        /// @remarks
        ///     The projectile will be launched using the current properties. Set the properties before launching the projectile. It
        ///     is safe to change properties after launching, but if an Update() takes place beforehand, not everything may be correct.
        ///     @par
        ///     Use <velocity> to apply a relative velocity. This is useful for fireing projectiles from fast moving objects.
        void Launch(const glm::vec3 &position, const glm::vec3 &velocity);


        /// Updates the projectile (moves it).
        ///
        /// @param deltaTimeInSeconds [in] The delta to use for time based movement.
        ///
        /// @remarks
        ///     This method will move the projectile and call the appropriate virtual methods.
        void Update(double deltaTimeInSeconds);


        /// Retrieves the position of the projectile.
        const glm::vec3 & GetPosition() const { return this->position; }

        /// Retrieves the velocity of the projectile.
        ///
        /// @remarks
        ///     An application can use this to determine the direction of travel of the projectile, which in turn can allow the application
        ///     to do things like orientate the visual representation, for example.
        ///     @par
        ///     The velocity can also be used for checking the force of impact, which can be useful for physics or damage modelling.
        const glm::vec3 & GetVelocity() const { return this->velocity; }



        ////////////////////////////////////////////////////////////////////////////
        // Virtual methods. Sub-classes can implement these method for customization.
        
        /// Called just before the projectile is moved.
        ///
        /// @param deltaTimeInSeconds [in]      The delta time for time-based movement.
        /// @param targetPosition     [in, out] The target position.
        ///
        /// @remarks
        ///     A sub-class can control the movement of the projectile by chaning the target position. The direction and velocity of
        ///     the projectile can be determined by looking at the current and target positions. This could be useful for things such
        ///     as guided rockets or more exotic function based movement. Indeed, the movement of the projectile can be fully customized
        ///     by simply changing the target position. Keep in mind, however, that default computations will always be performed.
        ///     @par
        ///     This method will be called just before the projectile is moved. Once this returns, the position of the projectile will
        ///     be set to <targetPosition>, which will then in turn modify the direction and velocity.
        ///     @par
        ///     This is a good time to do collision detection.
        ///     @par
        ///     <deltaTimeInSeconds> can be used to keep track of the life time of the projectile.
        virtual void OnUpdate(double deltaTimeInSeconds, glm::vec3 &targetPosition);

        /// Called after the projectile is launched with Launch().
        ///
        /// @remarks
        ///     This is a good place to keep track of the initial velocity for things like damage modelling based on velocity relative
        ///     to the start velocity.
        virtual void OnLaunch();


        /// Retrieves the amount of gravity to apply to the projectile in units/s^2.
        ///
        /// @param gravity [out] A reference to the variable that will receive the gravity.
        ///
        /// @remarks
        ///     The default implementation will return (0, 0, 0), meaning no gravity. For Earth gravity, a sub-class should return (0, -9.81, 0).
        virtual void GetGravity(glm::vec3 &gravity) const;

        /// Retrieves the acceleration of the projectile units/s^2.
        ///
        /// @param acceleration [out] A reference to the variable that will receive the acceleration.
        ///
        /// @remarks
        ///     The default implementation will return (0, 0, 0), meaning no acceleration or deceleration.
        ///     @par
        ///     This can be used to apply a negative acceleration for things like a simplified air resistance approximation. It can also
        ///     be used for doing exotic movements such as dragging it over in a particular direction.
        virtual void GetAcceleration(glm::vec3 &acceleration) const;



    protected:

        /// The current position of the projectile.
        glm::vec3 position;

        /// Rhe projectile's current velocity.
        glm::vec3 velocity;
    };
}


// DefaultProjectile.
namespace GTEngine
{
    class  Scene;
    class  SceneNode;
    struct ClosestRayTestCallback;

    /// Class representing a default projectile that can be used in most situations.
    ///
    /// This projectile does collision detection against scene nodes in a given scene. The scene is set in the constructor.
    ///
    /// Like Projectile, this class does not use a SceneNode or GameObject.
    ///
    /// The gravity will be based on the scene's gravity. Use SetGravityFactor() to control how much gravity to apply. To disable gravity,
    /// do SetGravityFactor(0.0f). Alternatively, compensate for gravity with SetAcceleration(-scene.GetGravity()).
    ///
    /// This is a very generic implementation of Projectile. Consider a game-specific implementation if things are a bit too bloated in
    /// DefaultProjectile.
    class DefaultProjectile : public Projectile
    {
    public:

        /// Constructor.
        DefaultProjectile(Scene &scene, short collisionGroup = -1, short collisionMask = -1);

        /// Destructor.
        virtual ~DefaultProjectile();


        /// Sets the collision filter.
        ///
        /// @param group [in] The new collision group.
        /// @param mask  [in] The new collision mask.
        void SetCollisionFilter(short group, short mask);

        /// Retrieves the collision group.
        short GetCollisionGroup() const { return this->collisionGroup; }

        /// Retrieves the collision mask.
        short GetCollisionMask() const { return this->collisionMask; }

        /// Sets the acceleration of the projectile.
        ///
        /// @param acceleration [in] The new acceleration.
        void SetAcceleration(const glm::vec3 &acceleration);

        /// Sets the factor to apply to gravity.
        ///
        /// @param factor [in] The new gravity factor.
        void SetGravityFactor(const glm::vec3 &gravityFactor);

        /// Sets the scene node to ignore with collision detection.
        ///
        /// @param ignoredNode [in] A pointer to the scene node to ignore. Can be null.
        void SetIgnoredNode(SceneNode* ignoredNode);



        /////////////////////////////////////////////////////////
        // Virtual Implementations

        /// Projectile::OnUpdate().
        virtual void OnUpdate(double deltaTimeInSeconds, glm::vec3 &targetPosition);

        /// Projectile::GetGravity().
        virtual void GetGravity(glm::vec3 &gravity) const;

        /// Projectile::GetAcceleration().
        virtual void GetAcceleration(glm::vec3 &acceleration) const;


        /////////////////////////////////////////////////////////
        // Events.

        /// Called when the projectile collides with a scene node.
        virtual void OnCollide(SceneNode &, ClosestRayTestCallback &) {}



    protected:

        /// The scene that owns this projectile. The projectile will do collision detection against the objects in this scene.
        Scene &scene;

        /// The collision group for collision detection.
        short collisionGroup;

        /// The collision mask for collision detection.
        short collisionMask;

        /// The acceleration to apply to the projectile. This is controlled with SetAcceleration().
        glm::vec3 acceleration;

        /// The factor to apply to gravity. Set this to (0, 0, 0) to disable gravity.
        glm::vec3 gravityFactor;

        /// A projectile does not usually want to collide with whatever object launched it. This attribute allows an application to ignore
        /// collisions with a given scene node. This can be null.
        SceneNode* ignoredNode;


    private:    // No copying.
        DefaultProjectile(const DefaultProjectile &);
        DefaultProjectile & operator=(const DefaultProjectile &);
    };
}

#endif