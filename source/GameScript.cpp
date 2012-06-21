
#include <GTEngine/GameScript.hpp>
#include <GTEngine/Errors.hpp>

namespace GTEngine
{
    GameScript::GameScript(Game &game)
        : game(game), lastError()
    {
        // The first thing we want to do is load some defaults.
        const char* defaultScript = 
            "Display =\n"
            "{\n"
            "    Width  = 1280;\n"
            "    Height = 720;\n"
            "    FOV    = 90;\n"
            "    \n"
            "    Textures =\n"
            "    {\n"
            "        Anisotropy = 16;\n"
            "    }\n"
            "};\n";

        this->Execute(defaultScript);
    }

    GameScript::~GameScript()
    {
    }

    bool GameScript::Load(const char* script)
    {
        if (!GTCore::Script::Load(script))
        {
            this->lastError = this->ToString(-1);
            return false;
        }

        return true;
    }

    bool GameScript::Execute()
    {
        if (!GTCore::Script::Execute())
        {
            this->lastError = this->ToString(-1);
            PostError("Script Error: %s", this->lastError.c_str());

            return false;
        }

        return true;
    }
}
