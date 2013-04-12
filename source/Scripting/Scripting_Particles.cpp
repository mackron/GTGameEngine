// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/Scripting/Scripting_Particles.hpp>
#include <GTEngine/ParticleSystem.hpp>

namespace GTEngine
{
    namespace Scripting
    {
        bool LoadParticlesLibrary(GTCore::Script &script)
        {
            bool successful = true;


            // ParticleEmitter
            successful = successful && script.Execute
            (
                "GTEngine.ParticleEmitter = {};"
                "GTEngine.ParticleEmitter.__index = GTEngine.ParticleEmitter;"

                "function GTEngine.ParticleEmitter:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.ParticleEmitter);"
                "        new._internalPtr = internalPtr;"
                "    return new;"
                "end;"


                "function GTEngine.ParticleEmitter:EnableBurstMode()"
                "   GTEngine.System.ParticleEmitter.EnableBurstMode(self._internalPtr);"
                "end;"

                "function GTEngine.ParticleEmitter:DisableBurstMode()"
                "   GTEngine.System.ParticleEmitter.DisableBurstMode(self._internalPtr);"
                "end;"

                "function GTEngine.ParticleEmitter:IsBurstModeEnabled()"
                "   return GTEngine.System.ParticleEmitter.IsBurstModeEnabled(self._internalPtr);"
                "end;"


                "function GTEngine.ParticleEmitter:SetDurationInSeconds(durationInSeconds)"
                "    GTEngine.System.ParticleEmitter.SetDurationInSeconds(self._internalPtr, durationInSeconds);"
                "end;"

                "function GTEngine.ParticleEmitter:GetDurationInSeconds()"
                "    return GTEngine.System.ParticleEmitter.GetDurationInSeconds(self._internalPtr);"
                "end;"


                "function GTEngine.ParticleEmitter:SetEmissionRatePerSecond(emissionRatePerSecond)"
                "    GTEngine.System.ParticleEmitter.SetEmissionRatePerSecond(self._internalPtr, emissionRatePerSecond);"
                "end;"

                "function GTEngine.ParticleEmitter:GetEmissionRatePerSecond()"
                "    return GTEngine.System.ParticleEmitter.GetEmissionRatePerSecond(self._internalPtr);"
                "end;"


                "function GTEngine.ParticleEmitter:SetStartSpeed(startSpeed)"
                "    GTEngine.System.ParticleEmitter.SetStartSpeed(self._internalPtr, startSpeed);"
                "end;"

                "function GTEngine.ParticleEmitter:GetStartSpeed()"
                "    return GTEngine.System.ParticleEmitter.GetStartSpeed(self._internalPtr);"
                "end;"


                "function GTEngine.ParticleEmitter:SetLifetime(lifetime)"
                "    GTEngine.System.ParticleEmitter.SetLifetime(self._internalPtr, lifetime);"
                "end;"

                "function GTEngine.ParticleEmitter:GetLifetime()"
                "    return GTEngine.System.ParticleEmitter.GetLifetime(self._internalPtr);"
                "end;"


                "function GTEngine.ParticleEmitter:SetGravityFactor(gravityFactor)"
                "    GTEngine.System.ParticleEmitter.SetGravityFactor(self._internalPtr, gravityFactor);"
                "end;"

                "function GTEngine.ParticleEmitter:GetGravityFactor()"
                "    return GTEngine.System.ParticleEmitter.GetGravityFactor(self._internalPtr);"
                "end;"


                "function GTEngine.ParticleEmitter:GetEmissionShapeType()"
                "    return GTEngine.System.ParticleEmitter.GetEmissionShapeType(self._internalPtr);"
                "end;"

                "function GTEngine.ParticleEmitter:SetEmissionShapeType(shapeType)"
                "    return GTEngine.System.ParticleEmitter.SetEmissionShapeType(self._internalPtr, shapeType);"
                "end;"


                "function GTEngine.ParticleEmitter:SetConeEmissionShape(radius, angle)"
                "    return GTEngine.System.ParticleEmitter.SetConeEmissionShape(self._internalPtr, radius, angle);"
                "end;"

                "function GTEngine.ParticleEmitter:SetSphereEmissionShape(radius)"
                "    return GTEngine.System.ParticleEmitter.SetSphereEmissionShape(self._internalPtr, radius);"
                "end;"

                "function GTEngine.ParticleEmitter:SetBoxEmissionShape(x, y, z)"
                "    return GTEngine.System.ParticleEmitter.SetBoxEmissionShape(self._internalPtr, x, y, z);"
                "end;"

                "function GTEngine.ParticleEmitter:GetEmissionShapeProperties()"
                "    return GTEngine.System.ParticleEmitter.GetEmissionShapeProperties(self._internalPtr);"
                "end;"


                "function GTEngine.ParticleEmitter:SetBillboardMaterial(relativePath)"
                "    return GTEngine.System.ParticleEmitter.SetBillboardMaterial(self._internalPtr, relativePath);"
                "end;"

                "function GTEngine.ParticleEmitter:GetBillboardMaterialRelativePath()"
                "    return GTEngine.System.ParticleEmitter.GetBillboardMaterialRelativePath(self._internalPtr);"
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
                        script.Push("ParticleEmitter");
                        script.PushNewTable();
                        {
                            script.SetTableFunction(-1, "EnableBurstMode",                  ParticleEmitterFFI::EnableBurstMode);
                            script.SetTableFunction(-1, "DisableBurstMode",                 ParticleEmitterFFI::DisableBurstMode);
                            script.SetTableFunction(-1, "IsBurstModeEnabled",               ParticleEmitterFFI::IsBurstModeEnabled);
                            script.SetTableFunction(-1, "SetDurationInSeconds",             ParticleEmitterFFI::SetDurationInSeconds);
                            script.SetTableFunction(-1, "GetDurationInSeconds",             ParticleEmitterFFI::GetDurationInSeconds);
                            script.SetTableFunction(-1, "SetEmissionRatePerSecond",         ParticleEmitterFFI::SetEmissionRatePerSecond);
                            script.SetTableFunction(-1, "GetEmissionRatePerSecond",         ParticleEmitterFFI::GetEmissionRatePerSecond);
                            script.SetTableFunction(-1, "SetStartSpeed",                    ParticleEmitterFFI::SetStartSpeed);
                            script.SetTableFunction(-1, "GetStartSpeed",                    ParticleEmitterFFI::GetStartSpeed);
                            script.SetTableFunction(-1, "SetLifetime",                      ParticleEmitterFFI::SetLifetime);
                            script.SetTableFunction(-1, "GetLifetime",                      ParticleEmitterFFI::GetLifetime);
                            script.SetTableFunction(-1, "SetGravityFactor",                 ParticleEmitterFFI::SetGravityFactor);
                            script.SetTableFunction(-1, "GetGravityFactor",                 ParticleEmitterFFI::GetGravityFactor);
                            script.SetTableFunction(-1, "GetEmissionShapeType",             ParticleEmitterFFI::GetEmissionShapeType);
                            script.SetTableFunction(-1, "SetEmissionShapeType",             ParticleEmitterFFI::SetEmissionShapeType);
                            script.SetTableFunction(-1, "SetConeEmissionShape",             ParticleEmitterFFI::SetConeEmissionShape);
                            script.SetTableFunction(-1, "SetSphereEmissionShape",           ParticleEmitterFFI::SetSphereEmissionShape);
                            script.SetTableFunction(-1, "SetBoxEmissionShape",              ParticleEmitterFFI::SetBoxEmissionShape);
                            script.SetTableFunction(-1, "GetEmissionShapeProperties",       ParticleEmitterFFI::GetEmissionShapeProperties);
                            script.SetTableFunction(-1, "SetBillboardMaterial",             ParticleEmitterFFI::SetBillboardMaterial);
                            script.SetTableFunction(-1, "GetBillboardMaterialRelativePath", ParticleEmitterFFI::GetBillboardMaterialRelativePath);
                        }
                        script.SetTableValue(-3);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }


            // ParticleSystem
            successful = successful && script.Execute
            (
                "GTEngine.ParticleSystem = {};"
                "GTEngine.ParticleSystem.__index = GTEngine.ParticleSystem;"

                "function GTEngine.ParticleSystem:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.ParticleSystem);"
                "        new._internalPtr = internalPtr;"
                "    return new;"
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
                        script.Push("ParticleSystem");
                        script.PushNewTable();
                        {
                        }
                        script.SetTableValue(-3);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }


            // ParticleSystemDefinition
            successful = successful && script.Execute
            (
                "GTEngine.ParticleSystemDefinition = {};"
                "GTEngine.ParticleSystemDefinition.__index = GTEngine.ParticleSystemDefinition;"

                "function GTEngine.ParticleSystemDefinition:Create(internalPtr)"
                "    local new = {};"
                "    setmetatable(new, GTEngine.ParticleSystemDefinition);"
                "        new._internalPtr = internalPtr;"
                "        new.Emitters = {};"
                ""
                "        self:UpdateEmitters();"
                "    return new;"
                "end;"

                "function GTEngine.ParticleSystemDefinition:UpdateEmitters()"
                "    local count = self:GetEmitterCount();"
                "    for i=1,count do"
                "        local iEmitterPtr = self:GetEmitterPtrByIndex(i);"
                "        if self.Emitters[i] == nil or self.Emitters[i]._internalPtr ~= iEmitterPtr then"
                "            self.Emitters[i] = GTEngine.ParticleEmitter:Create(iEmitterPtr);"
                "        end;"
                "    end;"
                "end;"

                "function GTEngine.ParticleSystemDefinition:GetEmitterByIndex(index)"
                "    self:UpdateEmitters();"
                "    return self.Emitters[index];"
                "end;"


                "function GTEngine.ParticleSystemDefinition:GetEmitterCount()"
                "    return GTEngine.System.ParticleSystemDefinition.GetEmitterCount(self._internalPtr);"
                "end;"

                "function GTEngine.ParticleSystemDefinition:GetEmitterPtrByIndex(index)"
                "    return GTEngine.System.ParticleSystemDefinition.GetEmitterPtrByIndex(self._internalPtr, index);"
                "end;"

                "function GTEngine.ParticleSystemDefinition:AppendNewEmitter()"
                "    return GTEngine.System.ParticleSystemDefinition.AppendNewEmitter(self._internalPtr);"
                "end;"

                "function GTEngine.ParticleSystemDefinition:DeleteEmitterByIndex(index)"
                "    return GTEngine.System.ParticleSystemDefinition.DeleteEmitterByIndex(self._internalPtr, index);"
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
                        script.Push("ParticleSystemDefinition");
                        script.PushNewTable();
                        {
                            script.SetTableFunction(-1, "GetEmitterCount",      ParticleSystemDefinitionFFI::GetEmitterCount);
                            script.SetTableFunction(-1, "GetEmitterPtrByIndex", ParticleSystemDefinitionFFI::GetEmitterPtrByIndex);
                            script.SetTableFunction(-1, "AppendNewEmitter",     ParticleSystemDefinitionFFI::AppendNewEmitter);
                            script.SetTableFunction(-1, "DeleteEmitterByIndex", ParticleSystemDefinitionFFI::DeleteEmitterByIndex);
                        }
                        script.SetTableValue(-3);
                    }
                    script.Pop(1);
                }
                script.Pop(1);
            }


