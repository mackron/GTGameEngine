// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/Scene.hpp>
#include <GTEngine/SceneNodeClassLibrary.hpp>
#include <GTEngine/ScriptLibrary.hpp>
#include <GTEngine/Physics/CollisionShapeTypes.hpp>

#include <GTCore/Path.hpp>


namespace GTEngine
{
    namespace Scripting
    {
        bool LoadGTEngineScriptLibrary(GTCore::Script &script)
        {
            bool successful = LoadExtendedMathLibrary(script);


            // GTEngine table.
            script.PushNewTable();
            {
                ///////////////////////////////////////////////////
                // GTEngine.System
                script.Push("System");
                script.PushNewTable();
                {
                    script.Push("SceneNode");
                    script.PushNewTable();
                    {
                    }
                    script.SetTableValue(-3);

                    script.Push("Scene");
                    script.PushNewTable();
                    {
                    }
                    script.SetTableValue(-3);


                    script.Push("SubEditor");
                    script.PushNewTable();
                    {
                    }
                    script.SetTableValue(-3);

                    script.Push("ModelEditor");
                    script.PushNewTable();
                    {
                    }
                    script.SetTableValue(-3);

                    script.Push("SceneEditor");
                    script.PushNewTable();
                    {
                    }
                    script.SetTableValue(-3);
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.Display
                
                script.Push("Display");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, "Width",      1280);
                    script.SetTableValue(-1, "Height",     720);
                    script.SetTableValue(-1, "Fullscreen", false);


                    script.Push("Textures");
                    script.PushNewTable();
                    {
                        script.SetTableValue(-1, "Anisotropy", 16);
                    }
                    script.SetTableValue(-3);
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.Renderer
                
                script.Push("Renderer");
                script.PushNewTable();
                {
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.Audio

                script.Push("Audio");
                script.PushNewTable();
                {
                }
                script.SetTableValue(-3);



                ///////////////////////////////////////////////////
                // GTEngine.<Misc>

                script.Push("RegisteredScenes");
                script.PushNewTable();
                script.SetTableValue(-3);

                script.Push("ScriptDefinitions");
                script.PushNewTable();
                script.SetTableValue(-3);


                script.Push("SceneNodeEventHandlers");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, 1, "OnUpdate");
                    script.SetTableValue(-1, 2, "OnStartup");
                    script.SetTableValue(-1, 3, "OnShutdown");
                    script.SetTableValue(-1, 4, "OnShow");
                    script.SetTableValue(-1, 5, "OnHide");
                    script.SetTableValue(-1, 6, "OnObjectEnter");
                    script.SetTableValue(-1, 7, "OnEnterObject");
                    script.SetTableValue(-1, 8, "OnObjectLeave");
                    script.SetTableValue(-1, 9, "OnLeaveObject");
                }
                script.SetTableValue(-3);
            }
            script.SetGlobal("GTEngine");


            successful = successful && script.Execute
            (
                "function GTEngine.IsSceneNodeEventHandler(name)"
                "    return table.indexof(GTEngine.SceneNodeEventHandlers, name) ~= nil;"
                "end;"
            );


            // Components
            //
            // Component script objects are actually very temporary. They are only used to give an interface to allow the internal representation to be modified. When a component is retrieved
            // from a scene node, it will actually create a whole new instantiation of the object.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("Components");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, "Model",             ModelComponent::Name);
                    script.SetTableValue(-1, "Camera",            CameraComponent::Name);
                    script.SetTableValue(-1, "PointLight",        PointLightComponent::Name);
                    script.SetTableValue(-1, "SpotLight",         SpotLightComponent::Name);
                    script.SetTableValue(-1, "DirectionalLight",  DirectionalLightComponent::Name);
                    script.SetTableValue(-1, "AmbientLight",      AmbientLightComponent::Name);
                    script.SetTableValue(-1, "Dynamics",          DynamicsComponent::Name);
                    script.SetTableValue(-1, "Proximity",         ProximityComponent::Name);
                    script.SetTableValue(-1, "Script",            ScriptComponent::Name);
                    script.SetTableValue(-1, "EditorMetadata",    EditorMetadataComponent::Name);
                }
                script.SetTableValue(-3);

