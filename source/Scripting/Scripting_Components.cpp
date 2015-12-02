// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/Scripting/Scripting_Components.hpp>
#include <GTEngine/Scripting/Scripting_Animation.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/Script.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GT
{
    bool LoadComponentsLibrary(GT::Script &script)
    {
        script.GetGlobal("GTEngine");
        assert(script.IsTable(-1));
        {
            ///////////////////////////////////////////////////
            // GTEngine.Components

            script.Push("Components");
            script.PushNewTable();
            {
                script.SetTableValue(-1, "Camera",            CameraComponent::Name);
                script.SetTableValue(-1, "Model",             ModelComponent::Name);
                script.SetTableValue(-1, "Camera",            CameraComponent::Name);
                script.SetTableValue(-1, "PointLight",        PointLightComponent::Name);
                script.SetTableValue(-1, "SpotLight",         SpotLightComponent::Name);
                script.SetTableValue(-1, "DirectionalLight",  DirectionalLightComponent::Name);
                script.SetTableValue(-1, "AmbientLight",      AmbientLightComponent::Name);
                script.SetTableValue(-1, "Dynamics",          DynamicsComponent::Name);
                script.SetTableValue(-1, "Proximity",         ProximityComponent::Name);
                script.SetTableValue(-1, "Script",            ScriptComponent::Name);
                script.SetTableValue(-1, "ParticleSystem",    ParticleSystemComponent::Name);
                script.SetTableValue(-1, "Prefab",            PrefabComponent::Name);
                script.SetTableValue(-1, "EditorMetadata",    EditorMetadataComponent::Name);
            }
            script.SetTableValue(-3);
        }
        script.Pop(1);


        // Create the GTEngine.System.<Component> tables.
        bool successful = script.Execute
        (
            "for key,value in pairs(GTEngine.Components) do"
            "    GTEngine.System[value .. 'Component'] = {};"
            "end;"
        );


        successful = successful && script.Execute
        (
            // CameraComponent
            "GTEngine.CameraComponent = {};"
            "GTEngine.CameraComponent.__index = GTEngine.CameraComponent;"

            "function GTEngine.CameraComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.CameraComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.CameraComponent:Set3DProjection(fov, aspect, zNear, zFar)"
            "    GTEngine.System.CameraComponent.Set3DProjection(self._internalPtr, fov, aspect, zNear, zFar);"
            "end;"

            "function GTEngine.CameraComponent:GetFOV()"
            "    return GTEngine.System.CameraComponent.GetFOV(self._internalPtr);"
            "end;"

            "function GTEngine.CameraComponent:GetAspectRatio()"
            "    return GTEngine.System.CameraComponent.GetAspectRatio(self._internalPtr);"
            "end;"

            "function GTEngine.CameraComponent:GetNearClippingPlane()"
            "    return GTEngine.System.CameraComponent.GetNearClippingPlane(self._internalPtr);"
            "end;"

            "function GTEngine.CameraComponent:GetFarClippingPlane()"
            "    return GTEngine.System.CameraComponent.GetFarClippingPlane(self._internalPtr);"
            "end;"




            // ModelComponent
            "GTEngine.ModelComponent = {};"
            "GTEngine.ModelComponent.__index = GTEngine.ModelComponent;"

            "function GTEngine.ModelComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.ModelComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.ModelComponent:SetModel(filePath)"
            "    GTEngine.System.ModelComponent.SetModel(self._internalPtr, filePath);"
            "end;"

            "function GTEngine.ModelComponent:GetModelPath()"
            "    return GTEngine.System.ModelComponent.GetModelPath(self._internalPtr);"
            "end;"

            "function GTEngine.ModelComponent:EnableShadowCasting()"
            "    GTEngine.System.ModelComponent.EnableShadowCasting(self._internalPtr);"
            "end;"

            "function GTEngine.ModelComponent:DisableShadowCasting()"
            "    GTEngine.System.ModelComponent.DisableShadowCasting(self._internalPtr);"
            "end;"

            "function GTEngine.ModelComponent:IsShadowCastingEnabled()"
            "    return GTEngine.System.ModelComponent.IsShadowCastingEnabled(self._internalPtr);"
            "end;"

            "function GTEngine.ModelComponent:SetMaterial(index, path)"
            "    return GTEngine.System.ModelComponent.SetMaterial(self._internalPtr, index, path);"
            "end;"

            "function GTEngine.ModelComponent:GetMaterialPath(index)"
            "    return GTEngine.System.ModelComponent.GetMaterialPath(self._internalPtr, index);"
            "end;"

            "function GTEngine.ModelComponent:GetMaterialCount()"
            "    return GTEngine.System.ModelComponent.GetMaterialCount(self._internalPtr);"
            "end;"

            "function GTEngine.ModelComponent:SetMaterialParameter(index, name, value)"
            "    return GTEngine.System.ModelComponent.SetMaterialParameter(self._internalPtr, index, name, value);"
            "end;"


            "function GTEngine.ModelComponent:PlayAnimationSegmentByName(segmentName, loop)"
            "    GTEngine.System.ModelComponent.PlayAnimationSegmentByName(self._internalPtr, segmentName, loop);"
            "end;"

            "function GTEngine.ModelComponent:PlayAnimationSequence(segmentName, loop)"
            "    GTEngine.System.ModelComponent.PlayAnimationSequence(self._internalPtr, segmentName, loop);"
            "end;"

            "function GTEngine.ModelComponent:PauseAnimation()"
            "    GTEngine.System.ModelComponent.PauseAnimation(self._internalPtr);"
            "end;"

            "function GTEngine.ModelComponent:StopAnimation()"
            "    GTEngine.System.ModelComponent.StopAnimation(self._internalPtr);"
            "end;"

            "function GTEngine.ModelComponent:GetBoneTransformByName(boneName)"
            "    return GTEngine.System.ModelComponent.GetBoneTransformByName(self._internalPtr, boneName);"
            "end;"

            "function GTEngine.ModelComponent:GetBoneTransformByIndex(boneIndex)"
            "    return GTEngine.System.ModelComponent.GetBoneTransformByIndex(self._internalPtr, boneIndex);"
            "end;"

            "function GTEngine.ModelComponent:GetBoneIndexByName(boneName)"
            "    return GTEngine.System.ModelComponent.GetBoneIndexByName(self._internalPtr, boneName);"
            "end;"




            // PointLightComponent
            "GTEngine.PointLightComponent = {};"
            "GTEngine.PointLightComponent.__index = GTEngine.PointLightComponent;"

            "function GTEngine.PointLightComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.PointLightComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.PointLightComponent:SetColour(r, g, b)"
            "    GTEngine.System.PointLightComponent.SetColour(self._internalPtr, r, g, b);"
            "end;"

            "function GTEngine.PointLightComponent:GetColour()"
            "    return GTEngine.System.PointLightComponent.GetColour(self._internalPtr);"
            "end;"


            "function GTEngine.PointLightComponent:SetRadius(radius)"
            "    GTEngine.System.PointLightComponent.SetRadius(self._internalPtr, radius);"
            "end;"

            "function GTEngine.PointLightComponent:GetRadius()"
            "    return GTEngine.System.PointLightComponent.GetRadius(self._internalPtr);"
            "end;"

            "function GTEngine.PointLightComponent:SetFalloff(falloff)"
            "    GTEngine.System.PointLightComponent.SetFalloff(self._internalPtr, falloff);"
            "end;"

            "function GTEngine.PointLightComponent:GetFalloff()"
            "    return GTEngine.System.PointLightComponent.GetFalloff(self._internalPtr);"
            "end;"


            "function GTEngine.PointLightComponent:EnableShadowCasting()"
            "    GTEngine.System.PointLightComponent.EnableShadowCasting(self._internalPtr);"
            "end;"

            "function GTEngine.PointLightComponent:DisableShadowCasting()"
            "    GTEngine.System.PointLightComponent.DisableShadowCasting(self._internalPtr);"
            "end;"

            "function GTEngine.PointLightComponent:IsShadowCastingEnabled()"
            "    return GTEngine.System.PointLightComponent.IsShadowCastingEnabled(self._internalPtr);"
            "end;"


            // SpotLightComponent
            "GTEngine.SpotLightComponent = {};"
            "GTEngine.SpotLightComponent.__index = GTEngine.SpotLightComponent;"

            "function GTEngine.SpotLightComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.SpotLightComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.SpotLightComponent:SetColour(r, g, b)"
            "    GTEngine.System.SpotLightComponent.SetColour(self._internalPtr, r, g, b);"
            "end;"

            "function GTEngine.SpotLightComponent:GetColour()"
            "    return GTEngine.System.SpotLightComponent.GetColour(self._internalPtr);"
            "end;"


            "function GTEngine.SpotLightComponent:SetLength(radius)"
            "    GTEngine.System.SpotLightComponent.SetLength(self._internalPtr, radius);"
            "end;"

            "function GTEngine.SpotLightComponent:GetLength()"
            "    return GTEngine.System.SpotLightComponent.GetLength(self._internalPtr);"
            "end;"

            "function GTEngine.SpotLightComponent:SetFalloff(falloff)"
            "    GTEngine.System.SpotLightComponent.SetFalloff(self._internalPtr, falloff);"
            "end;"

            "function GTEngine.SpotLightComponent:GetFalloff()"
            "    return GTEngine.System.SpotLightComponent.GetFalloff(self._internalPtr);"
            "end;"


            "function GTEngine.SpotLightComponent:EnableShadowCasting()"
            "    GTEngine.System.SpotLightComponent.EnableShadowCasting(self._internalPtr);"
            "end;"

            "function GTEngine.SpotLightComponent:DisableShadowCasting()"
            "    GTEngine.System.SpotLightComponent.DisableShadowCasting(self._internalPtr);"
            "end;"

            "function GTEngine.SpotLightComponent:IsShadowCastingEnabled()"
            "    return GTEngine.System.SpotLightComponent.IsShadowCastingEnabled(self._internalPtr);"
            "end;"


            "function GTEngine.SpotLightComponent:SetAngles(inner, outer)"
            "    GTEngine.System.SpotLightComponent.SetAngles(self._internalPtr, inner, outer);"
            "end;"

            "function GTEngine.SpotLightComponent:GetAngles()"
            "    return GTEngine.System.SpotLightComponent.GetAngles(self._internalPtr);"
            "end;"



            // DirectionalLightComponent
            "GTEngine.DirectionalLightComponent = {};"
            "GTEngine.DirectionalLightComponent.__index = GTEngine.DirectionalLightComponent;"

            "function GTEngine.DirectionalLightComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.DirectionalLightComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.DirectionalLightComponent:SetColour(r, g, b)"
            "    GTEngine.System.DirectionalLightComponent.SetColour(self._internalPtr, r, g, b);"
            "end;"

            "function GTEngine.DirectionalLightComponent:GetColour()"
            "    return GTEngine.System.DirectionalLightComponent.GetColour(self._internalPtr);"
            "end;"

            "function GTEngine.DirectionalLightComponent:EnableShadowCasting()"
            "    GTEngine.System.DirectionalLightComponent.EnableShadowCasting(self._internalPtr);"
            "end;"

            "function GTEngine.DirectionalLightComponent:DisableShadowCasting()"
            "    GTEngine.System.DirectionalLightComponent.DisableShadowCasting(self._internalPtr);"
            "end;"

            "function GTEngine.DirectionalLightComponent:IsShadowCastingEnabled()"
            "    return GTEngine.System.DirectionalLightComponent.IsShadowCastingEnabled(self._internalPtr);"
            "end;"


            // AmbientLightComponent
            "GTEngine.AmbientLightComponent = {};"
            "GTEngine.AmbientLightComponent.__index = GTEngine.AmbientLightComponent;"

            "function GTEngine.AmbientLightComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.AmbientLightComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.AmbientLightComponent:SetColour(r, g, b)"
            "    GTEngine.System.AmbientLightComponent.SetColour(self._internalPtr, r, g, b);"
            "end;"

            "function GTEngine.AmbientLightComponent:GetColour()"
            "    return GTEngine.System.AmbientLightComponent.GetColour(self._internalPtr);"
            "end;"



            // DynamicsComponent
            "GTEngine.DynamicsComponent = {};"
            "GTEngine.DynamicsComponent.__index = GTEngine.DynamicsComponent;"

            "function GTEngine.DynamicsComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.DynamicsComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.DynamicsComponent:SetMass(mass)"
            "    GTEngine.System.DynamicsComponent.SetMass(self._internalPtr, mass);"
            "end;"

            "function GTEngine.DynamicsComponent:GetMass()"
            "    return GTEngine.System.DynamicsComponent.GetMass(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:IsStatic()"
            "    return GTEngine.System.DynamicsComponent.IsStatic(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:IsKinematic(kinematic)"
            "    return GTEngine.System.DynamicsComponent.IsKinematic(self._internalPtr, kinematic);"
            "end;"

            "function GTEngine.DynamicsComponent:SetFriction(friction)"
            "    GTEngine.System.DynamicsComponent.SetFriction(self._internalPtr, friction);"
            "end;"

            "function GTEngine.DynamicsComponent:GetFriction()"
            "    return GTEngine.System.DynamicsComponent.GetFriction(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:SetRestitution(restitution)"
            "    GTEngine.System.DynamicsComponent.SetRestitution(self._internalPtr, restitution);"
            "end;"

            "function GTEngine.DynamicsComponent:GetRestitution()"
            "    return GTEngine.System.DynamicsComponent.GetRestitution(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:SetDamping(linearDamping, angularDamping)"
            "    GTEngine.System.DynamicsComponent.SetDamping(self._internalPtr, linearDamping, angularDamping);"
            "end;"

            "function GTEngine.DynamicsComponent:GetLinearDamping()"
            "    return GTEngine.System.DynamicsComponent.GetLinearDamping(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:GetAngularDamping()"
            "    return GTEngine.System.DynamicsComponent.GetAngularDamping(self._internalPtr);"
            "end;"


            "function GTEngine.DynamicsComponent:SetCollisionFilter(group, mask)"
            "    GTEngine.System.DynamicsComponent.SetCollisionFilter(self._internalPtr, group, mask);"
            "end;"

            "function GTEngine.DynamicsComponent:GetCollisionGroup()"
            "    return GTEngine.System.DynamicsComponent.GetCollisionGroup(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:GetCollisionMask()"
            "    return GTEngine.System.DynamicsComponent.GetCollisionMask(self._internalPtr);"
            "end;"


            "function GTEngine.DynamicsComponent:AddCollisionGroup(groupName)"
            "    return GTEngine.System.DynamicsComponent.AddCollisionGroup(self._internalPtr, groupName);"
            "end;"

            "function GTEngine.DynamicsComponent:RemoveCollisionGroup(groupName)"
            "    return GTEngine.System.DynamicsComponent.RemoveCollisionGroup(self._internalPtr, groupName);"
            "end;"

            "function GTEngine.DynamicsComponent:RemoveCollisionGroupByIndex(groupIndex)"
            "    return GTEngine.System.DynamicsComponent.RemoveCollisionGroupByIndex(self._internalPtr, groupIndex);"
            "end;"

            "function GTEngine.DynamicsComponent:GetCollisionGroupCount()"
            "    return GTEngine.System.DynamicsComponent.GetCollisionGroupCount(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:GetCollisionGroupName(groupIndex)"
            "    return GTEngine.System.DynamicsComponent.GetCollisionGroupName(self._internalPtr, groupIndex);"
            "end;"

            "function GTEngine.DynamicsComponent:AddCollisionGroupMask(groupName)"
            "    return GTEngine.System.DynamicsComponent.AddCollisionGroupMask(self._internalPtr, groupName);"
            "end;"

            "function GTEngine.DynamicsComponent:RemoveCollisionGroupMask(groupName)"
            "    return GTEngine.System.DynamicsComponent.RemoveCollisionGroupMask(self._internalPtr, groupName);"
            "end;"

            "function GTEngine.DynamicsComponent:RemoveCollisionGroupMaskByIndex(groupIndex)"
            "    return GTEngine.System.DynamicsComponent.RemoveCollisionGroupMaskByIndex(self._internalPtr, groupIndex);"
            "end;"

            "function GTEngine.DynamicsComponent:GetCollisionGroupMaskCount()"
            "    return GTEngine.System.DynamicsComponent.GetCollisionGroupMaskCount(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:GetCollisionGroupMaskName(groupIndex)"
            "    return GTEngine.System.DynamicsComponent.GetCollisionGroupMaskName(self._internalPtr, groupIndex);"
            "end;"



            "function GTEngine.DynamicsComponent:SetLinearVelocity(x, y, z)"
            "    GTEngine.System.DynamicsComponent.SetLinearVelocity(self._internalPtr, x, y, z);"
            "end;"

            "function GTEngine.DynamicsComponent:GetLinearVelocity()"
            "    return GTEngine.System.DynamicsComponent.GetLinearVelocity(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:SetAngularVelocity(x, y, z)"
            "    GTEngine.System.DynamicsComponent.SetAngularVelocity(self._internalPtr, x, y, z);"
            "end;"

            "function GTEngine.DynamicsComponent:GetAngularVelocity()"
            "    return GTEngine.System.DynamicsComponent.GetAngularVelocity(self._internalPtr);"
            "end;"


            "function GTEngine.DynamicsComponent:SetLinearFactor(x, y, z)"
            "    GTEngine.System.DynamicsComponent.SetLinearFactor(self._internalPtr, x, y, z);"
            "end;"

            "function GTEngine.DynamicsComponent:SetAngularFactor(x, y, z)"
            "    GTEngine.System.DynamicsComponent.SetAngularFactor(self._internalPtr, x, y, z);"
            "end;"


            "function GTEngine.DynamicsComponent:SetGravity(x, y, z)"
            "    GTEngine.System.DynamicsComponent.SetGravity(self._internalPtr, x, y, z);"
            "end;"

            "function GTEngine.DynamicsComponent:GetGravity()"
            "    return GTEngine.System.DynamicsComponent.GetGravity(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:ApplyGravity()"
            "    GTEngine.System.DynamicsComponent.ApplyGravity(self._internalPtr);"
            "end;"


            "function GTEngine.DynamicsComponent:ApplyCentralForce(x, y, z)"
            "    GTEngine.System.DynamicsComponent.ApplyCentralForce(self._internalPtr, x, y, z);"
            "end;"

            "function GTEngine.DynamicsComponent:ApplyTorque(x, y, z)"
            "    GTEngine.System.DynamicsComponent.ApplyTorque(self._internalPtr, x, y, z);"
            "end;"

            "function GTEngine.DynamicsComponent:ApplyCentralImpulse(x, y, z)"
            "    GTEngine.System.DynamicsComponent.ApplyCentralImpulse(self._internalPtr, x, y, z);"
            "end;"

            "function GTEngine.DynamicsComponent:ApplyImpulse(x, y, z, relPosX, relPosY, relPosZ)"
            "    GTEngine.System.DynamicsComponent.ApplyImpulse(self._internalPtr, x, y, z, relPosX, relPosY, relPosZ);"
            "end;"

            "function GTEngine.DynamicsComponent:ApplyTorqueImpulse(x, y, z)"
            "    GTEngine.System.DynamicsComponent.ApplyTorqueImpulse(self._internalPtr, x, y, z);"
            "end;"


            "function GTEngine.DynamicsComponent:DisableDeactivation()"
            "    GTEngine.System.DynamicsComponent.DisableDeactivation(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:EnableDeactivation()"
            "    GTEngine.System.DynamicsComponent.EnableDeactivation(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:IsDeactivationEnabled()"
            "    return GTEngine.System.DynamicsComponent.IsDeactivationEnabled(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:Activate()"
            "    GTEngine.System.DynamicsComponent.Activate(self._internalPtr);"
            "end;"


            "function GTEngine.DynamicsComponent:EnableNavigationMeshGeneration()"
            "    GTEngine.System.DynamicsComponent.EnableNavigationMeshGeneration(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:DisableNavigationMeshGeneration()"
            "    GTEngine.System.DynamicsComponent.DisableNavigationMeshGeneration(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:IsNavigationMeshGenerationEnabled()"
            "    return GTEngine.System.DynamicsComponent.IsNavigationMeshGenerationEnabled(self._internalPtr);"
            "end;"


            "function GTEngine.DynamicsComponent:AddBoxCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddBoxCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.DynamicsComponent:AddSphereCollisionShape(radius, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddSphereCollisionShape(self._internalPtr, radius, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.DynamicsComponent:AddEllipsoidCollisionShape(radiusX, radiusY, radiusZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddEllipsoidCollisionShape(self._internalPtr, radiusX, radiusY, radiusZ, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.DynamicsComponent:AddCylinderXCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddCylinderXCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.DynamicsComponent:AddCylinderYCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddCylinderYCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.DynamicsComponent:AddCylinderZCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddCylinderZCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.DynamicsComponent:AddCapsuleXCollisionShape(radius, length, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddCapsuleXCollisionShape(self._internalPtr, radius, length, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.DynamicsComponent:AddCapsuleYCollisionShape(radius, length, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddCapsuleYCollisionShape(self._internalPtr, radius, length, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.DynamicsComponent:AddCapsuleZCollisionShape(radius, length, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddCapsuleZCollisionShape(self._internalPtr, radius, length, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.DynamicsComponent:AddModelConvexHullsCollisionShape(margin, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.AddModelConvexHullsCollisionShape(self._internalPtr, margin, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.DynamicsComponent:RemoveAllCollisionShapes()"
            "    GTEngine.System.DynamicsComponent.RemoveAllCollisionShapes(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:RemoveCollisionShapeAtIndex(index)"
            "    GTEngine.System.DynamicsComponent.RemoveCollisionShapeAtIndex(self._internalPtr, index);"
            "end;"


            "function GTEngine.DynamicsComponent:GetCollisionShapeCount()"
            "    return GTEngine.System.DynamicsComponent.GetCollisionShapeCount(self._internalPtr);"
            "end;"

            "function GTEngine.DynamicsComponent:GetCollisionShapeAtIndex(index)"
            "    return GTEngine.System.DynamicsComponent.GetCollisionShapeAtIndex(self._internalPtr, index);"
            "end;"

            "function GTEngine.DynamicsComponent:IsUsingConvexHullsFromModel()"
            "    return GTEngine.System.DynamicsComponent.IsUsingConvexHullsFromModel(self._internalPtr);"
            "end;"


            "function GTEngine.DynamicsComponent:SetCollisionShapeOffset(index, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.DynamicsComponent.SetCollisionShapeOffset(self._internalPtr, index, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.DynamicsComponent:SetBoxCollisionShapeHalfExtents(index, halfX, halfY, halfZ)"
            "    return GTEngine.System.DynamicsComponent.SetBoxCollisionShapeHalfExtents(self._internalPtr, index, halfX, halfY, halfZ);"
            "end;"

            "function GTEngine.DynamicsComponent:SetSphereCollisionShapeRadius(index, radius)"
            "    return GTEngine.System.DynamicsComponent.SetSphereCollisionShapeRadius(self._internalPtr, index, radius);"
            "end;"

            "function GTEngine.DynamicsComponent:SetEllipsoidCollisionShapeRadius(index, radiusX, radiusY, radiusZ)"
            "    return GTEngine.System.DynamicsComponent.SetEllipsoidCollisionShapeRadius(self._internalPtr, index, radiusX, radiusY, radiusZ);"
            "end;"

            "function GTEngine.DynamicsComponent:SetCylinderCollisionShapeSize(index, radius, length)"
            "    return GTEngine.System.DynamicsComponent.SetCylinderCollisionShapeSize(self._internalPtr, index, radius, length);"
            "end;"

            "function GTEngine.DynamicsComponent:SetCapsuleCollisionShapeSize(index, radius, height)"
            "    return GTEngine.System.DynamicsComponent.SetCapsuleCollisionShapeSize(self._internalPtr, index, radius, height);"
            "end;"

            "function GTEngine.DynamicsComponent:SetModelConvexHullsMargins(index, margin)"
            "    return GTEngine.System.DynamicsComponent.SetModelConvexHullsMargins(self._internalPtr, index, margin);"
            "end;"



            // ProximityComponent
            "GTEngine.ProximityComponent = {};"
            "GTEngine.ProximityComponent.__index = GTEngine.ProximityComponent;"

            "function GTEngine.ProximityComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.ProximityComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.ProximityComponent:SetCollisionFilter(group, mask)"
            "    GTEngine.System.ProximityComponent.SetCollisionFilter(self._internalPtr, group, mask);"
            "end;"

            "function GTEngine.ProximityComponent:GetCollisionGroup()"
            "    return GTEngine.System.ProximityComponent.GetCollisionGroup(self._internalPtr);"
            "end;"

            "function GTEngine.ProximityComponent:GetCollisionMask()"
            "    return GTEngine.System.ProximityComponent.GetCollisionMask(self._internalPtr);"
            "end;"


            "function GTEngine.ProximityComponent:AddCollisionGroup(groupName)"
            "    return GTEngine.System.ProximityComponent.AddCollisionGroup(self._internalPtr, groupName);"
            "end;"

            "function GTEngine.ProximityComponent:RemoveCollisionGroup(groupName)"
            "    return GTEngine.System.ProximityComponent.RemoveCollisionGroup(self._internalPtr, groupName);"
            "end;"

            "function GTEngine.ProximityComponent:RemoveCollisionGroupByIndex(groupIndex)"
            "    return GTEngine.System.ProximityComponent.RemoveCollisionGroupByIndex(self._internalPtr, groupIndex);"
            "end;"

            "function GTEngine.ProximityComponent:GetCollisionGroupCount()"
            "    return GTEngine.System.ProximityComponent.GetCollisionGroupCount(self._internalPtr);"
            "end;"

            "function GTEngine.ProximityComponent:GetCollisionGroupName(groupIndex)"
            "    return GTEngine.System.ProximityComponent.GetCollisionGroupName(self._internalPtr, groupIndex);"
            "end;"

            "function GTEngine.ProximityComponent:AddCollisionGroupMask(groupName)"
            "    return GTEngine.System.ProximityComponent.AddCollisionGroupMask(self._internalPtr, groupName);"
            "end;"

            "function GTEngine.ProximityComponent:RemoveCollisionGroupMask(groupName)"
            "    return GTEngine.System.ProximityComponent.RemoveCollisionGroupMask(self._internalPtr, groupName);"
            "end;"

            "function GTEngine.ProximityComponent:RemoveCollisionGroupMaskByIndex(groupIndex)"
            "    return GTEngine.System.ProximityComponent.RemoveCollisionGroupMaskByIndex(self._internalPtr, groupIndex);"
            "end;"

            "function GTEngine.ProximityComponent:GetCollisionGroupMaskCount()"
            "    return GTEngine.System.ProximityComponent.GetCollisionGroupMaskCount(self._internalPtr);"
            "end;"

            "function GTEngine.ProximityComponent:GetCollisionGroupMaskName(groupIndex)"
            "    return GTEngine.System.ProximityComponent.GetCollisionGroupMaskName(self._internalPtr, groupIndex);"
            "end;"



            "function GTEngine.ProximityComponent:AddBoxCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddBoxCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.ProximityComponent:AddSphereCollisionShape(radius, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddSphereCollisionShape(self._internalPtr, radius, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.ProximityComponent:AddEllipsoidCollisionShape(radiusX, radiusY, radiusZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddEllipsoidCollisionShape(self._internalPtr, radiusX, radiusY, radiusZ, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.ProximityComponent:AddCylinderXCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddCylinderXCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.ProximityComponent:AddCylinderYCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddCylinderYCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.ProximityComponent:AddCylinderZCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddCylinderZCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.ProximityComponent:AddCapsuleXCollisionShape(radius, length, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddCapsuleXCollisionShape(self._internalPtr, radius, length, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.ProximityComponent:AddCapsuleYCollisionShape(radius, length, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddCapsuleYCollisionShape(self._internalPtr, radius, length, offsetX, offsetY, offsetZ);"
            "end;"

            "function GTEngine.ProximityComponent:AddCapsuleZCollisionShape(radius, length, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddCapsuleZCollisionShape(self._internalPtr, radius, length, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.ProximityComponent:AddModelConvexHullsCollisionShape(margin, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.AddModelConvexHullsCollisionShape(self._internalPtr, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.ProximityComponent:RemoveAllCollisionShapes()"
            "    GTEngine.System.ProximityComponent.RemoveAllCollisionShapes(self._internalPtr);"
            "end;"

            "function GTEngine.ProximityComponent:RemoveCollisionShapeAtIndex(index)"
            "    GTEngine.System.ProximityComponent.RemoveCollisionShapeAtIndex(self._internalPtr, index);"
            "end;"


            "function GTEngine.ProximityComponent:GetCollisionShapeCount()"
            "    return GTEngine.System.ProximityComponent.GetCollisionShapeCount(self._internalPtr);"
            "end;"

            "function GTEngine.ProximityComponent:GetCollisionShapeAtIndex(index)"
            "    return GTEngine.System.ProximityComponent.GetCollisionShapeAtIndex(self._internalPtr, index);"
            "end;"

            "function GTEngine.ProximityComponent:IsUsingConvexHullsFromModel()"
            "    return GTEngine.System.ProximityComponent.IsUsingConvexHullsFromModel(self._internalPtr);"
            "end;"


            "function GTEngine.ProximityComponent:SetCollisionShapeOffset(index, offsetX, offsetY, offsetZ)"
            "    GTEngine.System.ProximityComponent.SetCollisionShapeOffset(self._internalPtr, index, offsetX, offsetY, offsetZ);"
            "end;"


            "function GTEngine.ProximityComponent:SetBoxCollisionShapeHalfExtents(index, halfX, halfY, halfZ)"
            "    return GTEngine.System.ProximityComponent.SetBoxCollisionShapeHalfExtents(self._internalPtr, index, halfX, halfY, halfZ);"
            "end;"

            "function GTEngine.ProximityComponent:SetSphereCollisionShapeRadius(index, radius)"
            "    return GTEngine.System.ProximityComponent.SetSphereCollisionShapeRadius(self._internalPtr, index, radius);"
            "end;"

            "function GTEngine.ProximityComponent:SetEllipsoidCollisionShapeRadius(index, radiusX, radiusY, radiusZ)"
            "    return GTEngine.System.ProximityComponent.SetEllipsoidCollisionShapeRadius(self._internalPtr, index, radiusX, radiusY, radiusZ);"
            "end;"

            "function GTEngine.ProximityComponent:SetCylinderCollisionShapeSize(index, radius, length)"
            "    return GTEngine.System.ProximityComponent.SetCylinderCollisionShapeSize(self._internalPtr, index, radius, length);"
            "end;"

            "function GTEngine.ProximityComponent:SetCapsuleCollisionShapeSize(index, radius, height)"
            "    return GTEngine.System.ProximityComponent.SetCapsuleCollisionShapeSize(self._internalPtr, index, radius, height);"
            "end;"

            "function GTEngine.ProximityComponent:SetModelConvexHullsMargins(index, margin)"
            "    return GTEngine.System.ProximityComponent.SetModelConvexHullsMargins(self._internalPtr, index, margin);"
            "end;"


            "function GTEngine.ProximityComponent:IsAnythingInside()"
            "    return GTEngine.System.ProximityComponent.IsAnythingInside(self._internalPtr);"
            "end;"



            // ScriptComponent
            "GTEngine.ScriptComponent = {};"
            "GTEngine.ScriptComponent.__index = GTEngine.ScriptComponent;"

            "function GTEngine.ScriptComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.ScriptComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.ScriptComponent:AddScript(relativePath)"
            "    return GTEngine.System.ScriptComponent.AddScript(self._internalPtr, relativePath);"
            "end;"

            "function GTEngine.ScriptComponent:RemoveScriptByRelativePath(relativePath)"
            "    GTEngine.System.ScriptComponent.RemoveScriptByRelativePath(self._internalPtr, relativePath);"
            "end;"

            "function GTEngine.ScriptComponent:RemoveScriptByIndex(index)"
            "    GTEngine.System.ScriptComponent.RemoveScriptByIndex(self._internalPtr, index);"
            "end;"

            "function GTEngine.ScriptComponent:ReloadScript(index, newRelativePath)"
            "    GTEngine.System.ScriptComponent.ReloadScript(self._internalPtr, index, newRelativePath);"
            "end;"

            "function GTEngine.ScriptComponent:GetScriptRelativeFilePaths()"
            "    return GTEngine.System.ScriptComponent.GetScriptRelativeFilePaths(self._internalPtr);"
            "end;"

            "function GTEngine.ScriptComponent:GetScriptAbsoluteFilePaths()"
            "    return GTEngine.System.ScriptComponent.GetScriptAbsoluteFilePaths(self._internalPtr);"
            "end;"

            "function GTEngine.ScriptComponent:IsUsingScript(relativePath)"
            "    return GTEngine.System.ScriptComponent.IsUsingScript(self._internalPtr, relativePath);"
            "end;"

            "function GTEngine.ScriptComponent:GetPublicVariableNamesAndTypesByIndex(index)"
            "    return GTEngine.System.ScriptComponent.GetPublicVariableNamesAndTypesByIndex(self._internalPtr, index);"
            "end;"

            "function GTEngine.ScriptComponent:GetPublicVariableNamesAndTypesByRelativePath(index)"
            "    return GTEngine.System.ScriptComponent.GetPublicVariableNamesAndTypesByRelativePath(self._internalPtr, index);"
            "end;"

            "function GTEngine.ScriptComponent:GetPublicVariableNamesAndValues()"
            "    return GTEngine.System.ScriptComponent.GetPublicVariableNamesAndValues(self._internalPtr);"
            "end;"
                
            "function GTEngine.ScriptComponent:GetPublicVariableValue(name)"
            "    return GTEngine.System.ScriptComponent.GetPublicVariableValue(self._internalPtr, name);"
            "end;"

            "function GTEngine.ScriptComponent:SetPublicVariableValue(name, value)"
            "    return GTEngine.System.ScriptComponent.SetPublicVariableValue(self._internalPtr, name, value);"
            "end;"



            // ParticleSystemComponent
            "GTEngine.ParticleSystemComponent = {};"
            "GTEngine.ParticleSystemComponent.__index = GTEngine.ParticleSystemComponent;"

            "function GTEngine.ParticleSystemComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.ParticleSystemComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.ParticleSystemComponent:SetFromFile(relativePath)"
            "    return GTEngine.System.ParticleSystemComponent.SetFromFile(self._internalPtr, relativePath);"
            "end;"

            "function GTEngine.ParticleSystemComponent:GetRelativeFilePath()"
            "    return GTEngine.System.ParticleSystemComponent.GetRelativeFilePath(self._internalPtr);"
            "end;"

                
            "function GTEngine.ParticleSystemComponent:Play()"
            "    return GTEngine.System.ParticleSystemComponent.Play(self._internalPtr);"
            "end;"

            "function GTEngine.ParticleSystemComponent:Pause()"
            "    return GTEngine.System.ParticleSystemComponent.Pause(self._internalPtr);"
            "end;"

            "function GTEngine.ParticleSystemComponent:IsPlaying()"
            "    return GTEngine.System.ParticleSystemComponent.IsPlaying(self._internalPtr);"
            "end;"


            "function GTEngine.ParticleSystemComponent:PlayOnStartup(playOnStartup)"
            "    return GTEngine.System.ParticleSystemComponent.PlayOnStartup(self._internalPtr, playOnStartup);"
            "end;"

            "function GTEngine.ParticleSystemComponent:IsPlayingOnStartup()"
            "    return GTEngine.System.ParticleSystemComponent.IsPlayingOnStartup(self._internalPtr);"
            "end;"


            "function GTEngine.ParticleSystemComponent:Reset()"
            "    return GTEngine.System.ParticleSystemComponent.Reset(self._internalPtr);"
            "end;"


            "function GTEngine.ParticleSystemComponent:GetEmitterLifetime(emitterIndex)"
            "    return GTEngine.System.ParticleSystemComponent.GetEmitterLifetime(self._internalPtr, emitterIndex);"
            "end;"



            //PrefabComponent
            "GTEngine.PrefabComponent = {};"
            "GTEngine.PrefabComponent.__index = GTEngine.PrefabComponent;"

            "function GTEngine.PrefabComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.PrefabComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.PrefabComponent:GetPrefabRelativePath()"
            "    return GTEngine.System.PrefabComponent.GetPrefabRelativePath(self._internalPtr);"
            "end;"

            "function GTEngine.PrefabComponent:GetLocalHierarchyID()"
            "    return GTEngine.System.PrefabComponent.GetLocalHierarchyID(self._internalPtr);"
            "end;"



            // EditorMetadataComponent
            "GTEngine.EditorMetadataComponent = {};"
            "GTEngine.EditorMetadataComponent.__index = GTEngine.EditorMetadataComponent;"

            "function GTEngine.EditorMetadataComponent.Create(internalPtr)"
            "    local new = {};"
            "    setmetatable(new, GTEngine.EditorMetadataComponent);"
            "        new._internalPtr = internalPtr;"
            "    return new;"
            "end;"

            "function GTEngine.EditorMetadataComponent:Select()"
            "    GTEngine.System.EditorMetadataComponent.Select(self._internalPtr);"
            "end;"

            "function GTEngine.EditorMetadataComponent:Deselect()"
            "    GTEngine.System.EditorMetadataComponent.Deselect(self._internalPtr);"
            "end;"

            "function GTEngine.EditorMetadataComponent:IsSelected()"
            "    return GTEngine.System.EditorMetadataComponent.IsSelected(self._internalPtr);"
            "end;"


            "function GTEngine.EditorMetadataComponent:ShowSprite(texturePath, colour)"
            "    GTEngine.System.EditorMetadataComponent.ShowSprite(self._internalPtr, texturePath, colour);"
            "end;"

            "function GTEngine.EditorMetadataComponent:HideSprite()"
            "    GTEngine.System.EditorMetadataComponent.HideSprite(self._internalPtr);"
            "end;"

            "function GTEngine.EditorMetadataComponent:IsShowingSprite()"
            "    return GTEngine.System.EditorMetadataComponent.IsShowingSprite(self._internalPtr);"
            "end;"

            "function GTEngine.EditorMetadataComponent:GetSpriteTexturePath()"
            "    return GTEngine.System.EditorMetadataComponent.GetSpriteTexturePath(self._internalPtr);"
            "end;"


            "function GTEngine.EditorMetadataComponent:ShowDirectionArrow(texturePath, colour)"
            "    GTEngine.System.EditorMetadataComponent.ShowDirectionArrow(self._internalPtr, texturePath, colour);"
            "end;"

            "function GTEngine.EditorMetadataComponent:HideDirectionArrow()"
            "    GTEngine.System.EditorMetadataComponent.HideDirectionArrow(self._internalPtr);"
            "end;"

            "function GTEngine.EditorMetadataComponent:IsShowingDirectionArrow()"
            "    return GTEngine.System.EditorMetadataComponent.IsShowingDirectionArrow(self._internalPtr);"
            "end;"
        );



        if (successful)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("System");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push("CameraComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "Set3DProjection",      CameraComponentFFI::Set3DProjection);
                        script.SetTableFunction(-1, "GetFOV",               CameraComponentFFI::GetFOV);
                        script.SetTableFunction(-1, "GetAspectRatio",       CameraComponentFFI::GetAspectRatio);
                        script.SetTableFunction(-1, "GetNearClippingPlane", CameraComponentFFI::GetNearClippingPlane);
                        script.SetTableFunction(-1, "GetFarClippingPlane",  CameraComponentFFI::GetFarClippingPlane);
                    }
                    script.Pop(1);


                    script.Push("ModelComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "SetModel",                   ModelComponentFFI::SetModel);
                        script.SetTableFunction(-1, "GetModelPath",               ModelComponentFFI::GetModelPath);
                        script.SetTableFunction(-1, "EnableShadowCasting",        ModelComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",       ModelComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",     ModelComponentFFI::IsShadowCastingEnabled);
                        script.SetTableFunction(-1, "SetMaterial",                ModelComponentFFI::SetMaterial);
                        script.SetTableFunction(-1, "GetMaterialPath",            ModelComponentFFI::GetMaterialPath);
                        script.SetTableFunction(-1, "GetMaterialCount",           ModelComponentFFI::GetMaterialCount);
                        script.SetTableFunction(-1, "SetMaterialParameter",       ModelComponentFFI::SetMaterialParameter);
                        script.SetTableFunction(-1, "PlayAnimationSegmentByName", ModelComponentFFI::PlayAnimationSegmentByName);
                        script.SetTableFunction(-1, "PlayAnimationSequence",      ModelComponentFFI::PlayAnimationSequence);
                        script.SetTableFunction(-1, "PauseAnimation",             ModelComponentFFI::PauseAnimation);
                        script.SetTableFunction(-1, "StopAnimation",              ModelComponentFFI::StopAnimation);
                        script.SetTableFunction(-1, "GetBoneIndexByName",         ModelComponentFFI::GetBoneIndexByName);
                        script.SetTableFunction(-1, "GetBoneTransformByName",     ModelComponentFFI::GetBoneTransformByName);
                        script.SetTableFunction(-1, "GetBoneTransformByIndex",    ModelComponentFFI::GetBoneTransformByIndex);
                        script.SetTableFunction(-1, "SetBoneTransformByName",     ModelComponentFFI::SetBoneTransformByName);
                        script.SetTableFunction(-1, "SetBoneTransformByIndex",    ModelComponentFFI::SetBoneTransformByIndex);
                    }
                    script.Pop(1);


                    script.Push("PointLightComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "SetColour",               PointLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               PointLightComponentFFI::GetColour);
                        script.SetTableFunction(-1, "SetRadius",               PointLightComponentFFI::SetRadius);
                        script.SetTableFunction(-1, "GetRadius",               PointLightComponentFFI::GetRadius);
                        script.SetTableFunction(-1, "SetFalloff",              PointLightComponentFFI::SetFalloff);
                        script.SetTableFunction(-1, "GetFalloff",              PointLightComponentFFI::GetFalloff);
                        script.SetTableFunction(-1, "EnableShadowCasting",     PointLightComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",    PointLightComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",  PointLightComponentFFI::IsShadowCastingEnabled);
                    }
                    script.Pop(1);


                    script.Push("SpotLightComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "SetColour",               SpotLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               SpotLightComponentFFI::GetColour);
                        script.SetTableFunction(-1, "SetLength",               SpotLightComponentFFI::SetLength);
                        script.SetTableFunction(-1, "GetLength",               SpotLightComponentFFI::GetLength);
                        script.SetTableFunction(-1, "SetFalloff",              SpotLightComponentFFI::SetFalloff);
                        script.SetTableFunction(-1, "GetFalloff",              SpotLightComponentFFI::GetFalloff);
                        script.SetTableFunction(-1, "EnableShadowCasting",     SpotLightComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",    SpotLightComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",  SpotLightComponentFFI::IsShadowCastingEnabled);
                        script.SetTableFunction(-1, "SetAngles",               SpotLightComponentFFI::SetAngles);
                        script.SetTableFunction(-1, "GetAngles",               SpotLightComponentFFI::GetAngles);
                    }
                    script.Pop(1);


                    script.Push("DirectionalLightComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "SetColour",               DirectionalLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               DirectionalLightComponentFFI::GetColour);
                        script.SetTableFunction(-1, "EnableShadowCasting",     DirectionalLightComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",    DirectionalLightComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",  DirectionalLightComponentFFI::IsShadowCastingEnabled);
                    }
                    script.Pop(1);


                    script.Push("AmbientLightComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "SetColour",               AmbientLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               AmbientLightComponentFFI::GetColour);
                    }
                    script.Pop(1);


                    script.Push("DynamicsComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "SetMass",                                        DynamicsComponentFFI::SetMass);
                        script.SetTableFunction(-1, "GetMass",                                        DynamicsComponentFFI::GetMass);
                        script.SetTableFunction(-1, "IsStatic",                                       DynamicsComponentFFI::IsStatic);
                        script.SetTableFunction(-1, "IsKinematic",                                    DynamicsComponentFFI::IsKinematic);
                        script.SetTableFunction(-1, "SetFriction",                                    DynamicsComponentFFI::SetFriction);
                        script.SetTableFunction(-1, "GetFriction",                                    DynamicsComponentFFI::GetFriction);
                        script.SetTableFunction(-1, "SetRestitution",                                 DynamicsComponentFFI::SetRestitution);
                        script.SetTableFunction(-1, "GetRestitution",                                 DynamicsComponentFFI::GetRestitution);
                        script.SetTableFunction(-1, "SetDamping",                                     DynamicsComponentFFI::SetDamping);
                        script.SetTableFunction(-1, "GetLinearDamping",                               DynamicsComponentFFI::GetLinearDamping);
                        script.SetTableFunction(-1, "GetAngularDamping",                              DynamicsComponentFFI::GetAngularDamping);

                        script.SetTableFunction(-1, "SetLinearVelocity",                              DynamicsComponentFFI::SetLinearVelocity);
                        script.SetTableFunction(-1, "GetLinearVelocity",                              DynamicsComponentFFI::GetLinearVelocity);
                        script.SetTableFunction(-1, "SetAngularVelocity",                             DynamicsComponentFFI::SetAngularVelocity);
                        script.SetTableFunction(-1, "GetAngularVelocity",                             DynamicsComponentFFI::GetAngularVelocity);

                        script.SetTableFunction(-1, "SetLinearFactor",                                DynamicsComponentFFI::SetLinearFactor);
                        script.SetTableFunction(-1, "SetAngularFactor",                               DynamicsComponentFFI::SetAngularFactor);

                        script.SetTableFunction(-1, "SetGravity",                                     DynamicsComponentFFI::SetGravity);
                        script.SetTableFunction(-1, "GetGravity",                                     DynamicsComponentFFI::GetGravity);
                        script.SetTableFunction(-1, "ApplyGravity",                                   DynamicsComponentFFI::ApplyGravity);

                        script.SetTableFunction(-1, "ApplyCentralForce",                              DynamicsComponentFFI::ApplyCentralForce);
                        script.SetTableFunction(-1, "ApplyTorque",                                    DynamicsComponentFFI::ApplyTorque);
                        script.SetTableFunction(-1, "ApplyCentralImpulse",                            DynamicsComponentFFI::ApplyCentralImpulse);
                        script.SetTableFunction(-1, "ApplyImpulse",                                   DynamicsComponentFFI::ApplyImpulse);
                        script.SetTableFunction(-1, "ApplyTorqueImpulse",                             DynamicsComponentFFI::ApplyTorqueImpulse);

                        script.SetTableFunction(-1, "DisableDeactivation",                            DynamicsComponentFFI::DisableDeactivation);
                        script.SetTableFunction(-1, "EnableDeactivation",                             DynamicsComponentFFI::EnableDeactivation);
                        script.SetTableFunction(-1, "IsDeactivationEnabled",                          DynamicsComponentFFI::IsDeactivationEnabled);
                        script.SetTableFunction(-1, "Activate",                                       DynamicsComponentFFI::Activate);

                        script.SetTableFunction(-1, "EnableNavigationMeshGeneration",                 DynamicsComponentFFI::EnableNavigationMeshGeneration);
                        script.SetTableFunction(-1, "DisableNavigationMeshGeneration",                DynamicsComponentFFI::DisableNavigationMeshGeneration);
                        script.SetTableFunction(-1, "IsNavigationMeshGenerationEnabled",              DynamicsComponentFFI::IsNavigationMeshGenerationEnabled);


                        script.SetTableFunction(-1, "SetCollisionFilter",                             DynamicsComponentFFI::SetCollisionFilter);
                        script.SetTableFunction(-1, "GetCollisionGroup",                              DynamicsComponentFFI::GetCollisionGroup);
                        script.SetTableFunction(-1, "GetCollisionMask",                               DynamicsComponentFFI::GetCollisionMask);

                        script.SetTableFunction(-1, "AddCollisionGroup",                              DynamicsComponentFFI::AddCollisionGroup);
                        script.SetTableFunction(-1, "RemoveCollisionGroup",                           DynamicsComponentFFI::RemoveCollisionGroup);
                        script.SetTableFunction(-1, "RemoveCollisionGroupByIndex",                    DynamicsComponentFFI::RemoveCollisionGroupByIndex);
                        script.SetTableFunction(-1, "GetCollisionGroupCount",                         DynamicsComponentFFI::GetCollisionGroupCount);
                        script.SetTableFunction(-1, "GetCollisionGroupName",                          DynamicsComponentFFI::GetCollisionGroupName);
                        script.SetTableFunction(-1, "AddCollisionGroupMask",                          DynamicsComponentFFI::AddCollisionGroupMask);
                        script.SetTableFunction(-1, "RemoveCollisionGroupMask",                       DynamicsComponentFFI::RemoveCollisionGroupMask);
                        script.SetTableFunction(-1, "RemoveCollisionGroupMaskByIndex",                DynamicsComponentFFI::RemoveCollisionGroupMaskByIndex);
                        script.SetTableFunction(-1, "GetCollisionGroupMaskCount",                     DynamicsComponentFFI::GetCollisionGroupMaskCount);
                        script.SetTableFunction(-1, "GetCollisionGroupMaskName",                      DynamicsComponentFFI::GetCollisionGroupMaskName);

                        script.SetTableFunction(-1, "AddBoxCollisionShape",                           DynamicsComponentFFI::AddBoxCollisionShape);
                        script.SetTableFunction(-1, "AddSphereCollisionShape",                        DynamicsComponentFFI::AddSphereCollisionShape);
                        script.SetTableFunction(-1, "AddEllipsoidCollisionShape",                     DynamicsComponentFFI::AddEllipsoidCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderXCollisionShape",                     DynamicsComponentFFI::AddCylinderXCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderYCollisionShape",                     DynamicsComponentFFI::AddCylinderYCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderZCollisionShape",                     DynamicsComponentFFI::AddCylinderZCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleXCollisionShape",                      DynamicsComponentFFI::AddCapsuleXCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleYCollisionShape",                      DynamicsComponentFFI::AddCapsuleYCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleZCollisionShape",                      DynamicsComponentFFI::AddCapsuleZCollisionShape);
                        script.SetTableFunction(-1, "AddModelConvexHullsCollisionShape",              DynamicsComponentFFI::AddModelConvexHullsCollisionShape);
                        script.SetTableFunction(-1, "RemoveAllCollisionShapes",                       DynamicsComponentFFI::RemoveAllCollisionShapes);
                        script.SetTableFunction(-1, "RemoveCollisionShapeAtIndex",                    DynamicsComponentFFI::RemoveCollisionShapeAtIndex);
                        script.SetTableFunction(-1, "GetCollisionShapeCount",                         DynamicsComponentFFI::GetCollisionShapeCount);
                        script.SetTableFunction(-1, "GetCollisionShapeAtIndex",                       DynamicsComponentFFI::GetCollisionShapeAtIndex);
                        script.SetTableFunction(-1, "SetCollisionShapeOffset",                        DynamicsComponentFFI::SetCollisionShapeOffset);
                        script.SetTableFunction(-1, "SetBoxCollisionShapeHalfExtents",                DynamicsComponentFFI::SetBoxCollisionShapeHalfExtents);
                        script.SetTableFunction(-1, "SetSphereCollisionShapeRadius",                  DynamicsComponentFFI::SetSphereCollisionShapeRadius);
                        script.SetTableFunction(-1, "SetEllipsoidCollisionShapeRadius",               DynamicsComponentFFI::SetEllipsoidCollisionShapeRadius);
                        script.SetTableFunction(-1, "SetCylinderCollisionShapeSize",                  DynamicsComponentFFI::SetCylinderCollisionShapeSize);
                        script.SetTableFunction(-1, "SetCapsuleCollisionShapeSize",                   DynamicsComponentFFI::SetCapsuleCollisionShapeSize);
                        script.SetTableFunction(-1, "SetModelConvexHullsMargins",                     DynamicsComponentFFI::SetModelConvexHullsMargins);
                    }
                    script.Pop(1);


                    script.Push("ProximityComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "SetCollisionFilter",                             ProximityComponentFFI::SetCollisionFilter);
                        script.SetTableFunction(-1, "GetCollisionGroup",                              ProximityComponentFFI::GetCollisionGroup);
                        script.SetTableFunction(-1, "GetCollisionMask",                               ProximityComponentFFI::GetCollisionMask);

                        script.SetTableFunction(-1, "AddCollisionGroup",                              ProximityComponentFFI::AddCollisionGroup);
                        script.SetTableFunction(-1, "RemoveCollisionGroup",                           ProximityComponentFFI::RemoveCollisionGroup);
                        script.SetTableFunction(-1, "RemoveCollisionGroupByIndex",                    ProximityComponentFFI::RemoveCollisionGroupByIndex);
                        script.SetTableFunction(-1, "GetCollisionGroupCount",                         ProximityComponentFFI::GetCollisionGroupCount);
                        script.SetTableFunction(-1, "GetCollisionGroupName",                          ProximityComponentFFI::GetCollisionGroupName);
                        script.SetTableFunction(-1, "AddCollisionGroupMask",                          ProximityComponentFFI::AddCollisionGroupMask);
                        script.SetTableFunction(-1, "RemoveCollisionGroupMask",                       ProximityComponentFFI::RemoveCollisionGroupMask);
                        script.SetTableFunction(-1, "RemoveCollisionGroupMaskByIndex",                ProximityComponentFFI::RemoveCollisionGroupMaskByIndex);
                        script.SetTableFunction(-1, "GetCollisionGroupMaskCount",                     ProximityComponentFFI::GetCollisionGroupMaskCount);
                        script.SetTableFunction(-1, "GetCollisionGroupMaskName",                      ProximityComponentFFI::GetCollisionGroupMaskName);

                        script.SetTableFunction(-1, "AddBoxCollisionShape",                           ProximityComponentFFI::AddBoxCollisionShape);
                        script.SetTableFunction(-1, "AddSphereCollisionShape",                        ProximityComponentFFI::AddSphereCollisionShape);
                        script.SetTableFunction(-1, "AddEllipsoidCollisionShape",                     ProximityComponentFFI::AddEllipsoidCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderXCollisionShape",                     ProximityComponentFFI::AddCylinderXCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderYCollisionShape",                     ProximityComponentFFI::AddCylinderYCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderZCollisionShape",                     ProximityComponentFFI::AddCylinderZCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleXCollisionShape",                      ProximityComponentFFI::AddCapsuleXCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleYCollisionShape",                      ProximityComponentFFI::AddCapsuleYCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleZCollisionShape",                      ProximityComponentFFI::AddCapsuleZCollisionShape);
                        script.SetTableFunction(-1, "AddModelConvexHullsCollisionShape",              ProximityComponentFFI::AddModelConvexHullsCollisionShape);
                        script.SetTableFunction(-1, "RemoveAllCollisionShapes",                       ProximityComponentFFI::RemoveAllCollisionShapes);
                        script.SetTableFunction(-1, "RemoveCollisionShapeAtIndex",                    ProximityComponentFFI::RemoveCollisionShapeAtIndex);
                        script.SetTableFunction(-1, "GetCollisionShapeCount",                         ProximityComponentFFI::GetCollisionShapeCount);
                        script.SetTableFunction(-1, "GetCollisionShapeAtIndex",                       ProximityComponentFFI::GetCollisionShapeAtIndex);
                        script.SetTableFunction(-1, "SetCollisionShapeOffset",                        ProximityComponentFFI::SetCollisionShapeOffset);
                        script.SetTableFunction(-1, "SetBoxCollisionShapeHalfExtents",                ProximityComponentFFI::SetBoxCollisionShapeHalfExtents);
                        script.SetTableFunction(-1, "SetSphereCollisionShapeRadius",                  ProximityComponentFFI::SetSphereCollisionShapeRadius);
                        script.SetTableFunction(-1, "SetEllipsoidCollisionShapeRadius",               ProximityComponentFFI::SetEllipsoidCollisionShapeRadius);
                        script.SetTableFunction(-1, "SetCylinderCollisionShapeSize",                  ProximityComponentFFI::SetCylinderCollisionShapeSize);
                        script.SetTableFunction(-1, "SetCapsuleCollisionShapeSize",                   ProximityComponentFFI::SetCapsuleCollisionShapeSize);
                        script.SetTableFunction(-1, "SetModelConvexHullsMargins",                     ProximityComponentFFI::SetModelConvexHullsMargins);

                        script.SetTableFunction(-1, "IsAnythingInside",                               ProximityComponentFFI::IsAnythingInside);
                    }
                    script.Pop(1);


                    script.Push("ParticleSystemComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "SetFromFile",         ParticleSystemComponentFFI::SetFromFile);
                        script.SetTableFunction(-1, "GetRelativeFilePath", ParticleSystemComponentFFI::GetRelativeFilePath);
                        script.SetTableFunction(-1, "Play",                ParticleSystemComponentFFI::Play);
                        script.SetTableFunction(-1, "Pause",               ParticleSystemComponentFFI::Pause);
                        script.SetTableFunction(-1, "IsPlaying",           ParticleSystemComponentFFI::IsPlaying);
                        script.SetTableFunction(-1, "PlayOnStartup",       ParticleSystemComponentFFI::PlayOnStartup);
                        script.SetTableFunction(-1, "IsPlayingOnStartup",  ParticleSystemComponentFFI::IsPlayingOnStartup);
                        script.SetTableFunction(-1, "Reset",               ParticleSystemComponentFFI::Reset);
                        script.SetTableFunction(-1, "GetEmitterLifetime",  ParticleSystemComponentFFI::GetEmitterLifetime);
                    }
                    script.Pop(1);


                    script.Push("ScriptComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "AddScript",                                    ScriptComponentFFI::AddScript);
                        script.SetTableFunction(-1, "RemoveScriptByRelativePath",                   ScriptComponentFFI::RemoveScriptByRelativePath);
                        script.SetTableFunction(-1, "RemoveScriptByIndex",                          ScriptComponentFFI::RemoveScriptByIndex);
                        script.SetTableFunction(-1, "ReloadScript",                                 ScriptComponentFFI::ReloadScript);
                        script.SetTableFunction(-1, "GetScriptRelativeFilePaths",                   ScriptComponentFFI::GetScriptRelativeFilePaths);
                        script.SetTableFunction(-1, "GetScriptAbsoluteFilePaths",                   ScriptComponentFFI::GetScriptAbsoluteFilePaths);
                        script.SetTableFunction(-1, "IsUsingScript",                                ScriptComponentFFI::IsUsingScript);
                        script.SetTableFunction(-1, "GetPublicVariableNamesAndTypesByIndex",        ScriptComponentFFI::GetPublicVariableNamesAndTypesByIndex);
                        script.SetTableFunction(-1, "GetPublicVariableNamesAndTypesByRelativePath", ScriptComponentFFI::GetPublicVariableNamesAndTypesByRelativePath);
                        script.SetTableFunction(-1, "GetPublicVariableNamesAndValues",              ScriptComponentFFI::GetPublicVariableNamesAndValues);
                        script.SetTableFunction(-1, "GetPublicVariableValue",                       ScriptComponentFFI::GetPublicVariableValue);
                        script.SetTableFunction(-1, "SetPublicVariableValue",                       ScriptComponentFFI::SetPublicVariableValue);
                    }
                    script.Pop(1);


                    script.Push("PrefabComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "GetPrefabRelativePath", PrefabComponentFFI::GetPrefabRelativePath);
                        script.SetTableFunction(-1, "GetLocalHierarchyID",   PrefabComponentFFI::GetLocalHierarchyID);
                    }
                    script.Pop(1);


                    script.Push("EditorMetadataComponent");
                    script.GetTableValue(-2);
                    assert(script.IsTable(-1));
                    {
                        script.SetTableFunction(-1, "Select",                  EditorMetadataComponentFFI::Select);
                        script.SetTableFunction(-1, "Deselect",                EditorMetadataComponentFFI::Deselect);
                        script.SetTableFunction(-1, "IsSelected",              EditorMetadataComponentFFI::IsSelected);
                        script.SetTableFunction(-1, "ShowSprite",              EditorMetadataComponentFFI::ShowSprite);
                        script.SetTableFunction(-1, "HideSprite",              EditorMetadataComponentFFI::HideSprite);
                        script.SetTableFunction(-1, "IsShowingSprite",         EditorMetadataComponentFFI::IsShowingSprite);
                        script.SetTableFunction(-1, "GetSpriteTexturePath",    EditorMetadataComponentFFI::GetSpriteTexturePath);
                        script.SetTableFunction(-1, "ShowDirectionArrow",      EditorMetadataComponentFFI::ShowDirectionArrow);
                        script.SetTableFunction(-1, "HideDirectionArrow",      EditorMetadataComponentFFI::HideDirectionArrow);
                        script.SetTableFunction(-1, "IsShowingDirectionArrow", EditorMetadataComponentFFI::IsShowingDirectionArrow);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }


        return successful;
    }




    //////////////////////////////////////////////////
    // GTEngine.System.CameraComponent

    namespace CameraComponentFFI
    {
        int Set3DProjection(GT::Script &script)
        {
            auto component = reinterpret_cast<CameraComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto fov    = script.ToFloat(2);
                auto aspect = script.ToFloat(3);
                auto zNear  = script.ToFloat(4);
                auto zFar   = script.ToFloat(5);
                component->Set3DProjection(fov, aspect, zNear, zFar);
            }

            return 0;
        }

        int GetFOV(GT::Script &script)
        {
            auto component = reinterpret_cast<CameraComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->perspective.fov);
            }
            else
            {
                script.Push(90.0f);
            }

            return 1;
        }

        int GetAspectRatio(GT::Script &script)
        {
            auto component = reinterpret_cast<CameraComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->perspective.aspect);
            }
            else
            {
                script.Push(16.0f / 9.0f);
            }

            return 1;
        }

        int GetNearClippingPlane(GT::Script &script)
        {
            auto component = reinterpret_cast<CameraComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->zNear);
            }
            else
            {
                script.Push(0.1);
            }

            return 1;
        }

        int GetFarClippingPlane(GT::Script &script)
        {
            auto component = reinterpret_cast<CameraComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->zFar);
            }
            else
            {
                script.Push(1000.0f);
            }

            return 1;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.ModelComponent
    namespace ModelComponentFFI
    {
        int SetModel(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            auto filePath  = script.ToString(2);

            if (component != nullptr && filePath != nullptr)
            {
                component->SetModel(filePath);
            }

            return 0;
        }

        int GetModelPath(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    script.Push(model->GetDefinition().GetRelativePath());
                }
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }

        int EnableShadowCasting(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->EnableShadowCasting();
            }

            return 0;
        }

        int DisableShadowCasting(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->DisableShadowCasting();
            }

            return 0;
        }

        int IsShadowCastingEnabled(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsShadowCastingEnabled());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }


        int SetMaterial(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto index        = script.ToInteger(2) - 1;        // Minus 1 because Lua is 1 based.
                auto materialPath = script.ToString(3);

                script.Push(component->SetMaterial(static_cast<size_t>(index), materialPath));
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int GetMaterialPath(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    auto index = script.ToInteger(2) - 1;   // Minus 1 because Lua is 1 based.

                    script.Push(model->meshes[index]->GetMaterial()->GetDefinition().relativePath.c_str());
                }
                else
                {
                    script.PushNil();
                }
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }

        int GetMaterialCount(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    script.Push(static_cast<int>(model->meshes.count));
                }
                else
                {
                    script.Push(0);
                }
            }
            else
            {
                script.Push(0);
            }

            return 1;
        }

        int SetMaterialParameter(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    auto index = script.ToInteger(2);
                    auto name  = script.ToString(3);

                    if (model->meshes.count > static_cast<size_t>(index))
                    {
                        auto mesh = model->meshes[index];
                        assert(mesh != nullptr);
                        {
                            auto material = mesh->GetMaterial();
                            if (material != nullptr)
                            {
                                if (script.IsNumber(4))
                                {
                                    // It's a scalar.
                                    material->SetParameter(name, script.ToFloat(4));
                                }
                                else if (script.IsString(4))
                                {
                                    // It's a string. Assume a path to a texture.
                                    material->SetParameter(name, Texture2DLibrary::Acquire(script.ToString(4)));
                                }
                                else
                                {
                                    // Check for math.vec2, math.vec3 or math.vec4.
                                    if (IsVector2(script, 4))
                                    {
                                        material->SetParameter(name, ToVector2(script, 4));
                                    }
                                    else if (IsVector3(script, 4))
                                    {
                                        material->SetParameter(name, ToVector3(script, 4));
                                    }
                                    else if (IsVector4(script, 4))
                                    {
                                        material->SetParameter(name, ToVector4(script, 4));
                                    }
                                }
                            }
                        }
                    }
                }
            }

            return 0;
        }


        int PlayAnimationSegmentByName(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    AnimationSequence sequence;
                    sequence.AddFrame(script.ToString(2), 0.0, script.ToBoolean(3));

                    model->PlayAnimation(sequence);
                }
            }

            return 0;
        }

        int PlayAnimationSequence(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    model->PlayAnimation(ToAnimationSequence(script, 2));
                }
            }

            return 0;
        }

        int PauseAnimation(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    model->PauseAnimation();
                }
            }

            return 0;
        }

        int StopAnimation(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    model->StopAnimation();
                }
            }

            return 0;
        }


        int GetBoneIndexByName(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    size_t boneIndex;
                    auto bone = model->GetBoneByName(script.ToString(2), &boneIndex);
                    if (bone != nullptr)
                    {
                        script.Push(static_cast<int>(boneIndex));
                    }
                    else
                    {
                        script.PushNil();
                    }
                }
                else
                {
                    script.PushNil();
                }
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }

        int GetBoneTransformByName(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    auto bone = model->GetBoneByName(script.ToString(2), nullptr);
                    if (bone != nullptr)
                    {
                        glm::vec3 position;
                        glm::quat orientation;
                        glm::vec3 scale;
                        bone->GetAbsoluteTransformComponents(position, orientation, scale);

                        script.PushNewTable();
                            
                        script.Push("position");
                        PushNewVector3(script, position);
                        script.SetTableValue(-3);

                        script.Push("orientation");
                        PushNewQuaternion(script, orientation);
                        script.SetTableValue(-3);

                        script.Push("scale");
                        PushNewVector3(script, scale);
                        script.SetTableValue(-3);
                    }
                    else
                    {
                        script.PushNil();
                    }
                }
                else
                {
                    script.PushNil();
                }
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }

        int GetBoneTransformByIndex(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    auto bone = model->GetBoneByIndex(static_cast<size_t>(script.ToInteger(2)));
                    if (bone != nullptr)
                    {
                        glm::vec3 position;
                        glm::quat orientation;
                        glm::vec3 scale;
                        bone->GetAbsoluteTransformComponents(position, orientation, scale);

                        script.PushNewTable();
                            
                        script.Push("position");
                        PushNewVector3(script, position);
                        script.SetTableValue(-3);

                        script.Push("orientation");
                        PushNewQuaternion(script, orientation);
                        script.SetTableValue(-3);

                        script.Push("scale");
                        PushNewVector3(script, scale);
                        script.SetTableValue(-3);
                    }
                    else
                    {
                        script.PushNil();
                    }
                }
                else
                {
                    script.PushNil();
                }
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int SetBoneTransformByName(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    auto bone = model->GetBoneByName(script.ToString(2), nullptr);
                    if (bone != nullptr)
                    {
                        glm::vec3 absolutePosition = ToVector3(script, 3);
                        glm::quat absoluteRotation = ToQuaternion(script, 4);

                        if (IsVector3(script, 5))
                        {
                            glm::vec3 absoluteScale = ToVector3(script, 5);
                            bone->SetAbsoluteTransform(absolutePosition, absoluteRotation, absoluteScale);
                        }
                        else
                        {
                            bone->SetAbsoluteTransform(absolutePosition, absoluteRotation);
                        }
                    }
                }
            }

            return 0;
        }

        int SetBoneTransformByIndex(GT::Script &script)
        {
            auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto model = component->GetModel();
                if (model != nullptr)
                {
                    auto bone = model->GetBoneByIndex(static_cast<size_t>(script.ToInteger(2)));
                    if (bone != nullptr)
                    {
                        glm::vec3 absolutePosition = ToVector3(script, 3);
                        glm::quat absoluteRotation = ToQuaternion(script, 4);

                        if (IsVector3(script, 5))
                        {
                            glm::vec3 absoluteScale = ToVector3(script, 5);
                            bone->SetAbsoluteTransform(absolutePosition, absoluteRotation, absoluteScale);
                        }
                        else
                        {
                            bone->SetAbsoluteTransform(absolutePosition, absoluteRotation);
                        }
                    }
                }
            }

            return 0;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.PointLightComponent
    namespace PointLightComponentFFI
    {
        int SetColour(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 colour;

                if (script.IsTable(2))
                {
                    colour = ToVector3(script, 2);
                }
                else
                {
                    colour.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    colour.y = script.IsNumber(3) ? script.ToFloat(3) : colour.x;
                    colour.z = script.IsNumber(4) ? script.ToFloat(4) : colour.x;
                }

                component->SetColour(colour);
            }

            return 0;
        }

        int GetColour(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                PushNewVector3(script, component->GetColour());
            }
            else
            {
                PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
            }

            return 1;
        }


        int GetRadius(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetRadius());
            }
            else
            {
                script.Push(0.0f);
            }

            return 1;
        }

        int SetRadius(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetRadius(script.ToFloat(2));
            }

            return 0;
        }


        int GetFalloff(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetFalloff());
            }
            else
            {
                script.Push(0.0f);
            }

            return 1;
        }

        int SetFalloff(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetFalloff(script.ToFloat(2));
            }

            return 0;
        }



        int EnableShadowCasting(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->EnableShadowCasting();
            }

            return 0;
        }

        int DisableShadowCasting(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->DisableShadowCasting();
            }

            return 0;
        }

        int IsShadowCastingEnabled(GT::Script &script)
        {
            auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsShadowCastingEnabled());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.SpotLightComponent
    namespace SpotLightComponentFFI
    {
        int SetColour(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 colour;

                if (script.IsTable(2))
                {
                    colour = ToVector3(script, 2);
                }
                else
                {
                    colour.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    colour.y = script.IsNumber(3) ? script.ToFloat(3) : colour.x;
                    colour.z = script.IsNumber(4) ? script.ToFloat(4) : colour.x;
                }

                component->SetColour(colour);
            }

            return 0;
        }

        int GetColour(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                PushNewVector3(script, component->GetColour());
            }
            else
            {
                PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
            }

            return 1;
        }


        int GetLength(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetLength());
            }
            else
            {
                script.Push(0.0f);
            }

            return 1;
        }

        int SetLength(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetLength(script.ToFloat(2));
            }

            return 0;
        }


        int GetFalloff(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetFalloff());
            }
            else
            {
                script.Push(0.0f);
            }

            return 1;
        }

        int SetFalloff(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetFalloff(script.ToFloat(2));
            }

            return 0;
        }


        int EnableShadowCasting(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->EnableShadowCasting();
            }

            return 0;
        }

        int DisableShadowCasting(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->DisableShadowCasting();
            }

            return 0;
        }

        int IsShadowCastingEnabled(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsShadowCastingEnabled());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }


        int SetAngles(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float inner = script.ToFloat(2);
                float outer = script.ToFloat(3);
                component->SetAngles(inner, outer);
            }

            return 0;
        }

        int GetAngles(GT::Script &script)
        {
            auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetInnerAngle());
                script.Push(component->GetOuterAngle());
            }
            else
            {
                script.Push(40.0f);
                script.Push(45.0f);
            }

            return 2;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.DirectionalLightComponent
    namespace DirectionalLightComponentFFI
    {
        int SetColour(GT::Script &script)
        {
            auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 colour;

                if (script.IsTable(2))
                {
                    colour = ToVector3(script, 2);
                }
                else
                {
                    colour.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    colour.y = script.IsNumber(3) ? script.ToFloat(3) : colour.x;
                    colour.z = script.IsNumber(4) ? script.ToFloat(4) : colour.x;
                }

                component->SetColour(colour);
            }

            return 0;
        }

        int GetColour(GT::Script &script)
        {
            auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                PushNewVector3(script, component->GetColour());
            }
            else
            {
                PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
            }

            return 1;
        }

        int EnableShadowCasting(GT::Script &script)
        {
            auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->EnableShadowCasting();
            }

            return 0;
        }

        int DisableShadowCasting(GT::Script &script)
        {
            auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->DisableShadowCasting();
            }

            return 0;
        }

        int IsShadowCastingEnabled(GT::Script &script)
        {
            auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsShadowCastingEnabled());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.AmbientLightComponent
    namespace AmbientLightComponentFFI
    {
        int SetColour(GT::Script &script)
        {
            auto component = reinterpret_cast<AmbientLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 colour;

                if (script.IsTable(2))
                {
                    colour = ToVector3(script, 2);
                }
                else
                {
                    colour.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    colour.y = script.IsNumber(3) ? script.ToFloat(3) : colour.x;
                    colour.z = script.IsNumber(4) ? script.ToFloat(4) : colour.x;
                }

                component->SetColour(colour);
            }

            return 0;
        }

        int GetColour(GT::Script &script)
        {
            auto component = reinterpret_cast<AmbientLightComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                PushNewVector3(script, component->GetColour());
            }
            else
            {
                PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
            }

            return 1;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.CollisionShapeComponent
    namespace CollisionShapeComponentFFI
    {
        int SetCollisionFilter(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetCollisionFilter(static_cast<short>(script.ToInteger(2)), static_cast<short>(script.ToInteger(3)));
            }

            return 0;
        }

        int GetCollisionGroup(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(static_cast<int>(component->GetCollisionGroup()));
            }
            else
            {
                script.Push(1);
            }

            return 1;
        }

        int GetCollisionMask(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(static_cast<int>(component->GetCollisionMask()));
            }
            else
            {
                script.Push(static_cast<int>(-1));
            }

            return 1;
        }



        int AddCollisionGroup(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->AddCollisionGroup(script.ToString(2));
            }

            return 0;
        }

        int RemoveCollisionGroup(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->RemoveCollisionGroup(script.ToString(2));
            }

            return 0;
        }

        int RemoveCollisionGroupByIndex(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->RemoveCollisionGroupByIndex(static_cast<size_t>(script.ToInteger(2) - 1));
            }

            return 0;
        }

        int GetCollisionGroupCount(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(static_cast<int>(component->GetCollisionGroupCount()));
            }
            else
            {
                script.Push(0);
            }

            return 1;
        }

        int GetCollisionGroupName(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetCollisionGroup(static_cast<size_t>(script.ToInteger(2) - 1)));
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int AddCollisionGroupMask(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->AddCollisionGroupMask(script.ToString(2));
            }

            return 0;
        }

        int RemoveCollisionGroupMask(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->RemoveCollisionGroupMask(script.ToString(2));
            }

            return 0;
        }

        int RemoveCollisionGroupMaskByIndex(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->RemoveCollisionGroupMaskByIndex(static_cast<size_t>(script.ToInteger(2) - 1));
            }

            return 0;
        }

        int GetCollisionGroupMaskCount(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(static_cast<int>(component->GetCollisionGroupMaskCount()));
            }
            else
            {
                script.Push(0);
            }

            return 1;
        }

        int GetCollisionGroupMaskName(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetCollisionGroupMask(static_cast<size_t>(script.ToInteger(2) - 1)));
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int AddBoxShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 extents;
                glm::vec3 offset;

                if (script.IsTable(2))
                {
                    extents = ToVector3(script, 2);
                        
                    if (script.IsTable(4))
                    {
                        offset = ToVector3(script, 3);
                    }
                    else
                    {
                        offset.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                        offset.y = script.IsNumber(4) ? script.ToFloat(4) : offset.x;
                        offset.z = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                    }
                }
                else
                {
                    extents.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    extents.y = script.IsNumber(3) ? script.ToFloat(3) : extents.x;
                    extents.z = script.IsNumber(4) ? script.ToFloat(4) : extents.x;

                    if (script.IsTable(5))
                    {
                        offset = ToVector3(script, 5);
                    }
                    else
                    {
                        offset.x = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                        offset.y = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                        offset.z = script.IsNumber(7) ? script.ToFloat(7) : offset.x;
                    }
                }

                component->AddBoxCollisionShape(extents.x, extents.y, extents.z, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddSphereShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float     radius = script.ToFloat(2);
                glm::vec3 offset;

                if (script.IsTable(3))
                {
                    offset = ToVector3(script, 3);
                }
                else
                {
                    offset.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                    offset.y = script.IsNumber(4) ? script.ToFloat(4) : offset.x;
                    offset.z = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                }

                component->AddSphereCollisionShape(radius, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddEllipsoidShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 extents;
                glm::vec3 offset;

                if (script.IsTable(2))
                {
                    extents = ToVector3(script, 2);
                        
                    if (script.IsTable(4))
                    {
                        offset = ToVector3(script, 3);
                    }
                    else
                    {
                        offset.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                        offset.y = script.IsNumber(4) ? script.ToFloat(4) : offset.x;
                        offset.z = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                    }
                }
                else
                {
                    extents.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    extents.y = script.IsNumber(3) ? script.ToFloat(3) : extents.x;
                    extents.z = script.IsNumber(4) ? script.ToFloat(4) : extents.x;

                    if (script.IsTable(5))
                    {
                        offset = ToVector3(script, 5);
                    }
                    else
                    {
                        offset.x = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                        offset.y = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                        offset.z = script.IsNumber(7) ? script.ToFloat(7) : offset.x;
                    }
                }

                component->AddEllipsoidCollisionShape(extents.x, extents.y, extents.z, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddCylinderXShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float radius = script.ToFloat(2);
                float length = script.ToFloat(3);
                glm::vec3 offset;

                if (script.IsTable(4))
                {
                    offset = ToVector3(script, 4);
                }
                else
                {
                    offset.x = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                    offset.y = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                    offset.z = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                }

                component->AddCylinderXCollisionShape(radius, length, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddCylinderYShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float radius = script.ToFloat(2);
                float length = script.ToFloat(3);
                glm::vec3 offset;

                if (script.IsTable(4))
                {
                    offset = ToVector3(script, 4);
                }
                else
                {
                    offset.x = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                    offset.y = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                    offset.z = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                }

                component->AddCylinderYCollisionShape(radius, length, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddCylinderZShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float radius = script.ToFloat(2);
                float length = script.ToFloat(3);
                glm::vec3 offset;

                if (script.IsTable(4))
                {
                    offset = ToVector3(script, 4);
                }
                else
                {
                    offset.x = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                    offset.y = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                    offset.z = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                }

                component->AddCylinderZCollisionShape(radius, length, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddCapsuleXShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float radius = script.ToFloat(2);
                float length = script.ToFloat(3);
                glm::vec3 offset;

                if (script.IsTable(4))
                {
                    offset = ToVector3(script, 4);
                }
                else
                {
                    offset.x = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                    offset.y = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                    offset.z = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                }

                component->AddCapsuleXCollisionShape(radius, length, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddCapsuleYShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float radius = script.ToFloat(2);
                float length = script.ToFloat(3);
                glm::vec3 offset;

                if (script.IsTable(4))
                {
                    offset = ToVector3(script, 4);
                }
                else
                {
                    offset.x = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                    offset.y = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                    offset.z = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                }

                component->AddCapsuleYCollisionShape(radius, length, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddCapsuleZShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float radius = script.ToFloat(2);
                float length = script.ToFloat(3);
                glm::vec3 offset;

                if (script.IsTable(4))
                {
                    offset = ToVector3(script, 4);
                }
                else
                {
                    offset.x = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                    offset.y = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                    offset.z = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                }

                component->AddCapsuleZCollisionShape(radius, length, offset.x, offset.y, offset.z);
            }

            return 0;
        }

        int AddModelConvexHullsCollisionShape(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                float margin = script.ToFloat(2);
                glm::vec3 offset;

                if (script.IsTable(3))
                {
                    offset = ToVector3(script, 3);
                }
                else
                {
                    offset.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                    offset.y = script.IsNumber(4) ? script.ToFloat(4) : offset.x;
                    offset.z = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                }

                component->AddModelConvexHullsCollisionShape(margin, offset.x, offset.y, offset.z);
            }

            return 0;
        }


        int RemoveAllShapes(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->RemoveAllCollisionShapes();
            }

            return 0;
        }

        int RemoveShapeAtIndex(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->RemoveCollisionShapeAtIndex(static_cast<size_t>(script.ToInteger(2) - 1));    // Minus 1 because Lua is 1 based.
            }

            return 0;
        }

        int GetShapeCount(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(static_cast<int>(component->GetCollisionShapeCount()));
            }

            return 1;
        }

        int GetShapeAtIndex(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);   // Lua is 1 based, so we subtract 1.
                auto & shape      = component->GetCollisionShapeAtIndex(shapeIndex);

                script.PushNewTable();
                {
                    CollisionShapeType type = GetCollisionShapeType(shape);

                    // Start with the type.
                    script.SetTableValue(-1, "type", type);

                    // Now the shape offset.
                    auto offset = component->GetCollisionShapeOffset(shapeIndex);

                    script.SetTableValue(-1, "offsetX", offset.x);
                    script.SetTableValue(-1, "offsetY", offset.y);
                    script.SetTableValue(-1, "offsetZ", offset.z);

                    // We now need to do shape-specific properties.
                    switch (type)
                    {
                    case CollisionShapeType_Box:
                        {
                            auto &box = static_cast<const btBoxShape &>(shape);

                            btVector3 halfExtents = box.getHalfExtentsWithMargin() / box.getLocalScaling();

                            script.SetTableValue(-1, "halfX", halfExtents.getX());
                            script.SetTableValue(-1, "halfY", halfExtents.getY());
                            script.SetTableValue(-1, "halfZ", halfExtents.getZ());

                            break;
                        }

                    case CollisionShapeType_Sphere:
                        {
                            auto &sphere = static_cast<const btSphereShape &>(shape);

                            script.SetTableValue(-1, "radius", sphere.getRadius() / sphere.getLocalScaling().getX());

                            break;
                        }

                    case CollisionShapeType_Ellipsoid:
                        {
                            auto &ellipsoid = static_cast<const btEllipsoidShape &>(shape);

                            btVector3 margin(ellipsoid.getMargin(), ellipsoid.getMargin(), ellipsoid.getMargin());
                            btVector3 radius = (ellipsoid.getImplicitShapeDimensions() + margin) / ellipsoid.getLocalScaling();

                            script.SetTableValue(-1, "radiusX", radius.getX());
                            script.SetTableValue(-1, "radiusY", radius.getY());
                            script.SetTableValue(-1, "radiusZ", radius.getZ());

                            break;
                        }

                    case CollisionShapeType_CylinderX:
                    case CollisionShapeType_CylinderY:
                    case CollisionShapeType_CylinderZ:
                        {
                            auto &cylinder = static_cast<const btCylinderShape &>(shape);

                            btVector3 halfExtents = cylinder.getHalfExtentsWithMargin() / cylinder.getLocalScaling();

                            if (type == CollisionShapeType_CylinderX)
                            {
                                script.SetTableValue(-1, "length", halfExtents.getX() * 2.0f);
                                script.SetTableValue(-1, "radius", halfExtents.getY());
                            }
                            else if (type == CollisionShapeType_CylinderY)
                            {
                                script.SetTableValue(-1, "length", halfExtents.getY() * 2.0f);
                                script.SetTableValue(-1, "radius", halfExtents.getZ());
                            }
                            else if (type == CollisionShapeType_CylinderZ)
                            {
                                script.SetTableValue(-1, "length", halfExtents.getZ() * 2.0f);
                                script.SetTableValue(-1, "radius", halfExtents.getX());
                            }

                            break;
                        }

                    case CollisionShapeType_CapsuleX:
                    case CollisionShapeType_CapsuleY:
                    case CollisionShapeType_CapsuleZ:
                        {
                            auto &capsule = static_cast<const btCapsuleShape &>(shape);

                            uint32_t upAxis     = static_cast<uint32_t>(capsule.getUpAxis());
                            uint32_t radiusAxis = (upAxis + 2) % 3;

                            script.SetTableValue(-1, "radius", capsule.getRadius()            / capsule.getLocalScaling()[radiusAxis]);
                            script.SetTableValue(-1, "height", capsule.getHalfHeight() * 2.0f / capsule.getLocalScaling()[upAxis]);

                            break;
                        }

                    case CollisionShapeType_ConvexHull:
                        {
                            break;
                        }

                    case CollisionShapeType_ModelConvexHulls:
                        {
                            auto &compound = static_cast<const btCompoundShape &>(shape);

                            script.SetTableValue(-1, "margin", compound.getMargin());

                            break;
                        }

                    case CollisionShapeType_None:
                    default: break;
                    }
                }

                return 1;
            }

            return 0;
        }


        int SetShapeOffset(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t shapeIndex = static_cast<size_t>(script.ToInteger(2)) - 1;        // Minus 1 because Lua is 1 based.
                glm::vec3 offset;

                if (script.IsTable(3))
                {
                    offset = ToVector3(script, 3);
                }
                else
                {
                    offset.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                    offset.y = script.IsNumber(4) ? script.ToFloat(4) : offset.x;
                    offset.z = script.IsNumber(5) ? script.ToFloat(5) : offset.x;
                }

                component->SetCollisionShapeOffset(shapeIndex, offset.x, offset.y, offset.z);
            }

            return 0;
        }


        int SetBoxShapeHalfExtents(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                glm::vec3 extents;

                if (script.IsTable(3))
                {
                    extents = ToVector3(script, 3);
                }
                else
                {
                    extents.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                    extents.y = script.IsNumber(4) ? script.ToFloat(4) : extents.x;
                    extents.z = script.IsNumber(5) ? script.ToFloat(5) : extents.x;
                }

                script.Push(component->SetBoxCollisionShapeHalfExtents(shapeIndex, extents.x, extents.y, extents.z));
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetSphereShapeRadius(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                float  radius     = script.ToFloat(3);

                script.Push(component->SetSphereCollisionShapeRadius(shapeIndex, radius));
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetEllipsoidShapeRadius(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                glm::vec3 extents;

                if (script.IsTable(3))
                {
                    extents = ToVector3(script, 3);
                }
                else
                {
                    extents.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                    extents.y = script.IsNumber(4) ? script.ToFloat(4) : extents.x;
                    extents.z = script.IsNumber(5) ? script.ToFloat(5) : extents.x;
                }

                script.Push(component->SetEllipsoidCollisionShapeRadius(shapeIndex, extents.x, extents.y, extents.z));
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetCylinderShapeSize(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                float  radius     = script.ToFloat(3);
                float  length     = script.ToFloat(4);

                script.Push(component->SetCylinderCollisionShapeSize(shapeIndex, radius, length));
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetCapsuleShapeSize(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                float  radius     = script.ToFloat(3);
                float  height     = script.ToFloat(4);

                script.Push(component->SetCapsuleCollisionShapeSize(shapeIndex, radius, height));
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int SetModelConvexHullsMargins(GT::Script &script)
        {
            auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                float  margin     = script.ToFloat(3);

                script.Push(component->SetModelConvexHullsMargins(shapeIndex, margin));
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.DynamicsComponent
    namespace DynamicsComponentFFI
    {
        int SetMass(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetMass(script.ToFloat(2));
            }

            return 0;
        }

        int GetMass(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetMass());
            }
            else
            {
                script.Push(0.0f);
            }

            return 1;
        }

        int IsStatic(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsStatic());
            }
            else
            {
                script.Push(1);
            }

            return 1;
        }

        int IsKinematic(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                if (script.IsNil(2))
                {
                    script.Push(component->IsKinematic());
                    return 1;
                }
                else
                {
                    component->IsKinematic(script.ToBoolean(2));
                }
            }

            return 0;
        }


        int SetFriction(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetFriction(script.ToFloat(2));
            }

            return 0;
        }

        int GetFriction(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetFriction());
            }
            else
            {
                script.Push(0.5f);
            }

            return 1;
        }

        int SetRestitution(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetRestitution(script.ToFloat(2));
            }

            return 0;
        }

        int GetRestitution(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetRestitution());
            }
            else
            {
                script.Push(0.5f);
            }

            return 1;
        }

        int SetDamping(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetDamping(script.ToFloat(2), script.ToFloat(3));
            }

            return 0;
        }

        int GetLinearDamping(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetLinearDamping());
            }
            else
            {
                script.Push(0.0f);
            }

            return 1;
        }

        int GetAngularDamping(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetAngularDamping());
            }
            else
            {
                script.Push(0.0f);
            }

            return 1;
        }



        int SetLinearVelocity(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->SetLinearVelocity(value);
            }

            return 0;
        }

        int GetLinearVelocity(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                PushNewVector3(script, component->GetLinearVelocity());
            }
            else
            {
                PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
            }

            return 1;
        }

        int SetAngularVelocity(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->SetAngularVelocity(value);
            }

            return 0;
        }

        int GetAngularVelocity(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                PushNewVector3(script, component->GetAngularVelocity());
            }
            else
            {
                PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
            }

            return 1;
        }


        int SetLinearFactor(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->SetLinearFactor(value);
            }

            return 0;
        }

        int SetAngularFactor(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->SetAngularFactor(value);
            }

            return 0;
        }


        int SetGravity(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->SetGravity(value);
            }

            return 0;
        }

        int GetGravity(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                PushNewVector3(script, component->GetGravity());
            }
            else
            {
                PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
            }

            return 1;
        }

        int ApplyGravity(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->ApplyGravity();
            }

            return 0;
        }


        int ApplyCentralForce(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->ApplyCentralForce(value);
            }

            return 0;
        }

        int ApplyTorque(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->ApplyTorque(value);
            }

            return 0;
        }

        int ApplyImpulse(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 impulse = ToVector3(script, 2);
                glm::vec3 offset  = ToVector3(script, 3);

                component->ApplyImpulse(impulse, offset);
            }

            return 0;
        }

        int ApplyCentralImpulse(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->ApplyCentralImpulse(value);
            }

            return 0;
        }

        int ApplyTorqueImpulse(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                glm::vec3 value;

                if (script.IsTable(2))
                {
                    value = ToVector3(script, 2);
                }
                else
                {
                    value.x = script.IsNumber(2) ? script.ToFloat(2) : 0.0f;
                    value.y = script.IsNumber(3) ? script.ToFloat(3) : value.x;
                    value.z = script.IsNumber(4) ? script.ToFloat(4) : value.x;
                }

                component->ApplyTorqueImpulse(value);
            }

            return 0;
        }


        int DisableDeactivation(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->DisableDeactivation();
            }

            return 0;
        }

        int EnableDeactivation(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->EnableDeactivation();
            }

            return 0;
        }

        int IsDeactivationEnabled(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsDeactivationEnabled());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int Activate(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->Activate();
            }

            return 0;
        }


        int EnableNavigationMeshGeneration(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->EnableNavigationMeshGeneration();
            }

            return 0;
        }

        int DisableNavigationMeshGeneration(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->DisableNavigationMeshGeneration();
            }

            return 0;
        }

        int IsNavigationMeshGenerationEnabled(GT::Script &script)
        {
            auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsNavigationMeshGenerationEnabled());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }



        int SetCollisionFilter(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetCollisionFilter(script);
        }

        int GetCollisionGroup(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroup(script);
        }

        int GetCollisionMask(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionMask(script);
        }


        int AddCollisionGroup(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCollisionGroup(script);
        }

        int RemoveCollisionGroup(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveCollisionGroup(script);
        }

        int RemoveCollisionGroupByIndex(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveCollisionGroupByIndex(script);
        }

        int GetCollisionGroupCount(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroupCount(script);
        }

        int GetCollisionGroupName(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroupName(script);
        }


        int AddCollisionGroupMask(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCollisionGroupMask(script);
        }

        int RemoveCollisionGroupMask(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveCollisionGroupMask(script);
        }

        int RemoveCollisionGroupMaskByIndex(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveCollisionGroupMaskByIndex(script);
        }

        int GetCollisionGroupMaskCount(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroupMaskCount(script);
        }

        int GetCollisionGroupMaskName(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroupMaskName(script);
        }



        int AddBoxCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddBoxShape(script);
        }

        int AddSphereCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddSphereShape(script);
        }

        int AddEllipsoidCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddEllipsoidShape(script);
        }

        int AddCylinderXCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCylinderXShape(script);
        }

        int AddCylinderYCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCylinderYShape(script);
        }

        int AddCylinderZCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCylinderZShape(script);
        }

        int AddCapsuleXCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCapsuleXShape(script);
        }

        int AddCapsuleYCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCapsuleYShape(script);
        }

        int AddCapsuleZCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCapsuleZShape(script);
        }

        int AddModelConvexHullsCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddModelConvexHullsCollisionShape(script);
        }


        int RemoveAllCollisionShapes(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveAllShapes(script);
        }

        int RemoveCollisionShapeAtIndex(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveShapeAtIndex(script);
        }

        int GetCollisionShapeCount(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetShapeCount(script);
        }

        int GetCollisionShapeAtIndex(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetShapeAtIndex(script);
        }


        int SetCollisionShapeOffset(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetShapeOffset(script);
        }


        int SetBoxCollisionShapeHalfExtents(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetBoxShapeHalfExtents(script);
        }

        int SetSphereCollisionShapeRadius(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetSphereShapeRadius(script);
        }

        int SetEllipsoidCollisionShapeRadius(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetEllipsoidShapeRadius(script);
        }

        int SetCylinderCollisionShapeSize(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetCylinderShapeSize(script);
        }

        int SetCapsuleCollisionShapeSize(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetCapsuleShapeSize(script);
        }

        int SetModelConvexHullsMargins(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetModelConvexHullsMargins(script);
        }
    }

    //////////////////////////////////////////////////
    // GTEngine.System.ProximityComponent
    namespace ProximityComponentFFI
    {
        int SetCollisionFilter(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetCollisionFilter(script);
        }

        int GetCollisionGroup(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroup(script);
        }

        int GetCollisionMask(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionMask(script);
        }


        int AddCollisionGroup(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCollisionGroup(script);
        }

        int RemoveCollisionGroup(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveCollisionGroup(script);
        }

        int RemoveCollisionGroupByIndex(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveCollisionGroupByIndex(script);
        }

        int GetCollisionGroupCount(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroupCount(script);
        }

        int GetCollisionGroupName(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroupName(script);
        }


        int AddCollisionGroupMask(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCollisionGroupMask(script);
        }

        int RemoveCollisionGroupMask(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveCollisionGroupMask(script);
        }

        int RemoveCollisionGroupMaskByIndex(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveCollisionGroupMaskByIndex(script);
        }

        int GetCollisionGroupMaskCount(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroupMaskCount(script);
        }

        int GetCollisionGroupMaskName(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetCollisionGroupMaskName(script);
        }



        int AddBoxCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddBoxShape(script);
        }

        int AddSphereCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddSphereShape(script);
        }

        int AddEllipsoidCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddEllipsoidShape(script);
        }

        int AddCylinderXCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCylinderXShape(script);
        }

        int AddCylinderYCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCylinderYShape(script);
        }

        int AddCylinderZCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCylinderZShape(script);
        }

        int AddCapsuleXCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCapsuleXShape(script);
        }

        int AddCapsuleYCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCapsuleYShape(script);
        }

        int AddCapsuleZCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddCapsuleZShape(script);
        }

        int AddModelConvexHullsCollisionShape(GT::Script &script)
        {
            return CollisionShapeComponentFFI::AddModelConvexHullsCollisionShape(script);
        }


        int RemoveAllCollisionShapes(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveAllShapes(script);
        }

        int RemoveCollisionShapeAtIndex(GT::Script &script)
        {
            return CollisionShapeComponentFFI::RemoveShapeAtIndex(script);
        }

        int GetCollisionShapeCount(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetShapeCount(script);
        }

        int GetCollisionShapeAtIndex(GT::Script &script)
        {
            return CollisionShapeComponentFFI::GetShapeAtIndex(script);
        }


        int SetCollisionShapeOffset(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetShapeOffset(script);
        }


        int SetBoxCollisionShapeHalfExtents(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetBoxShapeHalfExtents(script);
        }

        int SetSphereCollisionShapeRadius(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetSphereShapeRadius(script);
        }

        int SetEllipsoidCollisionShapeRadius(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetEllipsoidShapeRadius(script);
        }

        int SetCylinderCollisionShapeSize(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetCylinderShapeSize(script);
        }

        int SetCapsuleCollisionShapeSize(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetCapsuleShapeSize(script);
        }

        int SetModelConvexHullsMargins(GT::Script &script)
        {
            return CollisionShapeComponentFFI::SetModelConvexHullsMargins(script);
        }



        int IsAnythingInside(GT::Script &script)
        {
            auto component = reinterpret_cast<ProximityComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetSceneNodesInsideVolume().count > 0);
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.ScriptComponent
    namespace ScriptComponentFFI
    {
        int AddScript(GT::Script &script)
        {
            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->AddScript(script.ToString(2)) != nullptr);
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int RemoveScriptByRelativePath(GT::Script &script)
        {
            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->RemoveScript(script.ToString(2));
            }

            return 0;
        }

        int RemoveScriptByIndex(GT::Script &script)
        {
            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->RemoveScriptByIndex(static_cast<size_t>(script.ToInteger(2) - 1));                   // <-- Subract 1 because Lua is 1 based.
            }

            return 0;
        }

        int ReloadScript(GT::Script &script)
        {
            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->ReloadScript(static_cast<size_t>(script.ToInteger(2) - 1));                          // <-- Subtract 1 because Lua is 1 based.
            }

            return 0;
        }


        int GetScriptRelativeFilePaths(GT::Script &script)
        {
            script.PushNewTable();

            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t fileCount = component->GetScriptCount();

                for (size_t i = 0; i < fileCount; ++i)
                {
                    auto relativePath = component->GetScriptRelativePathByIndex(i);
                    script.SetTableValue(-1, static_cast<int>(i + 1), (relativePath != nullptr) ? relativePath : "");      // <-- i + 1 because Lua is 1-based.
                }
            }

            return 1;
        }

        int GetScriptAbsoluteFilePaths(GT::Script &script)
        {
            script.PushNewTable();

            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t fileCount = component->GetScriptCount();

                for (size_t i = 0; i < fileCount; ++i)
                {
                    auto absolutePath = component->GetScriptAbsolutePathByIndex(i);
                    script.SetTableValue(-1, static_cast<int>(i + 1), (absolutePath != nullptr) ? absolutePath : "");      // <-- i + 1 because Lua is 1-based.
                }
            }

            return 1;
        }

        int IsUsingScript(GT::Script &script)
        {
            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetScriptDefinitionByRelativePath(script.ToString(2)) != nullptr);
            }

            return 1;
        }


        int GetPublicVariableNamesAndTypesByIndex(GT::Script &script)
        {
            script.PushNewTable();

            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto definition = component->GetScriptDefinitionByIndex(static_cast<size_t>(script.ToInteger(2) - 1));  // <-- minus 1 because Lua is 1-based.
                if (definition != nullptr)
                {
                    size_t variableCount = definition->GetPublicVariableCount();
                    for (size_t i = 0; i < variableCount; ++i)
                    {
                        auto variable = definition->GetPublicVariableByIndex(i);
                        assert(variable != nullptr);
                        {
                            script.Push(static_cast<int>(i + 1));       // <-- The index/key. +1 because Lua is 1 based.
                            script.PushNewTable();
                            script.SetTableValue(-1, "name", variable->GetName());
                            script.SetTableValue(-1, "type", static_cast<int>(variable->GetType()));

                            script.SetTableValue(-3);
                        }
                    }
                }
            }

            return 1;
        }

        int GetPublicVariableNamesAndTypesByRelativePath(GT::Script &script)
        {
            script.PushNewTable();

            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto definition = component->GetScriptDefinitionByRelativePath(script.ToString(2));
                if (definition != nullptr)
                {
                    size_t variableCount = definition->GetPublicVariableCount();
                    for (size_t i = 0; i < variableCount; ++i)
                    {
                        auto variable = definition->GetPublicVariableByIndex(i);
                        assert(variable != nullptr);
                        {
                            script.Push(static_cast<int>(i + 1));       // <-- The index/key. +1 because Lua is 1 based.
                            script.PushNewTable();
                            script.SetTableValue(-1, "name", variable->GetName());
                            script.SetTableValue(-1, "type", static_cast<int>(variable->GetType()));

                            script.SetTableValue(-3);
                        }
                    }
                }
            }

            return 1;
        }



        int GetPublicVariableNamesAndValues(GT::Script &script)
        {
            script.PushNewTable();

            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                size_t variableCount = component->GetPublicVariableCount();
                for (size_t i = 0; i < variableCount; ++i)
                {
                    auto variable = component->GetPublicVariableByIndex(i);
                    assert(variable != nullptr);
                    {
                        switch (variable->GetType())
                        {
                        case ScriptVariableType_Number:
                            {
                                script.SetTableValue(-1, variable->GetName(), static_cast<ScriptVariable_Number*>(variable)->GetValue());
                                break;
                            }

                        case ScriptVariableType_Vec2:
                            {
                                auto variableVec2 = static_cast<ScriptVariable_Vec2*>(variable);

                                script.Push(variable->GetName());
                                PushNewVector2(script, static_cast<float>(variableVec2->GetX()), static_cast<float>(variableVec2->GetY()));

                                script.SetTableValue(-3);

                                break;
                            }

                        case ScriptVariableType_Vec3:
                            {
                                auto variableVec3 = static_cast<ScriptVariable_Vec3*>(variable);

                                script.Push(variable->GetName());
                                PushNewVector3(script, static_cast<float>(variableVec3->GetX()), static_cast<float>(variableVec3->GetY()), static_cast<float>(variableVec3->GetZ()));

                                script.SetTableValue(-3);

                                break;
                            }

                        case ScriptVariableType_Vec4:
                            {
                                auto variableVec4 = static_cast<ScriptVariable_Vec4*>(variable);

                                script.Push(variable->GetName());
                                PushNewVector4(script, static_cast<float>(variableVec4->GetX()), static_cast<float>(variableVec4->GetY()), static_cast<float>(variableVec4->GetZ()), static_cast<float>(variableVec4->GetW()));

                                script.SetTableValue(-3);

                                break;
                            }

                        case ScriptVariableType_Boolean:
                            {
                                script.SetTableValue(-1, variable->GetName(), static_cast<ScriptVariable_Boolean*>(variable)->GetValue());
                                break;
                            }

                        case ScriptVariableType_String:
                            {
                                script.SetTableValue(-1, variable->GetName(), static_cast<ScriptVariable_String*>(variable)->GetValue());
                                break;
                            }

                        case ScriptVariableType_Prefab:
                            {
                                script.SetTableValue(-1, variable->GetName(), static_cast<ScriptVariable_Prefab*>(variable)->GetValue());
                                break;
                            }

                        case ScriptVariableType_Unknown:
                        default: break;
                        }
                    }
                }
            }

            return 1;
        }

        int GetPublicVariableValue(GT::Script &script)
        {
            auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto variable = component->GetPublicVariableByName(script.ToString(2));
                if (variable != nullptr)
                {
                    switch (variable->GetType())
                    {
                    case ScriptVariableType_Number:
                        {
                            script.Push(static_cast<ScriptVariable_Number*>(variable)->GetValue());
                            break;
                        }

                    case ScriptVariableType_Vec2:
                        {
                            auto variableVec2 = static_cast<ScriptVariable_Vec2*>(variable);
                                        
                            script.PushNewTable();
                            script.SetTableValue(-1, "x", variableVec2->GetX());
                            script.SetTableValue(-1, "y", variableVec2->GetY());

                            break;
                        }

                    case ScriptVariableType_Vec3:
                        {
                            auto variableVec3 = static_cast<ScriptVariable_Vec3*>(variable);
                                        
                            script.PushNewTable();
                            script.SetTableValue(-1, "x", variableVec3->GetX());
                            script.SetTableValue(-1, "y", variableVec3->GetY());
                            script.SetTableValue(-1, "z", variableVec3->GetZ());

                            break;
                        }

                    case ScriptVariableType_Vec4:
                        {
                            auto variableVec4 = static_cast<ScriptVariable_Vec4*>(variable);
                                        
                            script.PushNewTable();
                            script.SetTableValue(-1, "x", variableVec4->GetX());
                            script.SetTableValue(-1, "y", variableVec4->GetY());
                            script.SetTableValue(-1, "z", variableVec4->GetZ());
                            script.SetTableValue(-1, "w", variableVec4->GetW());

                            break;
                        }

                    case ScriptVariableType_Boolean:
                        {
                            script.Push(static_cast<ScriptVariable_Boolean*>(variable)->GetValue());
                            break;
                        }

                    case ScriptVariableType_String:
                        {
                            script.Push(static_cast<ScriptVariable_String*>(variable)->GetValue());
                            break;
                        }

                    case ScriptVariableType_Prefab:
                        {
                            script.Push(static_cast<ScriptVariable_Prefab*>(variable)->GetValue());
                            break;
                        }


                    case ScriptVariableType_Unknown:
                    default:
                        {
                            script.PushNil();
                            break;
                        }
                    }
                }
                else
                {
                    script.PushNil();
                }
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int SetPublicVariableValue(GT::Script &script)
        {
            auto component = static_cast<ScriptComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                if (script.IsNumber(3))
                {
                    component->SetPublicVariableValue(script.ToString(2), script.ToDouble(3));
                }
                else if (script.IsTable(3))
                {
                    // Value could be a vector type. We'll need to look at the contents of the table to determine the type.
                    Map<int, double> values;

                    int i = 0;
                    script.PushNil();
                    while(i < 4 && script.Next(-2))
                    {
                        if (script.IsNumber(-1))
                        {
                            int mapIndex = -1;
                            if (script.IsNumber(-2))
                            {
                                auto key = script.ToInteger(-2);
                                if (key >= 0 && key <= 3)
                                {
                                    mapIndex = key;
                                }
                            }
                            else if (script.IsString(-2))
                            {
                                auto key = script.ToString(-2);
                                if (Strings::Equal<false>(key, "x"))
                                {
                                    mapIndex = 0;
                                }
                                else if (Strings::Equal<false>(key, "y"))
                                {
                                    mapIndex = 1;
                                }
                                else if (Strings::Equal<false>(key, "z"))
                                {
                                    mapIndex = 2;
                                }
                                else if (Strings::Equal<false>(key, "w"))
                                {
                                    mapIndex = 3;
                                }
                            }


                            if (mapIndex >= 0 && mapIndex <= 3)
                            {
                                values.Add(mapIndex, script.ToDouble(-1));
                            }
                        }

                        ++i;
                        script.Pop(1);
                    }

                    if (values.count == 2)
                    {
                        component->SetPublicVariableValue(script.ToString(2), values.buffer[0]->value, values.buffer[1]->value);
                    }
                    else if (values.count == 3)
                    {
                        component->SetPublicVariableValue(script.ToString(2), values.buffer[0]->value, values.buffer[1]->value, values.buffer[2]->value);
                    }
                    else if (values.count >= 4)
                    {
                        component->SetPublicVariableValue(script.ToString(2), values.buffer[0]->value, values.buffer[1]->value, values.buffer[2]->value, values.buffer[3]->value);
                    }
                }
                else if (script.IsBoolean(3))
                {
                    component->SetPublicVariableValue(script.ToString(2), script.ToBoolean(3));
                }
                else if (script.IsString(3))
                {
                    component->SetPublicVariableValue(script.ToString(2), script.ToString(3));
                }
            }

            return 0;
        }
    }



    //////////////////////////////////////////////////
    // GTEngine.System.ParticleSystemComponent

    namespace ParticleSystemComponentFFI
    {
        int SetFromFile(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->SetParticleSystem(script.ToString(2));
            }

            return 0;
        }

        int GetRelativeFilePath(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                auto particleSystem = component->GetParticleSystem();
                if (particleSystem != nullptr)
                {
                    script.Push(particleSystem->GetDefinition().GetRelativePath());
                }
                else
                {
                    script.PushNil();
                }
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int Play(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->Play();
            }

            return 0;
        }

        int Pause(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->Pause();
            }

            return 0;
        }

        int IsPlaying(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsPlaying());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }


        int PlayOnStartup(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->PlayOnStartup(script.ToBoolean(2));
            }

            return 0;
        }

        int IsPlayingOnStartup(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsPlayingOnStartup());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }


        int Reset(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->Reset();
            }

            return 0;
        }


        int GetEmitterLifetime(GT::Script &script)
        {
            auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                double lifetimeMin;
                double lifetimeMax;
                component->GetEmitterLifetime(script.ToInteger(2), lifetimeMin, lifetimeMax);

                script.Push(lifetimeMin);
                script.Push(lifetimeMax);
            }
            else
            {
                script.Push(0.0);
                script.Push(0.0);
            }

            return 2;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.PrefabComponent
        
    namespace PrefabComponentFFI
    {
        int GetPrefabRelativePath(GT::Script &script)
        {
            auto component = static_cast<PrefabComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetPrefabRelativePath());
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }

        int GetLocalHierarchyID(GT::Script &script)
        {
            auto component = static_cast<PrefabComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(static_cast<int>(component->GetLocalHierarchyID()));
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }
    }


    //////////////////////////////////////////////////
    // GTEngine.System.EditorMetadataComponent

    namespace EditorMetadataComponentFFI
    {
        int Select(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->Select();
            }

            return 0;
        }

        int Deselect(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->Deselect();
            }

            return 0;
        }

        int IsSelected(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsSelected());
            }

            return 1;
        }


        int ShowSprite(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                if (script.IsString(2))
                {
                    glm::vec3 colour(1.0f, 1.0f, 1.0f);
                    if (script.IsTable(3))
                    {
                        script.Push("r");
                        script.GetTableValue(3);
                        {
                            colour.x = script.ToFloat(-1);
                        }
                        script.Pop(1);

                        script.Push("g");
                        script.GetTableValue(3);
                        {
                            colour.y = script.ToFloat(-1);
                        }
                        script.Pop(1);

                        script.Push("b");
                        script.GetTableValue(3);
                        {
                            colour.z = script.ToFloat(-1);
                        }
                        script.Pop(1);
                    }

                    component->ShowSprite(script.ToString(2), colour);
                }
            }

            return 0;
        }

        int HideSprite(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->HideSprite();
            }

            return 0;
        }

        int IsShowingSprite(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsUsingSprite());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }

        int GetSpriteTexturePath(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->GetSpriteTexturePath());
            }
            else
            {
                script.PushNil();
            }

            return 1;
        }


        int ShowDirectionArrow(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->ShowDirectionArrow();
            }

            return 0;
        }

        int HideDirectionArrow(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                component->HideDirectionArrow();
            }

            return 0;
        }

        int IsShowingDirectionArrow(GT::Script &script)
        {
            auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
            if (component != nullptr)
            {
                script.Push(component->IsShowingDirectionArrow());
            }
            else
            {
                script.Push(false);
            }

            return 1;
        }
    }
}
