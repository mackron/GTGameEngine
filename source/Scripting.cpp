
#include <GTEngine/Scripting.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Audio.hpp>
#include <GTEngine/IO.hpp>
#include <GTEngine/Scene.hpp>

#include <GTCore/Path.hpp>


namespace GTEngine
{
    namespace Scripting
    {
        bool LoadGTEngineScriptLibrary(GTCore::Script &script)
        {
            bool successful = true;

            successful = successful && script.Execute
            (
                "GTEngine          = {};"
                
                "GTEngine.System   = {};"
                "GTEngine.System.ModelComponent            = {};"
                "GTEngine.System.CameraComponent           = {};"
                "GTEngine.System.PointLightComponent       = {};"
                "GTEngine.System.SpotLightComponent        = {};"
                "GTEngine.System.DirectionalLightComponent = {};"
                "GTEngine.System.AmbientLightComponent     = {};"
                "GTEngine.System.DynamicsComponent         = {};"
                "GTEngine.System.EditorMetadataComponent   = {};"
                "GTEngine.System.SceneNode                 = {};"
                "GTEngine.System.Scene                     = {};"

                "GTEngine.Renderer = {};"
                "GTEngine.Audio    = {};"
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
                    script.SetTableValue(-1, "EditorMetadata",    EditorMetadataComponent::Name);
                }
                script.SetTableValue(-3);
            }
            script.Pop(1);


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



                "function GTEngine.DynamicsComponent:AddBoxCollisionShape(halfX, halfY, halfZ, offsetX, offsetY, offsetZ)"
                "    GTEngine.System.DynamicsComponent.AddBoxCollisionShape(self._internalPtr, halfX, halfY, halfZ, offsetX, offsetY, offsetZ);"
                "end;"

                "function GTEngine.DynamicsComponent:AddCylinderCollisionShape(radius, offsetX, offsetY, offsetZ)"
                "    GTEngine.System.DynamicsComponent.AddCylinderCollisionShape(self._internalPtr, radius, offsetX, offsetY, offsetZ);"
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



                // EditorMetadataComponent
                "GTEngine.EditorMetadataComponent = {};"
                "GTEngine.EditorMetadataComponent.__index = GTEngine.EditorMetadataComponent;"

                "function GTEngine.EditorMetadataComponent.Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.EditorMetadataComponent);"
                "        new._internalPtr = internalPtr;"
                "    return new;"
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
                "    return new;"
                "end;"

                "function GTEngine.SceneNode:Delete()"
                "    if self._deleteInternalPtr then"
                "        GTEngine.System.SceneNode.Delete(self._internalPtr);"
                "    end;"
                "end;"


                "function GTEngine.SceneNode:GetName()"
                "    return GTEngine.System.SceneNode.GetName(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetName(name)"
                "    GTEngine.System.SceneNode.SetName(self._internalPtr, name);"
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


                "function GTEngine.SceneNode:Refresh()"
                "    return GTEngine.System.SceneNode.Refresh(self._internalPtr);"
                "end;"


                "function GTEngine.SceneNode:GetPosition()"
                "    return GTEngine.System.SceneNode.GetPosition(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetPosition(x, y, z)"
                "    return GTEngine.System.SceneNode.SetPosition(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:GetRotationXYZ()"
                "    return GTEngine.System.SceneNode.GetRotationXYZ(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetRotationXYZ(x, y, z)"
                "    return GTEngine.System.SceneNode.SetRotationXYZ(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.SceneNode:GetScale()"
                "    return GTEngine.System.SceneNode.GetScale(self._internalPtr);"
                "end;"

                "function GTEngine.SceneNode:SetScale(x, y, z)"
                "    return GTEngine.System.SceneNode.SetScale(self._internalPtr, x, y, z);"
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
                "    return new;"
                "end;"


                "function GTEngine.Scene:AddSceneNode(sceneNode)"
                "    GTEngine.System.Scene.AddSceneNode(self._internalPtr, sceneNode._internalPtr);"
                "end;"

                "function GTEngine.Scene.RemoveSceneNode(sceneNode)"
                "    GTEngine.System.Scene.RemoveSceneNode(self._internalPtr, sceneNode._internalPtr);"
                "end;"



                "GTEngine.RegisteredScenes = {};"
            );



