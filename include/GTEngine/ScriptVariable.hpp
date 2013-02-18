// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptVariable_hpp_
#define __GTEngine_ScriptVariable_hpp_

#include "ScriptVariableTypes.hpp"

namespace GTEngine
{
    /// Class representing a public variable in a scene node script.
    ///
    /// The main purpose to this class is to make it possible to build an interface for editting tools.
    class ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable(ScriptVariableType type);

        /// Destructor.
        virtual ~ScriptVariable();


        /// Retrieves the type of this variable.
        ScriptVariableType GetType() const;



    private:

        /// The type of this variable (float, int, string, etc).
        ScriptVariableType type;
    };
}

#endif