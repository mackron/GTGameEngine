// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Components.hpp>
#include <GTEngine/Scripting/Scripting_Animation.hpp>
#include <GTEngine/Scripting/Scripting_Math.hpp>
#include <GTEngine/SceneNode.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadComponentsLibrary(GTCore::Script &script)
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


                "function GTEngine.DynamicsComponent:AddConvexHullCollisionShapesFromModelComponent(margin)"
                "    GTEngine.System.DynamicsComponent.AddConvexHullCollisionShapesFromModelComponent(self._internalPtr, margin);"
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

                "function GTEngine.DynamicsComponent:SetCylinderCollisionShapeHalfExtents(index, halfX, halfY, halfZ)"
                "    return GTEngine.System.DynamicsComponent.SetCylinderCollisionShapeHalfExtents(self._internalPtr, index, halfX, halfY, halfZ);"
                "end;"

                "function GTEngine.DynamicsComponent:SetCapsuleCollisionShapeSize(index, radius, height)"
                "    return GTEngine.System.DynamicsComponent.SetCapsuleCollisionShapeSize(self._internalPtr, index, radius, height);"
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


                "function GTEngine.ProximityComponent:AddConvexHullCollisionShapesFromModelComponent(margin)"
                "    GTEngine.System.ProximityComponent.AddConvexHullCollisionShapesFromModelComponent(self._internalPtr, margin);"
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

                "function GTEngine.ProximityComponent:SetCylinderCollisionShapeHalfExtents(index, halfX, halfY, halfZ)"
                "    return GTEngine.System.ProximityComponent.SetCylinderCollisionShapeHalfExtents(self._internalPtr, index, halfX, halfY, halfZ);"
                "end;"

                "function GTEngine.ProximityComponent:SetCapsuleCollisionShapeSize(index, radius, height)"
                "    return GTEngine.System.ProximityComponent.SetCapsuleCollisionShapeSize(self._internalPtr, index, radius, height);"
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

                "function GTEngine.ScriptComponent:GetScriptFilePaths()"
                "    return GTEngine.System.ScriptComponent.GetScriptFilePaths(self._internalPtr);"
                "end;"

                "function GTEngine.ScriptComponent:IsUsingScript(relativePath)"
                "    return GTEngine.System.ScriptComponent.IsUsingScript(self._internalPtr, relativePath);"
                "end;"

                "function GTEngine.ScriptComponent:GetPublicVariableNamesAndTypesByIndex(index)"
                "    return GTEngine.System.ScriptComponent.GetPublicVariableNamesAndTypesByIndex(self._internalPtr, index);"
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
                            script.SetTableFunction(-1, "PlayAnimationSegmentByName", ModelComponentFFI::PlayAnimationSegmentByName);
                            script.SetTableFunction(-1, "PlayAnimationSequence",      ModelComponentFFI::PlayAnimationSequence);
                            script.SetTableFunction(-1, "PauseAnimation",             ModelComponentFFI::PauseAnimation);
                            script.SetTableFunction(-1, "StopAnimation",              ModelComponentFFI::StopAnimation);
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

                            script.SetTableFunction(-1, "AddBoxCollisionShape",                           DynamicsComponentFFI::AddBoxCollisionShape);
                            script.SetTableFunction(-1, "AddSphereCollisionShape",                        DynamicsComponentFFI::AddSphereCollisionShape);
                            script.SetTableFunction(-1, "AddEllipsoidCollisionShape",                     DynamicsComponentFFI::AddEllipsoidCollisionShape);
                            script.SetTableFunction(-1, "AddCylinderXCollisionShape",                     DynamicsComponentFFI::AddCylinderXCollisionShape);
                            script.SetTableFunction(-1, "AddCylinderYCollisionShape",                     DynamicsComponentFFI::AddCylinderYCollisionShape);
                            script.SetTableFunction(-1, "AddCylinderZCollisionShape",                     DynamicsComponentFFI::AddCylinderZCollisionShape);
                            script.SetTableFunction(-1, "AddCapsuleXCollisionShape",                      DynamicsComponentFFI::AddCapsuleXCollisionShape);
                            script.SetTableFunction(-1, "AddCapsuleYCollisionShape",                      DynamicsComponentFFI::AddCapsuleYCollisionShape);
                            script.SetTableFunction(-1, "AddCapsuleZCollisionShape",                      DynamicsComponentFFI::AddCapsuleZCollisionShape);
                            script.SetTableFunction(-1, "SetCollisionShapesToModelConvexHulls",           DynamicsComponentFFI::SetCollisionShapesToModelConvexHulls);
                            script.SetTableFunction(-1, "RemoveAllCollisionShapes",                       DynamicsComponentFFI::RemoveAllCollisionShapes);
                            script.SetTableFunction(-1, "RemoveCollisionShapeAtIndex",                    DynamicsComponentFFI::RemoveCollisionShapeAtIndex);
                            script.SetTableFunction(-1, "GetCollisionShapeCount",                         DynamicsComponentFFI::GetCollisionShapeCount);
                            script.SetTableFunction(-1, "GetCollisionShapeAtIndex",                       DynamicsComponentFFI::GetCollisionShapeAtIndex);
                            script.SetTableFunction(-1, "IsUsingConvexHullsFromModel",                    DynamicsComponentFFI::IsUsingConvexHullsFromModel);
                            script.SetTableFunction(-1, "SetCollisionShapeOffset",                        DynamicsComponentFFI::SetCollisionShapeOffset);
                            script.SetTableFunction(-1, "SetBoxCollisionShapeHalfExtents",                DynamicsComponentFFI::SetBoxCollisionShapeHalfExtents);
                            script.SetTableFunction(-1, "SetSphereCollisionShapeRadius",                  DynamicsComponentFFI::SetSphereCollisionShapeRadius);
                            script.SetTableFunction(-1, "SetEllipsoidCollisionShapeRadius",               DynamicsComponentFFI::SetEllipsoidCollisionShapeRadius);
                            script.SetTableFunction(-1, "SetCylinderCollisionShapeHalfExtents",           DynamicsComponentFFI::SetCylinderCollisionShapeHalfExtents);
                            script.SetTableFunction(-1, "SetCapsuleCollisionShapeSize",                   DynamicsComponentFFI::SetCapsuleCollisionShapeSize);
                        }
                        script.Pop(1);


                        script.Push("ProximityComponent");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.SetTableFunction(-1, "SetCollisionFilter",                             ProximityComponentFFI::SetCollisionFilter);
                            script.SetTableFunction(-1, "GetCollisionGroup",                              ProximityComponentFFI::GetCollisionGroup);
                            script.SetTableFunction(-1, "GetCollisionMask",                               ProximityComponentFFI::GetCollisionMask);

                            script.SetTableFunction(-1, "AddBoxCollisionShape",                           ProximityComponentFFI::AddBoxCollisionShape);
                            script.SetTableFunction(-1, "AddSphereCollisionShape",                        ProximityComponentFFI::AddSphereCollisionShape);
                            script.SetTableFunction(-1, "AddEllipsoidCollisionShape",                     ProximityComponentFFI::AddEllipsoidCollisionShape);
                            script.SetTableFunction(-1, "AddCylinderXCollisionShape",                     ProximityComponentFFI::AddCylinderXCollisionShape);
                            script.SetTableFunction(-1, "AddCylinderYCollisionShape",                     ProximityComponentFFI::AddCylinderYCollisionShape);
                            script.SetTableFunction(-1, "AddCylinderZCollisionShape",                     ProximityComponentFFI::AddCylinderZCollisionShape);
                            script.SetTableFunction(-1, "AddCapsuleXCollisionShape",                      ProximityComponentFFI::AddCapsuleXCollisionShape);
                            script.SetTableFunction(-1, "AddCapsuleYCollisionShape",                      ProximityComponentFFI::AddCapsuleYCollisionShape);
                            script.SetTableFunction(-1, "AddCapsuleZCollisionShape",                      ProximityComponentFFI::AddCapsuleZCollisionShape);
                            script.SetTableFunction(-1, "SetCollisionShapesToModelConvexHulls",           ProximityComponentFFI::SetCollisionShapesToModelConvexHulls);
                            script.SetTableFunction(-1, "RemoveAllCollisionShapes",                       ProximityComponentFFI::RemoveAllCollisionShapes);
                            script.SetTableFunction(-1, "RemoveCollisionShapeAtIndex",                    ProximityComponentFFI::RemoveCollisionShapeAtIndex);
                            script.SetTableFunction(-1, "GetCollisionShapeCount",                         ProximityComponentFFI::GetCollisionShapeCount);
                            script.SetTableFunction(-1, "GetCollisionShapeAtIndex",                       ProximityComponentFFI::GetCollisionShapeAtIndex);
                            script.SetTableFunction(-1, "IsUsingConvexHullsFromModel",                    ProximityComponentFFI::IsUsingConvexHullsFromModel);
                            script.SetTableFunction(-1, "SetCollisionShapeOffset",                        ProximityComponentFFI::SetCollisionShapeOffset);
                            script.SetTableFunction(-1, "SetBoxCollisionShapeHalfExtents",                ProximityComponentFFI::SetBoxCollisionShapeHalfExtents);
                            script.SetTableFunction(-1, "SetSphereCollisionShapeRadius",                  ProximityComponentFFI::SetSphereCollisionShapeRadius);
                            script.SetTableFunction(-1, "SetEllipsoidCollisionShapeRadius",               ProximityComponentFFI::SetEllipsoidCollisionShapeRadius);
                            script.SetTableFunction(-1, "SetCylinderCollisionShapeHalfExtents",           ProximityComponentFFI::SetCylinderCollisionShapeHalfExtents);
                            script.SetTableFunction(-1, "SetCapsuleCollisionShapeSize",                   ProximityComponentFFI::SetCapsuleCollisionShapeSize);
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
                        }
                        script.Pop(1);


                        script.Push("ScriptComponent");
                        script.GetTableValue(-2);
                        assert(script.IsTable(-1));
                        {
                            script.SetTableFunction(-1, "AddScript",                                ScriptComponentFFI::AddScript);
                            script.SetTableFunction(-1, "RemoveScriptByRelativePath",               ScriptComponentFFI::RemoveScriptByRelativePath);
                            script.SetTableFunction(-1, "RemoveScriptByIndex",                      ScriptComponentFFI::RemoveScriptByIndex);
                            script.SetTableFunction(-1, "ReloadScript",                             ScriptComponentFFI::ReloadScript);
                            script.SetTableFunction(-1, "GetScriptFilePaths",                       ScriptComponentFFI::GetScriptFilePaths);
                            script.SetTableFunction(-1, "IsUsingScript",                            ScriptComponentFFI::IsUsingScript);
                            script.SetTableFunction(-1, "GetPublicVariableNamesAndTypesByIndex",    ScriptComponentFFI::GetPublicVariableNamesAndTypesByIndex);
                            script.SetTableFunction(-1, "GetPublicVariableNamesAndValues",          ScriptComponentFFI::GetPublicVariableNamesAndValues);
                            script.SetTableFunction(-1, "GetPublicVariableValue",                   ScriptComponentFFI::GetPublicVariableValue);
                            script.SetTableFunction(-1, "SetPublicVariableValue",                   ScriptComponentFFI::SetPublicVariableValue);
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
            int Set3DProjection(GTCore::Script &script)
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

            int GetFOV(GTCore::Script &script)
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

            int GetAspectRatio(GTCore::Script &script)
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

            int GetNearClippingPlane(GTCore::Script &script)
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

            int GetFarClippingPlane(GTCore::Script &script)
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
            int SetModel(GTCore::Script &script)
            {
                auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
                auto filePath  = script.ToString(2);

                if (component != nullptr && filePath != nullptr)
                {
                    component->SetModel(filePath);
                }

                return 0;
            }

            int GetModelPath(GTCore::Script &script)
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

            int EnableShadowCasting(GTCore::Script &script)
            {
                auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->EnableShadowCasting();
                }

                return 0;
            }

            int DisableShadowCasting(GTCore::Script &script)
            {
                auto component = reinterpret_cast<ModelComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->DisableShadowCasting();
                }

                return 0;
            }

            int IsShadowCastingEnabled(GTCore::Script &script)
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


            int SetMaterial(GTCore::Script &script)
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

            int GetMaterialPath(GTCore::Script &script)
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

            int GetMaterialCount(GTCore::Script &script)
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


            int PlayAnimationSegmentByName(GTCore::Script &script)
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

            int PlayAnimationSequence(GTCore::Script &script)
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

            int PauseAnimation(GTCore::Script &script)
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

            int StopAnimation(GTCore::Script &script)
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
        }


        //////////////////////////////////////////////////
        // GTEngine.System.PointLightComponent
        namespace PointLightComponentFFI
        {
            int SetColour(GTCore::Script &script)
            {
                auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 colour;

                    if (script.IsTable(2))
                    {
                        colour = Scripting::ToVector3(script, 2);
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

            int GetColour(GTCore::Script &script)
            {
                auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    Scripting::PushNewVector3(script, component->GetColour());
                }
                else
                {
                    Scripting::PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
                }

                return 1;
            }


            int GetRadius(GTCore::Script &script)
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

            int SetRadius(GTCore::Script &script)
            {
                auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetRadius(script.ToFloat(2));
                }

                return 0;
            }


            int GetFalloff(GTCore::Script &script)
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

            int SetFalloff(GTCore::Script &script)
            {
                auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetFalloff(script.ToFloat(2));
                }

                return 0;
            }



            int EnableShadowCasting(GTCore::Script &script)
            {
                auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->EnableShadowCasting();
                }

                return 0;
            }

            int DisableShadowCasting(GTCore::Script &script)
            {
                auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->DisableShadowCasting();
                }

                return 0;
            }

            int IsShadowCastingEnabled(GTCore::Script &script)
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
            int SetColour(GTCore::Script &script)
            {
                auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 colour;

                    if (script.IsTable(2))
                    {
                        colour = Scripting::ToVector3(script, 2);
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

            int GetColour(GTCore::Script &script)
            {
                auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    Scripting::PushNewVector3(script, component->GetColour());
                }
                else
                {
                    Scripting::PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
                }

                return 1;
            }


            int GetLength(GTCore::Script &script)
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

            int SetLength(GTCore::Script &script)
            {
                auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetLength(script.ToFloat(2));
                }

                return 0;
            }


            int GetFalloff(GTCore::Script &script)
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

            int SetFalloff(GTCore::Script &script)
            {
                auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetFalloff(script.ToFloat(2));
                }

                return 0;
            }


            int EnableShadowCasting(GTCore::Script &script)
            {
                auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->EnableShadowCasting();
                }

                return 0;
            }

            int DisableShadowCasting(GTCore::Script &script)
            {
                auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->DisableShadowCasting();
                }

                return 0;
            }

            int IsShadowCastingEnabled(GTCore::Script &script)
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


            int SetAngles(GTCore::Script &script)
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

            int GetAngles(GTCore::Script &script)
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
            int SetColour(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 colour;

                    if (script.IsTable(2))
                    {
                        colour = Scripting::ToVector3(script, 2);
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

            int GetColour(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    Scripting::PushNewVector3(script, component->GetColour());
                }
                else
                {
                    Scripting::PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
                }

                return 3;
            }

            int EnableShadowCasting(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->EnableShadowCasting();
                }

                return 0;
            }

            int DisableShadowCasting(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->DisableShadowCasting();
                }

                return 0;
            }

            int IsShadowCastingEnabled(GTCore::Script &script)
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
            int SetColour(GTCore::Script &script)
            {
                auto component = reinterpret_cast<AmbientLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 colour;

                    if (script.IsTable(2))
                    {
                        colour = Scripting::ToVector3(script, 2);
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

            int GetColour(GTCore::Script &script)
            {
                auto component = reinterpret_cast<AmbientLightComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    Scripting::PushNewVector3(script, component->GetColour());
                }
                else
                {
                    Scripting::PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
                }

                return 1;
            }
        }


        //////////////////////////////////////////////////
        // GTEngine.System.CollisionShapeComponent
        namespace CollisionShapeComponentFFI
        {
            int SetCollisionFilter(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetCollisionFilter(static_cast<short>(script.ToInteger(2)), static_cast<short>(script.ToInteger(3)));
                }

                return 0;
            }

            int GetCollisionGroup(GTCore::Script &script)
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

            int GetCollisionMask(GTCore::Script &script)
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


            int AddBoxShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 extents;
                    glm::vec3 offset;

                    if (script.IsTable(2))
                    {
                        extents = Scripting::ToVector3(script, 2);
                        
                        if (script.IsTable(4))
                        {
                            offset = Scripting::ToVector3(script, 3);
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
                            offset = Scripting::ToVector3(script, 5);
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

            int AddSphereShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    float     radius = script.ToFloat(2);
                    glm::vec3 offset;

                    if (script.IsTable(3))
                    {
                        offset = Scripting::ToVector3(script, 3);
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

            int AddEllipsoidShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 extents;
                    glm::vec3 offset;

                    if (script.IsTable(2))
                    {
                        extents = Scripting::ToVector3(script, 2);
                        
                        if (script.IsTable(4))
                        {
                            offset = Scripting::ToVector3(script, 3);
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
                            offset = Scripting::ToVector3(script, 5);
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

            int AddCylinderXShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 extents;
                    glm::vec3 offset;

                    if (script.IsTable(2))
                    {
                        extents = Scripting::ToVector3(script, 2);
                        
                        if (script.IsTable(4))
                        {
                            offset = Scripting::ToVector3(script, 3);
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
                            offset = Scripting::ToVector3(script, 5);
                        }
                        else
                        {
                            offset.x = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            offset.y = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                            offset.z = script.IsNumber(7) ? script.ToFloat(7) : offset.x;
                        }
                    }

                    component->AddCylinderXCollisionShape(extents.x, extents.y, extents.z, offset.x, offset.y, offset.z);
                }

                return 0;
            }

            int AddCylinderYShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 extents;
                    glm::vec3 offset;

                    if (script.IsTable(2))
                    {
                        extents = Scripting::ToVector3(script, 2);
                        
                        if (script.IsTable(4))
                        {
                            offset = Scripting::ToVector3(script, 3);
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
                            offset = Scripting::ToVector3(script, 5);
                        }
                        else
                        {
                            offset.x = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            offset.y = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                            offset.z = script.IsNumber(7) ? script.ToFloat(7) : offset.x;
                        }
                    }

                    component->AddCylinderYCollisionShape(extents.x, extents.y, extents.z, offset.x, offset.y, offset.z);
                }

                return 0;
            }

            int AddCylinderZShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 extents;
                    glm::vec3 offset;

                    if (script.IsTable(2))
                    {
                        extents = Scripting::ToVector3(script, 2);
                        
                        if (script.IsTable(4))
                        {
                            offset = Scripting::ToVector3(script, 3);
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
                            offset = Scripting::ToVector3(script, 5);
                        }
                        else
                        {
                            offset.x = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            offset.y = script.IsNumber(6) ? script.ToFloat(6) : offset.x;
                            offset.z = script.IsNumber(7) ? script.ToFloat(7) : offset.x;
                        }
                    }

                    component->AddCylinderZCollisionShape(extents.x, extents.y, extents.z, offset.x, offset.y, offset.z);
                }

                return 0;
            }

            int AddCapsuleXShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    float radius = script.ToFloat(2);
                    float length = script.ToFloat(3);
                    glm::vec3 offset;

                    if (script.IsTable(4))
                    {
                        offset = Scripting::ToVector3(script, 4);
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

            int AddCapsuleYShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    float radius = script.ToFloat(2);
                    float length = script.ToFloat(3);
                    glm::vec3 offset;

                    if (script.IsTable(4))
                    {
                        offset = Scripting::ToVector3(script, 4);
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

            int AddCapsuleZShape(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    float radius = script.ToFloat(2);
                    float length = script.ToFloat(3);
                    glm::vec3 offset;

                    if (script.IsTable(4))
                    {
                        offset = Scripting::ToVector3(script, 4);
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

            int SetCollisionShapesToModelConvexHulls(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetCollisionShapesToModelConvexHulls(script.ToFloat(2));
                }

                return 0;
            }

            int RemoveAllShapes(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->RemoveAllCollisionShapes();
                }

                return 0;
            }

            int RemoveShapeAtIndex(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->RemoveCollisionShapeAtIndex(static_cast<size_t>(script.ToInteger(2) - 1));    // Minus 1 because Lua is 1 based.
                }

                return 0;
            }

            int GetShapeCount(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    script.Push(static_cast<int>(component->GetCollisionShapeCount()));
                }

                return 1;
            }

            int GetShapeAtIndex(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);   // Lua is 1 based, so we subtract 1.
                    auto & shape      = component->GetCollisionShapeAtIndex(shapeIndex);

                    script.PushNewTable();
                    {
                        if (component->IsUsingConvexHullsFromModel())
                        {
                            script.SetTableValue(-1, "type", CollisionShapeType_ModelConvexHulls);
                        }
                        else
                        {
                            CollisionShapeType type = GetCollisionShapeType(shape);

                            // Start with the type.
                            script.SetTableValue(-1, "type", type);

                            // Now the shape offset.
                            auto &offset = component->GetCollisionShape().getChildTransform(shapeIndex).getOrigin();

                            script.SetTableValue(-1, "offsetX", offset.getX());
                            script.SetTableValue(-1, "offsetY", offset.getY());
                            script.SetTableValue(-1, "offsetZ", offset.getZ());

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

                                    script.SetTableValue(-1, "halfX", halfExtents.getX());
                                    script.SetTableValue(-1, "halfY", halfExtents.getY());
                                    script.SetTableValue(-1, "halfZ", halfExtents.getZ());

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
                                    break;
                                }

                            case CollisionShapeType_None:
                            default: break;
                            }
                        }
                    }

                    return 1;
                }

                return 0;
            }

            int IsUsingConvexHullsFromModel(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    script.Push(static_cast<int>(component->IsUsingConvexHullsFromModel()));
                }

                return 1;
            }

            int SetShapeOffset(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    size_t shapeIndex = static_cast<size_t>(script.ToInteger(2)) - 1;        // Minus 1 because Lua is 1 based.
                    glm::vec3 offset;

                    if (script.IsTable(3))
                    {
                        offset = Scripting::ToVector3(script, 3);
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


            int SetBoxShapeHalfExtents(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                    glm::vec3 extents;

                    if (script.IsTable(3))
                    {
                        extents = Scripting::ToVector3(script, 3);
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

            int SetSphereShapeRadius(GTCore::Script &script)
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

            int SetEllipsoidShapeRadius(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                    glm::vec3 extents;

                    if (script.IsTable(3))
                    {
                        extents = Scripting::ToVector3(script, 3);
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

            int SetCylinderShapeHalfExtents(GTCore::Script &script)
            {
                auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                    glm::vec3 extents;

                    if (script.IsTable(3))
                    {
                        extents = Scripting::ToVector3(script, 3);
                    }
                    else
                    {
                        extents.x = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                        extents.y = script.IsNumber(4) ? script.ToFloat(4) : extents.x;
                        extents.z = script.IsNumber(5) ? script.ToFloat(5) : extents.x;
                    }

                    script.Push(component->SetCylinderCollisionShapeHalfExtents(shapeIndex, extents.x, extents.y, extents.z));
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }

            int SetCapsuleShapeSize(GTCore::Script &script)
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
        }


        //////////////////////////////////////////////////
        // GTEngine.System.DynamicsComponent
        namespace DynamicsComponentFFI
        {
            int SetMass(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetMass(script.ToFloat(2));
                }

                return 0;
            }

            int GetMass(GTCore::Script &script)
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

            int IsStatic(GTCore::Script &script)
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

            int IsKinematic(GTCore::Script &script)
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


            int SetFriction(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetFriction(script.ToFloat(2));
                }

                return 0;
            }

            int GetFriction(GTCore::Script &script)
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

            int SetRestitution(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetRestitution(script.ToFloat(2));
                }

                return 0;
            }

            int GetRestitution(GTCore::Script &script)
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

            int SetDamping(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetDamping(script.ToFloat(2), script.ToFloat(3));
                }

                return 0;
            }

            int GetLinearDamping(GTCore::Script &script)
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

            int GetAngularDamping(GTCore::Script &script)
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



            int SetLinearVelocity(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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

            int GetLinearVelocity(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    Scripting::PushNewVector3(script, component->GetLinearVelocity());
                }
                else
                {
                    Scripting::PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
                }

                return 1;
            }

            int SetAngularVelocity(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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

            int GetAngularVelocity(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    Scripting::PushNewVector3(script, component->GetAngularVelocity());
                }
                else
                {
                    Scripting::PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
                }

                return 1;
            }


            int SetLinearFactor(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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

            int SetAngularFactor(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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


            int SetGravity(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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

            int GetGravity(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    Scripting::PushNewVector3(script, component->GetGravity());
                }
                else
                {
                    Scripting::PushNewVector3(script, glm::vec3(0.0f, 0.0f, 0.0f));
                }

                return 1;
            }

            int ApplyGravity(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->ApplyGravity();
                }

                return 0;
            }


            int ApplyCentralForce(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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

            int ApplyTorque(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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

            int ApplyImpulse(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 impulse = Scripting::ToVector3(script, 2);
                    glm::vec3 offset  = Scripting::ToVector3(script, 3);

                    component->ApplyImpulse(impulse, offset);
                }

                return 0;
            }

            int ApplyCentralImpulse(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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

            int ApplyTorqueImpulse(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    glm::vec3 value;

                    if (script.IsTable(2))
                    {
                        value = Scripting::ToVector3(script, 2);
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


            int DisableDeactivation(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->DisableDeactivation();
                }

                return 0;
            }

            int EnableDeactivation(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->EnableDeactivation();
                }

                return 0;
            }

            int IsDeactivationEnabled(GTCore::Script &script)
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

            int Activate(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->Activate();
                }

                return 0;
            }


            int EnableNavigationMeshGeneration(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->EnableNavigationMeshGeneration();
                }

                return 0;
            }

            int DisableNavigationMeshGeneration(GTCore::Script &script)
            {
                auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->DisableNavigationMeshGeneration();
                }

                return 0;
            }

            int IsNavigationMeshGenerationEnabled(GTCore::Script &script)
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



            int SetCollisionFilter(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetCollisionFilter(script);
            }

            int GetCollisionGroup(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::GetCollisionGroup(script);
            }

            int GetCollisionMask(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::GetCollisionMask(script);
            }


            int AddBoxCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddBoxShape(script);
            }

            int AddSphereCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddSphereShape(script);
            }

            int AddEllipsoidCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddEllipsoidShape(script);
            }

            int AddCylinderXCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCylinderXShape(script);
            }

            int AddCylinderYCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCylinderYShape(script);
            }

            int AddCylinderZCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCylinderZShape(script);
            }

            int AddCapsuleXCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCapsuleXShape(script);
            }

            int AddCapsuleYCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCapsuleYShape(script);
            }

            int AddCapsuleZCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCapsuleZShape(script);
            }

            int SetCollisionShapesToModelConvexHulls(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetCollisionShapesToModelConvexHulls(script);
            }

            int RemoveAllCollisionShapes(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::RemoveAllShapes(script);
            }

            int RemoveCollisionShapeAtIndex(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::RemoveShapeAtIndex(script);
            }

            int GetCollisionShapeCount(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::GetShapeCount(script);
            }

            int GetCollisionShapeAtIndex(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::GetShapeAtIndex(script);
            }

            int IsUsingConvexHullsFromModel(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::IsUsingConvexHullsFromModel(script);
            }

            int SetCollisionShapeOffset(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetShapeOffset(script);
            }


            int SetBoxCollisionShapeHalfExtents(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetBoxShapeHalfExtents(script);
            }

            int SetSphereCollisionShapeRadius(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetSphereShapeRadius(script);
            }

            int SetEllipsoidCollisionShapeRadius(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetEllipsoidShapeRadius(script);
            }

            int SetCylinderCollisionShapeHalfExtents(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetCylinderShapeHalfExtents(script);
            }

            int SetCapsuleCollisionShapeSize(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetCapsuleShapeSize(script);
            }
        }

        //////////////////////////////////////////////////
        // GTEngine.System.ProximityComponent
        namespace ProximityComponentFFI
        {
            int SetCollisionFilter(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetCollisionFilter(script);
            }

            int GetCollisionGroup(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::GetCollisionGroup(script);
            }

            int GetCollisionMask(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::GetCollisionMask(script);
            }


            int AddBoxCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddBoxShape(script);
            }

            int AddSphereCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddSphereShape(script);
            }

            int AddEllipsoidCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddEllipsoidShape(script);
            }

            int AddCylinderXCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCylinderXShape(script);
            }

            int AddCylinderYCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCylinderYShape(script);
            }

            int AddCylinderZCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCylinderZShape(script);
            }

            int AddCapsuleXCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCapsuleXShape(script);
            }

            int AddCapsuleYCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCapsuleYShape(script);
            }

            int AddCapsuleZCollisionShape(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::AddCapsuleZShape(script);
            }

            int SetCollisionShapesToModelConvexHulls(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetCollisionShapesToModelConvexHulls(script);
            }

            int RemoveAllCollisionShapes(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::RemoveAllShapes(script);
            }

            int RemoveCollisionShapeAtIndex(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::RemoveShapeAtIndex(script);
            }

            int GetCollisionShapeCount(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::GetShapeCount(script);
            }

            int GetCollisionShapeAtIndex(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::GetShapeAtIndex(script);
            }

            int IsUsingConvexHullsFromModel(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::IsUsingConvexHullsFromModel(script);
            }

            int SetCollisionShapeOffset(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetShapeOffset(script);
            }


            int SetBoxCollisionShapeHalfExtents(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetBoxShapeHalfExtents(script);
            }

            int SetSphereCollisionShapeRadius(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetSphereShapeRadius(script);
            }

            int SetEllipsoidCollisionShapeRadius(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetEllipsoidShapeRadius(script);
            }

            int SetCylinderCollisionShapeHalfExtents(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetCylinderShapeHalfExtents(script);
            }

            int SetCapsuleCollisionShapeSize(GTCore::Script &script)
            {
                return CollisionShapeComponentFFI::SetCapsuleShapeSize(script);
            }
        }


        //////////////////////////////////////////////////
        // GTEngine.System.ScriptComponent
        namespace ScriptComponentFFI
        {
            int AddScript(GTCore::Script &script)
            {
                auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    script.Push(component->AddScript(script.ToString(2)) != nullptr);
                }

                return 1;
            }

            int RemoveScriptByRelativePath(GTCore::Script &script)
            {
                auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->RemoveScript(script.ToString(2));
                }

                return 0;
            }

            int RemoveScriptByIndex(GTCore::Script &script)
            {
                auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->RemoveScriptByIndex(static_cast<size_t>(script.ToInteger(2) - 1));                   // <-- Subract 1 because Lua is 1 based.
                }

                return 0;
            }

            int ReloadScript(GTCore::Script &script)
            {
                auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->ReloadScript(static_cast<size_t>(script.ToInteger(2) - 1));                          // <-- Subtract 1 because Lua is 1 based.
                }

                return 0;
            }


            int GetScriptFilePaths(GTCore::Script &script)
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

            int IsUsingScript(GTCore::Script &script)
            {
                auto component = reinterpret_cast<ScriptComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    script.Push(component->GetScriptDefinitionByRelativePath(script.ToString(2)) != nullptr);
                }

                return 1;
            }

            int GetPublicVariableNamesAndTypesByIndex(GTCore::Script &script)
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


            int GetPublicVariableNamesAndValues(GTCore::Script &script)
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
                                    script.PushNewTable();
                                    script.SetTableValue(-1, "x", variableVec2->GetX());
                                    script.SetTableValue(-1, "y", variableVec2->GetY());

                                    script.SetTableValue(-3);

                                    break;
                                }

                            case ScriptVariableType_Vec3:
                                {
                                    auto variableVec3 = static_cast<ScriptVariable_Vec3*>(variable);

                                    script.Push(variable->GetName());
                                    script.PushNewTable();
                                    script.SetTableValue(-1, "x", variableVec3->GetX());
                                    script.SetTableValue(-1, "y", variableVec3->GetY());
                                    script.SetTableValue(-1, "z", variableVec3->GetZ());

                                    script.SetTableValue(-3);

                                    break;
                                }

                            case ScriptVariableType_Vec4:
                                {
                                    auto variableVec4 = static_cast<ScriptVariable_Vec4*>(variable);

                                    script.Push(variable->GetName());
                                    script.PushNewTable();
                                    script.SetTableValue(-1, "x", variableVec4->GetX());
                                    script.SetTableValue(-1, "y", variableVec4->GetY());
                                    script.SetTableValue(-1, "z", variableVec4->GetZ());
                                    script.SetTableValue(-1, "w", variableVec4->GetW());

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

            int GetPublicVariableValue(GTCore::Script &script)
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


            int SetPublicVariableValue(GTCore::Script &script)
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
                        GTCore::Map<int, double> values;

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
                                    if (GTCore::Strings::Equal<false>(key, "x"))
                                    {
                                        mapIndex = 0;
                                    }
                                    else if (GTCore::Strings::Equal<false>(key, "y"))
                                    {
                                        mapIndex = 1;
                                    }
                                    else if (GTCore::Strings::Equal<false>(key, "z"))
                                    {
                                        mapIndex = 2;
                                    }
                                    else if (GTCore::Strings::Equal<false>(key, "w"))
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
            int SetFromFile(GTCore::Script &script)
            {
                auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->SetParticleSystem(script.ToString(2));
                }

                return 0;
            }

            int GetRelativeFilePath(GTCore::Script &script)
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


            int Play(GTCore::Script &script)
            {
                auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->Play();
                }

                return 0;
            }

            int Pause(GTCore::Script &script)
            {
                auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->Pause();
                }

                return 0;
            }

            int IsPlaying(GTCore::Script &script)
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


            int PlayOnStartup(GTCore::Script &script)
            {
                auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->PlayOnStartup(script.ToBoolean(2));
                }

                return 0;
            }

            int IsPlayingOnStartup(GTCore::Script &script)
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


            int Reset(GTCore::Script &script)
            {
                auto component = static_cast<ParticleSystemComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->Reset();
                }

                return 0;
            }
        }


        //////////////////////////////////////////////////
        // GTEngine.System.PrefabComponent
        
        namespace PrefabComponentFFI
        {
            int GetPrefabRelativePath(GTCore::Script &script)
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

            int GetLocalHierarchyID(GTCore::Script &script)
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
            int Select(GTCore::Script &script)
            {
                auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->Select();
                }

                return 0;
            }

            int Deselect(GTCore::Script &script)
            {
                auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->Deselect();
                }

                return 0;
            }

            int IsSelected(GTCore::Script &script)
            {
                auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    script.Push(component->IsSelected());
                }

                return 1;
            }


            int ShowSprite(GTCore::Script &script)
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

            int HideSprite(GTCore::Script &script)
            {
                auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->HideSprite();
                }

                return 0;
            }

            int IsShowingSprite(GTCore::Script &script)
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

            int GetSpriteTexturePath(GTCore::Script &script)
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


            int ShowDirectionArrow(GTCore::Script &script)
            {
                auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->ShowDirectionArrow();
                }

                return 0;
            }

            int HideDirectionArrow(GTCore::Script &script)
            {
                auto component = reinterpret_cast<EditorMetadataComponent*>(script.ToPointer(1));
                if (component != nullptr)
                {
                    component->HideDirectionArrow();
                }

                return 0;
            }

            int IsShowingDirectionArrow(GTCore::Script &script)
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
}