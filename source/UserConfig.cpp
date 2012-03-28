
#include <GTEngine/UserConfig.hpp>
#include <GTEngine/Errors.hpp>

#include <GTCore/Strings/List.hpp>
#include <GTCore/Script.hpp>
#include <cassert>

namespace GTEngine
{
    GTCore::Script* UserConfigScript = nullptr;

    bool UserConfig::Startup(const char* clientDefaultConfig)
    {
        UserConfigScript = new GTCore::Script;

        // We load the server defaults first because it will allow the client defaults to override the server defaults if desired.
        const char* serverDefaultConfig =
            "LogFile = 'logs/engine.html';\n"

            "Display =\n"
            "{\n"
            "    Width  = 1280;\n"
            "    Height = 720;\n"
            "    FOV    = 90;\n"
            "    \n"
            "    Textures =\n"
            "    {\n"
            "        Anisotropy = 4;\n"
            "    }\n"
            "};\n";

        if (UserConfig::Load(serverDefaultConfig))
        {
            if (clientDefaultConfig != nullptr)
            {
                return UserConfig::Load(clientDefaultConfig);
            }

            // We'll get here is clientDefaultConfig was null. null is allowed.
            return true;
        }

        return false;
    }

    void UserConfig::Shutdown()
    {
        delete UserConfigScript;
        UserConfigScript = nullptr;
    }


    bool UserConfig::Load(const char* config)
    {
        assert(UserConfigScript != nullptr);

        if (UserConfigScript->LoadAndExecute(config))
        {
            return true;
        }

        // If we make it here, it means we failed to load the config script. The error message will be on the top of the script's stack.
        GTEngine::PostError("Config Script Error: %s", UserConfigScript->ToString(-1));
        return false;
    }

    bool UserConfig::LoadFile(const char* filename)
    {
        assert(UserConfigScript != nullptr);

        if (UserConfigScript->LoadFileAndExecute(filename))
        {
            return true;
        }

        // If we make it here, it means we failed to load the config script. The error message will be on the top of the script's stack.
        GTEngine::PostError("%s: %s", filename, UserConfigScript->ToString(-1));
        return false;
    }


    // *** Property Retrieval ***
    const char* UserConfig::GetString(const char* propertyName)
    {
        assert(UserConfigScript != nullptr);

        if (UserConfigScript->Get(propertyName))
        {
            if (UserConfigScript->IsString(-1))
            {
                auto result = UserConfigScript->ToString(-1);
                UserConfigScript->Pop(1);

                return result;
            }
        }

        return nullptr;
    }

    int UserConfig::GetInteger(const char* propertyName)
    {
        assert(UserConfigScript != nullptr);

        if (UserConfigScript->Get(propertyName))
        {
            if (UserConfigScript->IsNumber(-1))
            {
                auto result = UserConfigScript->ToInteger(-1);
                UserConfigScript->Pop(1);

                return result;
            }
        }

        return 0;
    }

    double UserConfig::GetDouble(const char* propertyName)
    {
        assert(UserConfigScript != nullptr);

        if (UserConfigScript->Get(propertyName))
        {
            if (UserConfigScript->IsNumber(-1))
            {
                auto result = UserConfigScript->ToDouble(-1);
                UserConfigScript->Pop(1);

                return result;
            }
        }

        return 0.0;
    }

    bool UserConfig::GetBoolean(const char* propertyName)
    {
        assert(UserConfigScript != nullptr);

        if (UserConfigScript->Get(propertyName))
        {
            if (UserConfigScript->IsBoolean(-1))
            {
                auto result = UserConfigScript->ToBoolean(-1);
                UserConfigScript->Pop(1);

                return result;
            }
        }

        return 0;
    }
}