                script.Push("CollisionShapeTypes");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, "None",             CollisionShapeType_None);
                    script.SetTableValue(-1, "Box",              CollisionShapeType_Box);
                    script.SetTableValue(-1, "Sphere",           CollisionShapeType_Sphere);
                    script.SetTableValue(-1, "Ellipsoid",        CollisionShapeType_Ellipsoid);
                    script.SetTableValue(-1, "CylinderX",        CollisionShapeType_CylinderX);
                    script.SetTableValue(-1, "CylinderY",        CollisionShapeType_CylinderY);
                    script.SetTableValue(-1, "CylinderZ",        CollisionShapeType_CylinderZ);
                    script.SetTableValue(-1, "CapsuleX",         CollisionShapeType_CapsuleX);
                    script.SetTableValue(-1, "CapsuleY",         CollisionShapeType_CapsuleY);
                    script.SetTableValue(-1, "CapsuleZ",         CollisionShapeType_CapsuleZ);
                    script.SetTableValue(-1, "ConvexHull",       CollisionShapeType_ConvexHull);
                    script.SetTableValue(-1, "ModelConvexHulls", CollisionShapeType_ModelConvexHulls);       // A special type representing the case when the convex hulls from the model is being used.
                }
                script.SetTableValue(-3);

                script.Push("ScriptVariableTypes");
                script.PushNewTable();
                {
                    script.SetTableValue(-1, "Unknown", ScriptVariableType_Unknown);
                    script.SetTableValue(-1, "None",    ScriptVariableType_None);
                    script.SetTableValue(-1, "Number",  ScriptVariableType_Number);
                    script.SetTableValue(-1, "Vec2",    ScriptVariableType_Vec2);
                    script.SetTableValue(-1, "Vec3",    ScriptVariableType_Vec3);
                    script.SetTableValue(-1, "Vec4",    ScriptVariableType_Vec4);
                    script.SetTableValue(-1, "Boolean", ScriptVariableType_Boolean);
                    script.SetTableValue(-1, "String",  ScriptVariableType_String);
                    script.SetTableValue(-1, "Prefab",  ScriptVariableType_Prefab);
                }
                script.SetTableValue(-3);
            }
            script.Pop(1);


            // Create the GTEngine.System.<Component> tables.
            successful = successful && script.Execute
            (
                "for key,value in pairs(GTEngine.Components) do"
                "    GTEngine.System[value .. 'Component'] = {};"
                "end;"
            );


            successful = successful && script.Execute
            (
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

                "function GTEngine.PointLightComponent:EnableShadowCasting()"
                "    GTEngine.System.PointLightComponent.EnableShadowCasting(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:DisableShadowCasting()"
                "    GTEngine.System.PointLightComponent.DisableShadowCasting(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:IsShadowCastingEnabled()"
                "    return GTEngine.System.PointLightComponent.IsShadowCastingEnabled(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:GetConstantAttenuation()"
                "    return GTEngine.System.PointLightComponent.GetConstantAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:GetLinearAttenuation()"
                "    return GTEngine.System.PointLightComponent.GetLinearAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:GetQuadraticAttenuation()"
                "    return GTEngine.System.PointLightComponent.GetQuadraticAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.PointLightComponent:SetAttenuation(constant, linear, quadratic)"
                "    GTEngine.System.PointLightComponent.SetAttenuation(self._internalPtr, constant, linear, quadratic);"
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

                "function GTEngine.SpotLightComponent:EnableShadowCasting()"
                "    GTEngine.System.SpotLightComponent.EnableShadowCasting(self._internalPtr);"
                "end;"

                "function GTEngine.SpotLightComponent:DisableShadowCasting()"
                "    GTEngine.System.SpotLightComponent.DisableShadowCasting(self._internalPtr);"
                "end;"

                "function GTEngine.SpotLightComponent:IsShadowCastingEnabled()"
                "    return GTEngine.System.SpotLightComponent.IsShadowCastingEnabled(self._internalPtr);"
                "end;"

                "function GTEngine.SpotLightComponent:GetConstantAttenuation()"
                "    return GTEngine.System.SpotLightComponent.GetConstantAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.SpotLightComponent:GetLinearAttenuation()"
                "    return GTEngine.System.SpotLightComponent.GetLinearAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.SpotLightComponent:GetQuadraticAttenuation()"
                "    return GTEngine.System.SpotLightComponent.GetQuadraticAttenuation(self._internalPtr);"
                "end;"

                "function GTEngine.SpotLightComponent:SetAttenuation(constant, linear, quadratic)"
                "    GTEngine.System.SpotLightComponent.SetAttenuation(self._internalPtr, constant, linear, quadratic);"
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
                "    GTEngine.System.DynamicsComponent.SetRestitution(self._internalPtr, linearDamping, angularDamping);"
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
                "    GTEngine.System.ScriptComponent.AddScript(self._internalPtr, relativePath);"
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


            // SceneNode
            successful = successful && script.Execute
            (
                "GTEngine.SceneNode = {};"
                "GTEngine.SceneNode.__index = GTEngine.SceneNode;"

                "function GTEngine.SceneNode:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.SceneNode);"
                "        if internalPtr == nil then"
                "            new._internalPtr       = GTEngine.System.SceneNode.Create();"
                "            new._deleteInternalPtr = true;"
                "        else"
                "            new._internalPtr       = internalPtr;"
                "            new._deleteInternalPtr = false;"
                "        end"
                ""
                "        new._eventHandlers   = {};"
                "        new._scriptVariables = {};"
                ""
                "        new:RegisterScriptComponent();"
                "    return new;"
                "end;"

                "function GTEngine.SceneNode:Delete()"
                "    if self._deleteInternalPtr then"
                "        GTEngine.System.SceneNode.Delete(self._internalPtr);"
                "    end;"
                "end;"


                "function GTEngine.SceneNode:RegisterScriptComponent()"
                "    local prevScriptVariables = self._scriptVariables;"
                "    self._eventHandlers   = {};"
                "    self._scriptVariables = {};"
                ""
                "    local scriptComponent = self:GetComponent(GTEngine.Components.Script);"
                "    if scriptComponent ~= nil then"
                "        local scriptFilePaths = scriptComponent:GetScriptFilePaths();"
                "        for i,scriptPath in ipairs(scriptFilePaths) do"
                "            self:LinkToScript(GTEngine.ScriptDefinitions[scriptPath]);"
                "            self:UpdatePublicVariables();"
                "        end;"
                "    end;"
                ""
                "    self:RemoveUnusedScriptVariables(prevScriptVariables);"
                "end;"

                "function GTEngine.SceneNode:LinkToScript(script)"
                "    if script ~= nil then"
                "        for key,value in pairs(script) do"
                "            if not GTEngine.IsSceneNodeEventHandler(key) then"
                "                self[key] = value;"
                "                self._scriptVariables[#self._scriptVariables + 1] = key;"
                "            else"
                "                self:RegisterEventHandler(key, value);"
                "            end;"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.SceneNode:UpdatePublicVariables(scriptComponent)"
                "    if scriptComponent == nil then scriptComponent = self:GetComponent(GTEngine.Components.Script) end;"
                "    if scriptComponent ~= nil then"
                "        local variables = scriptComponent:GetPublicVariableNamesAndValues();"
                "        for name,value in pairs(variables) do"
                "            self[name] = value;"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.SceneNode:RegisterEventHandler(name, value)"
                "    if name ~= nil and value ~= nil then"
                "        if self._eventHandlers[name] == nil then"
                "            self._eventHandlers[name] = {};"
                ""
                "            self[name] = function(self, ...)"
                "                for i,eventHandler in pairs(self._eventHandlers[name]) do"
                "                    eventHandler(self, ...);"
                "                end;"
                "            end;"
                "        end;"
                ""
                "        self._eventHandlers[name][#self._eventHandlers[name] + 1] = value;" 
                "    end;"
                "end;"

                "function GTEngine.SceneNode:RemoveUnusedScriptVariables(prevScriptVariables)"
                "    for i,variableName in ipairs(prevScriptVariables) do"
                "        if table.indexof(self._scriptVariables, variableName) == nil then"
                "            self[variableName] = nil;"
                "        end;"
                "    end;"
                "end;"



                "function GTEngine.SceneNode:GetID()"
                "    return GTEngine.System.SceneNode.GetID(self._internalPtr);"
                "end;"


                "function GTEngine.SceneNode:GetName()"
                "    return GTEngine.System.SceneNode.GetName(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetName(name)"
                "    GTEngine.System.SceneNode.SetName(self._internalPtr, name);"
                "end;"


                "function GTEngine.SceneNode:AttachChild(childNode)"
                "    GTEngine.System.SceneNode.AttachChild(self._internalPtr, childNode._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:Orphan()"
                "    GTEngine.System.SceneNode.Orphan(self._internalPtr);"
                "end;"


                "function GTEngine.SceneNode:AddComponent(componentID)"
                "    return GTEngine.System.SceneNode.AddComponent(self._internalPtr, componentID);"
                "end;"

                "function GTEngine.SceneNode:RemoveComponent(componentID)"
                "    return GTEngine.System.SceneNode.RemoveComponent(self._internalPtr, componentID);"
                "end;"

                "function GTEngine.SceneNode:GetComponent(componentID)"
                "    return GTEngine.System.SceneNode.GetComponent(self._internalPtr, componentID);"
                "end;"


                "function GTEngine.SceneNode:GetPosition()"
                "    return GTEngine.System.SceneNode.GetPosition(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:GetWorldPosition()"
                "    return GTEngine.System.SceneNode.GetWorldPosition(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetPosition(x, y, z)"
                "    return GTEngine.System.SceneNode.SetPosition(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:SetWorldPosition(x, y, z)"
                "    return GTEngine.System.SceneNode.SetWorldPosition(self._internalPtr, x, y, z);"
                "end;"


                "function GTEngine.SceneNode:GetRotationXYZ()"
                "    return GTEngine.System.SceneNode.GetRotationXYZ(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:GetWorldRotationXYZ()"
                "    return GTEngine.System.SceneNode.GetWorldRotationXYZ(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetRotationXYZ(x, y, z)"
                "    return GTEngine.System.SceneNode.SetRotationXYZ(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:SetWorldRotationXYZ(x, y, z)"
                "    return GTEngine.System.SceneNode.SetWorldRotationXYZ(self._internalPtr, x, y, z);"
                "end;"


                "function GTEngine.SceneNode:GetScale()"
                "    return GTEngine.System.SceneNode.GetScale(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:GetWorldScale()"
                "    return GTEngine.System.SceneNode.GetWorldScale(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetScale(x, y, z)"
                "    return GTEngine.System.SceneNode.SetScale(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:SetWorldScale(x, y, z)"
                "    return GTEngine.System.SceneNode.SetWorldScale(self._internalPtr, x, y, z);"
                "end;"



                "function GTEngine.SceneNode:GetAttachedComponentIDs()"
                "    return GTEngine.System.SceneNode.GetAttachedComponentIDs(self._internalPtr);"
                "end;"


                "function GTEngine.SceneNode:Show()"
                "    GTEngine.System.SceneNode.Show(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:Hide()"
                "    GTEngine.System.SceneNode.Hide(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:IsVisible()"
                "    return GTEngine.System.SceneNode.IsVisible(self._internalPtr);"
                "end;"


                "function GTEngine.SceneNode:GetScenePtr()"
                "    return GTEngine.System.SceneNode.GetScenePtr(self._internalPtr);"
                "end;"
            );


            // Scene
            successful = successful && script.Execute
            (
                "GTEngine.Scene = {};"
                "GTEngine.Scene.__index = GTEngine.Scene;"

                "function GTEngine.Scene:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.Scene);"
                "        new._internalPtr = internalPtr;"
                "        new._sceneNodes  = {};"
                ""
                "        new:InstantiateSceneNodesWithScriptComponents();"
                "    return new;"
                "end;"


                "function GTEngine.Scene:AddSceneNode(sceneNode)"
                "    GTEngine.System.Scene.AddSceneNode(self._internalPtr, sceneNode._internalPtr);"
                "end;"

                "function GTEngine.Scene:RemoveSceneNode(sceneNode)"
                "    GTEngine.System.Scene.RemoveSceneNode(self._internalPtr, sceneNode._internalPtr);"
                "end;"

                "function GTEngine.Scene:GetSceneNodePtrs()"
                "    return GTEngine.System.Scene.GetSceneNodePtrs(self._internalPtr);"
                "end;"


                "function GTEngine.Scene:InstantiateSceneNodesWithScriptComponents()"
                "    local sceneNodes = self:GetSceneNodePtrs();"
                "    for sceneNodeID,sceneNodePtr in pairs(sceneNodes) do"
                "        local scriptComponent = GTEngine.System.SceneNode.GetComponent(sceneNodePtr, GTEngine.Components.Script);"
                "        if scriptComponent ~= nil then"
                "            self:InstantiateSceneNode(sceneNodeID, sceneNodePtr);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.Scene:InstantiateSceneNode(sceneNodeID, sceneNodePtr)"
                "    local sceneNode = self._sceneNodes[sceneNodeID];"
                "    if sceneNode == nil then"
                "        if sceneNodePtr == nil then sceneNodePtr = self:GetSceneNodePtrByID(sceneNodeID) end;"
                "        if sceneNodePtr ~= nil then"
                "            sceneNode = GTEngine.SceneNode:Create(sceneNodePtr);"
                "            self._sceneNodes[sceneNodeID] = sceneNode;"
                "        end;"
                "    end;"
                ""
                "    return sceneNode;"
                "end;"

                "function GTEngine.Scene:UninstantiateSceneNode(sceneNodeID)"
                "    self._sceneNodes[sceneNodeID] = nil;"
                "end;"

                "function GTEngine.Scene:GetSceneNodeByID(sceneNodeID)"
                "    return self:InstantiateSceneNode(sceneNodeID);"
                "end;"

                "function GTEngine.Scene:GetSceneNodePtrByID(sceneNodeID)"
                "    return GTEngine.System.Scene.GetSceneNodePtrByID(self._internalPtr, sceneNodeID);"
                "end;"


                "function GTEngine.Scene:CreateNewSceneNode()"
                "    local sceneNodePtr = GTEngine.System.Scene.CreateNewSceneNode(self._internalPtr);"
                "    if sceneNodePtr ~= nil then"
                "        return GTEngine.SceneNode:Create(sceneNodePtr);"
                "    end;"
                ""
                "    return nil;"
                "end;"



                "GTEngine.RegisteredScenes = {};"
            );



            // Here we load the FFI.
            script.GetGlobal("GTEngine");
            if (script.IsTable(-1))
            {
                script.SetTableFunction(-1, "GetExecutableDirectory", FFI::GetExecutableDirectory);

                script.SetTableFunction(-1, "IsModelFile",  FFI::IsModelFile);
                script.SetTableFunction(-1, "IsImageFile",  FFI::IsImageFile);
                script.SetTableFunction(-1, "IsSoundFile",  FFI::IsSoundFile);
                script.SetTableFunction(-1, "IsSceneFile",  FFI::IsSceneFile);
                script.SetTableFunction(-1, "IsPrefabFile", FFI::IsPrefabFile);

                script.SetTableFunction(-1, "CreatePrefab", FFI::CreatePrefab);


                script.Push("System");
                script.GetTableValue(-2);
                if (script.IsTable(-1))
                {
                    script.Push("SceneNode");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "Create",                  FFI::SystemFFI::SceneNodeFFI::Create);
                        script.SetTableFunction(-1, "Delete",                  FFI::SystemFFI::SceneNodeFFI::Delete);

                        script.SetTableFunction(-1, "GetID",                   FFI::SystemFFI::SceneNodeFFI::GetID);
                        script.SetTableFunction(-1, "GetName",                 FFI::SystemFFI::SceneNodeFFI::GetName);
                        script.SetTableFunction(-1, "SetName",                 FFI::SystemFFI::SceneNodeFFI::SetName);

                        script.SetTableFunction(-1, "GetParentPtr",            FFI::SystemFFI::SceneNodeFFI::GetParentPtr);
                        script.SetTableFunction(-1, "AttachChild",             FFI::SystemFFI::SceneNodeFFI::AttachChild);
                        script.SetTableFunction(-1, "Orphan",                  FFI::SystemFFI::SceneNodeFFI::Orphan);
                        script.SetTableFunction(-1, "IsAncestor",              FFI::SystemFFI::SceneNodeFFI::IsAncestor);
                        script.SetTableFunction(-1, "IsDescendant",            FFI::SystemFFI::SceneNodeFFI::IsDescendant);
                        script.SetTableFunction(-1, "IsRelative",              FFI::SystemFFI::SceneNodeFFI::IsRelative);
                        script.SetTableFunction(-1, "GetChildrenIDs",          FFI::SystemFFI::SceneNodeFFI::GetChildrenIDs);

                        script.SetTableFunction(-1, "AddComponent",            FFI::SystemFFI::SceneNodeFFI::AddComponent);
                        script.SetTableFunction(-1, "RemoveComponent",         FFI::SystemFFI::SceneNodeFFI::RemoveComponent);
                        script.SetTableFunction(-1, "GetComponent",            FFI::SystemFFI::SceneNodeFFI::GetComponent);

                        script.SetTableFunction(-1, "GetPosition",             FFI::SystemFFI::SceneNodeFFI::GetPosition);
                        script.SetTableFunction(-1, "GetWorldPosition",        FFI::SystemFFI::SceneNodeFFI::GetWorldPosition);
                        script.SetTableFunction(-1, "SetPosition",             FFI::SystemFFI::SceneNodeFFI::SetPosition);
                        script.SetTableFunction(-1, "SetWorldPosition",        FFI::SystemFFI::SceneNodeFFI::SetWorldPosition);
                        script.SetTableFunction(-1, "GetRotationXYZ",          FFI::SystemFFI::SceneNodeFFI::GetRotationXYZ);
                        script.SetTableFunction(-1, "GetWorldRotationXYZ",     FFI::SystemFFI::SceneNodeFFI::GetWorldRotationXYZ);
                        script.SetTableFunction(-1, "SetRotationXYZ",          FFI::SystemFFI::SceneNodeFFI::SetRotationXYZ);
                        script.SetTableFunction(-1, "SetWorldRotationXYZ",     FFI::SystemFFI::SceneNodeFFI::SetWorldRotationXYZ);
                        script.SetTableFunction(-1, "GetScale",                FFI::SystemFFI::SceneNodeFFI::GetScale);
                        script.SetTableFunction(-1, "GetWorldScale",           FFI::SystemFFI::SceneNodeFFI::GetWorldScale);
                        script.SetTableFunction(-1, "SetScale",                FFI::SystemFFI::SceneNodeFFI::SetScale);
                        script.SetTableFunction(-1, "SetWorldScale",           FFI::SystemFFI::SceneNodeFFI::SetWorldScale);

                        script.SetTableFunction(-1, "GetAttachedComponentIDs", FFI::SystemFFI::SceneNodeFFI::GetAttachedComponentIDs);

                        script.SetTableFunction(-1, "Show",                    FFI::SystemFFI::SceneNodeFFI::Show);
                        script.SetTableFunction(-1, "Hide",                    FFI::SystemFFI::SceneNodeFFI::Hide);
                        script.SetTableFunction(-1, "IsVisible",               FFI::SystemFFI::SceneNodeFFI::IsVisible);

                        script.SetTableFunction(-1, "GetScenePtr",             FFI::SystemFFI::SceneNodeFFI::GetScenePtr);
                    }
                    script.Pop(1);


                    script.Push("Scene");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "AddSceneNode",        FFI::SystemFFI::SceneFFI::AddSceneNode);
                        script.SetTableFunction(-1, "RemoveSceneNode",     FFI::SystemFFI::SceneFFI::RemoveSceneNode);
                        script.SetTableFunction(-1, "CreateNewSceneNode",  FFI::SystemFFI::SceneFFI::CreateNewSceneNode);
                        script.SetTableFunction(-1, "GetSceneNodePtrs",    FFI::SystemFFI::SceneFFI::GetSceneNodePtrs);
                        script.SetTableFunction(-1, "GetSceneNodePtrByID", FFI::SystemFFI::SceneFFI::GetSceneNodePtrByID);
                    }
                    script.Pop(1);



                    script.Push("ModelComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetModel",                FFI::SystemFFI::ModelComponentFFI::SetModel);
                        script.SetTableFunction(-1, "GetModelPath",            FFI::SystemFFI::ModelComponentFFI::GetModelPath);
                        script.SetTableFunction(-1, "EnableShadowCasting",     FFI::SystemFFI::ModelComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",    FFI::SystemFFI::ModelComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",  FFI::SystemFFI::ModelComponentFFI::IsShadowCastingEnabled);
                        script.SetTableFunction(-1, "SetMaterial",             FFI::SystemFFI::ModelComponentFFI::SetMaterial);
                        script.SetTableFunction(-1, "GetMaterialPath",         FFI::SystemFFI::ModelComponentFFI::GetMaterialPath);
                        script.SetTableFunction(-1, "GetMaterialCount",        FFI::SystemFFI::ModelComponentFFI::GetMaterialCount);
                    }
                    script.Pop(1);


                    script.Push("PointLightComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetColour",               FFI::SystemFFI::PointLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               FFI::SystemFFI::PointLightComponentFFI::GetColour);
                        script.SetTableFunction(-1, "EnableShadowCasting",     FFI::SystemFFI::PointLightComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",    FFI::SystemFFI::PointLightComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",  FFI::SystemFFI::PointLightComponentFFI::IsShadowCastingEnabled);
                        script.SetTableFunction(-1, "GetConstantAttenuation",  FFI::SystemFFI::PointLightComponentFFI::GetConstantAttenuation);
                        script.SetTableFunction(-1, "GetLinearAttenuation",    FFI::SystemFFI::PointLightComponentFFI::GetLinearAttenuation);
                        script.SetTableFunction(-1, "GetQuadraticAttenuation", FFI::SystemFFI::PointLightComponentFFI::GetQuadraticAttenuation);
                        script.SetTableFunction(-1, "SetAttenuation",          FFI::SystemFFI::PointLightComponentFFI::SetAttenuation);
                    }
                    script.Pop(1);


                    script.Push("SpotLightComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetColour",               FFI::SystemFFI::SpotLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               FFI::SystemFFI::SpotLightComponentFFI::GetColour);
                        script.SetTableFunction(-1, "EnableShadowCasting",     FFI::SystemFFI::SpotLightComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",    FFI::SystemFFI::SpotLightComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",  FFI::SystemFFI::SpotLightComponentFFI::IsShadowCastingEnabled);
                        script.SetTableFunction(-1, "GetConstantAttenuation",  FFI::SystemFFI::SpotLightComponentFFI::GetConstantAttenuation);
                        script.SetTableFunction(-1, "GetLinearAttenuation",    FFI::SystemFFI::SpotLightComponentFFI::GetLinearAttenuation);
                        script.SetTableFunction(-1, "GetQuadraticAttenuation", FFI::SystemFFI::SpotLightComponentFFI::GetQuadraticAttenuation);
                        script.SetTableFunction(-1, "SetAttenuation",          FFI::SystemFFI::SpotLightComponentFFI::SetAttenuation);
                        script.SetTableFunction(-1, "SetAngles",               FFI::SystemFFI::SpotLightComponentFFI::SetAngles);
                        script.SetTableFunction(-1, "GetAngles",               FFI::SystemFFI::SpotLightComponentFFI::GetAngles);
                    }
                    script.Pop(1);


                    script.Push("DirectionalLightComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetColour",               FFI::SystemFFI::DirectionalLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               FFI::SystemFFI::DirectionalLightComponentFFI::GetColour);
                        script.SetTableFunction(-1, "EnableShadowCasting",     FFI::SystemFFI::DirectionalLightComponentFFI::EnableShadowCasting);
                        script.SetTableFunction(-1, "DisableShadowCasting",    FFI::SystemFFI::DirectionalLightComponentFFI::DisableShadowCasting);
                        script.SetTableFunction(-1, "IsShadowCastingEnabled",  FFI::SystemFFI::DirectionalLightComponentFFI::IsShadowCastingEnabled);
                    }
                    script.Pop(1);


                    script.Push("AmbientLightComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetColour",               FFI::SystemFFI::AmbientLightComponentFFI::SetColour);
                        script.SetTableFunction(-1, "GetColour",               FFI::SystemFFI::AmbientLightComponentFFI::GetColour);
                    }
                    script.Pop(1);


                    script.Push("DynamicsComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetMass",                                        FFI::SystemFFI::DynamicsComponentFFI::SetMass);
                        script.SetTableFunction(-1, "GetMass",                                        FFI::SystemFFI::DynamicsComponentFFI::GetMass);
                        script.SetTableFunction(-1, "IsStatic",                                       FFI::SystemFFI::DynamicsComponentFFI::IsStatic);
                        script.SetTableFunction(-1, "IsKinematic",                                    FFI::SystemFFI::DynamicsComponentFFI::IsKinematic);
                        script.SetTableFunction(-1, "SetFriction",                                    FFI::SystemFFI::DynamicsComponentFFI::SetFriction);
                        script.SetTableFunction(-1, "GetFriction",                                    FFI::SystemFFI::DynamicsComponentFFI::GetFriction);
                        script.SetTableFunction(-1, "SetRestitution",                                 FFI::SystemFFI::DynamicsComponentFFI::SetRestitution);
                        script.SetTableFunction(-1, "GetRestitution",                                 FFI::SystemFFI::DynamicsComponentFFI::GetRestitution);
                        script.SetTableFunction(-1, "SetDamping",                                     FFI::SystemFFI::DynamicsComponentFFI::SetDamping);
                        script.SetTableFunction(-1, "GetLinearDamping",                               FFI::SystemFFI::DynamicsComponentFFI::GetLinearDamping);
                        script.SetTableFunction(-1, "GetAngularDamping",                              FFI::SystemFFI::DynamicsComponentFFI::GetAngularDamping);

                        script.SetTableFunction(-1, "SetLinearVelocity",                              FFI::SystemFFI::DynamicsComponentFFI::SetLinearVelocity);
                        script.SetTableFunction(-1, "GetLinearVelocity",                              FFI::SystemFFI::DynamicsComponentFFI::GetLinearVelocity);
                        script.SetTableFunction(-1, "SetAngularVelocity",                             FFI::SystemFFI::DynamicsComponentFFI::SetAngularVelocity);
                        script.SetTableFunction(-1, "GetAngularVelocity",                             FFI::SystemFFI::DynamicsComponentFFI::GetAngularVelocity);

                        script.SetTableFunction(-1, "SetLinearFactor",                                FFI::SystemFFI::DynamicsComponentFFI::SetLinearFactor);
                        script.SetTableFunction(-1, "SetAngularFactor",                               FFI::SystemFFI::DynamicsComponentFFI::SetAngularFactor);

                        script.SetTableFunction(-1, "SetGravity",                                     FFI::SystemFFI::DynamicsComponentFFI::SetGravity);
                        script.SetTableFunction(-1, "GetGravity",                                     FFI::SystemFFI::DynamicsComponentFFI::GetGravity);
                        script.SetTableFunction(-1, "ApplyGravity",                                   FFI::SystemFFI::DynamicsComponentFFI::ApplyGravity);

                        script.SetTableFunction(-1, "ApplyCentralForce",                              FFI::SystemFFI::DynamicsComponentFFI::ApplyCentralForce);
                        script.SetTableFunction(-1, "ApplyTorque",                                    FFI::SystemFFI::DynamicsComponentFFI::ApplyTorque);
                        script.SetTableFunction(-1, "ApplyCentralImpulse",                            FFI::SystemFFI::DynamicsComponentFFI::ApplyCentralImpulse);
                        script.SetTableFunction(-1, "ApplyImpulse",                                   FFI::SystemFFI::DynamicsComponentFFI::ApplyImpulse);
                        script.SetTableFunction(-1, "ApplyTorqueImpulse",                             FFI::SystemFFI::DynamicsComponentFFI::ApplyTorqueImpulse);

                        script.SetTableFunction(-1, "DisableDeactivation",                            FFI::SystemFFI::DynamicsComponentFFI::DisableDeactivation);
                        script.SetTableFunction(-1, "EnableDeactivation",                             FFI::SystemFFI::DynamicsComponentFFI::EnableDeactivation);
                        script.SetTableFunction(-1, "IsDeactivationEnabled",                          FFI::SystemFFI::DynamicsComponentFFI::IsDeactivationEnabled);
                        script.SetTableFunction(-1, "Activate",                                       FFI::SystemFFI::DynamicsComponentFFI::Activate);

                        script.SetTableFunction(-1, "EnableNavigationMeshGeneration",                 FFI::SystemFFI::DynamicsComponentFFI::EnableNavigationMeshGeneration);
                        script.SetTableFunction(-1, "DisableNavigationMeshGeneration",                FFI::SystemFFI::DynamicsComponentFFI::DisableNavigationMeshGeneration);
                        script.SetTableFunction(-1, "IsNavigationMeshGenerationEnabled",              FFI::SystemFFI::DynamicsComponentFFI::IsNavigationMeshGenerationEnabled);


                        script.SetTableFunction(-1, "SetCollisionFilter",                             FFI::SystemFFI::DynamicsComponentFFI::SetCollisionFilter);
                        script.SetTableFunction(-1, "GetCollisionGroup",                              FFI::SystemFFI::DynamicsComponentFFI::GetCollisionGroup);
                        script.SetTableFunction(-1, "GetCollisionMask",                               FFI::SystemFFI::DynamicsComponentFFI::GetCollisionMask);

                        script.SetTableFunction(-1, "AddBoxCollisionShape",                           FFI::SystemFFI::DynamicsComponentFFI::AddBoxCollisionShape);
                        script.SetTableFunction(-1, "AddSphereCollisionShape",                        FFI::SystemFFI::DynamicsComponentFFI::AddSphereCollisionShape);
                        script.SetTableFunction(-1, "AddEllipsoidCollisionShape",                     FFI::SystemFFI::DynamicsComponentFFI::AddEllipsoidCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderXCollisionShape",                     FFI::SystemFFI::DynamicsComponentFFI::AddCylinderXCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderYCollisionShape",                     FFI::SystemFFI::DynamicsComponentFFI::AddCylinderYCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderZCollisionShape",                     FFI::SystemFFI::DynamicsComponentFFI::AddCylinderZCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleXCollisionShape",                      FFI::SystemFFI::DynamicsComponentFFI::AddCapsuleXCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleYCollisionShape",                      FFI::SystemFFI::DynamicsComponentFFI::AddCapsuleYCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleZCollisionShape",                      FFI::SystemFFI::DynamicsComponentFFI::AddCapsuleZCollisionShape);
                        script.SetTableFunction(-1, "SetCollisionShapesToModelConvexHulls",           FFI::SystemFFI::DynamicsComponentFFI::SetCollisionShapesToModelConvexHulls);
                        script.SetTableFunction(-1, "RemoveAllCollisionShapes",                       FFI::SystemFFI::DynamicsComponentFFI::RemoveAllCollisionShapes);
                        script.SetTableFunction(-1, "RemoveCollisionShapeAtIndex",                    FFI::SystemFFI::DynamicsComponentFFI::RemoveCollisionShapeAtIndex);
                        script.SetTableFunction(-1, "GetCollisionShapeCount",                         FFI::SystemFFI::DynamicsComponentFFI::GetCollisionShapeCount);
                        script.SetTableFunction(-1, "GetCollisionShapeAtIndex",                       FFI::SystemFFI::DynamicsComponentFFI::GetCollisionShapeAtIndex);
                        script.SetTableFunction(-1, "IsUsingConvexHullsFromModel",                    FFI::SystemFFI::DynamicsComponentFFI::IsUsingConvexHullsFromModel);
                        script.SetTableFunction(-1, "SetCollisionShapeOffset",                        FFI::SystemFFI::DynamicsComponentFFI::SetCollisionShapeOffset);
                        script.SetTableFunction(-1, "SetBoxCollisionShapeHalfExtents",                FFI::SystemFFI::DynamicsComponentFFI::SetBoxCollisionShapeHalfExtents);
                        script.SetTableFunction(-1, "SetSphereCollisionShapeRadius",                  FFI::SystemFFI::DynamicsComponentFFI::SetSphereCollisionShapeRadius);
                        script.SetTableFunction(-1, "SetEllipsoidCollisionShapeRadius",               FFI::SystemFFI::DynamicsComponentFFI::SetEllipsoidCollisionShapeRadius);
                        script.SetTableFunction(-1, "SetCylinderCollisionShapeHalfExtents",           FFI::SystemFFI::DynamicsComponentFFI::SetCylinderCollisionShapeHalfExtents);
                        script.SetTableFunction(-1, "SetCapsuleCollisionShapeSize",                   FFI::SystemFFI::DynamicsComponentFFI::SetCapsuleCollisionShapeSize);
                    }
                    script.Pop(1);


                    script.Push("ProximityComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "SetCollisionFilter",                             FFI::SystemFFI::ProximityComponentFFI::SetCollisionFilter);
                        script.SetTableFunction(-1, "GetCollisionGroup",                              FFI::SystemFFI::ProximityComponentFFI::GetCollisionGroup);
                        script.SetTableFunction(-1, "GetCollisionMask",                               FFI::SystemFFI::ProximityComponentFFI::GetCollisionMask);

                        script.SetTableFunction(-1, "AddBoxCollisionShape",                           FFI::SystemFFI::ProximityComponentFFI::AddBoxCollisionShape);
                        script.SetTableFunction(-1, "AddSphereCollisionShape",                        FFI::SystemFFI::ProximityComponentFFI::AddSphereCollisionShape);
                        script.SetTableFunction(-1, "AddEllipsoidCollisionShape",                     FFI::SystemFFI::ProximityComponentFFI::AddEllipsoidCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderXCollisionShape",                     FFI::SystemFFI::ProximityComponentFFI::AddCylinderXCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderYCollisionShape",                     FFI::SystemFFI::ProximityComponentFFI::AddCylinderYCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderZCollisionShape",                     FFI::SystemFFI::ProximityComponentFFI::AddCylinderZCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleXCollisionShape",                      FFI::SystemFFI::ProximityComponentFFI::AddCapsuleXCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleYCollisionShape",                      FFI::SystemFFI::ProximityComponentFFI::AddCapsuleYCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleZCollisionShape",                      FFI::SystemFFI::ProximityComponentFFI::AddCapsuleZCollisionShape);
                        script.SetTableFunction(-1, "SetCollisionShapesToModelConvexHulls",           FFI::SystemFFI::ProximityComponentFFI::SetCollisionShapesToModelConvexHulls);
                        script.SetTableFunction(-1, "RemoveAllCollisionShapes",                       FFI::SystemFFI::ProximityComponentFFI::RemoveAllCollisionShapes);
                        script.SetTableFunction(-1, "RemoveCollisionShapeAtIndex",                    FFI::SystemFFI::ProximityComponentFFI::RemoveCollisionShapeAtIndex);
                        script.SetTableFunction(-1, "GetCollisionShapeCount",                         FFI::SystemFFI::ProximityComponentFFI::GetCollisionShapeCount);
                        script.SetTableFunction(-1, "GetCollisionShapeAtIndex",                       FFI::SystemFFI::ProximityComponentFFI::GetCollisionShapeAtIndex);
                        script.SetTableFunction(-1, "IsUsingConvexHullsFromModel",                    FFI::SystemFFI::ProximityComponentFFI::IsUsingConvexHullsFromModel);
                        script.SetTableFunction(-1, "SetCollisionShapeOffset",                        FFI::SystemFFI::ProximityComponentFFI::SetCollisionShapeOffset);
                        script.SetTableFunction(-1, "SetBoxCollisionShapeHalfExtents",                FFI::SystemFFI::ProximityComponentFFI::SetBoxCollisionShapeHalfExtents);
                        script.SetTableFunction(-1, "SetSphereCollisionShapeRadius",                  FFI::SystemFFI::ProximityComponentFFI::SetSphereCollisionShapeRadius);
                        script.SetTableFunction(-1, "SetEllipsoidCollisionShapeRadius",               FFI::SystemFFI::ProximityComponentFFI::SetEllipsoidCollisionShapeRadius);
                        script.SetTableFunction(-1, "SetCylinderCollisionShapeHalfExtents",           FFI::SystemFFI::ProximityComponentFFI::SetCylinderCollisionShapeHalfExtents);
                        script.SetTableFunction(-1, "SetCapsuleCollisionShapeSize",                   FFI::SystemFFI::ProximityComponentFFI::SetCapsuleCollisionShapeSize);
                    }
                    script.Pop(1);


                    script.Push("ScriptComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "AddScript",                                FFI::SystemFFI::ScriptComponentFFI::AddScript);
                        script.SetTableFunction(-1, "RemoveScriptByRelativePath",               FFI::SystemFFI::ScriptComponentFFI::RemoveScriptByRelativePath);
                        script.SetTableFunction(-1, "RemoveScriptByIndex",                      FFI::SystemFFI::ScriptComponentFFI::RemoveScriptByIndex);
                        script.SetTableFunction(-1, "ReloadScript",                             FFI::SystemFFI::ScriptComponentFFI::ReloadScript);
                        script.SetTableFunction(-1, "GetScriptFilePaths",                       FFI::SystemFFI::ScriptComponentFFI::GetScriptFilePaths);
                        script.SetTableFunction(-1, "GetPublicVariableNamesAndTypesByIndex",    FFI::SystemFFI::ScriptComponentFFI::GetPublicVariableNamesAndTypesByIndex);
                        script.SetTableFunction(-1, "GetPublicVariableNamesAndValues",          FFI::SystemFFI::ScriptComponentFFI::GetPublicVariableNamesAndValues);
                        script.SetTableFunction(-1, "GetPublicVariableValue",                   FFI::SystemFFI::ScriptComponentFFI::GetPublicVariableValue);
                        script.SetTableFunction(-1, "SetPublicVariableValue",                   FFI::SystemFFI::ScriptComponentFFI::SetPublicVariableValue);
                    }
                    script.Pop(1);


                    script.Push("EditorMetadataComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "Select",                  FFI::SystemFFI::EditorMetadataComponentFFI::Select);
                        script.SetTableFunction(-1, "Deselect",                FFI::SystemFFI::EditorMetadataComponentFFI::Deselect);
                        script.SetTableFunction(-1, "IsSelected",              FFI::SystemFFI::EditorMetadataComponentFFI::IsSelected);
                        script.SetTableFunction(-1, "ShowSprite",              FFI::SystemFFI::EditorMetadataComponentFFI::ShowSprite);
                        script.SetTableFunction(-1, "HideSprite",              FFI::SystemFFI::EditorMetadataComponentFFI::HideSprite);
                        script.SetTableFunction(-1, "IsShowingSprite",         FFI::SystemFFI::EditorMetadataComponentFFI::IsShowingSprite);
                        script.SetTableFunction(-1, "GetSpriteTexturePath",    FFI::SystemFFI::EditorMetadataComponentFFI::GetSpriteTexturePath);
                        script.SetTableFunction(-1, "ShowDirectionArrow",      FFI::SystemFFI::EditorMetadataComponentFFI::ShowDirectionArrow);
                        script.SetTableFunction(-1, "HideDirectionArrow",      FFI::SystemFFI::EditorMetadataComponentFFI::HideDirectionArrow);
                        script.SetTableFunction(-1, "IsShowingDirectionArrow", FFI::SystemFFI::EditorMetadataComponentFFI::IsShowingDirectionArrow);
                    }
                    script.Pop(1);


                    script.Push("SubEditor");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "GetAbsolutePath",              FFI::SystemFFI::SubEditorFFI::GetAbsolutePath);
                        script.SetTableFunction(-1, "GetRelativePath",              FFI::SystemFFI::SubEditorFFI::GetRelativePath);
                    }
                    script.Pop(1);

                    script.Push("ModelEditor");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "GetMaterials",                 FFI::SystemFFI::ModelEditorFFI::GetMaterials);
                        script.SetTableFunction(-1, "GetConvexHullBuildSettings",   FFI::SystemFFI::ModelEditorFFI::GetConvexHullBuildSettings);
                        script.SetTableFunction(-1, "PlayAnimation",                FFI::SystemFFI::ModelEditorFFI::PlayAnimation);
                        script.SetTableFunction(-1, "StopAnimation",                FFI::SystemFFI::ModelEditorFFI::StopAnimation);
                        script.SetTableFunction(-1, "SetMaterial",                  FFI::SystemFFI::ModelEditorFFI::SetMaterial);
                        script.SetTableFunction(-1, "ShowConvexDecomposition",      FFI::SystemFFI::ModelEditorFFI::ShowConvexDecomposition);
                        script.SetTableFunction(-1, "HideConvexDecomposition",      FFI::SystemFFI::ModelEditorFFI::HideConvexDecomposition);
                        script.SetTableFunction(-1, "BuildConvexDecomposition",     FFI::SystemFFI::ModelEditorFFI::BuildConvexDecomposition);
                    }
                    script.Pop(1);

                    script.Push("SceneEditor");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "GetScenePtr",                         FFI::SystemFFI::SceneEditorFFI::GetScenePtr);
                        script.SetTableFunction(-1, "DeselectAll",                         FFI::SystemFFI::SceneEditorFFI::DeselectAll);
                        script.SetTableFunction(-1, "SelectSceneNode",                     FFI::SystemFFI::SceneEditorFFI::SelectSceneNode);
                        script.SetTableFunction(-1, "DeselectSceneNode",                   FFI::SystemFFI::SceneEditorFFI::DeselectSceneNode);
                        script.SetTableFunction(-1, "GetSelectedSceneNodeCount",           FFI::SystemFFI::SceneEditorFFI::GetSelectedSceneNodeCount);
                        script.SetTableFunction(-1, "GetFirstSelectedSceneNodePtr",        FFI::SystemFFI::SceneEditorFFI::GetFirstSelectedSceneNodePtr);
                        script.SetTableFunction(-1, "GetSelectedSceneNodeIDs",             FFI::SystemFFI::SceneEditorFFI::GetSelectedSceneNodeIDs);
                        script.SetTableFunction(-1, "IsSceneNodeSelectedByID",             FFI::SystemFFI::SceneEditorFFI::IsSceneNodeSelectedByID);
                        script.SetTableFunction(-1, "TryGizmoMouseSelect",                 FFI::SystemFFI::SceneEditorFFI::TryGizmoMouseSelect);
                        script.SetTableFunction(-1, "DoMouseSelection",                    FFI::SystemFFI::SceneEditorFFI::DoMouseSelection);
                        script.SetTableFunction(-1, "RemoveSelectedSceneNodes",            FFI::SystemFFI::SceneEditorFFI::RemoveSelectedSceneNodes);
                        script.SetTableFunction(-1, "RemoveSceneNode",                     FFI::SystemFFI::SceneEditorFFI::RemoveSceneNode);
                        script.SetTableFunction(-1, "DuplicateSelectedSceneNodes",         FFI::SystemFFI::SceneEditorFFI::DuplicateSelectedSceneNodes);
                        script.SetTableFunction(-1, "DuplicateSceneNode",                  FFI::SystemFFI::SceneEditorFFI::DuplicateSceneNode);
                        script.SetTableFunction(-1, "SwitchGizmoToTranslateMode",          FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToTranslateMode);
                        script.SetTableFunction(-1, "SwitchGizmoToRotateMode",             FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToRotateMode);
                        script.SetTableFunction(-1, "SwitchGizmoToScaleMode",              FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToScaleMode);
                        script.SetTableFunction(-1, "SwitchGizmoToLocalSpace",             FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToLocalSpace);
                        script.SetTableFunction(-1, "SwitchGizmoToGlobalSpace",            FFI::SystemFFI::SceneEditorFFI::SwitchGizmoToGlobalSpace);
                        script.SetTableFunction(-1, "ToggleGizmoSpace",                    FFI::SystemFFI::SceneEditorFFI::ToggleGizmoSpace);
                        script.SetTableFunction(-1, "IsGizmoInLocalSpace",                 FFI::SystemFFI::SceneEditorFFI::IsGizmoInLocalSpace);
                        script.SetTableFunction(-1, "IsGizmoInGlobalSpace",                FFI::SystemFFI::SceneEditorFFI::IsGizmoInGlobalSpace);

                        script.SetTableFunction(-1, "StartPlaying",                        FFI::SystemFFI::SceneEditorFFI::StartPlaying);
                        script.SetTableFunction(-1, "StopPlaying",                         FFI::SystemFFI::SceneEditorFFI::StopPlaying);
                        script.SetTableFunction(-1, "IsPlaying",                           FFI::SystemFFI::SceneEditorFFI::IsPlaying);

                        script.SetTableFunction(-1, "EnablePhysicsSimulation",             FFI::SystemFFI::SceneEditorFFI::EnablePhysicsSimulation);
                        script.SetTableFunction(-1, "DisablePhysicsSimulation",            FFI::SystemFFI::SceneEditorFFI::DisablePhysicsSimulation);
                        script.SetTableFunction(-1, "IsPhysicsSimulationEnabled",          FFI::SystemFFI::SceneEditorFFI::IsPhysicsSimulationEnabled);
                        script.SetTableFunction(-1, "CommitStateStackFrame",               FFI::SystemFFI::SceneEditorFFI::CommitStateStackFrame);
                        script.SetTableFunction(-1, "Undo",                                FFI::SystemFFI::SceneEditorFFI::Undo);
                        script.SetTableFunction(-1, "Redo",                                FFI::SystemFFI::SceneEditorFFI::Redo);
                        script.SetTableFunction(-1, "InstantiatePrefab",                   FFI::SystemFFI::SceneEditorFFI::InstantiatePrefab);
                        script.SetTableFunction(-1, "GetSceneNodePtrByID",                 FFI::SystemFFI::SceneEditorFFI::GetSceneNodePtrByID);
                        script.SetTableFunction(-1, "GetSceneNodes",                       FFI::SystemFFI::SceneEditorFFI::GetSceneNodes);
                        script.SetTableFunction(-1, "PositionSceneNodeInFrontOfCamera",    FFI::SystemFFI::SceneEditorFFI::PositionSceneNodeInFrontOfCamera);
                    }
                    script.Pop(1);
                }
                script.Pop(1);



                script.Push("Renderer");
                script.GetTableValue(-2);
                if (script.IsTable(-1))
                {
                    script.SetTableFunction(-1, "EnableVSync",  FFI::RendererFFI::EnableVSync);
                    script.SetTableFunction(-1, "DisableVSync", FFI::RendererFFI::DisableVSync);
                }
                script.Pop(1);


                script.Push("Audio");
                script.GetTableValue(-2);
                if (script.IsTable(-1))
                {
                    script.SetTableFunction(-1, "Play", FFI::AudioFFI::Play);
                }
                script.Pop(1);
            }
            script.Pop(1);

            return successful;
        }


        bool RegisterScene(GTCore::Script &script, Scene &scene)
        {
            // We map Lua Scene objects to the C++ pointer counterparts. We store this in GTEngine.RegisteredScenes.

            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    // Key (C++ pointer).
                    script.Push(&scene);

                    // Value (Lua object).
                    script.Push("Scene");
                    script.GetTableValue(-4);       // <-- GTEngine
                    assert(script.IsTable(-1));
                    {
                        script.Push("Create");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);       // Argument 1: GTEngine.Scene.
                            script.Push(&scene);        // Argument 2: The C++ pointer.

                            script.Call(2, 1);          // 1 return value: The Lua Scene object.

                            // We need to move the return value so that it's in the correct position after all of the pops below.
                            script.InsertIntoStack(-2);
                        }
                    }
                    script.Pop(1);

                    assert(script.IsPointer(-2));
                    assert(script.IsTable(-1));
                    {
                        // The key and value are in place, so now we just want to set it.
                        script.SetTableValue(-3);
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);


            return true;
        }

        void UnregisterScene(GTCore::Script &script, Scene &scene)
        {
            // For now we'll just set the value in GTEngine.RegisteredScenes to nil, but this might need improving later on.

            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(&scene);        // Key   - C++ pointer.
                    script.PushNil();           // Value - Lua object, or in this case nil so that it's removed.
                    script.SetTableValue(-3);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void InstantiateSceneNode(GTCore::Script &script, SceneNode &sceneNode)
        {
            // We need to find the scene in GTEngine.RegisteredScenes which will be keyed by the scene. We then want to call InstantiateSceneNode() on it.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(sceneNode.GetScene());
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))                         // <-- Don't want to use an assert here. It's a valid case for this to not exist. In this case, we just ignore.
                    {
                        script.Push("InstantiateSceneNode");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);                               // <-- 'self'
                            script.Push(static_cast<int>(sceneNode.GetID()));   // <-- 'sceneNodeID'
                            script.Push(&sceneNode);                            // <-- 'sceneNodePtr'
                            script.Call(3, 0);
                        }
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void UninstantiateSceneNode(GTCore::Script &script, SceneNode &sceneNode)
        {
            // We need to find the scene in GTEngine.RegisteredScenes which will be keyed by the scene. We then want to call InstantiateSceneNode() on it.
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(sceneNode.GetScene());
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))                         // <-- Don't want to use an assert here. It's a valid case for this to not exist. In this case, we just ignore.
                    {
                        script.Push("UninstantiateSceneNode");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);                               // <-- 'self'
                            script.Push(static_cast<int>(sceneNode.GetID()));   // <-- 'sceneNodeID'
                            script.Call(2, 0);
                        }
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void UpdatePublicVariables(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("UpdatePublicVariables");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void RegisterScriptComponent(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("RegisterScriptComponent");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PushSceneNode(GTCore::Script &script, SceneNode &sceneNode)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("RegisteredScenes");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(sceneNode.GetScene());
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))                         // <-- Don't want to use an assert here. It's a valid case for this to not exist. In this case, we just ignore.
                    {
                        script.Push("GetSceneNodeByID");
                        script.GetTableValue(-2);
                        assert(script.IsFunction(-1));
                        {
                            script.PushValue(-2);                               // <-- 'self'
                            script.Push(static_cast<int>(sceneNode.GetID()));   // <-- 'sceneNodeID'
                            script.Call(2, 1);

                            script.InsertIntoStack(-4);
                        }
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }
            script.Pop(1);

            assert(script.IsTable(-1));
        }

        void PostSceneNodeEvent_OnUpdate(GTCore::Script &script, SceneNode &sceneNode, double deltaTimeInSeconds)
        {
            // First we push the scene node.
            Scripting::PushSceneNode(script, sceneNode);

            // Now we just call OnUpdate on it.
            assert(script.IsTable(-1));
            {
                script.Push("OnUpdate");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Push(deltaTimeInSeconds);        // <-- 'deltaTimeInSeconds'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnStartup(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnStartup");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnShutdown(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnShutdown");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnShow(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnShow");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnHide(GTCore::Script &script, SceneNode &sceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnHide");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                   // <-- 'self'
                    script.Call(1, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnObjectEnter(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnObjectEnter");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                               // <-- 'self'
                    Scripting::PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnEnterObject(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnEnterObject");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                               // <-- 'self'
                    Scripting::PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnObjectLeave(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnObjectLeave");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                               // <-- 'self'
                    Scripting::PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }

        void PostSceneNodeEvent_OnLeaveObject(GTCore::Script &script, SceneNode &sceneNode, SceneNode &otherSceneNode)
        {
            Scripting::PushSceneNode(script, sceneNode);
            assert(script.IsTable(-1));
            {
                script.Push("OnLeaveObject");
                script.GetTableValue(-2);
                assert(script.IsFunction(-1));
                {
                    script.PushValue(-2);                               // <-- 'self'
                    Scripting::PushSceneNode(script, otherSceneNode);   // <-- 'otherSceneNode'
                    script.Call(2, 0);
                }
            }
            script.Pop(1);
        }



        bool LoadScriptDefinition(GTCore::Script &script, const char* scriptRelativePath, const char* scriptString)
        {
            // We actually want to do this as a text script for now.
            GTCore::Strings::List<char> fullScriptString;
            fullScriptString.Append("GTEngine.__CreateScriptClass = function()");
            fullScriptString.Append("    local self = {}");
            fullScriptString.Append("    "); fullScriptString.Append(scriptString);
            fullScriptString.Append("    return self;");
            fullScriptString.Append("end;");
            fullScriptString.Append("GTEngine.ScriptDefinitions['"); fullScriptString.Append(scriptRelativePath); fullScriptString.Append("'] = GTEngine.__CreateScriptClass();");

            return script.Execute(fullScriptString.c_str());
        }

        void UnloadScriptDefinition(GTCore::Script &script, const char* scriptRelativePath)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("ScriptDefinitions");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    script.Push(scriptRelativePath);    // Key   - The file path.
                    script.PushNil();                   // Value - Lua object, or in this case nil so that it's removed.
                    script.SetTableValue(-3);
                }
                script.Pop(1);
            }
            script.Pop(1);
        }

        void SyncScriptDefinitionsWithLibrary(GTCore::Script &script)
        {
            script.GetGlobal("GTEngine");
            assert(script.IsTable(-1));
            {
                script.Push("ScriptDefinitions");
                script.GetTableValue(-2);
                assert(script.IsTable(-1));
                {
                    // 1) Find the definitions to remove.
                    GTCore::Vector<GTCore::String> definitionsToRemove;

                    for (script.PushNil(); script.Next(-2); script.Pop(1))
                    {
                        auto relativePath = script.ToString(-2);
                        if (!ScriptLibrary::IsLoaded(relativePath))
                        {
                            definitionsToRemove.PushBack(relativePath);
                        }
                    }

                    // 2) Remove applicable definitions.
                    for (size_t i = 0; i < definitionsToRemove.count; ++i)
                    {
                        script.Push(definitionsToRemove[i].c_str());
                        script.PushNil();
                        script.SetTableValue(-3);
                    }


                    // 3) Update existing definitions and add new definitions.
                    size_t definitionCount = ScriptLibrary::GetLoadedDefinitionCount();
                    for (size_t i = 0; i < definitionCount; ++i)
                    {
                        auto definition = ScriptLibrary::GetLoadedDefinitionByIndex(i);
                        assert(definition != nullptr);
                        {
                            Scripting::LoadScriptDefinition(script, definition->GetRelativePath(), definition->GetScriptString());
                        }
                    }
                }
                script.Pop(1);
            }
            script.Pop(1);
        }




        namespace FFI
        {
            int GetExecutableDirectory(GTCore::Script &script)
            {
                script.Push(GTEngine::GetExecutableDirectory());
                return 1;
            }


            int IsModelFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedModelExtension(script.ToString(1)));
                return 1;
            }

            int IsImageFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedImageExtension(script.ToString(1)));
                return 1;
            }

            int IsSoundFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedSoundExtension(script.ToString(1)));
                return 1;
            }

            int IsSceneFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedSceneExtension(script.ToString(1)));
                return 1;
            }

            int IsPrefabFile(GTCore::Script &script)
            {
                script.Push(IO::IsSupportedPrefabExtension(script.ToString(1)));
                return 1;
            }

            int IsTextFile(GTCore::Script &script)
            {
                // There can be any number of text files. Perhaps we should assume that if it's not a resouce file like a model and texture, we should assume a text file?

                auto extension = GTCore::Path::Extension(script.ToString(1));

                bool result = GTCore::Strings::Equal<false>(extension, "")       ||
                              GTCore::Strings::Equal<false>(extension, "txt")    ||
                              GTCore::Strings::Equal<false>(extension, "lua")    ||
                              GTCore::Strings::Equal<false>(extension, "cfg")    ||
                              GTCore::Strings::Equal<false>(extension, "xml")    ||
                              GTCore::Strings::Equal<false>(extension, "script") ||
                              GTCore::Strings::Equal<false>(extension, "style");

                script.Push(result);
                return 1;
            }




            void AddSceneNodeAndChildrenToPrefab(SceneNodeClass &prefab, SceneNode &sceneNode, uint64_t parentID)
            {
                auto metadata = sceneNode.GetComponent<EditorMetadataComponent>();
                assert(metadata != nullptr);
                {
                    uint64_t id = 0;

                    // If the prefab is already linked to a different node, we actually want to auto-generate every ID by leaving the IDs set at 0.
                    if (GTCore::Strings::Equal<false>(prefab.GetRelativePath(), metadata->GetPrefabRelativePath()))
                    {
                        // If a prefab scene node of the same ID already exists, we'll need to regenerate an ID for that node.
                        if (prefab.GetSerializerByID(metadata->GetPrefabID()) == nullptr)
                        {
                            id = metadata->GetPrefabID();
                        }
                    }

                    id = prefab.AddSingleSceneNode(sceneNode, id, parentID);


                    // The scene node needs to be linked to the prefab.
                    metadata->LinkToPrefab(prefab.GetRelativePath(), id);


                    // And now we need to add the children.
                    for (auto childNode = sceneNode.GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
                    {
                        AddSceneNodeAndChildrenToPrefab(prefab, *childNode, id);
                    }
                }
            }

            // TODO: Move this to GTEngine.Editor (not SceneEditor). Rename to UpdatePrefab().
            int CreatePrefab(GTCore::Script &script)
            {
                auto absolutePath   = script.ToString(1);
                auto makeRelativeTo = script.ToString(2);
                auto sceneNode      = static_cast<SceneNode*>(script.ToPointer(3));

                if (absolutePath != nullptr && makeRelativeTo != nullptr && sceneNode != nullptr)
                {
                    auto prefab = SceneNodeClassLibrary::Acquire(absolutePath, makeRelativeTo);
                    if (prefab != nullptr)
                    {
                        prefab->Clear();
                        AddSceneNodeAndChildrenToPrefab(*prefab, *sceneNode, 0);

                        prefab->WriteToFile();

                        SceneNodeClassLibrary::Unacquire(prefab);
                    }
                }

                return 0;
            }



            //////////////////////////////////////////////////
            // GTEngine.System
            namespace SystemFFI
            {
                //////////////////////////////////////////////////
                // GTEngine.System.SceneNode
                namespace SceneNodeFFI
                {
                    int Create(GTCore::Script &script)
                    {
                        script.Push(new SceneNode());
                        return 1;
                    }

                    int Delete(GTCore::Script &script)
                    {
                        delete reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        return 0;
                    }


                    int GetID(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            script.Push(static_cast<int>(sceneNode->GetID()));
                        }
                        else
                        {
                            script.Push(0);
                        }

                        return 1;
                    }


                    int GetName(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            script.Push(sceneNode->GetName());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }

                    int SetName(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetName(script.ToString(2));
                        }

                        return 0;
                    }


                    int GetParentPtr(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto parent = sceneNode->GetParent();
                            if (parent != nullptr)
                            {
                                script.Push(sceneNode->GetParent());
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

                    int AttachChild(GTCore::Script &script)
                    {
                        auto parentNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        auto childNode  = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (parentNode != nullptr && childNode != nullptr)
                        {
                            parentNode->AttachChild(*childNode, script.ToBoolean(3));
                        }

                        return 0;
                    }

                    int Orphan(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->DetachFromParent(script.ToBoolean(2));
                        }

                        return 0;
                    }

                    int IsAncestor(GTCore::Script &script)
                    {
                        auto sceneNode         = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        auto ancestorSceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneNode != nullptr && ancestorSceneNode != nullptr)
                        {
                            script.Push(sceneNode->IsAncestor(*ancestorSceneNode));
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }

                    int IsDescendant(GTCore::Script &script)
                    {
                        auto sceneNode           = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        auto descendantSceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneNode != nullptr && descendantSceneNode != nullptr)
                        {
                            script.Push(sceneNode->IsDescendant(*descendantSceneNode));
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }

                    int IsRelative(GTCore::Script &script)
                    {
                        auto sceneNode         = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        auto relativeSceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneNode != nullptr && relativeSceneNode != nullptr)
                        {
                            script.Push(sceneNode->IsRelated(*relativeSceneNode));
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int GetChildrenIDs(GTCore::Script &script)
                    {
                        script.PushNewTable();

                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            int index = 0;

                            for (auto childNode = sceneNode->GetFirstChild(); childNode != nullptr; childNode = childNode->GetNextSibling())
                            {
                                script.SetTableValue(-1, ++index, static_cast<int>(childNode->GetID()));
                            }
                        }

                        return 1;
                    }


                    int AddComponent(GTCore::Script &script)
                    {
                        auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        auto componentName = script.ToString(2);

                        if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
                        {
                            PushComponent(script, "ModelComponent", sceneNode->AddComponent<ModelComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
                        {
                            PushComponent(script, "PointLightComponent", sceneNode->AddComponent<PointLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, SpotLightComponent::Name))
                        {
                            PushComponent(script, "SpotLightComponent", sceneNode->AddComponent<SpotLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, DirectionalLightComponent::Name))
                        {
                            PushComponent(script, "DirectionalLightComponent", sceneNode->AddComponent<DirectionalLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, AmbientLightComponent::Name))
                        {
                            PushComponent(script, "AmbientLightComponent", sceneNode->AddComponent<AmbientLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, DynamicsComponent::Name))
                        {
                            PushComponent(script, "DynamicsComponent", sceneNode->AddComponent<DynamicsComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, ProximityComponent::Name))
                        {
                            PushComponent(script, "ProximityComponent", sceneNode->AddComponent<ProximityComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, ScriptComponent::Name))
                        {
                            PushComponent(script, "ScriptComponent", sceneNode->AddComponent<ScriptComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
                        {
                            PushComponent(script, "EditorMetadataComponent", sceneNode->AddComponent<EditorMetadataComponent>());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }

                    int RemoveComponent(GTCore::Script &script)
                    {
                        reinterpret_cast<SceneNode*>(script.ToPointer(1))->RemoveComponentByName(script.ToString(2));
                        return 0;
                    }

                    int GetComponent(GTCore::Script &script)
                    {
                        auto sceneNode     = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        auto componentName = script.ToString(2);

                        if (GTCore::Strings::Equal(componentName, ModelComponent::Name))
                        {
                            PushComponent(script, "ModelComponent", sceneNode->GetComponent<ModelComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, PointLightComponent::Name))
                        {
                            PushComponent(script, "PointLightComponent", sceneNode->GetComponent<PointLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, SpotLightComponent::Name))
                        {
                            PushComponent(script, "SpotLightComponent", sceneNode->GetComponent<SpotLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, DirectionalLightComponent::Name))
                        {
                            PushComponent(script, "DirectionalLightComponent", sceneNode->GetComponent<DirectionalLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, AmbientLightComponent::Name))
                        {
                            PushComponent(script, "AmbientLightComponent", sceneNode->GetComponent<AmbientLightComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, DynamicsComponent::Name))
                        {
                            PushComponent(script, "DynamicsComponent", sceneNode->GetComponent<DynamicsComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, ProximityComponent::Name))
                        {
                            PushComponent(script, "ProximityComponent", sceneNode->GetComponent<ProximityComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, ScriptComponent::Name))
                        {
                            PushComponent(script, "ScriptComponent", sceneNode->GetComponent<ScriptComponent>());
                        }
                        else if (GTCore::Strings::Equal(componentName, EditorMetadataComponent::Name))
                        {
                            PushComponent(script, "EditorMetadataComponent", sceneNode->GetComponent<EditorMetadataComponent>());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }



                    int GetPosition(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto &position = sceneNode->GetPosition();

                            script.Push(position.x);
                            script.Push(position.y);
                            script.Push(position.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }

                    int GetWorldPosition(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto position = sceneNode->GetWorldPosition();

                            script.Push(position.x);
                            script.Push(position.y);
                            script.Push(position.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }


                    int SetPosition(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetPosition(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int SetWorldPosition(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetWorldPosition(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }



                    int GetRotationXYZ(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto &orientation = sceneNode->GetOrientation();

                            script.Push(glm::pitch(orientation));
                            script.Push(glm::yaw(orientation));
                            script.Push(glm::roll(orientation));
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }

                    int GetWorldRotationXYZ(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto orientation = sceneNode->GetWorldOrientation();

                            script.Push(glm::pitch(orientation));
                            script.Push(glm::yaw(orientation));
                            script.Push(glm::roll(orientation));
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }


                    int SetRotationXYZ(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            // Order: pitch/yaw/roll
                            sceneNode->SetOrientation(glm::quat(glm::vec3(glm::radians(script.ToFloat(2)), glm::radians(script.ToFloat(3)), glm::radians(script.ToFloat(4)))));
                        }

                        return 0;
                    }

                    int SetWorldRotationXYZ(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            // Order: pitch/yaw/roll
                            sceneNode->SetWorldOrientation(glm::quat(glm::vec3(glm::radians(script.ToFloat(2)), glm::radians(script.ToFloat(3)), glm::radians(script.ToFloat(4)))));
                        }

                        return 0;
                    }




                    int GetScale(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto &scale = sceneNode->GetScale();

                            script.Push(scale.x);
                            script.Push(scale.y);
                            script.Push(scale.z);
                        }
                        else
                        {
                            script.Push(1.0f);
                            script.Push(1.0f);
                            script.Push(1.0f);
                        }

                        return 3;
                    }

                    int GetWorldScale(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            auto scale = sceneNode->GetWorldScale();

                            script.Push(scale.x);
                            script.Push(scale.y);
                            script.Push(scale.z);
                        }
                        else
                        {
                            script.Push(1.0f);
                            script.Push(1.0f);
                            script.Push(1.0f);
                        }

                        return 3;
                    }


                    int SetScale(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetScale(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int SetWorldScale(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetWorldScale(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }



                    int GetAttachedComponentIDs(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            script.PushNewTable();


                            GTCore::Vector<GTCore::String> componentNames;
                            sceneNode->GetAttachedComponentNames(componentNames);

                            for (size_t i = 0; i < componentNames.count; ++i)
                            {
                                script.SetTableValue(-1, i + 1, componentNames[i].c_str());
                            }
                        }

                        return 1;
                    }


                    int Show(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->Show();
                        }

                        return 0;
                    }

                    int Hide(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->Hide();
                        }

                        return 0;
                    }

                    int IsVisible(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            script.Push(sceneNode->IsVisible());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }

                    int GetScenePtr(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr && sceneNode->GetScene() != nullptr)
                        {
                            script.Push(sceneNode->GetScene());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    void PushComponent(GTCore::Script &script, const char* componentClassName, Component* component)
                    {
                        if (component != nullptr)
                        {
                            // GTEngine.<componentClassName>.Create(component);

                            script.GetGlobal("GTEngine");
                            assert(script.IsTable(-1));
                            {
                                script.Push(componentClassName);
                                script.GetTableValue(-2);
                                assert(script.IsTable(-1));
                                {
                                    script.Push("Create");
                                    script.GetTableValue(-2);
                                    assert(script.IsFunction(-1));
                                    {
                                        script.Push(component);
                                        script.Call(1, 1);

                                        // GTEngine and GTEngine.<componentClassName> need to be popped, so we need to insert the return value into the location that will eventually place
                                        // it at -1 after popping those tables.
                                        script.InsertIntoStack(-3);
                                    }
                                }
                                script.Pop(1);
                            }
                            script.Pop(1);
                        }
                        else
                        {
                            script.PushNil();
                        }
                    }
                }


                //////////////////////////////////////////////////
                // GTEngine.System.Scene
                namespace SceneFFI
                {
                    int AddSceneNode(GTCore::Script &script)
                    {
                        auto scene     = reinterpret_cast<Scene*    >(script.ToPointer(1));
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (scene != nullptr && sceneNode != nullptr)
                        {
                            scene->AddSceneNode(*sceneNode);
                        }

                        return 0;
                    }

                    int RemoveSceneNode(GTCore::Script &script)
                    {
                        auto scene     = reinterpret_cast<Scene*    >(script.ToPointer(1));
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (scene != nullptr && sceneNode != nullptr)
                        {
                            scene->RemoveSceneNode(*sceneNode);
                        }

                        return 0;
                    }

                    int CreateNewSceneNode(GTCore::Script &script)
                    {
                        auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                        if (scene != nullptr)
                        {
                            script.Push(scene->CreateNewSceneNode());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }


                    int GetSceneNodePtrs(GTCore::Script &script)
                    {
                        script.PushNewTable();

                        auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                        if (scene != nullptr)
                        {
                            size_t sceneNodeCount = scene->GetSceneNodeCount();
                            for (size_t i = 0; i < sceneNodeCount; ++i)
                            {
                                auto sceneNode = scene->GetSceneNodeByIndex(i);
                                assert(sceneNode != nullptr);
                                {
                                    script.SetTableValue(-1, static_cast<int>(sceneNode->GetID()), sceneNode);
                                }
                            }
                        }

                        return 1;
                    }


                    int GetSceneNodePtrByID(GTCore::Script &script)
                    {
                        auto scene = reinterpret_cast<Scene*>(script.ToPointer(1));
                        if (scene != nullptr)
                        {
                            script.Push(scene->GetSceneNodeByID(static_cast<uint64_t>(script.ToInteger(2))));
                        }
                        else
                        {
                            script.PushNil();
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
                                script.Push(model->GetDefinition().fileName.c_str());
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
                            auto model = component->GetModel();
                            if (model != nullptr)
                            {
                                auto index        = script.ToInteger(2) - 1;        // Minus 1 because Lua is 1 based.
                                auto materialPath = script.ToString(3);

                                script.Push(model->meshes[index]->SetMaterial(materialPath));
                            }
                            else
                            {
                                script.Push(false);
                            }
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

                                script.Push(model->meshes[index]->GetMaterial()->GetDefinition().fileName.c_str());
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
                            component->SetColour(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetColour(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            auto &colour = component->GetColour();

                            script.Push(colour.x);
                            script.Push(colour.y);
                            script.Push(colour.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
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


                    int GetConstantAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetConstantAttenuation());
                        }
                        else
                        {
                            script.Push(1.0f);
                        }

                        return 1;
                    }

                    int GetLinearAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetLinearAttenuation());
                        }
                        else
                        {
                            script.Push(0.0f);
                        }

                        return 1;
                    }

                    int GetQuadraticAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetQuadraticAttenuation());
                        }
                        else
                        {
                            script.Push(0.0666f);
                        }

                        return 1;
                    }

                    int SetAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<PointLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float constant  = script.ToFloat(2);
                            float linear    = script.ToFloat(3);
                            float quadratic = script.ToFloat(4);
                            component->SetAttenuation(constant, linear, quadratic);
                        }

                        return 0;
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
                            component->SetColour(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetColour(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            auto &colour = component->GetColour();

                            script.Push(colour.x);
                            script.Push(colour.y);
                            script.Push(colour.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
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


                    int GetConstantAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetConstantAttenuation());
                        }
                        else
                        {
                            script.Push(1.0f);
                        }

                        return 1;
                    }

                    int GetLinearAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetLinearAttenuation());
                        }
                        else
                        {
                            script.Push(0.0f);
                        }

                        return 1;
                    }

                    int GetQuadraticAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            script.Push(component->GetQuadraticAttenuation());
                        }
                        else
                        {
                            script.Push(0.0666f);
                        }

                        return 1;
                    }

                    int SetAttenuation(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<SpotLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float constant  = script.ToFloat(2);
                            float linear    = script.ToFloat(3);
                            float quadratic = script.ToFloat(4);
                            component->SetAttenuation(constant, linear, quadratic);
                        }

                        return 0;
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
                            component->SetColour(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetColour(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DirectionalLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            auto &colour = component->GetColour();

                            script.Push(colour.x);
                            script.Push(colour.y);
                            script.Push(colour.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
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
                            component->SetColour(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetColour(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<AmbientLightComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            auto &colour = component->GetColour();

                            script.Push(colour.x);
                            script.Push(colour.y);
                            script.Push(colour.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
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
                            float halfX   = script.ToFloat(2);
                            float halfY   = script.ToFloat(3);
                            float halfZ   = script.ToFloat(4);
                            float offsetX = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            float offsetY = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;
                            float offsetZ = script.IsNumber(7) ? script.ToFloat(7) : 0.0f;

                            component->AddBoxCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }

                    int AddSphereShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float radius  = script.ToFloat(2);
                            float offsetX = script.IsNumber(3) ? script.ToFloat(3) : 0.0f;
                            float offsetY = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                            float offsetZ = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;

                            component->AddSphereCollisionShape(radius, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }

                    int AddEllipsoidShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float radiusX = script.ToFloat(2);
                            float radiusY = script.ToFloat(3);
                            float radiusZ = script.ToFloat(4);
                            float offsetX = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            float offsetY = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;
                            float offsetZ = script.IsNumber(7) ? script.ToFloat(7) : 0.0f;

                            component->AddEllipsoidCollisionShape(radiusX, radiusY, radiusZ, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }

                    int AddCylinderXShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float halfX   = script.ToFloat(2);
                            float halfY   = script.ToFloat(3);
                            float halfZ   = script.ToFloat(4);
                            float offsetX = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            float offsetY = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;
                            float offsetZ = script.IsNumber(7) ? script.ToFloat(7) : 0.0f;

                            component->AddCylinderXCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }

                    int AddCylinderYShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float halfX   = script.ToFloat(2);
                            float halfY   = script.ToFloat(3);
                            float halfZ   = script.ToFloat(4);
                            float offsetX = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            float offsetY = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;
                            float offsetZ = script.IsNumber(7) ? script.ToFloat(7) : 0.0f;

                            component->AddCylinderYCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }

                    int AddCylinderZShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float halfX   = script.ToFloat(2);
                            float halfY   = script.ToFloat(3);
                            float halfZ   = script.ToFloat(4);
                            float offsetX = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            float offsetY = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;
                            float offsetZ = script.IsNumber(7) ? script.ToFloat(7) : 0.0f;

                            component->AddCylinderZCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }

                    int AddCapsuleXShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float radius  = script.ToFloat(2);
                            float length  = script.ToFloat(3);
                            float offsetX = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                            float offsetY = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            float offsetZ = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;

                            component->AddCapsuleXCollisionShape(radius, length, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }

                    int AddCapsuleYShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float radius  = script.ToFloat(2);
                            float length  = script.ToFloat(3);
                            float offsetX = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                            float offsetY = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            float offsetZ = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;

                            component->AddCapsuleYCollisionShape(radius, length, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }

                    int AddCapsuleZShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            float radius  = script.ToFloat(2);
                            float length  = script.ToFloat(3);
                            float offsetX = script.IsNumber(4) ? script.ToFloat(4) : 0.0f;
                            float offsetY = script.IsNumber(5) ? script.ToFloat(5) : 0.0f;
                            float offsetZ = script.IsNumber(6) ? script.ToFloat(6) : 0.0f;

                            component->AddCapsuleZCollisionShape(radius, length, offsetX, offsetY, offsetZ);
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
                            float  offsetX    = script.ToFloat(3);
                            float  offsetY    = script.ToFloat(4);
                            float  offsetZ    = script.ToFloat(5);

                            component->SetCollisionShapeOffset(shapeIndex, offsetX, offsetY, offsetZ);
                        }

                        return 0;
                    }


                    int SetBoxShapeHalfExtents(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<CollisionShapeComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            size_t shapeIndex = static_cast<size_t>(script.ToInteger(2) - 1);
                            float  halfX      = script.ToFloat(3);
                            float  halfY      = script.ToFloat(4);
                            float  halfZ      = script.ToFloat(5);

                            script.Push(component->SetBoxCollisionShapeHalfExtents(shapeIndex, halfX, halfY, halfZ));
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
                            float  radiusX    = script.ToFloat(3);
                            float  radiusY    = script.ToFloat(4);
                            float  radiusZ    = script.ToFloat(5);

                            script.Push(component->SetEllipsoidCollisionShapeRadius(shapeIndex, radiusX, radiusY, radiusZ));
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
                            float  halfX      = script.ToFloat(3);
                            float  halfY      = script.ToFloat(4);
                            float  halfZ      = script.ToFloat(5);

                            script.Push(component->SetCylinderCollisionShapeHalfExtents(shapeIndex, halfX, halfY, halfZ));
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
                            component->SetLinearVelocity(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetLinearVelocity(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            glm::vec3 velocity = component->GetLinearVelocity();

                            script.Push(velocity.x);
                            script.Push(velocity.y);
                            script.Push(velocity.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }

                    int SetAngularVelocity(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->SetAngularVelocity(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetAngularVelocity(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            glm::vec3 velocity = component->GetAngularVelocity();

                            script.Push(velocity.x);
                            script.Push(velocity.y);
                            script.Push(velocity.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
                    }


                    int SetLinearFactor(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->SetLinearFactor(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int SetAngularFactor(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            if (script.IsNil(3))
                            {
                                component->SetAngularFactor(script.ToFloat(2));
                            }
                            else
                            {
                                component->SetAngularFactor(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                            }
                        }

                        return 0;
                    }


                    int SetGravity(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->SetGravity(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int GetGravity(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            glm::vec3 gravity = component->GetGravity();

                            script.Push(gravity.x);
                            script.Push(gravity.y);
                            script.Push(gravity.z);
                        }
                        else
                        {
                            script.Push(0.0f);
                            script.Push(0.0f);
                            script.Push(0.0f);
                        }

                        return 3;
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
                            component->ApplyCentralForce(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int ApplyTorque(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->ApplyTorque(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int ApplyCentralImpulse(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->ApplyCentralImpulse(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
                        }

                        return 0;
                    }

                    int ApplyImpulse(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->ApplyImpulse(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4), script.ToFloat(5), script.ToFloat(6), script.ToFloat(7));
                        }

                        return 0;
                    }

                    int ApplyTorqueImpulse(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->ApplyTorqueImpulse(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
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
                            component->AddScript(script.ToString(2));
                        }

                        return 0;
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
                            component->ReloadScript(static_cast<size_t>(script.ToInteger(2) - 1), script.ToString(3));      // <-- Subtract 1 because Lua is 1 based.
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



                namespace SubEditorFFI
                {
                    int GetAbsolutePath(GTCore::Script &script)
                    {
                        auto subEditor = reinterpret_cast<SubEditor*>(script.ToPointer(1));
                        if (subEditor != nullptr)
                        {
                            script.Push(subEditor->GetAbsolutePath());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }

                    int GetRelativePath(GTCore::Script &script)
                    {
                        auto subEditor = reinterpret_cast<SubEditor*>(script.ToPointer(1));
                        if (subEditor != nullptr)
                        {
                            script.Push(subEditor->GetRelativePath());
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }
                }

                namespace ModelEditorFFI
                {
                    int GetMaterials(GTCore::Script &script)
                    {
                        script.PushNewTable();

                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            GTCore::Vector<GTCore::String> materials;
                            modelEditor->GetMaterials(materials);

                            for (size_t i = 0; i < materials.count; ++i)
                            {
                                script.SetTableValue(-1, static_cast<int>(i + 1), materials[i].c_str());
                            }
                        }

                        return 1;
                    }

                    int GetConvexHullBuildSettings(GTCore::Script &script)
                    {
                        ConvexHullBuildSettings settings;

                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->GetConvexHullBuildSettings(settings);
                        }

                        script.PushNewTable();
                        script.SetTableValue(-1, "compacityWeight",               settings.compacityWeight);
                        script.SetTableValue(-1, "volumeWeight",                  settings.volumeWeight);
                        script.SetTableValue(-1, "minClusters",                   static_cast<int>(settings.minClusters));
                        script.SetTableValue(-1, "verticesPerCH",                 static_cast<int>(settings.verticesPerCH));
                        script.SetTableValue(-1, "concavity",                     settings.concavity);
                        script.SetTableValue(-1, "smallClusterThreshold",         settings.smallClusterThreshold);
                        script.SetTableValue(-1, "connectedComponentsDist",       settings.connectedComponentsDist);
                        script.SetTableValue(-1, "simplifiedTriangleCountTarget", static_cast<int>(settings.simplifiedTriangleCountTarget));
                        script.SetTableValue(-1, "addExtraDistPoints",            settings.addExtraDistPoints);
                        script.SetTableValue(-1, "addFacesPoints",                settings.addFacesPoints);

                        return 1;
                    }

                    int PlayAnimation(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->PlayAnimation();
                        }

                        return 0;
                    }

                    int StopAnimation(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->StopAnimation();
                        }

                        return 0;
                    }

                    int SetMaterial(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            script.Push(modelEditor->SetMaterial(script.ToInteger(2) - 1, script.ToString(3)));     // Minus 1 in the first argument because Lua is 1 based.
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }

                    int ShowConvexDecomposition(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->ShowConvexDecomposition();
                        }

                        return 0;
                    }

                    int HideConvexDecomposition(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            modelEditor->HideConvexDecomposition();
                        }

                        return 0;
                    }

                    int BuildConvexDecomposition(GTCore::Script &script)
                    {
                        auto modelEditor = reinterpret_cast<ModelEditor*>(script.ToPointer(1));
                        if (modelEditor != nullptr)
                        {
                            ConvexHullBuildSettings settings;
                            settings.compacityWeight               = script.ToFloat(2);
                            settings.volumeWeight                  = script.ToFloat(3);
                            settings.minClusters                   = static_cast<unsigned int>(script.ToInteger(4));
                            settings.verticesPerCH                 = static_cast<unsigned int>(script.ToInteger(5));
                            settings.concavity                     = script.ToFloat(6);
                            settings.smallClusterThreshold         = script.ToFloat(7);
                            settings.connectedComponentsDist       = script.ToFloat(8);
                            settings.simplifiedTriangleCountTarget = static_cast<unsigned int>(script.ToInteger(9));
                            settings.addExtraDistPoints            = script.ToBoolean(10);
                            settings.addFacesPoints                = script.ToBoolean(11);

                            modelEditor->BuildConvexDecomposition(settings);
                        }

                        return 0;
                    }
                }

                namespace SceneEditorFFI
                {
                    int GetScenePtr(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(&sceneEditor->GetScene());
                        }
                        else
                        {
                            script.Push(static_cast<void*>(nullptr));
                        }

                        return 1;
                    }

                    int DeselectAll(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DeselectAll(0);
                        }

                        return 0;
                    }

                    int SelectSceneNode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                            if (sceneNode != nullptr)
                            {
                                unsigned int selectionOptions = 0;
                                if (script.ToBoolean(3))
                                {
                                    selectionOptions |= SceneEditor::SelectionOption_NoScriptNotify;
                                }

                                sceneEditor->SelectSceneNode(*sceneNode, selectionOptions);
                            }
                        }

                        return 0;
                    }

                    int DeselectSceneNode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(2));
                            if (sceneNode != nullptr)
                            {
                                unsigned int selectionOptions = 0;
                                if (script.ToBoolean(3))
                                {
                                    selectionOptions |= SceneEditor::SelectionOption_NoScriptNotify;
                                }

                                sceneEditor->DeselectSceneNode(*sceneNode, selectionOptions);
                            }
                        }

                        return 0;
                    }

                    int GetSelectedSceneNodeCount(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(static_cast<int>(sceneEditor->GetSelectedSceneNodeCount()));
                        }
                        else
                        {
                            script.Push(0);
                        }

                        return 1;
                    }

                    int GetFirstSelectedSceneNodePtr(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->GetFirstSelectedSceneNode());
                        }
                        else
                        {
                            script.Push(static_cast<void*>(nullptr));
                        }

                        return 1;
                    }


                    int GetSelectedSceneNodeIDs(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            GTCore::Vector<uint64_t> selectedSceneNodeIDs;
                            sceneEditor->GetSelectedSceneNodeIDs(selectedSceneNodeIDs);

                            script.PushNewTable();
                            {
                                for (size_t i = 0; i < selectedSceneNodeIDs.count; ++i)
                                {
                                    script.SetTableValue(-1, static_cast<int>(i + 1), static_cast<int>(selectedSceneNodeIDs[i]));
                                }
                            }
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }

                    int IsSceneNodeSelectedByID(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            auto sceneNode = sceneEditor->GetSceneNodeByID(static_cast<uint64_t>(script.ToInteger(2)));
                            if (sceneNode != nullptr)
                            {
                                script.Push(sceneEditor->IsSceneNodeSelected(*sceneNode));
                            }
                            else
                            {
                                script.Push(false);
                            }
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int TryGizmoMouseSelect(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->TryGizmoMouseSelect());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }

                    int DoMouseSelection(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DoMouseSelection();
                        }

                        return 0;
                    }


                    int RemoveSelectedSceneNodes(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->RemoveSelectedSceneNodes();
                        }

                        return 0;
                    }

                    int RemoveSceneNode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneEditor != nullptr && sceneNode != nullptr)
                        {
                            sceneEditor->RemoveSceneNode(*sceneNode);
                        }

                        return 0;
                    }


                    int DuplicateSelectedSceneNodes(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DuplicateSelectedSceneNodes();
                        }

                        return 0;
                    }

                    int DuplicateSceneNode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneEditor != nullptr && sceneNode != nullptr)
                        {
                            sceneEditor->DuplicateSceneNode(*sceneNode);
                        }

                        return 0;
                    }



                    int SwitchGizmoToTranslateMode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToTranslateMode();
                        }

                        return 0;
                    }

                    int SwitchGizmoToRotateMode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToRotateMode();
                        }

                        return 0;
                    }

                    int SwitchGizmoToScaleMode(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToScaleMode();
                        }

                        return 0;
                    }

                    int SwitchGizmoToLocalSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToLocalSpace();
                        }

                        return 0;
                    }

                    int SwitchGizmoToGlobalSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->SwitchGizmoToGlobalSpace();
                        }

                        return 0;
                    }

                    int ToggleGizmoSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->ToggleGizmoSpace();
                        }

                        return 0;
                    }

                    int IsGizmoInLocalSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsGizmoInLocalSpace());
                        }
                        else
                        {
                            script.Push(true);      // Local space by default.
                        }

                        return 1;
                    }

                    int IsGizmoInGlobalSpace(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsGizmoInGlobalSpace());
                        }
                        else
                        {
                            script.Push(true);      // Global space by default.
                        }

                        return 1;
                    }


                    int StartPlaying(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->StartPlaying();
                        }

                        return 0;
                    }

                    int StopPlaying(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->StopPlaying();
                        }

                        return 0;
                    }

                    int IsPlaying(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsPlaying());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int EnablePhysicsSimulation(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->EnablePhysicsSimulation();
                        }

                        return 0;
                    }

                    int DisablePhysicsSimulation(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->DisablePhysicsSimulation();
                        }

                        return 0;
                    }

                    int IsPhysicsSimulationEnabled(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->IsPhysicsSimulationEnabled());
                        }
                        else
                        {
                            script.Push(false);
                        }

                        return 1;
                    }


                    int CommitStateStackFrame(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->CommitStateStackFrame();
                        }

                        return 0;
                    }

                    int Undo(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->Undo();
                        }

                        return 0;
                    }

                    int Redo(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            sceneEditor->Redo();
                        }

                        return 0;
                    }


                    int InstantiatePrefab(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            script.Push(sceneEditor->InstantiatePrefab(script.ToString(2)));
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }


                    int GetSceneNodePtrByID(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            uint64_t sceneNodeID = static_cast<uint64_t>(script.ToInteger(2));
                            script.Push(sceneEditor->GetScene().GetSceneNodeByID(sceneNodeID));
                        }
                        else
                        {
                            script.PushNil();
                        }

                        return 1;
                    }


                    int GetSceneNodes(GTCore::Script &script)
                    {
                        script.PushNewTable();

                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        if (sceneEditor != nullptr)
                        {
                            auto &sceneNodes = sceneEditor->GetScene().GetSceneNodes();

                            for (size_t i = 0; i < sceneNodes.GetCount(); ++i)
                            {
                                script.SetTableValue(-1, static_cast<int>(i + 1), sceneNodes.GetSceneNodeAtIndex(i));
                            }
                        }

                        return 1;
                    }


                    int PositionSceneNodeInFrontOfCamera(GTCore::Script &script)
                    {
                        auto sceneEditor = reinterpret_cast<SceneEditor*>(script.ToPointer(1));
                        auto sceneNode   = reinterpret_cast<SceneNode*>(script.ToPointer(2));

                        if (sceneEditor != nullptr && sceneNode != nullptr)
                        {
                            auto &cameraSceneNode = sceneEditor->GetCameraSceneNode();
                            {
                                sceneNode->SetWorldPosition(cameraSceneNode.GetWorldPosition() + (cameraSceneNode.GetWorldForwardVector() * 15.0f));
                            }
                        }

                        return 0;
                    }
                }
            }



            //////////////////////////////////////////////////
            // GTEngine.Rendering
            namespace RendererFFI
            {
                int EnableVSync(GTCore::Script &)
                {
                    Renderer::SetSwapInterval(1);
                    return 0;
                }

                int DisableVSync(GTCore::Script &)
                {
                    Renderer::SetSwapInterval(0);
                    return 0;
                }
            }

            //////////////////////////////////////////////////
            // GTEngine.Audio
            namespace AudioFFI
            {
                int Play(GTCore::Script &script)
                {
                    AudioComposer::Play(script.ToString(1));
                    return 0;
                }
            }
        }
    }
}