            // Here we load the FFI.
            script.GetGlobal("GTEngine");
            if (script.IsTable(-1))
            {
                script.SetTableFunction(-1, "GetExecutableDirectory", FFI::GetExecutableDirectory);
                
                script.SetTableFunction(-1, "IsModelFile", FFI::IsModelFile);
                script.SetTableFunction(-1, "IsImageFile", FFI::IsImageFile);
                script.SetTableFunction(-1, "IsSoundFile", FFI::IsSoundFile);
                script.SetTableFunction(-1, "IsSceneFile", FFI::IsSceneFile);



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

                        script.SetTableFunction(-1, "GetName",                 FFI::SystemFFI::SceneNodeFFI::GetName);
                        script.SetTableFunction(-1, "SetName",                 FFI::SystemFFI::SceneNodeFFI::SetName);

                        script.SetTableFunction(-1, "AddComponent",            FFI::SystemFFI::SceneNodeFFI::AddComponent);
                        script.SetTableFunction(-1, "RemoveComponent",         FFI::SystemFFI::SceneNodeFFI::RemoveComponent);
                        script.SetTableFunction(-1, "GetComponent",            FFI::SystemFFI::SceneNodeFFI::GetComponent);

                        script.SetTableFunction(-1, "Refresh",                 FFI::SystemFFI::SceneNodeFFI::Refresh);

                        script.SetTableFunction(-1, "GetPosition",             FFI::SystemFFI::SceneNodeFFI::GetPosition);
                        script.SetTableFunction(-1, "SetPosition",             FFI::SystemFFI::SceneNodeFFI::SetPosition);
                        script.SetTableFunction(-1, "GetRotationXYZ",          FFI::SystemFFI::SceneNodeFFI::GetRotationXYZ);
                        script.SetTableFunction(-1, "SetRotationXYZ",          FFI::SystemFFI::SceneNodeFFI::SetRotationXYZ);
                        script.SetTableFunction(-1, "GetScale",                FFI::SystemFFI::SceneNodeFFI::GetScale);
                        script.SetTableFunction(-1, "SetScale",                FFI::SystemFFI::SceneNodeFFI::SetScale);

                        script.SetTableFunction(-1, "GetAttachedComponentIDs", FFI::SystemFFI::SceneNodeFFI::GetAttachedComponentIDs);

                        script.SetTableFunction(-1, "Show",                    FFI::SystemFFI::SceneNodeFFI::Show);
                        script.SetTableFunction(-1, "Hide",                    FFI::SystemFFI::SceneNodeFFI::Hide);
                        script.SetTableFunction(-1, "IsVisible",               FFI::SystemFFI::SceneNodeFFI::IsVisible);
                    }
                    script.Pop(1);