            // Misc. stuff like enumerators.
            if (successful)
            {
                script.GetGlobal("GTEngine");
                assert(script.IsTable(-1));
                {
                    // Emission shapes.
                    script.Push("ParticleEmissionShapes");
                    script.PushNewTable();
                    {
                        script.SetTableValue(-1, "Cone",   ParticleEmitter::EmissionShapeType_Cone);
                        script.SetTableValue(-1, "Sphere", ParticleEmitter::EmissionShapeType_Sphere);
                        script.SetTableValue(-1, "Box",    ParticleEmitter::EmissionShapeType_Box);
                    }
                    script.SetTableValue(-3);

                    // Particle shapes.
                    script.Push("ParticleShapeTypes");
                    script.PushNewTable();
                    {
                        script.SetTableValue(-1, "Billboard", ParticleEmitter::ParticleShapeType_Billboard);
                        script.SetTableValue(-1, "Model",     ParticleEmitter::ParticleShapeType_Model);
                    }
                    script.SetTableValue(-3);
                }
                script.Pop(1);
            }


            return successful;
        }



        namespace ParticleEmitterFFI
        {
            int EnableBurstMode(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    emitter->EnableBurstMode();
                }

                return 0;
            }

            int DisableBurstMode(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    emitter->DisableBurstMode();
                }

                return 0;
            }

            int IsBurstModeEnabled(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(emitter->IsBurstModeEnabled());
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }


            int SetDurationInSeconds(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    emitter->SetDurationInSeconds(script.ToDouble(2));
                }

                return 0;
            }

            int GetDurationInSeconds(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(emitter->GetDurationInSeconds());
                }
                else
                {
                    script.Push(5.0);
                }

                return 1;
            }


            int SetEmissionRatePerSecond(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    emitter->SetEmissionRatePerSecond(script.ToDouble(2));
                }

                return 0;
            }

            int GetEmissionRatePerSecond(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(emitter->GetEmissionRatePerSecond());
                }
                else
                {
                    script.Push(10.0);
                }

                return 1;
            }


            int SetStartSpeed(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    emitter->SetStartSpeed(script.ToDouble(2));
                }

                return 0;
            }

            int GetStartSpeed(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(emitter->GetStartSpeed());
                }
                else
                {
                    script.Push(5.0);
                }

                return 1;
            }


            int SetLifetime(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    emitter->SetLifetime(script.ToDouble(2));
                }

                return 0;
            }

            int GetLifetime(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(emitter->GetLifetime());
                }
                else
                {
                    script.Push(5.0);
                }

                return 1;
            }


            int SetGravityFactor(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    emitter->SetGravityFactor(script.ToDouble(2));
                }

                return 0;
            }

            int GetGravityFactor(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(emitter->GetGravityFactor());
                }
                else
                {
                    script.Push(5.0);
                }

                return 1;
            }


            int GetEmissionShapeType(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(emitter->GetEmissionShapeType());
                }
                else
                {
                    script.Push(ParticleEmitter::EmissionShapeType_Cone);
                }

                return 1;
            }

            int SetEmissionShapeType(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    emitter->SetEmissionShapeType(static_cast<ParticleEmitter::EmissionShapeType>(script.ToInteger(2)));
                }

                return 0;
            }


            int SetConeEmissionShape(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    float radius = script.ToFloat(2);
                    float angle  = script.ToFloat(3);

                    emitter->SetConeEmissionShape(radius, angle);
                }

                return 0;
            }

            int SetSphereEmissionShape(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    float radius = script.ToFloat(2);

                    emitter->SetSphereEmissionShape(radius);
                }

                return 0;
            }

            int SetBoxEmissionShape(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    float x = script.ToFloat(2);
                    float y = script.ToFloat(3);
                    float z = script.ToFloat(4);

                    emitter->SetBoxEmissionShape(x, y, z);
                }

                return 0;
            }

            int GetEmissionShapeProperties(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    switch (emitter->GetEmissionShapeType())
                    {
                    case ParticleEmitter::EmissionShapeType_Cone:
                        {
                            float radius;
                            float angle;
                            emitter->GetConeEmissionShape(radius, angle);

                            script.Push(radius);
                            script.Push(angle);

                            return 2;
                        }

                    case ParticleEmitter::EmissionShapeType_Sphere:
                        {
                            float radius;
                            emitter->GetSphereEmissionShape(radius);

                            script.Push(radius);

                            return 1;
                        }

                    case ParticleEmitter::EmissionShapeType_Box:
                        {
                            float x;
                            float y;
                            float z;
                            emitter->GetBoxEmissionShape(x, y, z);

                            script.Push(x);
                            script.Push(y);
                            script.Push(z);

                            return 3;
                        }


                    default: break;
                    }
                }

                return 0;
            }


            int SetBillboardMaterial(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(emitter->SetMaterial(script.ToString(2)));
                }
                else
                {
                    script.Push(false);
                }

                return 1;
            }

            int GetBillboardMaterialRelativePath(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    auto material = emitter->GetMaterial();
                    if (material != nullptr)
                    {
                        script.Push(material->GetDefinition().relativePath.c_str());
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
        }

        namespace ParticleSystemFFI
        {
        }

        namespace ParticleSystemDefinitionFFI
        {
            int GetEmitterCount(GTCore::Script &script)
            {
                auto definition = static_cast<ParticleSystemDefinition*>(script.ToPointer(1));
                if (definition != nullptr)
                {
                    script.Push(static_cast<int>(definition->GetEmitterCount()));
                }
                else
                {
                    script.Push(0);
                }

                return 1;
            }

            int GetEmitterPtrByIndex(GTCore::Script &script)
            {
                auto definition = static_cast<ParticleSystemDefinition*>(script.ToPointer(1));
                if (definition != nullptr)
                {
                    script.Push(definition->GetEmitter(script.ToInteger(2) - 1));       // Minus 1 because Lua is one based.
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }

            int AppendNewEmitter(GTCore::Script &script)
            {
                auto definition = static_cast<ParticleSystemDefinition*>(script.ToPointer(1));
                if (definition != nullptr)
                {
                    script.Push(definition->AppendNewEmitter());
                }
                else
                {
                    script.PushNil();
                }

                return 1;
            }

            int DeleteEmitterByIndex(GTCore::Script &script)
            {
                auto definition = static_cast<ParticleSystemDefinition*>(script.ToPointer(1));
                if (definition != nullptr)
                {
                    definition->DeleteEmitterByIndex(script.ToInteger(2) - 1);       // Minus 1 because Lua is one based.
                }

                return 0;
            }
        }
    }
}