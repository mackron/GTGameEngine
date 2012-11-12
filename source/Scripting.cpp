
#include <GTEngine/Scripting.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTEngine/Rendering/Renderer.hpp>
#include <GTEngine/Audio/AudioComposer.hpp>
#include <GTEngine/ModelLibrary.hpp>

#include <GTImage/Loader.hpp>

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
                "GTEngine.Renderer = {};"
                "GTEngine.Audio    = {};"
            );

            // Here we load the FFI.
            script.GetGlobal("GTEngine");
            if (script.IsTable(-1))
            {
                script.SetTableFunction(-1, "GetExecutableDirectory", FFI::GetExecutableDirectory);
                
                script.SetTableFunction(-1, "IsModelFile", FFI::IsModelFile);
                script.SetTableFunction(-1, "IsImageFile", FFI::IsImageFile);
                script.SetTableFunction(-1, "IsSoundFile", FFI::IsSoundFile);


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
                script.Push(ModelLibrary::IsExtensionSupported(GTCore::Path::Extension(script.ToString(1))));
                return 1;
            }

            int IsImageFile(GTCore::Script &script)
            {
                script.Push(GTImage::Loader::IsExtensionSupported(GTCore::Path::Extension(script.ToString(1))));
                return 1;
            }

            int IsSoundFile(GTCore::Script &script)
            {
                script.Push(AudioComposer::IsExtensionSupported(GTCore::Path::Extension(script.ToString(1))));
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