                    script.Push("Scene");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "AddSceneNode",    FFI::SystemFFI::SceneFFI::AddSceneNode);
                        script.SetTableFunction(-1, "RemoveSceneNode", FFI::SystemFFI::SceneFFI::RemoveSceneNode);
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
                        script.SetTableFunction(-1, "SetCollisionFilter",                             FFI::SystemFFI::DynamicsComponentFFI::SetCollisionFilter);
                        script.SetTableFunction(-1, "GetCollisionGroup",                              FFI::SystemFFI::DynamicsComponentFFI::GetCollisionGroup);
                        script.SetTableFunction(-1, "GetCollisionMask",                               FFI::SystemFFI::DynamicsComponentFFI::GetCollisionMask);

                        script.SetTableFunction(-1, "SetLinearVelocity",                              FFI::SystemFFI::DynamicsComponentFFI::SetLinearVelocity);
                        script.SetTableFunction(-1, "GetLinearVelocity",                              FFI::SystemFFI::DynamicsComponentFFI::GetLinearVelocity);
                        script.SetTableFunction(-1, "SetAngularVelocity",                             FFI::SystemFFI::DynamicsComponentFFI::SetAngularVelocity);
                        script.SetTableFunction(-1, "GetAngularVelocity",                             FFI::SystemFFI::DynamicsComponentFFI::GetAngularVelocity);

                        script.SetTableFunction(-1, "SetLinearFactor",                                FFI::SystemFFI::DynamicsComponentFFI::SetLinearFactor);
                        script.SetTableFunction(-1, "SetAngularFactor",                               FFI::SystemFFI::DynamicsComponentFFI::SetAngularFactor);

                        script.SetTableFunction(-1, "AddBoxCollisionShape",                           FFI::SystemFFI::DynamicsComponentFFI::AddBoxCollisionShape);
                        script.SetTableFunction(-1, "AddSphereCollisionShape",                        FFI::SystemFFI::DynamicsComponentFFI::AddSphereCollisionShape);
                        script.SetTableFunction(-1, "AddEllipsoidCollisionShape",                     FFI::SystemFFI::DynamicsComponentFFI::AddEllipsoidCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderXCollisionShape",                     FFI::SystemFFI::DynamicsComponentFFI::AddCylinderXCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderYCollisionShape",                     FFI::SystemFFI::DynamicsComponentFFI::AddCylinderYCollisionShape);
                        script.SetTableFunction(-1, "AddCylinderZCollisionShape",                     FFI::SystemFFI::DynamicsComponentFFI::AddCylinderZCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleXCollisionShape",                      FFI::SystemFFI::DynamicsComponentFFI::AddCapsuleXCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleYCollisionShape",                      FFI::SystemFFI::DynamicsComponentFFI::AddCapsuleYCollisionShape);
                        script.SetTableFunction(-1, "AddCapsuleZCollisionShape",                      FFI::SystemFFI::DynamicsComponentFFI::AddCapsuleZCollisionShape);
                        script.SetTableFunction(-1, "AddConvexHullCollisionShapesFromModelComponent", FFI::SystemFFI::DynamicsComponentFFI::AddConvexHullCollisionShapesFromModelComponent);
                        script.SetTableFunction(-1, "RemoveAllCollisionShapes",                       FFI::SystemFFI::DynamicsComponentFFI::RemoveAllCollisionShapes);
                    }
                    script.Pop(1);



                    script.Push("EditorMetadataComponent");
                    script.GetTableValue(-2);
                    if (script.IsTable(-1))
                    {
                        script.SetTableFunction(-1, "ShowSprite",              FFI::SystemFFI::EditorMetadataComponentFFI::ShowSprite);
                        script.SetTableFunction(-1, "HideSprite",              FFI::SystemFFI::EditorMetadataComponentFFI::HideSprite);
                        script.SetTableFunction(-1, "IsShowingSprite",         FFI::SystemFFI::EditorMetadataComponentFFI::IsShowingSprite);
                        script.SetTableFunction(-1, "GetSpriteTexturePath",    FFI::SystemFFI::EditorMetadataComponentFFI::GetSpriteTexturePath);
                        script.SetTableFunction(-1, "ShowDirectionArrow",      FFI::SystemFFI::EditorMetadataComponentFFI::ShowDirectionArrow);
                        script.SetTableFunction(-1, "HideDirectionArrow",      FFI::SystemFFI::EditorMetadataComponentFFI::HideDirectionArrow);
                        script.SetTableFunction(-1, "IsShowingDirectionArrow", FFI::SystemFFI::EditorMetadataComponentFFI::IsShowingDirectionArrow);
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


                    int Refresh(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->Refresh();
                        }

                        return 0;
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

                    int SetPosition(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetPosition(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
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

                    int SetScale(GTCore::Script &script)
                    {
                        auto sceneNode = reinterpret_cast<SceneNode*>(script.ToPointer(1));
                        if (sceneNode != nullptr)
                        {
                            sceneNode->SetScale(script.ToFloat(2), script.ToFloat(3), script.ToFloat(4));
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


                    int SetCollisionFilter(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->SetDamping(static_cast<short>(script.ToInteger(2)), static_cast<short>(script.ToInteger(3)));
                        }

                        return 0;
                    }

                    int GetCollisionGroup(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                        return 1;
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

                        return 1;
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


                    int AddBoxCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddSphereCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddEllipsoidCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddCylinderXCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddCylinderYCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddCylinderZCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddCapsuleXCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddCapsuleYCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddCapsuleZCollisionShape(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
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

                    int AddConvexHullCollisionShapesFromModelComponent(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            auto modelComponent = component->GetNode().GetComponent<ModelComponent>();
                            if (modelComponent != nullptr)
                            {
                                auto model = modelComponent->GetModel();
                                if (model != nullptr)
                                {
                                    component->AddConvexHullShapesFromModel(*model, script.ToFloat(2));
                                }
                            }
                        }

                        return 0;
                    }

                    int RemoveAllCollisionShapes(GTCore::Script &script)
                    {
                        auto component = reinterpret_cast<DynamicsComponent*>(script.ToPointer(1));
                        if (component != nullptr)
                        {
                            component->RemoveAllCollisionShapes();
                        }

                        return 0;
                    }
                }



                //////////////////////////////////////////////////
                // GTEngine.System.EditorMetadataComponent
                namespace EditorMetadataComponentFFI
                {
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
