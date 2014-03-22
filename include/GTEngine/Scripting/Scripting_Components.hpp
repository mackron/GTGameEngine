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
        bool LoadComponentsLibrary(GTLib::Script &script);



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
            int Set3DProjection(GTLib::Script &script);

            /// Retrieves the field of view of the given camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetFOV(GTLib::Script &script);

            /// Retrieves the aspect ratio of the given camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetAspectRatio(GTLib::Script &script);

            /// Retrieves the near clipping plane of the given camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetNearClippingPlane(GTLib::Script &script);

            /// Retrieves the far clipping plane of the given camera.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetFarClippingPlane(GTLib::Script &script);
        }


        namespace ModelComponentFFI
        {
            /// Sets the model for the given component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose model is being set.
            ///     Argument 2: The file path of the model being set.
            int SetModel(GTLib::Script &script);

            /// Retrieves the path of the model currently set on the given model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose model's path is being retrieved.
            int GetModelPath(GTLib::Script &script);

            /// Enables shadow casting.
            int EnableShadowCasting(GTLib::Script &script);

            /// Disables shadow casting.
            int DisableShadowCasting(GTLib::Script &script);

            /// Determines whether or not shadow casting is enabled.
            int IsShadowCastingEnabled(GTLib::Script &script);


            /// Sets a material of the model currently set on the given model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the material being set.
            ///     Argument 3: The name of the new material.
            int SetMaterial(GTLib::Script &script);

            /// Retrieves the name of the material at the given index of the model currently set on the given model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetMaterialPath(GTLib::Script &script);

            /// Retrieves the number of materials on the model currently attached to the given model component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetMaterialCount(GTLib::Script &script);

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
            ///       - string (assumed to be a path to a texture)
            int SetMaterialParameter(GTLib::Script &script);


            /// Plays an animation segment by it's name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the segment to play.
            ///     Argument 3: A boolean controlling whether or not to loop the animation.
            ///
            ///     It is possible for segments to share the same name. This will play the first occurance.
            int PlayAnimationSegmentByName(GTLib::Script &script);

            /// Plays the given animation sequence.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The AnimationSequence object to play.
            int PlayAnimationSequence(GTLib::Script &script);

            /// Pauses the currently playing animation, if any.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int PauseAnimation(GTLib::Script &script);

            /// Stops the currently playing animation, returning it to it's original pose.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int StopAnimation(GTLib::Script &script);


            /// Retrieves the index of the bone of the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the bone whose index is being retrieved.
            ///
            ///     If the bone does not exist, nil will be returned.
            int GetBoneIndexByName(GTLib::Script &script);

            /// Retrieves the transformation of the bone of the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the bone.
            ///     Return:     A table containing the attributes "position", "orientation" and "scale"
            ///
            ///     The returned values are the absolute transformation.
            int GetBoneTransformByName(GTLib::Script &script);

            /// Retrieves the transformation of the bone at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the bone.
            ///     Return:     A table containing the attributes "position", "orientation" and "scale"
            ///
            ///     The returned values are the absolute transformation.
            int GetBoneTransformByIndex(GTLib::Script &script);
            
            /// Sets the absolute transformation of the bone of the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the bone.
            ///     Argument 3: The new absolute position of the bone.
            ///     Argument 4: The new absolute orientation of the bone.
            ///     Argument 5: The new absolute scale of the bone (can be nil).
            int SetBoneTransformByName(GTLib::Script &script);

            /// Sets the absolute transformation of the bone of the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the bone.
            ///     Argument 3: The new absolute position of the bone.
            ///     Argument 4: The new absolute orientation of the bone.
            ///     Argument 5: The new absolute scale of the bone (can be nil).
            int SetBoneTransformByIndex(GTLib::Script &script);
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
            int SetColour(GTLib::Script &script);

            /// Retrieves the colour of the light as a math.vec3.
            int GetColour(GTLib::Script &script);


            /// Retrieves the radius of the light.
            int GetRadius(GTLib::Script &script);

            /// Sets the radius of the light.
            int SetRadius(GTLib::Script &script);


            /// Retrieves the exponential falloff factor.
            int GetFalloff(GTLib::Script &script);

            /// Sets the exponential falloff factor.
            int SetFalloff(GTLib::Script &script);


            /// Enables shadow casting on the light.
            int EnableShadowCasting(GTLib::Script &script);

            /// Disables shadow casting on the light.
            int DisableShadowCasting(GTLib::Script &script);

            /// Determines whether or not shadow casting is enabled.
            int IsShadowCastingEnabled(GTLib::Script &script);
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
            int SetColour(GTLib::Script &script);

            /// Retrieves the colour of the light as a math.vec3.
            int GetColour(GTLib::Script &script);


            /// Retrieves the length of the light.
            int GetLength(GTLib::Script &script);

            /// Sets the length of the light.
            int SetLength(GTLib::Script &script);


            /// Retrieves the exponential falloff factor.
            int GetFalloff(GTLib::Script &script);

            /// Sets the exponential falloff factor.
            int SetFalloff(GTLib::Script &script);


            /// Enables shadow casting on the light.
            int EnableShadowCasting(GTLib::Script &script);

            /// Disables shadow casting on the light.
            int DisableShadowCasting(GTLib::Script &script);

            /// Determines whether or not shadow casting is enabled.
            int IsShadowCastingEnabled(GTLib::Script &script);


            /// Sets the inner and outer angles of the spot light.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The inner angle.
            ///     Argument 3: The outer angle.
            int SetAngles(GTLib::Script &script);

            /// Retrieves the inner and outer angles of the spot light, as two floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetAngles(GTLib::Script &script);
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
            int SetColour(GTLib::Script &script);

            /// Retrieves the colour of the light as a math.vec3.
            int GetColour(GTLib::Script &script);


            /// Enables shadow casting on the light.
            int EnableShadowCasting(GTLib::Script &script);

            /// Disables shadow casting on the light.
            int DisableShadowCasting(GTLib::Script &script);

            /// Determines whether or not shadow casting is enabled.
            int IsShadowCastingEnabled(GTLib::Script &script);
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
            int SetColour(GTLib::Script &script);

            /// Retrieves the colour of the light as a math.vec3.
            int GetColour(GTLib::Script &script);
        }


        namespace CollisionShapeComponentFFI
        {
            /// Sets the collision filter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The collision group. Only the first 16 bits are used.
            ///     Argument 3: The collision mask. Only the first 16 bits are used.
            int SetCollisionFilter(GTLib::Script &script);

            /// Retrieves the collision group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroup(GTLib::Script &script);

            /// Retrieves the collision mask.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionMask(GTLib::Script &script);


            /// Adds a collision group, making the object part of that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to make the object part of.
            int AddCollisionGroup(GTLib::Script &script);

            /// Removes a collision group, removing the object from that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to remove the object from.
            int RemoveCollisionGroup(GTLib::Script &script);

            /// Removes a collision group by it's index, removing the object from that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group to remove the object from.
            int RemoveCollisionGroupByIndex(GTLib::Script &script);

            /// Retrieves the number of collision groups the object is part of.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroupCount(GTLib::Script &script);

            /// Retrieves the name of the collision group at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group.
            int GetCollisionGroupName(GTLib::Script &script);


            /// Adds a collision group to those the object allowed to collision with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to allow the object to collide with.
            int AddCollisionGroupMask(GTLib::Script &script);

            /// Removes a collision group to those the object is allowed to collide with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to no longer allow the object to collide with.
            int RemoveCollisionGroupMask(GTLib::Script &script);

            /// Removes a collision group to those the object is allowed to collide with, by it's index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group to no longer allow the object to collide with.
            int RemoveCollisionGroupMaskByIndex(GTLib::Script &script);

            /// Retrieves the number of collision groups the object is allowed to collide with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroupMaskCount(GTLib::Script &script);

            /// Retrieves the name of the collision group mask at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group mask.
            int GetCollisionGroupMaskName(GTLib::Script &script);



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
            int AddBoxShape(GTLib::Script &script);

            /// Adds a sphere collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 5: Optional x offset or a math.vec3 containing the offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddSphereShape(GTLib::Script &script);

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
            int AddEllipsoidShape(GTLib::Script &script);

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
            int AddCylinderXShape(GTLib::Script &script);
            int AddCylinderYShape(GTLib::Script &script);
            int AddCylinderZShape(GTLib::Script &script);

            /// Adds a capsure collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 3: The length.
            ///     Argument 5: Optional x offset or a math.vec3.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCapsuleXShape(GTLib::Script &script);
            int AddCapsuleYShape(GTLib::Script &script);
            int AddCapsuleZShape(GTLib::Script &script);

            /// Adds a set of convex hull collision shapes from the model defined in the attached model comonent, if any.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The margin to apply to the convex hulls.
            ///     Argument 3: Optional x offset or a math.vec3.
            ///     Argument 4: Optional y offset.
            ///     Argument 5: Optional z offset.
            int AddModelConvexHullsCollisionShape(GTLib::Script &script);


            /// Removes every collision shape.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int RemoveAllShapes(GTLib::Script &script);

            /// Removes the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer ot the component to delete.
            ///     Argument 2: The index of the shape to delete.
            int RemoveShapeAtIndex(GTLib::Script &script);

            /// Retrieves the number of collision shapes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetShapeCount(GTLib::Script &script);

            /// Retrieves a table containing information about the collision shape of the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape whose information is being retrieved.
            int GetShapeAtIndex(GTLib::Script &script);


            /// Sets the offset for the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape whose offset is being changed.
            ///     Argument 3, 4, 5: The new offset, as 3 floats, or a math.vec3
            int SetShapeOffset(GTLib::Script &script);

            /// Sets the extents of the box shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a box, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents or a math.vec3.
            int SetBoxShapeHalfExtents(GTLib::Script &script);

            /// Sets the radius of the sphere shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a sphere, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new radius.
            int SetSphereShapeRadius(GTLib::Script &script);

            /// Sets the radius of the ellipsoid shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new radius as 3 floats, or a math.vec3.
            int SetEllipsoidShapeRadius(GTLib::Script &script);

            /// Sets the extents of the cylinder shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a cylinder, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents or a math.vec3.
            int SetCylinderShapeSize(GTLib::Script &script);

            /// Sets the size of the capsule shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The radius of the shape.
            ///     Argument 4: The height of the shape.
            int SetCapsuleShapeSize(GTLib::Script &script);

            /// Sets the size of the margins of the model convex hull shapes.
            ///
            /// @remarks
            ///     If the shape at the given index is not a model convex hulls compound shape, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new margin to use with the convex hulls.
            int SetModelConvexHullsMargins(GTLib::Script &script);
        }


        namespace DynamicsComponentFFI
        {
            /// Sets the mass of the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose mass is being set.
            ///     Argument 2: The new mass.
            int SetMass(GTLib::Script &script);

            /// Retrieves the mass of the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the componen whose mass is being retrieved.
            int GetMass(GTLib::Script &script);

            /// Determines whether or not the object is static.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsStatic(GTLib::Script &script);

            /// Determines or sets whether or not the object is kinematic.
            ///
            /// @remarks
            ///     Argument 1: A pointer the component.
            ///     Argument 2: Whether or not the object should be made kinematic, or nil.
            ///
            ///     If the second argument is nil, this will act as a getter.
            int IsKinematic(GTLib::Script &script);


            /// Sets the friction.
            ///
            /// @remarks
            ///     Argument 1: A pointer ot the component.
            ///     Argument 2: The friction to apply to the value.
            int SetFriction(GTLib::Script &script);

            /// Retrieves the friction.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetFriction(GTLib::Script &script);

            /// Sets the restituion (bounciness).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The restitution.
            int SetRestitution(GTLib::Script &script);

            /// Retrieves the restitution (bounciness).
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetRestitution(GTLib::Script &script);

            /// Sets the damping.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The linear damping.
            ///     Argument 3: The angular damping.
            int SetDamping(GTLib::Script &script);

            /// Retrieves the linear damping.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetLinearDamping(GTLib::Script &script);

            /// Retrieves the angular damping.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetAngularDamping(GTLib::Script &script);


            /// Sets the linear velocity.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x velocity or a math.vec3().
            ///     Argument 3: The y velocity.
            ///     Argument 4: The z velocity.
            int SetLinearVelocity(GTLib::Script &script);

            /// Retrieves the linear velocity as 3 floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     The return value will be a math.vec3()
            int GetLinearVelocity(GTLib::Script &script);

            /// Sets the angular velocity.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x velocity or a math.vec3().
            ///     Argument 3: The y velocity.
            ///     Argument 4: The z velocity.
            int SetAngularVelocity(GTLib::Script &script);

            /// Retrieves the linear velocity as 3 floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     The return value will be a math.vec3()
            int GetAngularVelocity(GTLib::Script &script);


            /// Sets the linear factor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x factor or a math.vec3().
            ///     Argument 3: The y factor.
            ///     Argument 4: The z factor.
            int SetLinearFactor(GTLib::Script &script);

            /// Sets the angular factor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x factor or a math.vec3().
            ///     Argument 3: The y factor.
            ///     Argument 4: The z factor.
            int SetAngularFactor(GTLib::Script &script);


            /// Sets the gravity to use with the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x gravity or a math.vec3().
            ///     Argument 3: The y gravity.
            ///     Argument 4: The z gravity.
            int SetGravity(GTLib::Script &script);

            /// Retrieves the gravity being used with the object, as 3 floats.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     The return value will be a math.vec3()
            int GetGravity(GTLib::Script &script);

            /// Applies the gravity of the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int ApplyGravity(GTLib::Script &script);


            /// Applies a central force to the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x force or a math.vec3().
            ///     Argument 3: The y force.
            ///     Argument 4: The z force.
            int ApplyCentralForce(GTLib::Script &script);

            /// Applies a torque to the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x torque or a math.vec3().
            ///     Argument 3: The y torque.
            ///     Argument 4: The z torque.
            int ApplyTorque(GTLib::Script &script);

            /// Applies an impulse to the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: A math.vec3() containing the impule.
            ///     Argument 3: A math.vec3() containing the relative position to apply the impulse.
            int ApplyImpulse(GTLib::Script &script);

            /// Applies a central impulse to the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x impulse or a math.vec3().
            ///     Argument 3: The y impulse.
            ///     Argument 4: The z impulse.
            int ApplyCentralImpulse(GTLib::Script &script);

            /// Applies a torque impulse.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The x impulse or a math.vec3().
            ///     Argument 3: The y impulse.
            ///     Argument 4: The z impulse.
            int ApplyTorqueImpulse(GTLib::Script &script);


            /// Disables deactivation of the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int DisableDeactivation(GTLib::Script &script);

            /// Enables deactivation of an object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int EnableDeactivation(GTLib::Script &script);

            /// Determines whether or not deactivation is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsDeactivationEnabled(GTLib::Script &script);

            /// Activates the object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int Activate(GTLib::Script &script);


            /// Enables navigation mesh generation for this object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int EnableNavigationMeshGeneration(GTLib::Script &script);

            /// Disables navigation mesh generation for this object.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int DisableNavigationMeshGeneration(GTLib::Script &script);

            /// Determines whether or not navigation mesh generation is enabled.
            ///
            /// @remakrs
            ///     Argument 1: A pointer to the component.
            int IsNavigationMeshGenerationEnabled(GTLib::Script &script);



            /// Sets the collision filter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The collision group. Only the first 16 bits are used.
            ///     Argument 3: The collision mask. Only the first 16 bits are used.
            int SetCollisionFilter(GTLib::Script &script);

            /// Retrieves the collision group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroup(GTLib::Script &script);

            /// Retrieves the collision mask.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionMask(GTLib::Script &script);


            /// Adds a collision group, making the object part of that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to make the object part of.
            int AddCollisionGroup(GTLib::Script &script);

            /// Removes a collision group, removing the object from that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to remove the object from.
            int RemoveCollisionGroup(GTLib::Script &script);

            /// Removes a collision group by it's index, removing the object from that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group to remove the object from.
            int RemoveCollisionGroupByIndex(GTLib::Script &script);

            /// Retrieves the number of collision groups the object is part of.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroupCount(GTLib::Script &script);

            /// Retrieves the name of the collision group at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group.
            int GetCollisionGroupName(GTLib::Script &script);


            /// Adds a collision group to those the object allowed to collision with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to allow the object to collide with.
            int AddCollisionGroupMask(GTLib::Script &script);

            /// Removes a collision group to those the object is allowed to collide with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to no longer allow the object to collide with.
            int RemoveCollisionGroupMask(GTLib::Script &script);

            /// Removes a collision group to those the object is allowed to collide with, by it's index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group to no longer allow the object to collide with.
            int RemoveCollisionGroupMaskByIndex(GTLib::Script &script);

            /// Retrieves the number of collision groups the object is allowed to collide with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroupMaskCount(GTLib::Script &script);

            /// Retrieves the name of the collision group mask at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group mask.
            int GetCollisionGroupMaskName(GTLib::Script &script);


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
            int AddBoxCollisionShape(GTLib::Script &script);

            /// Adds a sphere collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddSphereCollisionShape(GTLib::Script &script);

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
            int AddEllipsoidCollisionShape(GTLib::Script &script);

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
            int AddCylinderXCollisionShape(GTLib::Script &script);
            int AddCylinderYCollisionShape(GTLib::Script &script);
            int AddCylinderZCollisionShape(GTLib::Script &script);

            /// Adds a capsure collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 3: The length.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCapsuleXCollisionShape(GTLib::Script &script);
            int AddCapsuleYCollisionShape(GTLib::Script &script);
            int AddCapsuleZCollisionShape(GTLib::Script &script);

            /// Adds a set of convex hull collision shapes from the model defined in the attached model comonent, if any.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The margin to apply to the convex hulls.
            ///     Argument 3: Optional x offset or a math.vec3.
            ///     Argument 4: Optional y offset.
            ///     Argument 5: Optional z offset.
            int AddModelConvexHullsCollisionShape(GTLib::Script &script);


            /// Removes every collision shape.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int RemoveAllCollisionShapes(GTLib::Script &script);

            /// Removes the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer ot the component to delete.
            ///     Argument 2: The index of the shape to delete.
            int RemoveCollisionShapeAtIndex(GTLib::Script &script);

            /// Retrieves the number of collision shapes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionShapeCount(GTLib::Script &script);

            /// Retrieves a table containing information about the collision shape of the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape whose information is being retrieved.
            int GetCollisionShapeAtIndex(GTLib::Script &script);


            /// Sets the offset for the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape whose offset is being changed.
            ///     Argument 3, 4, 5: The new offset, as 3 floats
            int SetCollisionShapeOffset(GTLib::Script &script);

            /// Sets the extents of the box shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a box, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents.
            int SetBoxCollisionShapeHalfExtents(GTLib::Script &script);

            /// Sets the radius of the sphere shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a sphere, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new radius.
            int SetSphereCollisionShapeRadius(GTLib::Script &script);

            /// Sets the radius of the ellipsoid shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new radius as 3 floats.
            int SetEllipsoidCollisionShapeRadius(GTLib::Script &script);

            /// Sets the extents of the cylinder shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a cylinder, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents.
            int SetCylinderCollisionShapeSize(GTLib::Script &script);

            /// Sets the size of the capsule shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The radius of the shape.
            ///     Argument 4: The height of the shape.
            int SetCapsuleCollisionShapeSize(GTLib::Script &script);

            /// Sets the size of the margins of the model convex hull shapes.
            ///
            /// @remarks
            ///     If the shape at the given index is not a model convex hulls compound shape, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new margin to use with the convex hulls.
            int SetModelConvexHullsMargins(GTLib::Script &script);
        }

        namespace ProximityComponentFFI
        {
            /// Sets the collision filter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The collision group. Only the first 16 bits are used.
            ///     Argument 3: The collision mask. Only the first 16 bits are used.
            int SetCollisionFilter(GTLib::Script &script);

            /// Retrieves the collision group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroup(GTLib::Script &script);

            /// Retrieves the collision mask.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionMask(GTLib::Script &script);


            /// Adds a collision group, making the object part of that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to make the object part of.
            int AddCollisionGroup(GTLib::Script &script);

            /// Removes a collision group, removing the object from that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to remove the object from.
            int RemoveCollisionGroup(GTLib::Script &script);

            /// Removes a collision group by it's index, removing the object from that group.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group to remove the object from.
            int RemoveCollisionGroupByIndex(GTLib::Script &script);

            /// Retrieves the number of collision groups the object is part of.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroupCount(GTLib::Script &script);

            /// Retrieves the name of the collision group at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group.
            int GetCollisionGroupName(GTLib::Script &script);


            /// Adds a collision group to those the object allowed to collision with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to allow the object to collide with.
            int AddCollisionGroupMask(GTLib::Script &script);

            /// Removes a collision group to those the object is allowed to collide with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the collision group to no longer allow the object to collide with.
            int RemoveCollisionGroupMask(GTLib::Script &script);

            /// Removes a collision group to those the object is allowed to collide with, by it's index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group to no longer allow the object to collide with.
            int RemoveCollisionGroupMaskByIndex(GTLib::Script &script);

            /// Retrieves the number of collision groups the object is allowed to collide with.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionGroupMaskCount(GTLib::Script &script);

            /// Retrieves the name of the collision group mask at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the collision group mask.
            int GetCollisionGroupMaskName(GTLib::Script &script);



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
            int AddBoxCollisionShape(GTLib::Script &script);

            /// Adds a sphere collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddSphereCollisionShape(GTLib::Script &script);

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
            int AddEllipsoidCollisionShape(GTLib::Script &script);

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
            int AddCylinderXCollisionShape(GTLib::Script &script);
            int AddCylinderYCollisionShape(GTLib::Script &script);
            int AddCylinderZCollisionShape(GTLib::Script &script);

            /// Adds a capsure collision shape to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The radius.
            ///     Argument 3: The length.
            ///     Argument 5: Optional x offset.
            ///     Argument 6: Optional y offset.
            ///     Argument 7: Optional z offset.
            int AddCapsuleXCollisionShape(GTLib::Script &script);
            int AddCapsuleYCollisionShape(GTLib::Script &script);
            int AddCapsuleZCollisionShape(GTLib::Script &script);

            /// Adds a set of convex hull collision shapes from the model defined in the attached model comonent, if any.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The margin to apply to the convex hulls.
            ///     Argument 3: Optional x offset or a math.vec3.
            ///     Argument 4: Optional y offset.
            ///     Argument 5: Optional z offset.
            int AddModelConvexHullsCollisionShape(GTLib::Script &script);


            /// Removes every collision shape.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int RemoveAllCollisionShapes(GTLib::Script &script);

            /// Removes the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer ot the component to delete.
            ///     Argument 2: The index of the shape to delete.
            int RemoveCollisionShapeAtIndex(GTLib::Script &script);

            /// Retrieves the number of collision shapes.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetCollisionShapeCount(GTLib::Script &script);

            /// Retrieves a table containing information about the collision shape of the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape whose information is being retrieved.
            int GetCollisionShapeAtIndex(GTLib::Script &script);


            /// Sets the offset for the collision shape at the given index.
            ///
            /// @remarks
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape whose offset is being changed.
            ///     Argument 3, 4, 5: The new offset, as 3 floats
            int SetCollisionShapeOffset(GTLib::Script &script);

            /// Sets the extents of the box shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a box, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents.
            int SetBoxCollisionShapeHalfExtents(GTLib::Script &script);

            /// Sets the radius of the sphere shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a sphere, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new radius.
            int SetSphereCollisionShapeRadius(GTLib::Script &script);

            /// Sets the radius of the ellipsoid shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new radius as 3 floats.
            int SetEllipsoidCollisionShapeRadius(GTLib::Script &script);

            /// Sets the extents of the cylinder shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not a cylinder, this function will fail.
            ///     @par
            ///     Argument 1:       A pointer to the component.
            ///     Argument 2:       The index of the shape.
            ///     Argument 3, 4, 5: The new half extents.
            int SetCylinderCollisionShapeSize(GTLib::Script &script);

            /// Sets the size of the capsule shape at the given index.
            ///
            /// @remarks
            ///     If the shape at the given index is not an ellipsoid, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The radius of the shape.
            ///     Argument 4: The height of the shape.
            int SetCapsuleCollisionShapeSize(GTLib::Script &script);

            /// Sets the size of the margins of the model convex hull shapes.
            ///
            /// @remarks
            ///     If the shape at the given index is not a model convex hulls compound shape, this function will fail.
            ///     @par
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the shape.
            ///     Argument 3: The new margin to use with the convex hulls.
            int SetModelConvexHullsMargins(GTLib::Script &script);


            /// Determines if anything is contained inside the collision volume.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///
            ///     The return value is a simple boolean.
            ///     @par
            ///     This does not include scene nodes that are related to the scene node that owns the component.
            int IsAnythingInside(GTLib::Script &script);
        }


        namespace ScriptComponentFFI
        {
            /// Adds a script file to the component.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The relative path of the script file being added.
            int AddScript(GTLib::Script &script);

            /// Removes a script file from the component by it's relative path.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The relative path of the script file being removed.
            int RemoveScriptByRelativePath(GTLib::Script &script);

            /// Removes a script file by it's index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the script being removed.
            int RemoveScriptByIndex(GTLib::Script &script);

            /// Reloads a script at the given index with a new file.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the script being reloaded.
            int ReloadScript(GTLib::Script &script);

            /// Retrieves the names of the script files the given script component uses.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetScriptFilePaths(GTLib::Script &script);

            /// Determines if the script component is using the given script file.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The relative path of the script.
            int IsUsingScript(GTLib::Script &script);

            /// Retrieves an array containing the names and types of the public variables of the script definition at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The index of the script definition whose public variables are being retried.
            ///
            ///     The return value is an array of {name, type} types.
            int GetPublicVariableNamesAndTypesByIndex(GTLib::Script &script);


            /// Retrieves the names and values of every public variable.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///
            ///     The returned table is keyed by the name. Note that this is out of order.
            int GetPublicVariableNamesAndValues(GTLib::Script &script);

            /// Retrieves the value of the public variable with the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the variable whose value is being retrieved.
            int GetPublicVariableValue(GTLib::Script &script);

            /// Sets the value of the public variable with the given name.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The name of the variable whose value is being set.
            ///     Argument 3: The value to assign to the variable.
            ///
            ///     If the value is an unsupported type, nothing will be changed.
            int SetPublicVariableValue(GTLib::Script &script);
        }


        namespace ParticleSystemComponentFFI
        {
            /// Sets the particle system from a relative file path.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: The relative path of the particle system to set.
            int SetFromFile(GTLib::Script &script);

            /// Retrieves the relative file path of the attached particle system.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetRelativeFilePath(GTLib::Script &script);


            /// Marks the particle system as playing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int Play(GTLib::Script &script);

            /// Marks the particle system as paused.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int Pause(GTLib::Script &script);

            /// Determines whether or not the particle system is playing.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsPlaying(GTLib::Script &script);


            /// Sets whether or not the particle system should be played on startup.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     Argument 2: Boolean: True if the particle system should be played on startup.
            int PlayOnStartup(GTLib::Script &script);

            /// Determines whether or not the particle system is playing on startup.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsPlayingOnStartup(GTLib::Script &script);


            /// Resets the particle system.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle system.
            int Reset(GTLib::Script &script);


            /// Retrieves the lifetime of the particle system.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle system.
            ///     Argument 2: The index of the emitter whose lifetime is being retrieved.
            int GetEmitterLifetime(GTLib::Script &script);
        }


        namespace PrefabComponentFFI
        {
            /// Retrieve the relative path of the prefab.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetPrefabRelativePath(GTLib::Script &script);

            /// Retrieves the local ID of the scene node this component is linked to in the prefab.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int GetLocalHierarchyID(GTLib::Script &script);
        }


        namespace EditorMetadataComponentFFI
        {
            /// Marks the node as selected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     Note that this isn't enough to fully select the node. Instead, this simply marks the node as selected.
            int Select(GTLib::Script &script);

            /// Marks the node as deselected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     Note that this isn't enough to fully deselect the node. Instead, this simply marks the node as selected.
            int Deselect(GTLib::Script &script);

            /// Determines whether or not the node is marked as selected.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            ///     @par
            ///     Returns a boolean.
            int IsSelected(GTLib::Script &script);


            /// Shows a sprite on the object while it's shown in the editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose sprite is being shown.
            ///     Argument 2: The file path of the texture to use on the sprite.
            ///     Argument 3: A table containing the colour to use with the sprite. Keys are r, g and b.
            int ShowSprite(GTLib::Script &script);

            /// Hides the sprite.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose sprite is being hidden.
            int HideSprite(GTLib::Script &script);

            /// Determines whether or not a sprite is being shown.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsShowingSprite(GTLib::Script &script);

            /// Retrieves the path of the texture being used for the sprite.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose sprite texture is being retrieved.
            int GetSpriteTexturePath(GTLib::Script &script);


            /// Shows a direction on the object while it's shown in the editor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose direction arrow is being shown.
            int ShowDirectionArrow(GTLib::Script &script);

            /// Hides the direction arrow.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component whose direction arrow is being hidden.
            int HideDirectionArrow(GTLib::Script &script);

            /// Determines whether or not the direction arrow is being shown.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the component.
            int IsShowingDirectionArrow(GTLib::Script &script);
        }
    }
}

#endif