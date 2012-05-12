
#ifndef __GTEngine_ArmatureAnimation_hpp_
#define __GTEngine_ArmatureAnimation_hpp_

#include <GTCore/String.hpp>

namespace GTEngine
{
    class ArmatureAnimation
    {
    public:

        /// Constructor.
        ArmatureAnimation(const char* name);

        /// Destructor.
        ~ArmatureAnimation();


        /// Retrieves the name of the animation.
        const char* GetName() const { return this->name.c_str(); }

        /// Sets the name of the animation.
        void SetName(const char* newName) { this->name = newName; }


    private:

        /// The name of the animation.
        GTCore::String name;
    };
}

#endif
