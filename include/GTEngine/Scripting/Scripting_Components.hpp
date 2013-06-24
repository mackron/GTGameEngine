// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Components_hpp_
#define __GTEngine_Scripting_Components_hpp_

#include "../GameScript.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the scene node components library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        bool LoadComponentsLibrary(GTCore::Script &script);



        namespace CameraComponentFFI
        {
            /// Applies a 3D projection to the given component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The field-of-view.
            ///     Argument 3: The aspect ratio.
            ///     Argument 4: The near clipping plane.
            ///     Argument 5: The far clipping plane.
            int Set3DProjection(GTCore::Script &script);

            /// Retrieves the field of view of the given camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetFOV(GTCore::Script &script);

            /// Retrieves the aspect ratio of the given camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetAspectRatio(GTCore::Script &script);

            /// Retrieves the near clipping plane of the given camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetNearClippingPlane(GTCore::Script &script);

            /// Retrieves the far clipping plane of the given camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetFarClippingPlane(GTCore::Script &script);
        }


        namespace ModelComponentFFI
        {
            /// Sets the model for the given component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose model is being set.
            ///     Argument 2: The file path of the model being set.
            int SetModel(GTCore::Script &script);

            /// Retrieves the path of the model currently set on the given model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose model's path is being retrieved.
            int GetModelPath(GTCore::Script &script);

            /// Enables shadow casting.
            int EnableShadowCasting(GTCore::Script &script);

            /// Disables shadow casting.
            int DisableShadowCasting(GTCore::Script &script);

            /// Determines whether or not shadow casting is enabled.
            int IsShadowCastingEnabled(GTCore::Script &script);


            /// Sets a material of the model currently set on the given model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the material being set.
            ///     Argument 3: The name of the new material.
            int SetMaterial(GTCore::Script &script);

            /// Retrieves the name of the material at the given index of the model currently set on the given model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetMaterialPath(GTCore::Script &script);

            /// Retrieves the number of materials on the model currently attached to the given model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetMaterialCount(GTCore::Script &script);

            /// Sets the uniform parameter of the given material.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the material whose uniform is being set.
            ///     Argument 3: The name of the uniform being set.
            ///     Argument 4: The value of the uniform.
            ///
            ///     Allowable values types are:
            ///       - Scalar
            ///       - math.vec2
            ///       - math.vec3
            ///       - math.vec4
            int SetMaterialParameter(GTCore::Script &script);


            /// Plays an animation segment by it's name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the segment to play.
            ///     Argument 3: A boolean controlling whether or not to loop the animation.
            ///
            ///     It is possible for segments to share the same name. This will play the first occurance.
            int PlayAnimationSegmentByName(GTCore::Script &script);

            /// Plays the given animation sequence.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The AnimationSequence object to play.
            int PlayAnimationSequence(GTCore::Script &script);

            /// Pauses the currently playing animation, if any.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int PauseAnimation(GTCore::Script &script);

            /// Stops the currently playing animation, returning it to it's original pose.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int StopAnimation(GTCore::Script &script);


            /// Retrieves the transformation of the bone of the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the bone.
            ///     Return:     A table containing the attributes "position", "orientation" and "scale"
            ///
            ///     The returned values are the absolute transformation.
            int GetBoneTransformByName(GTCore::Script &script);

            /// Retrieves the transformation of the bone at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the bone.
            ///     Return:     A table containing the attributes "position", "orientation" and "scale"
            ///
            ///     The returned values are the absolute transformation.
            int GetBoneTransformByIndex(GTCore::Script &script);

            /// Retrieves the index of the bone of the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the bone whose index is being retrieved.
            ///
            ///     If the bone does not exist, nil will be returned.
            int GetBoneIndexByName(GTCore::Script &script);
        }

        namespace PointLightComponentFFI
        {
            /// Sets the colour of the light.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose colour is being set.
            ///     Argument 2: The red colour component, or a math.vec3 containing the colours.
            ///     Argument 3: The green colour component.
            ///     Argument 4: The blue colour component.
            int SetColour(GTCore::Script &script);

            /// Retrieves the colour of the light as a math.vec3.
            int GetColour(GTCore::Script &script);


            /// Retrieves the radius of the light.
            int GetRadius(GTCore::Script &script);

            /// Sets the radius of the light.
            int SetRadius(GTCore::Script &script);


            /// Retrieves the exponential falloff factor.
            int GetFalloff(GTCore::Script &script);

            /// Sets the exponential falloff factor.
            int SetFalloff(GTCore::Script &script);


            /// Enables shadow casting on the light.
            int EnableShadowCasting(GTCore::Script &script);

            /// Disables shadow casting on the light.
            int DisableShadowCasting(GTCore::Script &script);

            /// Determines whether or not shadow casting is enabled.
            int IsShadowCastingEnabled(GTCore::Script &script);
        }

        namespace SpotLightComponentFFI
        {
            /// Sets the colour of the light.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose colour is being set.
            ///     Argument 2: The red colour component, or a math.vec3 containing the colours.
            ///     Argument 3: The green colour component.
            ///     Argument 4: The blue colour component.
            int SetColour(GTCore::Script &script);

            /// Retrieves the colour of the light as a math.vec3.
            int GetColour(GTCore::Script &script);


            /// Retrieves the length of the light.
            int GetLength(GTCore::Script &script);

            /// Sets the length of the light.
            int SetLength(GTCore::Script &script);


            /// Retrieves the exponential falloff factor.
            int GetFalloff(GTCore::Script &script);

            /// Sets the exponential falloff factor.
            int SetFalloff(GTCore::Script &script);


            /// Enables shadow casting on the light.
            int EnableShadowCasting(GTCore::Script &script);

            /// Disables shadow casting on the light.
            int DisableShadowCasting(GTCore::Script &script);

            /// Determines whether or not shadow casting is enabled.
            int IsShadowCastingEnabled(GTCore::Script &script);


            /// Sets the inner and outer angles of the spot light.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The inner angle.
            ///     Argument 3: The outer angle.
            int SetAngles(GTCore::Script &script);

            /// Retrieves the inner and outer angles of the spot light, as two floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetAngles(GTCore::Script &script);
        }

        namespace DirectionalLightComponentFFI
        {
            /// Sets the colour of the light.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose colour is being set.
            ///     Argument 2: The red colour component, or a math.vec3 containing the colours.
            ///     Argument 3: The green colour component.
            ///     Argument 4: The blue colour component.
            int SetColour(GTCore::Script &script);

            /// Retrieves the colour of the light as a math.vec3.
            int GetColour(GTCore::Script &script);


            /// Enables shadow casting on the light.
            int EnableShadowCasting(GTCore::Script &script);

            /// Disables shadow casting on the light.
            int DisableShadowCasting(GTCore::Script &script);

            /// Determines whether or not shadow casting is enabled.
            int IsShadowCastingEnabled(GTCore::Script &script);
        }

        namespace AmbientLightComponentFFI
        {
            /// Sets the colour of the light.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose colour is being set.
            ///     Argument 2: The red colour component, or a math.vec3 containing the colours.
            ///     Argument 3: The green colour component.
            ///     Argument 4: The blue colour component.
            int SetColour(GTCore::Script &script);

            /// Retrieves the colour of the light as a math.vec3.
            int GetColour(GTCore::Script &script);
        }


        namespace CollisionShapeComponentFFI
        {
            /// Sets the collision filter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The collision group. Only the first 16 bits are used.
            ///     Argument 3: The collision mask. Only the first 16 bits are used.
            int SetCollisionFilter(GTCore::Script &script);

            /// Retrieves the collision group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroup(GTCore::Script &script);

            /// Retrieves the collision mask.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionMask(GTCore::Script &script);


            /// Adds a box collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The half x extent or a math.vec3 containing the half extents.
            ///     Argument 3: The half y extent or a math.vec3 containing the offset.
            ///     Argument 4: The half z extent.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddBoxShape(GTCore::Script &script);

            /// Adds a sphere collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 5: Optional x offset or a math.vec3 containing the offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddSphereShape(GTCore::Script &script);

            /// Adds an ellipsoid collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x radius or a math.vec3 containing the radius.
            ///     Argument 3: The y radius or a math.vec3 containing the offset.
            ///     Argument 4: The z radius.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddEllipsoidShape(GTCore::Script &script);

            /// Adds a cylinder collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The half x extent or a math.vec3 containing the half extents.
            ///     Argument 3: The half y extent or a math.vec3 containing the offset.
            ///     Argument 4: The half z extent.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCylinderXShape(GTCore::Script &script);
            int AddCylinderYShape(GTCore::Script &script);
            int AddCylinderZShape(GTCore::Script &script);

            /// Adds a capsure collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 3: The length.
            ///     Argument 5: Optional x offset or a math.vec3.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCapsuleXShape(GTCore::Script &script);
            int AddCapsuleYShape(GTCore::Script &script);
            int AddCapsuleZShape(GTCore::Script &script);

            /// Sets the collision shapes of the object to that of the convex hulls of the model in the Model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The margin to apply to the collision shapes.
            int SetCollisionShapesToModelConvexHulls(GTCore::Script &script);

            /// Removes every collision shape.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int RemoveAllShapes(GTCore::Script &script);

            /// Removes the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer ot the component to delete.
            ///     Argument 2: The index of the shape to delete.
            int RemoveShapeAtIndex(GTCore::Script &script);

            /// Retrieves the number of collision shapes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetShapeCount(GTCore::Script &script);

            /// Retrieves a table containing information about the collision shape of the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape whose information is being retrieved.
            int GetShapeAtIndex(GTCore::Script &script);

            /// Determines whether or not we are using the convex hull of a model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsUsingConvexHullsFromModel(GTCore::Script &script);

            /// Sets the offset for the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape whose offset is being changed.
            ///     Argument 3, 4, 5: The new offset, as 3 floats, or a math.vec3
            int SetShapeOffset(GTCore::Script &script);

            /// Sets the extents of the box shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a box, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents or a math.vec3.
            int SetBoxShapeHalfExtents(GTCore::Script &script);

            /// Sets the radius of the sphere shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a sphere, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new radius.
            int SetSphereShapeRadius(GTCore::Script &script);

            /// Sets the radius of the ellipsoid shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new radius as 3 floats, or a math.vec3.
            int SetEllipsoidShapeRadius(GTCore::Script &script);

            /// Sets the extents of the cylinder shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a cylinder, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents or a math.vec3.
            int SetCylinderShapeHalfExtents(GTCore::Script &script);

            /// Sets the size of the capsule shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The radius of the shape.
            ///     Argument 4: The height of the shape.
            int SetCapsuleShapeSize(GTCore::Script &script);
        }


        namespace DynamicsComponentFFI
        {
            /// Sets the mass of the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose mass is being set.
            ///     Argument 2: The new mass.
            int SetMass(GTCore::Script &script);

            /// Retrieves the mass of the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the componen whose mass is being retrieved.
            int GetMass(GTCore::Script &script);

            /// Determines whether or not the object is static.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsStatic(GTCore::Script &script);

            /// Determines or sets whether or not the object is kinematic.
            ///
            /// @remarks
            ///     Argument 1: A pointer the component.
            ///     Argument 2: Whether or not the object should be made kinematic, or nil.
            ///
            ///     If the second argument is nil, this will act as a getter.
            int IsKinematic(GTCore::Script &script);


            /// Sets the friction.
            ///
            /// @remarks
            ///     Argument 1: A pointer ot the component.
            ///     Argument 2: The friction to apply to the value.
            int SetFriction(GTCore::Script &script);

            /// Retrieves the friction.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetFriction(GTCore::Script &script);

            /// Sets the restituion (bounciness).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The restitution.
            int SetRestitution(GTCore::Script &script);

            /// Retrieves the restitution (bounciness).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetRestitution(GTCore::Script &script);

            /// Sets the damping.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The linear damping.
            ///     Argument 3: The angular damping.
            int SetDamping(GTCore::Script &script);

            /// Retrieves the linear damping.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetLinearDamping(GTCore::Script &script);

            /// Retrieves the angular damping.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetAngularDamping(GTCore::Script &script);


            /// Sets the linear velocity.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x velocity or a math.vec3().
            ///     Argument 3: The y velocity.
            ///     Argument 4: The z velocity.
            int SetLinearVelocity(GTCore::Script &script);

            /// Retrieves the linear velocity as 3 floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     The return value will be a math.vec3()
            int GetLinearVelocity(GTCore::Script &script);

            /// Sets the angular velocity.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x velocity or a math.vec3().
            ///     Argument 3: The y velocity.
            ///     Argument 4: The z velocity.
            int SetAngularVelocity(GTCore::Script &script);

            /// Retrieves the linear velocity as 3 floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     The return value will be a math.vec3()
            int GetAngularVelocity(GTCore::Script &script);


            /// Sets the linear factor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x factor or a math.vec3().
            ///     Argument 3: The y factor.
            ///     Argument 4: The z factor.
            int SetLinearFactor(GTCore::Script &script);

            /// Sets the angular factor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x factor or a math.vec3().
            ///     Argument 3: The y factor.
            ///     Argument 4: The z factor.
            int SetAngularFactor(GTCore::Script &script);


            /// Sets the gravity to use with the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x gravity or a math.vec3().
            ///     Argument 3: The y gravity.
            ///     Argument 4: The z gravity.
            int SetGravity(GTCore::Script &script);

            /// Retrieves the gravity being used with the object, as 3 floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     The return value will be a math.vec3()
            int GetGravity(GTCore::Script &script);

            /// Applies the gravity of the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int ApplyGravity(GTCore::Script &script);


            /// Applies a central force to the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x force or a math.vec3().
            ///     Argument 3: The y force.
            ///     Argument 4: The z force.
            int ApplyCentralForce(GTCore::Script &script);

            /// Applies a torque to the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x torque or a math.vec3().
            ///     Argument 3: The y torque.
            ///     Argument 4: The z torque.
            int ApplyTorque(GTCore::Script &script);

            /// Applies an impulse to the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: A math.vec3() containing the impule.
            ///     Argument 3: A math.vec3() containing the relative position to apply the impulse.
            int ApplyImpulse(GTCore::Script &script);

            /// Applies a central impulse to the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x impulse or a math.vec3().
            ///     Argument 3: The y impulse.
            ///     Argument 4: The z impulse.
            int ApplyCentralImpulse(GTCore::Script &script);

            /// Applies a torque impulse.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x impulse or a math.vec3().
            ///     Argument 3: The y impulse.
            ///     Argument 4: The z impulse.
            int ApplyTorqueImpulse(GTCore::Script &script);


            /// Disables deactivation of the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int DisableDeactivation(GTCore::Script &script);

            /// Enables deactivation of an object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int EnableDeactivation(GTCore::Script &script);

            /// Determines whether or not deactivation is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsDeactivationEnabled(GTCore::Script &script);

            /// Activates the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int Activate(GTCore::Script &script);


            /// Enables navigation mesh generation for this object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int EnableNavigationMeshGeneration(GTCore::Script &script);

            /// Disables navigation mesh generation for this object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int DisableNavigationMeshGeneration(GTCore::Script &script);

            /// Determines whether or not navigation mesh generation is enabled.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the component.
            int IsNavigationMeshGenerationEnabled(GTCore::Script &script);



            /// Sets the collision filter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The collision group. Only the first 16 bits are used.
            ///     Argument 3: The collision mask. Only the first 16 bits are used.
            int SetCollisionFilter(GTCore::Script &script);

            /// Retrieves the collision group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroup(GTCore::Script &script);

            /// Retrieves the collision mask.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionMask(GTCore::Script &script);


            /// Adds a box collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The half x extent.
            ///     Argument 3: The half y extent.
            ///     Argument 4: The half z extent.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddBoxCollisionShape(GTCore::Script &script);

            /// Adds a sphere collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddSphereCollisionShape(GTCore::Script &script);

            /// Adds an ellipsoid collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x radius.
            ///     Argument 3: The y radius.
            ///     Argument 4: The z radius.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddEllipsoidCollisionShape(GTCore::Script &script);

            /// Adds a cylinder collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The half x extent.
            ///     Argument 3: The half y extent.
            ///     Argument 4: The half z extent.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCylinderXCollisionShape(GTCore::Script &script);
            int AddCylinderYCollisionShape(GTCore::Script &script);
            int AddCylinderZCollisionShape(GTCore::Script &script);

            /// Adds a capsure collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 3: The length.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCapsuleXCollisionShape(GTCore::Script &script);
            int AddCapsuleYCollisionShape(GTCore::Script &script);
            int AddCapsuleZCollisionShape(GTCore::Script &script);

            /// Sets the collision shapes of the object to that of the convex hulls of the model in the Model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The margin to apply to the collision shapes.
            int SetCollisionShapesToModelConvexHulls(GTCore::Script &script);

            /// Removes every collision shape.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int RemoveAllCollisionShapes(GTCore::Script &script);

            /// Removes the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer ot the component to delete.
            ///     Argument 2: The index of the shape to delete.
            int RemoveCollisionShapeAtIndex(GTCore::Script &script);

            /// Retrieves the number of collision shapes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionShapeCount(GTCore::Script &script);

            /// Retrieves a table containing information about the collision shape of the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape whose information is being retrieved.
            int GetCollisionShapeAtIndex(GTCore::Script &script);

            /// Determines whether or not we are using the convex hull of a model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsUsingConvexHullsFromModel(GTCore::Script &script);

            /// Sets the offset for the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape whose offset is being changed.
            ///     Argument 3, 4, 5: The new offset, as 3 floats
            int SetCollisionShapeOffset(GTCore::Script &script);

            /// Sets the extents of the box shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a box, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents.
            int SetBoxCollisionShapeHalfExtents(GTCore::Script &script);

            /// Sets the radius of the sphere shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a sphere, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new radius.
            int SetSphereCollisionShapeRadius(GTCore::Script &script);

            /// Sets the radius of the ellipsoid shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new radius as 3 floats.
            int SetEllipsoidCollisionShapeRadius(GTCore::Script &script);

            /// Sets the extents of the cylinder shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a cylinder, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents.
            int SetCylinderCollisionShapeHalfExtents(GTCore::Script &script);

            /// Sets the size of the capsule shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The radius of the shape.
            ///     Argument 4: The height of the shape.
            int SetCapsuleCollisionShapeSize(GTCore::Script &script);
        }

        namespace ProximityComponentFFI
        {
            /// Sets the collision filter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The collision group. Only the first 16 bits are used.
            ///     Argument 3: The collision mask. Only the first 16 bits are used.
            int SetCollisionFilter(GTCore::Script &script);

            /// Retrieves the collision group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroup(GTCore::Script &script);

            /// Retrieves the collision mask.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionMask(GTCore::Script &script);


            /// Adds a box collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The half x extent.
            ///     Argument 3: The half y extent.
            ///     Argument 4: The half z extent.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddBoxCollisionShape(GTCore::Script &script);

            /// Adds a sphere collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddSphereCollisionShape(GTCore::Script &script);

            /// Adds an ellipsoid collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x radius.
            ///     Argument 3: The y radius.
            ///     Argument 4: The z radius.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddEllipsoidCollisionShape(GTCore::Script &script);

            /// Adds a cylinder collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The half x extent.
            ///     Argument 3: The half y extent.
            ///     Argument 4: The half z extent.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCylinderXCollisionShape(GTCore::Script &script);
            int AddCylinderYCollisionShape(GTCore::Script &script);
            int AddCylinderZCollisionShape(GTCore::Script &script);

            /// Adds a capsure collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 3: The length.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCapsuleXCollisionShape(GTCore::Script &script);
            int AddCapsuleYCollisionShape(GTCore::Script &script);
            int AddCapsuleZCollisionShape(GTCore::Script &script);

            /// Sets the collision shapes of the object to that of the convex hulls of the model in the Model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The margin to apply to the collision shapes.
            int SetCollisionShapesToModelConvexHulls(GTCore::Script &script);

            /// Removes every collision shape.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int RemoveAllCollisionShapes(GTCore::Script &script);

            /// Removes the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer ot the component to delete.
            ///     Argument 2: The index of the shape to delete.
            int RemoveCollisionShapeAtIndex(GTCore::Script &script);

            /// Retrieves the number of collision shapes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionShapeCount(GTCore::Script &script);

            /// Retrieves a table containing information about the collision shape of the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape whose information is being retrieved.
            int GetCollisionShapeAtIndex(GTCore::Script &script);

            /// Determines whether or not we are using the convex hull of a model.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsUsingConvexHullsFromModel(GTCore::Script &script);

            /// Sets the offset for the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape whose offset is being changed.
            ///     Argument 3, 4, 5: The new offset, as 3 floats
            int SetCollisionShapeOffset(GTCore::Script &script);

            /// Sets the extents of the box shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a box, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents.
            int SetBoxCollisionShapeHalfExtents(GTCore::Script &script);

            /// Sets the radius of the sphere shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a sphere, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new radius.
            int SetSphereCollisionShapeRadius(GTCore::Script &script);

            /// Sets the radius of the ellipsoid shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new radius as 3 floats.
            int SetEllipsoidCollisionShapeRadius(GTCore::Script &script);

            /// Sets the extents of the cylinder shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a cylinder, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents.
            int SetCylinderCollisionShapeHalfExtents(GTCore::Script &script);

            /// Sets the size of the capsule shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The radius of the shape.
            ///     Argument 4: The height of the shape.
            int SetCapsuleCollisionShapeSize(GTCore::Script &script);
        }


        namespace ScriptComponentFFI
        {
            /// Adds a script file to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The relative path of the script file being added.
            int AddScript(GTCore::Script &script);

            /// Removes a script file from the component by it's relative path.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The relative path of the script file being removed.
            int RemoveScriptByRelativePath(GTCore::Script &script);

            /// Removes a script file by it's index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the script being removed.
            int RemoveScriptByIndex(GTCore::Script &script);

            /// Reloads a script at the given index with a new file.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the script being reloaded.
            int ReloadScript(GTCore::Script &script);

            /// Retrieves the names of the script files the given script component uses.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetScriptFilePaths(GTCore::Script &script);

            /// Determines if the script component is using the given script file.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The relative path of the script.
            int IsUsingScript(GTCore::Script &script);

            /// Retrieves an array containing the names and types of the public variables of the script definition at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the script definition whose public variables are being retried.
            ///
            ///     The return value is an array of {name, type} types.
            int GetPublicVariableNamesAndTypesByIndex(GTCore::Script &script);


            /// Retrieves the names and values of every public variable.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///
            ///     The returned table is keyed by the name. Note that this is out of order.
            int GetPublicVariableNamesAndValues(GTCore::Script &script);

            /// Retrieves the value of the public variable with the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the variable whose value is being retrieved.
            int GetPublicVariableValue(GTCore::Script &script);

            /// Sets the value of the public variable with the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the variable whose value is being set.
            ///     Argument 3: The value to assign to the variable.
            ///
            ///     If the value is an unsupported type, nothing will be changed.
            int SetPublicVariableValue(GTCore::Script &script);
        }


        namespace ParticleSystemComponentFFI
        {
            /// Sets the particle system from a relative file path.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The relative path of the particle system to set.
            int SetFromFile(GTCore::Script &script);

            /// Retrieves the relative file path of the attached particle system.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetRelativeFilePath(GTCore::Script &script);


            /// Marks the particle system as playing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int Play(GTCore::Script &script);

            /// Marks the particle system as paused.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int Pause(GTCore::Script &script);

            /// Determines whether or not the particle system is playing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsPlaying(GTCore::Script &script);


            /// Sets whether or not the particle system should be played on startup.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: Boolean: True if the particle system should be played on startup.
            int PlayOnStartup(GTCore::Script &script);

            /// Determines whether or not the particle system is playing on startup.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsPlayingOnStartup(GTCore::Script &script);


            /// Resets the particle system.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle system.
            int Reset(GTCore::Script &script);
        }


        namespace PrefabComponentFFI
        {
            /// Retrieve the relative path of the prefab.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetPrefabRelativePath(GTCore::Script &script);

            /// Retrieves the local ID of the scene node this component is linked to in the prefab.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetLocalHierarchyID(GTCore::Script &script);
        }


        namespace EditorMetadataComponentFFI
        {
            /// Marks the node as selected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     Note that this isn't enough to fully select the node. Instead, this simply marks the node as selected.
            int Select(GTCore::Script &script);

            /// Marks the node as deselected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     Note that this isn't enough to fully deselect the node. Instead, this simply marks the node as selected.
            int Deselect(GTCore::Script &script);

            /// Determines whether or not the node is marked as selected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     Returns a boolean.
            int IsSelected(GTCore::Script &script);


            /// Shows a sprite on the object while it's shown in the editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose sprite is being shown.
            ///     Argument 2: The file path of the texture to use on the sprite.
            ///     Argument 3: A table containing the colour to use with the sprite. Keys are r, g and b.
            int ShowSprite(GTCore::Script &script);

            /// Hides the sprite.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose sprite is being hidden.
            int HideSprite(GTCore::Script &script);

            /// Determines whether or not a sprite is being shown.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsShowingSprite(GTCore::Script &script);

            /// Retrieves the path of the texture being used for the sprite.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose sprite texture is being retrieved.
            int GetSpriteTexturePath(GTCore::Script &script);


            /// Shows a direction on the object while it's shown in the editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose direction arrow is being shown.
            int ShowDirectionArrow(GTCore::Script &script);

            /// Hides the direction arrow.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose direction arrow is being hidden.
            int HideDirectionArrow(GTCore::Script &script);

            /// Determines whether or not the direction arrow is being shown.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsShowingDirectionArrow(GTCore::Script &script);
        }
    }
}

#endif