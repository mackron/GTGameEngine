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

                "function GTEngine.ParticleEmitter:SetDurationInSeconds(durationInSeconds)"
                "    return GTEngine.System.ParticleEmitter.SetDurationInSeconds(self._internalPtr, durationInSeconds);"
                "end;"

                "function GTEngine.ParticleEmitter:GetDurationInSeconds()"
                "    return GTEngine.System.ParticleEmitter.GetDurationInSeconds(self._internalPtr);"
                "end;"


                "function GTEngine.ParticleEmitter:SetEmissionRatePerSecond(emissionRatePerSecond)"
                "    return GTEngine.System.ParticleEmitter.SetEmissionRatePerSecond(self._internalPtr, emissionRatePerSecond);"
                "end;"

                "function GTEngine.ParticleEmitter:GetEmissionRatePerSecond()"
                "    return GTEngine.System.ParticleEmitter.GetEmissionRatePerSecond(self._internalPtr);"
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
                            script.SetTableFunction(-1, "SetDurationInSeconds",      ParticleEmitterFFI::SetDurationInSeconds);
                            script.SetTableFunction(-1, "GetDurationInSeconds",      ParticleEmitterFFI::GetDurationInSeconds);
                            script.SetTableFunction(-1, "SetEmissionRatePerSecond",  ParticleEmitterFFI::SetEmissionRatePerSecond);
                            script.SetTableFunction(-1, "GetEmissionRatePerSecond",  ParticleEmitterFFI::GetEmissionRatePerSecond);
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


            return successful;
        }



        namespace ParticleEmitterFFI
        {
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
                    emitter->SetEmissionRatePerSecond(script.ToInteger(2));
                }

                return 0;
            }

            int GetEmissionRatePerSecond(GTCore::Script &script)
            {
                auto emitter = static_cast<ParticleEmitter*>(script.ToPointer(1));
                if (emitter != nullptr)
                {
                    script.Push(static_cast<int>(emitter->GetEmissionRatePerSecond()));
                }
                else
                {
                    script.Push(10);
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