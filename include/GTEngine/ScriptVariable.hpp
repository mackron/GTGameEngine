// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptVariable_hpp_
#define __GTEngine_ScriptVariable_hpp_

#include "ScriptVariableTypes.hpp"
#include <GTCore/String.hpp>

namespace GTEngine
{
    /// Class representing a public variable in a scene node script.
    ///
    /// The main purpose to this class is to make it possible to build an interface for editting tools.
    class ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable(ScriptVariableType type, const char* name);

        /// Destructor.
        virtual ~ScriptVariable();


        /// Retrieves the type of the variable.
        ScriptVariableType GetType() const;

        /// Retrieves the name of the variable.
        const char* GetName() const;



    private:

        /// The type of this variable (number, string, etc).
        ScriptVariableType type;

        /// The name of the variable.
        GTCore::String name;
    };


    /// Number script variable.
    class ScriptVariable_Number : public ScriptVariable
    {
    public:

        /// Constructor.
        ScriptVariable_Number(const char* name);

        /// Destructor.
        ~ScriptVariable_Number();


        /// Retrieves the value of the variable.
        double GetValue() const { return this->value; }

        /// Sets the value of the variable.
        void SetValue(double newValue) { this->value = newValue; }



    private:

        /// The value.
        double value;
    };
}

#endif